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

// Corporate 1000 bit default layouts (override if the target uses a custom mapping)
const C1000_LAYOUTS = {
  35: { fcStart: 2, fcLen: 12, cnStart: 14, cnLen: 20 }, // Bits 3–14 (Company/Facility), 15–34 (Card #)
  48: { fcStart: 2, fcLen: 22, cnStart: 24, cnLen: 23 }, // Bits 3–24 (Company/Facility), 25–47 (Card #)
};

// Helper to extract fields using the layout definition
function decodeCorporate1000(binaryData, bitLength) {
  const L = C1000_LAYOUTS[bitLength];
  if (!L) return null;
  const facilityCode = parseInt(binaryData.substring(L.fcStart, L.fcStart + L.fcLen), 2);
  const cardNumber  = parseInt(binaryData.substring(L.cnStart, L.cnStart + L.cnLen), 2);
  // For display, provide the raw hex (no preamble).
  const paddedData  = parseInt(binaryData, 2).toString(16).toUpperCase();
  const format = bitLength === 35 ? "C1000-35" : "C1000-48";
  return { format, facilityCode, cardNumber, paddedData };
}

function decodeWiegand(rawData, bitLength) {
  let binaryData = parseInt(rawData, 16).toString(2).padStart(bitLength, '0');
  let format, facilityCode, cardNumber, preamble, paddedData;

  switch (bitLength) {
    case 26:
      format = "H10301";
      facilityCode = parseInt(binaryData.substring(1, 9), 2);
      cardNumber = parseInt(binaryData.substring(9, 25), 2);
      preamble = '000000100000000001';
      paddedData = parseInt((preamble + binaryData), 2).toString(16);
      break;
    case 35: { // HID Corporate 1000 (common default layout. Configurable in C1000_LAYOUTS)
      const c = decodeCorporate1000(binaryData, 35);
      if (c) ({ format, facilityCode, cardNumber, paddedData } = c);
      else { format = "C1000-35"; facilityCode = "N/A"; cardNumber = "N/A"; paddedData = "N/A"; }
      break;
    }
    case 37:
      format = "H10304";
      facilityCode = parseInt(binaryData.substring(1, 17), 2);
      cardNumber = parseInt(binaryData.substring(17, 36), 2);
      preamble = '0000000';
      paddedData = parseInt((preamble + binaryData), 2).toString(16);
      break;
    case 46:
      format = "H800002";
      facilityCode = parseInt(binaryData.substring(1, 15), 2);
      cardNumber = parseInt(binaryData.substring(15, 45), 2);
      paddedData = 'N/A';
      break;
    case 48: { // HID Corporate 1000 (48-bit variant. Configurable in C1000_LAYOUTS)
      const c = decodeCorporate1000(binaryData, 48);
      if (c) ({ format, facilityCode, cardNumber, paddedData } = c);
      else { format = "C1000-48"; facilityCode = "N/A"; cardNumber = "N/A"; paddedData = "N/A"; }
      break;
    }
    default:
      format = "Unknown";
      facilityCode = "N/A";
      cardNumber = "N/A";
      paddedData = 'N/A';
  }

  return { format, facilityCode, cardNumber, paddedData };
}

// Helpers to calculate even/odd parity
const _evenParityBit = s => {
    let count = 0;
    for (let i = 0; i < s.length; i++) {
        if (s[i] === '1') count++;
    }
    return count % 2 === 0 ? 0 : 1;
};

const _oddParityBit = s => {
    let count = 0;
    for (let i = 0; i < s.length; i++) {
        if (s[i] === '1') count++;
    }
    return count % 2 === 1 ? 0 : 1;
};

function calculateWiegandParity(rawData, bitLength) {
  const isTriple = (bitLength === 35 || bitLength === 48);
  const dataLen = bitLength - (isTriple ? 3 : 2);
  const binaryData = parseInt(rawData, 16).toString(2).padStart(dataLen, '0');

  let evenParity, oddParity, middleParity;

  switch (bitLength) {
    case 26: {
      const first = binaryData.substring(0, 12);
      const last  = binaryData.substring(12);
      evenParity  = _evenParityBit(first);
      oddParity   = _oddParityBit(last);
      break;
    }
    case 37: {
      const first = binaryData.substring(0, 18);
      const last  = binaryData.substring(18);
      evenParity  = _evenParityBit(first);
      oddParity   = _oddParityBit(last);
      break;
    }
    case 46: {
      evenParity  = _evenParityBit(binaryData);
      oddParity   = _oddParityBit(binaryData);
      break;
    }
    case 35: { // Corporate 1000 (35-bit, 3 parity bits)
      const firstHalf = binaryData.substring(0, 16);
      const lastHalf  = binaryData.substring(16);
      evenParity   = _evenParityBit(firstHalf);
      middleParity = _oddParityBit(binaryData);
      oddParity    = _oddParityBit(lastHalf);
      break;
    }
    case 48: { // Corporate 1000 (48-bit, 3 parity bits)
      const firstPart = binaryData.substring(0, 22);
      const lastPart  = binaryData.substring(22);
      evenParity   = _evenParityBit(firstPart);
      middleParity = _oddParityBit(binaryData);
      oddParity    = _oddParityBit(lastPart);
      break;
    }
    default:
      return { error: "Unsupported bit length" };
  }

  return { evenParity, oddParity, middleParity };
}

