#include <MFRC522.h>
#include <SPI.h>

#define SAD 10
#define RST 5

MFRC522 nfc(SAD, RST);

void setup() {
  SPI.begin();
  Serial.begin(115200);

  Serial.println("Looking for MFRC522.");
  nfc.begin();

  uint8_t version = nfc.getFirmwareVersion();
  if (! version) {
    Serial.print("Didn't find MFRC522 board.");
    while(1); //halt
  }

  Serial.print("Found chip MFRC522 ");
  Serial.print("Firmware ver. 0x");
  Serial.print(version, HEX);
  Serial.println(".");
}

uint8_t keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };
uint8_t keyB[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };

void loop() {
  uint8_t status;
  uint8_t data[MAX_LEN];
  uint8_t serial[5];
  uint8_t i, len;

  status = nfc.requestCard(MF_REQIDL, data);

  if (status == MI_OK) {
    Serial.println("Card detected.");
    Serial.print("Type: ");
    Serial.print(data[0], HEX);
    Serial.println(data[1], HEX);

    status = nfc.anticollision(data);
    memcpy(serial, data, 5);

    Serial.println("The serial nb of the card is:");
    for (i = 0; i < 3; i++) {
      Serial.print(serial[i], HEX);
      Serial.print(", ");
    }
    Serial.println(serial[3], HEX);

    //len = nfc.cardCapacity(serial);
    for (i = 0; i < 64; i++) {
      status = nfc.authenticate(MF_AUTHENT1A, i, keyA, serial);
      if (status == MI_OK) {
        Serial.print("Authenticated block nb. 0x");
        Serial.print(i, HEX);
        Serial.println(" with key A.");
      } else {
        status = nfc.authenticate(MF_AUTHENT1B, i, keyB, serial);
        if (status == MI_OK) {
          Serial.print("Authenticated block nb. 0x");
          Serial.print(i, HEX);
          Serial.println(" with key B.");
        } else {
          Serial.print("Access denied at block nb. 0x");
          Serial.println(i, HEX);
        }
      }
    }

    nfc.haltCard();
  }
  delay(500);
}
