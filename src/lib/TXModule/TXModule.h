#pragma once

#include "targets.h"
#ifndef TARGET_NATIVE
#include "HardwareSerial.h"
#endif

#define OpenTXsyncPacketInterval 200  // in ms

// Link to handset (sync feedback):
//   crsf.setSyncParams(ModParams->interval);
//   crsf.JustSentRFpacket();
//   crsf.sendLinkStatisticsToTX();
//   crsf.sendTelemetryToTX(CRSFinBuffer);
//
// Communication with scripts inside handset via telemetry link (serial port
// TX):
//   crsf.sendLUAresponse(luaParams, 10);
//   crsf.sendLUAresponse(luaCommitPacket, 7);
//   switch (crsf.ParameterUpdateData[0])
//   crsf.sendLUAresponse(luaCommitPacket, 7);

// Periodics (to be called in some main loop):
//   sendSyncPacketToTX()

#define AutoSyncWaitPeriod 2000

// Better name: HandsetLink ???
class TXModule
{
protected:
  // Device throught which the TXModule is connected to a handset
  HardwareSerial* _dev;

  // Packet frequency / sync
  uint32_t packetInterval = 5000;  // default to 200hz as per 'normal'
  int32_t syncOffset = 0;
  uint32_t syncWaitPeriodCounter = 4000;  // 400us
  uint32_t syncOffsetSafeMargin;

  // Telemetry report back to handset
  uint32_t syncPacketInterval = 0;
  uint32_t syncLastSent = 0;

  // Stats
  uint32_t lastRecvChannels = 0;

  // Flush output buffers, etc
  void send();

public:
  TXModule() {}
  virtual ~TXModule() {}

  virtual void begin(HardwareSerial* dev);

  // Synchronisation with the handset (if supported)
  void setPacketInterval(uint32_t interval);

  // Radio packet has been sent (related to sync)
  void onRadioPacketSent();

  // Channel data has been just received (related to sync)
  void onChannelDataIn();

  // Call this periodically
  void poll(volatile uint16_t* channels);

  virtual void sendSyncPacketToTX() {}
  virtual void flushTxBuffers() {}
  virtual void consumeInputByte(uint8_t in, volatile uint16_t* channels) {}
};
