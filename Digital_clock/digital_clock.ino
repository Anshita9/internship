// Pin configurations
const byte segmentPins[7] = {2, 3, 4, 5, 6, 7, 8};
const byte selectorPins[4] = {9, 10, 11, 12};

const byte pinHrBtn  = A0;
const byte pinMinBtn = A1;
const byte pinRunBtn = A2;

// Common Anode/Cathode segment map (0 = ON, 1 = OFF)
byte numeralMap[10][7] = {
  {0, 0, 0, 0, 0, 0, 1}, // 0
  {1, 0, 0, 1, 1, 1, 1}, // 1
  {0, 0, 1, 0, 0, 1, 0}, // 2
  {0, 0, 0, 0, 1, 1, 0}, // 3
  {1, 0, 0, 1, 1, 0, 0}, // 4
  {0, 1, 0, 0, 1, 0, 0}, // 5
  {0, 1, 0, 0, 0, 0, 0}, // 6
  {0, 0, 0, 1, 1, 1, 1}, // 7
  {0, 0, 0, 0, 0, 0, 0}, // 8
  {0, 0, 0, 0, 1, 0, 0}  // 9
};

// Timekeeping variables
int activeHours = 0;
int activeMinutes = 0;
int activeSeconds = 0;

bool isClockRunning = false;
unsigned long lastTickTime = 0;

// Alternate display view state variables
bool viewSecondsMode = false;
unsigned long secondsViewTimer = 0;

// Hold-to-confirm button monitoring
unsigned long holdStartTime = 0;
bool isHoldActive = false;

// Button state tracking for edge detection
bool lastHrState  = HIGH;
bool lastMinState = HIGH;
bool lastRunState = HIGH;

void setup() {
  for (int idx = 0; idx < 7; idx++) {
    pinMode(segmentPins[idx], OUTPUT);
  }

  for (int idx = 0; idx < 4; idx++) {
    pinMode(selectorPins[idx], OUTPUT);
  }

  pinMode(pinHrBtn, INPUT_PULLUP);
  pinMode(pinMinBtn, INPUT_PULLUP);
  pinMode(pinRunBtn, INPUT_PULLUP);
}

void loop() {
  bool currentHrState  = digitalRead(pinHrBtn);
  bool currentMinState = digitalRead(pinMinBtn);
  bool currentRunState = digitalRead(pinRunBtn);

  // ----------------------------
  // TIME SETTING MODE
  // ----------------------------
  if (!isClockRunning) {
    // Hour Adjustment
    if (lastHrState == HIGH && currentHrState == LOW) {
      activeHours = (activeHours + 1) % 24;
    }

    // Minute Adjustment
    if (lastMinState == HIGH && currentMinState == LOW) {
      activeMinutes = (activeMinutes + 1) % 60;
    }

    // Kickstart Clock
    if (lastRunState == HIGH && currentRunState == LOW) {
      isClockRunning = true;
      lastTickTime = millis();
    }
  }

  // Update edge detection histories
  lastHrState  = currentHrState;
  lastMinState = currentMinState;
  lastRunState = currentRunState;

  // ----------------------------
  // CLOCK RUNNING
  // ----------------------------
  if (isClockRunning) {
    // Core 1-Second Time Interval Engine
    if (millis() - lastTickTime >= 1000) {
      lastTickTime += 1000;
      activeSeconds++;

      if (activeSeconds >= 60) {
        activeSeconds = 0;
        activeMinutes++;

        if (activeMinutes >= 60) {
          activeMinutes = 0;
          activeHours++;
        }

        if (activeHours >= 24) {
          activeHours = 0;
        }
      }
    }

    // Handle Long Press Detection (1.5 seconds) on Run Button
    if (digitalRead(pinRunBtn) == LOW) {
      if (!isHoldActive) {
        holdStartTime = millis();
        isHoldActive = true;
      }

      if (millis() - holdStartTime > 1500) {
        viewSecondsMode = true;
        secondsViewTimer = millis();
      }
    } else {
      isHoldActive = false;
    }

    // Handle Temporary Display Window (5 seconds timeout)
    if (viewSecondsMode) {
      if (millis() - secondsViewTimer > 5000) {
        viewSecondsMode = false;
      }
    }
  }

  renderDisplay();
}

void renderSegmentPattern(byte targetDigit) {
  for (int seg = 0; seg < 7; seg++) {
    digitalWrite(segmentPins[seg], numeralMap[targetDigit][seg]);
  }
}

void renderDisplay() {
  int displayBuffer[4];

  // Route correct numbers to buffer based on current view mode
  if (viewSecondsMode) {
    displayBuffer[0] = activeMinutes / 10;
    displayBuffer[1] = activeMinutes % 10;
    displayBuffer[2] = activeSeconds / 10;
    displayBuffer[3] = activeSeconds % 10;
  } else {
    displayBuffer[0] = activeHours / 10;
    displayBuffer[1] = activeHours % 10;
    displayBuffer[2] = activeMinutes / 10;
    displayBuffer[3] = activeMinutes % 10;
  }

  // Multiplexing loop over the 4 digits
  for (int activePos = 0; activePos < 4; activePos++) {
    // Clear and blank out all digits to prevent ghosting
    for (int clearIdx = 0; clearIdx < 4; clearIdx++) {
      digitalWrite(selectorPins[clearIdx], LOW);
    }

    renderSegmentPattern(displayBuffer[activePos]);

    // Activate the targeted digit cathode/anode pin
    digitalWrite(selectorPins[activePos], HIGH);

    delay(3);
  }
}
