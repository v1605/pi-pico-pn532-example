# Pi Pico PN532 Example
Simple Example code to read a simple NDEF text record using a PN532 module and Raspberry Pi Pico W (via the Ardunio code). This code uses a I2C and the Adafruit_PN532 library to read the nfc tag. This is not a robust NDEF solution, but many popular libraries do not work properly on the Pico's core.
