//Test on a Pi Pico W with SDA to pin GP4 and SCL to GP5

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   (2)
#define PN532_RESET (3)
#define HEADER_LENGTH 7

//I2C Config, swap for other interface
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing PN532...");
  while (!Serial);
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN532 module");
    while (1);
  }

  // Configure the NFC reader to read NFC tags
  nfc.SAMConfig();
  Serial.println("PN532 reader initialized");
  Serial.flush();
}

void loop() {
  Serial.println("Waiting for an NFC tag...");
  
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;
  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.println("NFC tag detected");
    uint8_t pageData[4];
    uint8_t ndefData[540]; // 4 * nfc page count
    int dataIndex = 0;

    for (int page = 4; page < 135; page++) {
      if (nfc.ntag2xx_ReadPage(page, pageData)) {
        for (int i = 0; i < 4; i++) {
          ndefData[dataIndex++] = pageData[i];
        }
      } else {
        break; //If fail, assume hit the end of the record
      }
    }
    Serial.println(parseNDEF(ndefData, dataIndex));
    Serial.flush();
  }
  delay(1000);
}

String parseNDEF(uint8_t *data, int length) {
  String result;
  if (data[0] != 0x03) {
    Serial.println("No NDEF data found.");
    return result;
  }

  //NDEF header info
  uint8_t ndefLength = data[1];
  uint8_t recordHeader = data[2];
  uint8_t typeLength = data[3];
  uint8_t payloadLength = data[4];

  if (data[5] == 'T') {
    uint8_t languageLength = data[6];
    for (int i = HEADER_LENGTH + languageLength; i < HEADER_LENGTH + payloadLength - 1; i++) {
      result += (char)data[i];
    }
  } else {
    Serial.println("Unsupported record type.");
  }
  return result;
}
