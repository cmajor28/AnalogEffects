#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

// Buttons 
enum {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_COUNT
};

// 7 Segment display
enum {
  SEGMENT_A,
  SEGMENT_B,
  SEGMENT_C,
  SEGMENT_D,
  SEGMENT_E,
  SEGMENT_F,
  SEGMENT_G,
  SEGMENT_COUNT  
};

// Constants
const unsigned long gDebounceDelay = 50;
const unsigned long gDisplayTime = 2000;

// Pins used
const int gRotaryPin = A0;
const int gButtonPins[BUTTON_COUNT] = { A1, A2 };
const int gDisplayPins[SEGMENT_COUNT] =  { 2, 3, 5, 6, 9, 10, 11 };

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// Debounce inputs
int debounce(int currValue, int *lastValue, unsigned long *lastDebounceTime, const unsigned long debounceDelay) {

  // If the switch changed, due to noise or pressing:
  if (currValue != *lastValue) {
    // reset the debouncing timer
    *lastDebounceTime = millis();
  }

  // The debounce time has elapsed
  if ((millis() - *lastDebounceTime) > debounceDelay) {
    return currValue;
  }

  return *lastValue;
}

// Get rotary switch position
int getRotaryPosition(int value) {

  int bin;
  bin = 8 * (value - 1024/8/2) / 1024;
  bin = bin < 0 ? 0 : bin;
  return bin;
}

// Update 7 segment display
int updateDisplay(int digit, unsigned long displayTime, int displayPins[SEGMENT_COUNT]) {

  static unsigned long lastDisplayTime = 0;
  static unsigned long lastStartTime = 0;
  static int lastDigit = -1;
  int displayBitMap;

  if (millis() > (lastStartTime + lastDisplayTime)) {
    // Display time has expired
    lastDigit = -1;
  }

  if (digit >= 0 && digit <= 9) {
    // New digit to display
    lastDigit = digit;
    lastStartTime = millis();
    lastDisplayTime = displayTime;
  }

  // Get 7 segment display bitmap
  switch (lastDigit) {
    case 0: {
      displayBitMap = ~0x3F;
    }
    case 1: {
      displayBitMap = ~0x06;
    }
    case 2: {
      displayBitMap = ~0x5B;
    }
    case 3: {
      displayBitMap = ~0x4F;
    }
    case 4: {
      displayBitMap = ~0x66;
    }
    case 5: {
      displayBitMap = ~0x6D;
    }
    case 6: {
      displayBitMap = ~0x7D;
    }
    case 7: {
      displayBitMap = ~0x07;
    }
    case 8: {
      displayBitMap = ~0x7F;
    }
    case 9: {
      displayBitMap = ~0x6F;
    }
    default: {
      displayBitMap = ~0x00;
    }
  }

  // Set outputs
  for (int i = 0; i < 7; i++) {
    digitalWrite(displayPins[i], (displayBitMap >> i) & 0x1);
  }

  return digit;
}

char * parseMessage(char c) {

  static char msgBuffer[128] = { 0 };
  static int msgIndex = 0;

  if (msgIndex > 0) {
    if (msgIndex >= sizeof(msgBuffer)-1) {
      // Buffer full
      msgIndex = 0;
    } else if (c == '}') {
      // Found end
      msgBuffer[msgIndex++] = c;
      msgBuffer[msgIndex] = '\0';
      msgIndex = 0;
      return msgBuffer;
    } else if (c == '{') {
      // Reset buffer
      msgIndex = 0;
      msgBuffer[msgIndex++] = c;
    } else {
      // Got character
      msgBuffer[msgIndex++] = c;
    }
  } else {
    if (c == '{') {
      msgBuffer[msgIndex++] = c;
    }
  }

  return NULL;
}

