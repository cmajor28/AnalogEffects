enum {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_COUNT
}

enum {
  SEGMENT_A,
  SEGMENT_B,
  SEGMENT_C,
  SEGMENT_D,
  SEGMENT_E,
  SEGMENT_F,
  SEGMENT_G,
  SEGMENT_COUNT  
}

const unsigned long debounceDelay = 50;

const int rotaryPin = A0;
const int buttonPin[BUTTON_COUNT] = { 0, 1 };
const int displayPin[SEGMENT_COUNT] =  { 2, 3, 5, 6, 9, 10, 11 };

int debounce(int currValue, int *lastValue, unsigned long *lastDebounceTime, const unsigned long debounceDelay) {

  // If the switch changed, due to noise or pressing:
  if (currValue != *lastValue) {
    // reset the debouncing timer
    *lastDebounceTime = millis();
  }

  // The deboucne time has elapsed
  if ((millis() - *lastDebounceTime) > debounceDelay) {
    return currValue;
  }

  return *lastValue;
}

int getRotaryPosition(value) {

  int bin;
  bin = 8 * (value - 1024/8/2) / 1024;
  bin = bin < 0 : 0 ? bin;
  return bin
}

int updateDisplay(int digit, unsigned long displayTime, int displayPins[SEGMENT_COUNT]) {

  static unsigned long lastDisplayTime = 0;
  static unsigned long lastStartTime = 0;
  static int lastDigit = -1;
  int displayBitMap;

  if (millis() > (lastStartTime + lastDisplayTime)) {
    lastDigit = -1;
  }

  if (digit >= 0 && digit <= 9) {
    lastDigit = digit;
    lastStartTime = millis();
    lastDisplayTime = displayTime;
  }

  switch (lastDigit) {
    case 0: {
      displayBitMap = 0x3F;
    }
    case 1: {
      displayBitMap = 0x06;
    }
    case 2: {
      displayBitMap = 0x5B;
    }
    case 3: {
      displayBitMap = 0x4F;
    }
    case 4: {
      displayBitMap = 0x66;
    }
    case 5: {
      displayBitMap = 0x6D;
    }
    case 6: {
      displayBitMap = 0x7D;
    }
    case 7: {
      displayBitMap = 0x07;
    }
    case 8: {
      displayBitMap = 0x7F;
    }
    case 9: {
      displayBitMap = 0x6F;
    }
    default: {
      displayBitMap = 0x00;
    }
  }

  for (int i = 0; i < 7; i++) {
    digitalWrite(displayPin[i], (displayBitMap >> i) & 0x01);
  }

  return digit;
}

void setup() {
  pinMode(buttonsPins[BUTTON_UP], INPUT);
  pinMode(buttonsPins[BUTTON_DOWN], INPUT);
  for (int i = 0; i < SEGMENT_COUNT; i++) {
    pinMode(displayPin[i], OUTPUT);
  }
}

void loop() {
  static int lastButtonState[BUTTON_COUNT] = { LOW, LOW };
  static int lastRotaryPosition = -1;
  
  static unsigned long lastButtonDebounceTime[BUTTON_COUNT] = 0;
  static unsigned long lastRotaryDebounceTime = 0;
  
  static int lastBank = -1;
  static int lastPreset = -1;
  
  int rotaryPosition;
  int buttonState[BUTTON_COUNT];
  int currBank = lastBank;
  int currPreset = lastPreset;
  String jsonMessage = "";

  // Get button states
  rotaryPosition = debounce(getRotaryPosition(analogRead(rotaryPin)), &lastRotaryPosition, &lastRotaryDebounceTime, debounceDelay);
  buttonState[BUTTON_UP] = debounce(digitalRead(buttonPin[BUTTON_UP]), &lastButtonState[BUTTON_UP], &lastButtonDebounceTime[BUTTON_UP], debounceDelay);
  buttonState[BUTTON_DOWN] = debounce(digitalRead(buttonPin[BUTTON_DOWN]), &lastButtonState[BUTTON_DOWN], &lastButtonDebounceTime[BUTTON_DOWN], debounceDelay);

  // If rotary switch has changed position
  if (rotaryPosition != lastRotaryPosition) {
    lastRotaryPosition = rotaryPosition;
    currPreset = rotaryPosition;
  }

  // If bank up has been pressed
  if (buttonState[BUTTON_UP] == LOW && lastButtonState[BUTTON_UP] == HIGH) {
    lastButtonState[BUTTON_UP] = buttonState[BUTTON_UP];
    currBank = (currBank + 1) % 16;
  }

  // If bank down has been pressed
  if (buttonState[BUTTON_DOWN] == LOW && lastButtonState[BUTTON_DOWN] == HIGH) {
    lastButtonState[BUTTON_DOWN] = buttonState[BUTTON_DOWN];
    currBank = (currBank + 16 - 1) % 16;
  }

  // Update 7 segment display to see if done
  updateDisplay(-1, 0);

  if (currPreset != lastPreset) {
    jsonMessage = jsonMessage + ",\"preset\"=" + currPreset;
    lastpreset = currPreset;
    updateDisplay(currPreset, displayTime);
  }

  if (currBank != lastBank) {
    jsonMessage = jsonMessage + ",\"bank\"=" + currBank;
    lastBank = currBank;
    updateDisplay(currBank, displayTime);
  }

  // If a json message is built
  if (jsonMessage != "") {
    jsonMesage.remove(0, 1); // Remove leading comma

    // Send json message over BLE
    
  }
}
