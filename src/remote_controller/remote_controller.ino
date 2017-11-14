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
const unsigned long gDebounceTime = 50;
const unsigned long gDisplayTime = 2000;
const unsigned long gBlinkTime = 500;
const float gBattLowVoltage = 3.3;
const float gBattOkayVoltage = 3.35;

// Pins used
const int gRotaryPin = A0;
const int gBatteryPin = A9;
const int gLedPin = A3;
const int gButtonPins[BUTTON_COUNT] = { A1, A2 };
const int gDisplayPins[SEGMENT_COUNT] =  { 2, 3, 5, 6, 10, 11, 12 };

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// value = debounce()

int debounce(int value, int lastValue, unsigned long *lastDebounceTime, long debounceTime) {

  int debounceValue = lastValue;
  
  // If the switch changed, due to noise or pressing:
  if (value != lastValue && *lastDebounceTime == -1) {
    // reset the debouncing timer
    *lastDebounceTime = millis();
  }

  if ((millis() - *lastDebounceTime) > debounceTime) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (value != lastValue) {
      *lastDebounceTime = -1;
      debounceValue = value;
    }
  }

  return debounceValue;
}

// Get rotary switch position
int getRotaryPosition(int value) {

  // TODO switch is backwards so swap ADC values
  value = 1024 - value;

  int bin;
  bin = 7 * (value + 1024/7/2) / 1024;
  bin = bin < 0 ? 0 : bin;
  return bin;
}

// Get battery voltage 
float getBatteryVoltage() {

  int adcValue = analogRead(gBatteryPin);
  float measuredVBat = adcValue;
  measuredVBat *= 2; // we divided by 2, so multiply back
  measuredVBat *= 3.3; // Multiply by 3.3V, our reference voltage
  measuredVBat /= 1024; // convert to voltage
  return measuredVBat;
}

