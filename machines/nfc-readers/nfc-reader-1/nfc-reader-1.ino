#include "../../nfc-reader/NfcReader.h"
#include "../../shared/shared_secrets.h"
#include "secrets.h"

void setup() {
  nfcReaderSetup(READER_ID);
}

void loop() {
  nfcReaderLoop();
}
