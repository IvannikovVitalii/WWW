#pragma once

#include "targets.h"

#if defined(GPIO_PIN_BUTTON) && (GPIO_PIN_BUTTON != UNDEF_PIN)
#include "button.h"
extern button button;
#endif

void TxInitSerial();
void TxInitLeds();
void TxInitBuzzer();

void TxUpdateLEDs(uint8_t isRXconnected, uint8_t tlm);
