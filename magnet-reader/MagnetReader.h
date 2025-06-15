#ifndef MAGNET_READER_H
#define MAGNET_READER_H
#include <Arduino.h>

void magnetReaderSetup(const String &deviceId);
void magnetReaderLoop();

#endif
