#ifndef NFC_READER_H
#define NFC_READER_H
#include <Arduino.h>

void nfcReaderSetup(const String &deviceId);
void nfcReaderLoop();

#endif
