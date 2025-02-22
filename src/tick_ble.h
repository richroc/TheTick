// Copyright (C) 2024, 2025 Jakub "lenwe" Kramarz
// This file is part of The Tick.
// 
// The Tick is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// The Tick is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License

#ifndef TICK_BLE_H
#define TICK_BLE_H

#ifdef USE_BLE

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <string>
#include <BLE2904.h>
#include <BLE2902.h>
// #include <BLE2901.h>
#include "WString.h"
#include "tick_utils.h"

BLEServer *pServer = NULL;
BLECharacteristic *cardCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *cardCharacteristic) {
    String value = String(cardCharacteristic->getValue().c_str());
    DBG_OUTPUT_PORT.print(F("BLE WRITE: "));
    DBG_OUTPUT_PORT.println(value);
    if (value.length() > 0) {
        String sendValue = value.substring(0, value.indexOf(":"));
        sendValue.toUpperCase();
        unsigned long bitcount = value.substring(value.indexOf(":") + 1).toInt();
        transmit_id(sendValue, bitcount);
    }
  }
};

void ble_init(void){
  // Create the BLE Device
  BLEDevice::init(dhcp_hostname.c_str());

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(ble_uuid_wiegand_service);
  
  // Create a BLE Characteristic
  cardCharacteristic = pService->createCharacteristic(
    ble_uuid_wiegand_characteristic,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
  );

  // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD)
  BLE2902* p2902Descriptor = new BLE2902();
  cardCharacteristic->addDescriptor(p2902Descriptor);

  // Creates BLE Descriptor 0x2904: Characteristic Presentation Format
  BLE2904* p2904Descriptor = new BLE2904();
  p2904Descriptor->setFormat(BLE2904::FORMAT_UTF8);
  cardCharacteristic->addDescriptor(p2904Descriptor);

  // Creates BLE Descriptor 0x2901: 0x2901 descriptor: Characteristic User Description
  // BLE2901* p2901Descriptor = new BLE2901();
  // p2901Descriptor->setDescription("Wiegand data");
  // p2901Descriptor->setAccessPermissions(ESP_GATT_PERM_READ);  // enforce read only - default is Read|Write
  // cardCharacteristic->addDescriptor(p2901Descriptor);


  cardCharacteristic->setCallbacks(new MyCallbacks());
  cardCharacteristic->setValue("0000:0");

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_UUID_WIEGAND_SERVICE);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  // Serial.println("Waiting a client connection to notify...");
}

void ble_loop(void){
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    pServer->startAdvertising();
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}

void ble_card_read(const char * card_id){
  cardCharacteristic->setValue(
    std::string(card_id)
  );
  if (deviceConnected) {
    cardCharacteristic->notify();
  }
}

#else

void ble_init(void){}
void ble_loop(void){}
void ble_card_read(const char * card_id){}

#endif
#endif