function encodeWiegand(format, facilityCode, cardNumber) {
  let bitLength;
  let binaryData;

  switch (format) {
    case "H10301":
      bitLength  = 26;
      binaryData = facilityCode.toString(2).padStart(8, '0')
                 + cardNumber.toString(2).padStart(16, '0');
      break;
    case "H10304":
      bitLength  = 37;
      binaryData = facilityCode.toString(2).padStart(16, '0')
                 + cardNumber.toString(2).padStart(19, '0');
      break;
    case "H800002":
      bitLength  = 46;
      binaryData = facilityCode.toString(2).padStart(14, '0')
                 + cardNumber.toString(2).padStart(30, '0');
      break;
    case "C1000-35":
      bitLength  = 35;
      binaryData = facilityCode.toString(2).padStart(12, '0')
                 + cardNumber.toString(2).padStart(20, '0');
      break;
    case "C1000-48":
      bitLength  = 48;
      binaryData = facilityCode.toString(2).padStart(22, '0')
                 + cardNumber.toString(2).padStart(23, '0');
      break;
    default:
      return { error: "Unsupported format" };
  }

  const { evenParity, oddParity, middleParity } =
    calculateWiegandParity(parseInt(binaryData, 2).toString(16), bitLength);

  let encodedData;
  if (bitLength === 35 || bitLength === 48) {
    // 3 parity bits: [P1][P2] DATA [P3]
    encodedData = `${evenParity}${middleParity}${binaryData}${oddParity}`;
  } else {
    // 2 parity bits: [P1] DATA [P2]
    encodedData = `${evenParity}${binaryData}${oddParity}`;
  }
  return parseInt(encodedData, 2).toString(16).toUpperCase() + ":" + String(bitLength);
}

function send_wiegand(data) {
  $.get("/txid?v=" + data);
  alert(`Data sent: ${data}`);
}

function send_wiegand_raw(event) {
  event.preventDefault();
  let wiegand_raw = $("#wiegand_raw")[0].value;
  send_wiegand(wiegand_raw);

  return false;
}

function send_wiegand_formatted(event) {
  event.preventDefault();
  let wiegand_format = $("#wiegand_format")[0].value;
  let wiegand_fc = parseInt($("#wiegand_fc")[0].value, 10);
  let wiegand_cn = parseInt($("#wiegand_cn")[0].value, 10);

  let wiegand_raw = encodeWiegand(
    wiegand_format,
    wiegand_fc,
    wiegand_cn
  );

  send_wiegand(wiegand_raw);

  return false;
}

$(document).ready(function () {
  $.get("/log.txt", function (data) {
    let lines = data.trim().split("\n");
    let tableBody = "";
    let epochs = get_epochs(lines);
    // epoch, timestamp, facility, data  
    lines.forEach(line => {
      let parts = line.split("; ");
      if (parts[2] == "wiegand") {
        let wiegand = parts[3];
        let dataParts = wiegand.split(":");
        if (dataParts.length === 2) {
          let rawData = dataParts[0];
          let bitLength = parseInt(dataParts[1], 10);

          if (rawData.length != bitLength) {
            let { format, facilityCode, cardNumber, paddedData } = decodeWiegand(rawData, bitLength);
            let time = get_epoch_time(epochs, parts[0], parts[1]);
            tableBody += `<tr><td>${time}</td><td>${wiegand}</td><td>${format}</td><td>${facilityCode}</td><td>${cardNumber}</td><td>${paddedData}</td><td><i class="fas fa-reply" onclick="send_wiegand('${wiegand}');"></i></td></tr>`;
          }
        }
      }
    });
    $("#dataTable tbody").html(tableBody);
    $('#dataTable').DataTable();
  });
});
