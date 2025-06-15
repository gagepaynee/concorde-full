#include "../../magnet-reader/MagnetReader.h"
#include "../../shared/shared_secrets.h"
#include "secrets.h"

void setup() {
  magnetReaderSetup(MAGNET_ID);
}

void loop() {
  magnetReaderLoop();
}
