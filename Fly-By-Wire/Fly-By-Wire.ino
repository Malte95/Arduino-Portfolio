#include <Servo.h>

Servo gearServo;

const int buttonPin = 2;

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

int speedKnots = 0;

GearState gearState = GEAR_RETRACTED;

void handleGearButtonPress() {

  if (gearState == GEAR_RETRACTED) {

    gearState = GEAR_EXTENDING;
    gearTargetAngle = 180;

  }
  else if (gearState == GEAR_EXTENDED) {

    if (speedKnots > 20) {

      gearState = GEAR_RETRACTING;
      gearTargetAngle = 0;

    }
    else {

      Serial.println("GEAR RETRACT BLOCKED");

    }
  }
}

void updateGearMovement() {

  if (gearState == GEAR_EXTENDING) {

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

void readInputs() {

  buttonState = digitalRead(buttonPin);

  int potentiometerState = analogRead(potentiometerPin);

  speedKnots = map(
    potentiometerState,
    0,
    1023,
    0,
    250
  );
}

void updateLeds() {

  if (gearState == GEAR_RETRACTED) {

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
}

void setup() {

  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

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

  updateGearMovement();

  updateLeds();

  lastButtonState = buttonState;
}