void setup() 
{
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Pedal Matrix Remote"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if (!ble.begin(VERBOSE_MODE)) {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  
  Serial.println(F("OK!"));

  if (FACTORYRESET_ENABLE) {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if (!ble.factoryReset()) {
      error(F("Couldn't factory reset."));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (!ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if (ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION)) {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println(F("Switching to DATA mode!"));
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));

  // Setup pins
  pinMode(gButtonPins[BUTTON_UP], INPUT);
  pinMode(gButtonPins[BUTTON_DOWN], INPUT);
  for (int i = 0; i < SEGMENT_COUNT; i++) {
    pinMode(gDisplayPins[i], OUTPUT);
  }
}

void loop() 
{
  static int lastButtonState[BUTTON_COUNT] = { LOW, LOW };
  static int lastRotaryPosition = -1;
  
  static unsigned long lastButtonDebounceTime[BUTTON_COUNT] = { 0 };
  static unsigned long lastRotaryDebounceTime = 0;
  
  static int lastBank = -1;
  static int lastPreset = -1;

  char *jsonMessageIn = NULL;
  String jsonMessageOut = "";
  int ret;

  // Check for bluetooth input
  while (ble.available()) {
    jsonMessageIn = parseMessage(ble.read());
    if (jsonMessageIn != NULL) {
      ret = sscanf(jsonMessageIn, "{\"bank\":%d", &lastBank);
      if (ret == 1) {
        Serial.println("Got message: " + String(jsonMessageIn));
        Serial.println("Setting current bank to " + String(lastBank) + ".");
      }
    }
  }
  
  int rotaryPosition;
  int buttonState[BUTTON_COUNT];
  int currBank = lastBank;
  int currPreset = lastPreset;

  // Get button states
  rotaryPosition = debounce(getRotaryPosition(analogRead(gRotaryPin)), &lastRotaryPosition, &lastRotaryDebounceTime, gDebounceDelay);
  buttonState[BUTTON_UP] = debounce(digitalRead(gButtonPins[BUTTON_UP]), &lastButtonState[BUTTON_UP], &lastButtonDebounceTime[BUTTON_UP], gDebounceDelay);
  buttonState[BUTTON_DOWN] = debounce(digitalRead(gButtonPins[BUTTON_DOWN]), &lastButtonState[BUTTON_DOWN], &lastButtonDebounceTime[BUTTON_DOWN], gDebounceDelay);

  // If rotary switch has changed position
  if (rotaryPosition != lastRotaryPosition) {
    lastRotaryPosition = rotaryPosition;
    currPreset = rotaryPosition + 1;
    Serial.println("Rotary switch position changed to " + String(rotaryPosition) + ".");
  }

  // If bank up has been pressed
  if (buttonState[BUTTON_UP] == LOW && lastButtonState[BUTTON_UP] == HIGH) {
    lastButtonState[BUTTON_UP] = buttonState[BUTTON_UP];
    currBank = (currBank - 1 + 1) % 16 + 1;
    Serial.println("BUTTON_UP has been pressed.");
  }

  // If bank down has been pressed
  if (buttonState[BUTTON_DOWN] == LOW && lastButtonState[BUTTON_DOWN] == HIGH) {
    lastButtonState[BUTTON_DOWN] = buttonState[BUTTON_DOWN];
    currBank = (currBank - 1 + 16 - 1) % 16 + 1;
    Serial.println("BUTTON_DOWN has been pressed.");
  }

  // Update 7 segment display to see if done
  updateDisplay(-1, 0, gDisplayPins);

  if (currPreset != lastPreset) {
    jsonMessageOut = jsonMessageOut + ",\"preset\"=" + currPreset;
    lastPreset = currPreset;
    updateDisplay(currPreset, gDisplayTime, gDisplayPins);
    Serial.println("Current preset changed to " + String(currPreset) + ".");
  }

  if (currBank != lastBank) {
    jsonMessageOut = jsonMessageOut + ",\"bank\"=" + currBank;
    lastBank = currBank;
    updateDisplay(currBank, gDisplayTime, gDisplayPins);
    Serial.println("Current bank changed to " + String(currBank) + ".");
  }

  // If a json message is built
  if (jsonMessageOut != "") {
    jsonMessageOut[0] = '{'; // Remove leading comma
    jsonMessageOut += '}';

    // Send json message over BLE
    ble.print(jsonMessageOut);
    Serial.println("Sending message: " + jsonMessageOut);
  }
}
