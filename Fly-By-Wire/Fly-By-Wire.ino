#include <Servo.h>

Servo gearServo;

const int buttonPin = 2;
const int resetButtonPin = 3;

const int redLedPin = 4;
const int yellowLedPin = 6;
const int greenLedPin = 8;

const int servoPin = 11;
const int potentiometerPin = A2;

enum GearState {
  GEAR_RETRACTED,
  GEAR_EXTENDING,
  GEAR_EXTENDED,
  GEAR_RETRACTING,
  GEAR_FAULT
};

int gearAngle = 0;
int gearTargetAngle = 0;

const int stepSize = 3;
const unsigned long moveInterval = 100;

unsigned long lastMoveTime = 0;

int buttonState = HIGH;
int lastButtonState = HIGH;
int resetButtonState = HIGH;
int lastResetButtonState = HIGH;

int speedKnots = 0;
int lastSpeedKnots = 0;

const unsigned long maxGearMoveTime = 8000;
unsigned long gearMoveStartTime = 0;

const unsigned long faultBlinkInterval = 500;
unsigned long lastFaultBlinkTime = 0;
bool faultLedOn = false;

GearState gearState = GEAR_RETRACTED;

bool retractBlockedWarning = false;
unsigned long warningStartTime = 0;
const unsigned long warningDuration = 2000;
const unsigned long warningBlinkInterval = 250;
unsigned long lastWarningBlinkTime = 0;
bool warningLedOn = false;

void handleGearButtonPress() {

  if (gearState == GEAR_RETRACTED) {

    gearState = GEAR_EXTENDING;
    gearTargetAngle = 180;
    gearMoveStartTime = millis();

  }
  else if (gearState == GEAR_EXTENDED) {

    if (speedKnots > 20) {

      gearState = GEAR_RETRACTING;
      gearTargetAngle = 0;
      gearMoveStartTime = millis();

    }
    else {

      Serial.println("GEAR RETRACT BLOCKED");

      retractBlockedWarning = true;
      warningStartTime = millis();
      lastWarningBlinkTime = millis();

    }
  }
}

void resetGearButtonPress() {
  if (gearState == GEAR_FAULT) {

    gearTargetAngle = 180;
    gearState = GEAR_EXTENDING;
    gearMoveStartTime = millis();

    faultLedOn = false;

    Serial.println("FAULT RESET: EXTENDING GEAR");
  }
}

void enterFaultState(String message) {
  gearState = GEAR_FAULT;
  Serial.println(message);
}

void updateGearMovement() {

  if (gearState == GEAR_EXTENDING) {

    if (millis() - gearMoveStartTime > maxGearMoveTime) {
      enterFaultState("GEAR FAULT: MOVEMENT TIMEOUT");
      return;
    }

    if (millis() - lastMoveTime >= moveInterval) {

      gearAngle += stepSize;
      gearServo.write(gearAngle);

      Serial.println(gearAngle);

      if (gearAngle >= gearTargetAngle) {

        gearAngle = gearTargetAngle;
        gearServo.write(gearAngle);

        gearState = GEAR_EXTENDED;

        Serial.println("GEAR EXTENDED");
      }

      lastMoveTime = millis();
    }
  }

  else if (gearState == GEAR_RETRACTING) {

    if (millis() - gearMoveStartTime > maxGearMoveTime) {
      enterFaultState("GEAR FAULT: MOVEMENT TIMEOUT");
      return;
    }

    if (millis() - lastMoveTime >= moveInterval) {

      gearAngle -= stepSize;
      gearServo.write(gearAngle);

      Serial.println(gearAngle);

      if (gearAngle <= gearTargetAngle) {

        gearAngle = gearTargetAngle;
        gearServo.write(gearAngle);

        gearState = GEAR_RETRACTED;

        Serial.println("GEAR RETRACTED");
      }

      lastMoveTime = millis();
    }
  }
}

void updateAirspeed() {
  int potentiometerState = analogRead(potentiometerPin);

  speedKnots = map(potentiometerState, 0, 1023, 0, 250);

  if (speedKnots != lastSpeedKnots) {
    Serial.println(speedKnots);
    lastSpeedKnots = speedKnots;
  }
}

void readInputs() {

  buttonState = digitalRead(buttonPin);
  resetButtonState = digitalRead(resetButtonPin);
  updateAirspeed();
}

void updateLeds() {

  if (retractBlockedWarning) {

    if (millis() - warningStartTime >= warningDuration) {
      retractBlockedWarning = false;
    }

    if (millis() - lastWarningBlinkTime >= warningBlinkInterval) {
      warningLedOn = !warningLedOn;
      lastWarningBlinkTime = millis();
    }

    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);

    if(warningLedOn) {
      digitalWrite(yellowLedPin, HIGH);
    }
    else {
      digitalWrite(yellowLedPin, LOW);
    }

  }

  else if (gearState == GEAR_RETRACTED) {

    digitalWrite(redLedPin, HIGH);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, LOW);

  }
  else if (gearState == GEAR_EXTENDED) {

    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);

  }
  else if (
    gearState == GEAR_EXTENDING ||
    gearState == GEAR_RETRACTING
  ) {

    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);

  }
else if (gearState == GEAR_FAULT) {
  if (millis() - lastFaultBlinkTime >= faultBlinkInterval) {
    faultLedOn = !faultLedOn;
    lastFaultBlinkTime = millis();
  }

  if (faultLedOn) {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(yellowLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
  }
  else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
  }
}
}

void setup() {

  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);

  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  gearServo.attach(servoPin);
  gearServo.write(gearAngle);

  updateLeds();
}

void loop() {

  readInputs();

  if (lastButtonState == HIGH &&
      buttonState == LOW) {

    handleGearButtonPress();
  }

  if (lastResetButtonState == HIGH &&
       resetButtonState == LOW) {
        resetGearButtonPress();
       }

  updateGearMovement();

  updateLeds();

  lastButtonState = buttonState;
  lastResetButtonState = resetButtonState;
}

