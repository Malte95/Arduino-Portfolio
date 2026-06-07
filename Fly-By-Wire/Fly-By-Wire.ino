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
const int moveInterval = 100;
unsigned long lastMoveTime = 0;
int buttonState = HIGH;
int lastButtonState = HIGH;
int speedKnots = 0;
GearState gearState = GEAR_RETRACTED;

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  gearServo.attach(servoPin);
  gearServo.write(gearAngle);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  int potentiometerState = analogRead(potentiometerPin);
  speedKnots = map(potentiometerState, 0, 1023, 0, 250);

  if (lastButtonState == HIGH && buttonState == LOW) {
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
  if (gearState == GEAR_EXTENDING) {

    if (millis() - lastMoveTime >= moveInterval) {

        gearAngle += stepSize;
        gearServo.write(gearAngle);

        if (gearAngle >= gearTargetAngle) {

            gearAngle = gearTargetAngle;
            gearServo.write(gearAngle);
            gearState = GEAR_EXTENDED;
        }

        lastMoveTime = millis();
    }
}
  if (gearState == GEAR_RETRACTING) {
    if (millis() - lastMoveTime >= moveInterval) {

      gearAngle -= stepSize;
      gearServo.write(gearAngle);

      if (gearAngle <= gearTargetAngle) {

          gearAngle = gearTargetAngle;
          gearServo.write(gearAngle);
          gearState = GEAR_RETRACTED;
      }

      lastMoveTime = millis();
    }
  }

  void updateLeds() {
    if (gearState == GEAR_RETRACTED) {
        digitalWrite(redLedPin, HIGH);
        digitalWrite(yellowLedPin, LOW);
        digitalWrite(greenLedPin, LOW);
    }
}

  lastButtonState = buttonState;
}

