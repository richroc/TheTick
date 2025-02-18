#ifndef TICK_SYSLOG_H
#define TICK_SYSLOG_H

#ifdef USE_SYSLOG

#ifndef USE_WIFI
#error "USE_SYSLOG must be used with USE_WIFI"
#endif

#include <WiFi.h>
#include <NetworkUdp.h>
NetworkUDP udp;

void syslog_init(void) {
  syslog(String(dhcp_hostname) + F(" starting up!"));
}

void syslog(String text) {
  if (WiFi.status() != WL_CONNECTED || syslog_server == INADDR_NONE)
    return;
  udp.beginPacket(syslog_server, syslog_port);
  udp.printf("<%d> %s %s: %s", syslog_priority, syslog_host, syslog_service_name, text.c_str());
  udp.endPacket();
  return;
}

#else

void syslog_init(void) {}
void syslog(String text) {}

#endif
#endif