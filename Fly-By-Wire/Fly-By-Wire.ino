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
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  int potentiometerState = analogRead(potentiometerPin);
  speedKnots = map(potentiometerState, 0, 1023, 0, 250);

  if (lastButtonState == HIGH && buttonState == LOW) {
    //Button was pressed
  }

  lastButtonState = buttonState;
}