// Update 7 segment display
int updateDisplay(char digit, unsigned long displayTime, int displayPins[SEGMENT_COUNT]) {

  static unsigned long lastDisplayTime = 0;
  static unsigned long lastStartTime = 0;
  static char lastDigit = -1;
  int displayBitMap;

  if (millis() > (lastStartTime + lastDisplayTime)) {
    // Display time has expired
    lastDigit = -1;
  }

  if ((digit >= '0' && digit <= '9') || (digit >= 'a' && digit <= 'z')) {
    // New digit to display
    lastDigit = digit;
    lastStartTime = millis();
    lastDisplayTime = displayTime;
  }

  // Get 7 segment display bitmap
  switch (lastDigit) {
    case '0': {
      displayBitMap = ~0x3F;
      break;
    }
    case '1': {
      displayBitMap = ~0x06;
      break;
    }
    case '2': {
      displayBitMap = ~0x5B;
      break;
    }
    case '3': {
      displayBitMap = ~0x4F;
      break;
    }
    case '4': {
      displayBitMap = ~0x66;
      break;
    }
    case '5': {
      displayBitMap = ~0x6D;
      break;
    }
    case '6': {
      displayBitMap = ~0x7D;
      break;
    }
    case '7': {
      displayBitMap = ~0x07;
      break;
    }
    case '8': {
      displayBitMap = ~0x7F;
      break;
    }
    case '9': {
      displayBitMap = ~0x6F;
      break;
    }
    case 'a': {
      displayBitMap = ~0x77;
      break;
    }
    case 'b': {
      displayBitMap = ~0x7C;
      break;
    }
    case 'c': {
      displayBitMap = ~0x39;
      break;
    }
    case 'd': {
      displayBitMap = ~0x5E;
      break;
    }
    case 'e': {
      displayBitMap = ~0x79;
      break;
    }
    case 'f': {
      displayBitMap = ~0x71;
      break;
    }
    case 'g': {
      displayBitMap = ~0x6F;
      break;
    }
    case 'h': {
      displayBitMap = ~0x76;
      break;
    }
    case 'i': {
      displayBitMap = ~0x06;
      break;
    }
    case 'j': {
      displayBitMap = ~0x0E;
      break;
    }
    case 'k': {
      displayBitMap = ~0x70;
      break;
    }
    case 'l': {
      displayBitMap = ~0x38;
      break;
    }
    case 'm': {
      displayBitMap = ~0x55;
      break;
    }
    case 'n': {
      displayBitMap = ~0x54;
      break;
    }
    case 'o': {
      displayBitMap = ~0x5C;
      break;
    }
    case 'p': {
      displayBitMap = ~0x73;
      break;
    }
    case 'q': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 'r': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 's': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 't': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 'u': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 'v': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 'w': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 'x': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 'y': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    case 'z': {
      // TODO
      displayBitMap = ~0x7F;
      break;
    }
    default: {
      displayBitMap = ~0x00;
      break;
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
//  while (!Serial);  // required for Flora & Micro
//  delay(500);

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

//  /* Wait for connection */
//  while (!ble.isConnected()) {
//      delay(500);
//  }

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
  pinMode(gLedPin, OUTPUT);
  digitalWrite(gLedPin, LOW);
  pinMode(gButtonPins[BUTTON_UP], INPUT);
  pinMode(gButtonPins[BUTTON_DOWN], INPUT);
  for (int i = 0; i < SEGMENT_COUNT; i++) {
    pinMode(gDisplayPins[i], OUTPUT);
    digitalWrite(gDisplayPins[i], HIGH);
  }
}

void loop() 
{
  static int lastButtonState[BUTTON_COUNT] = { HIGH, HIGH };
  static long lastButtonDebounceTime[BUTTON_COUNT] = { -1, -1 };
  static int lastRotaryPosition = -1;
  
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
  
  static unsigned long blinkStart = 0;
  static bool blinkEnabled = false;
  float batteryVoltage;
  
  batteryVoltage = getBatteryVoltage();
  
  // Blink led if power is low
  if (batteryVoltage <= gBattLowVoltage) {
    if (blinkEnabled) {
      if (blinkStart + gBlinkTime <= millis()) {
      	blinkStart = millis();
      	digitalWrite(gLedPin, !digitalRead(gLedPin));
      }
    } else {
      blinkEnabled = true;
      blinkStart = millis();
      digitalWrite(gLedPin, HIGH);
      Serial.println("Battery low!");
    }
  } else if (blinkEnabled && batteryVoltage >= gBattOkayVoltage) {
    Serial.println("Battery okay!");
    blinkEnabled = false;
    digitalWrite(gLedPin, LOW);
  }
  
  int rotaryPosition;
  int buttonState[BUTTON_COUNT];
  int currBank = lastBank;
  int currPreset = lastPreset;
  
  // Get button states
  rotaryPosition = getRotaryPosition(analogRead(gRotaryPin));
  for (int i = 0; i < BUTTON_COUNT; i++) {
    buttonState[i] = debounce(digitalRead(gButtonPins[i]), lastButtonState[i], &lastButtonDebounceTime[i], gDebounceTime);
  }
  
  // If rotary switch has changed position
  if (rotaryPosition != lastRotaryPosition) {
    currPreset = rotaryPosition + 1;
    Serial.println("Rotary switch position changed to " + String(rotaryPosition) + ".");
  }
  lastRotaryPosition = rotaryPosition;

  // If bank up has been pressed
  if (buttonState[BUTTON_UP] == LOW && lastButtonState[BUTTON_UP] == HIGH) {
    if (currBank == -1) {
      // Uninitialized case
      currBank = 16;
    }
    currBank = (currBank - 1 + 1) % 16 + 1;
    Serial.println("BUTTON_UP has been pressed.");
  }
  lastButtonState[BUTTON_UP] = buttonState[BUTTON_UP];
  
  // If bank down has been pressed
  if (buttonState[BUTTON_DOWN] == LOW && lastButtonState[BUTTON_DOWN] == HIGH) {
    if (currBank == -1) {
      // Uninitialized case
      currBank = 1;
    }
    currBank = (currBank - 1 + 16 - 1) % 16 + 1;
    Serial.println("BUTTON_DOWN has been pressed.");
  }
  lastButtonState[BUTTON_DOWN] = buttonState[BUTTON_DOWN];

  // Update 7 segment display to see if done
  updateDisplay(-1, 0, gDisplayPins);

  if (currPreset != lastPreset) {
    jsonMessageOut = jsonMessageOut + ",\"preset\":" + currPreset;
    lastPreset = currPreset;
    updateDisplay('1' - 1 + currPreset, gDisplayTime, gDisplayPins);
    Serial.println("Current preset changed to " + String(currPreset) + ".");
  }

  if (currBank != lastBank) {
    jsonMessageOut = jsonMessageOut + ",\"bank\":" + currBank;
    lastBank = currBank;
    updateDisplay('a' - 1 + currBank, gDisplayTime, gDisplayPins);
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
