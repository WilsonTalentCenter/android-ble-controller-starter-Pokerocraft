String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

const int FL_IN1 = 30; const int FL_IN2 = 28; const int FL_SPD = 8;
const int FR_IN1 = 31; const int FR_IN2 = 29; const int FR_SPD = 9;
const int BL_IN1 = 25; const int BL_IN2 = 23; const int BL_SPD = 11; //25, 23, 11
const int BR_IN1 = 24; const int BR_IN2 = 22; const int BR_SPD = 10; // 24, 22, 10
const int ARM_SPD = 7; const int ARM_DIR = 43; const int ARM_5V = 37; const int ARM_DIR2 = 41; const int ARM_GROUND = 39;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial1.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  int digitalPins[] = {FR_IN1, FR_IN2, FL_IN1, FL_IN2, BR_IN1, BR_IN2, BL_IN1, BL_IN2, ARM_DIR, ARM_GROUND};
  for (int i = 0; i < 9; i++) {
    pinMode(digitalPins[i], OUTPUT);
    digitalWrite(digitalPins[i], LOW);
  }
pinMode(ARM_5V, OUTPUT);
digitalWrite(ARM_5V, HIGH);

  int analogPins[] = {FR_SPD, FL_SPD, BR_SPD, BL_SPD, ARM_SPD};
  for (int i = 0; i < 5; i++){
    pinMode(analogPins[i], OUTPUT);
    analogWrite(analogPins[i], 0);
  }
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    inputString.trim();

    if (inputString.startsWith("J")) {
      parseJoystick(inputString);
    }
    else {
      int command = inputString.toInt();

      switch (command) {
        moveArm(command);
      }
    }
  }
}
/*
Effectively parses the joystick's data in order to drive
*/
void parseJoystick(String data) {
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);

  if (firstComma != -1 && secondComma != -1) {
    int x = data.substring(firstComma + 1, secondComma).toInt();
    int y = data.substring(secondComma + 1).toInt();
    drive(x, y);
  }
}
/*
Does some math (Addition for Left, Subtraction for Right), and then multiplies it by 2 for "Differential" mixing
*/
void drive(int x, int y) {
  int leftSpeed = (y - x) * 2;
  int rightSpeed = (y + x) * 2;

  controlMotor(leftSpeed, FL_IN1, FL_IN2, FL_SPD, true);
  controlMotor(leftSpeed, BL_IN1, BL_IN2, BL_SPD, true);
  controlMotor(rightSpeed, FR_IN1, FR_IN2, FR_SPD, false);
  controlMotor(rightSpeed, BR_IN1, BR_IN2, BR_SPD, false);
}

void moveArm(int direction) {
  switch(direction) {
    case 0:
      digitalWrite(ARM_DIR, HIGH);
      digitalWrite(ARM_DIR2, LOW);
      analogWrite(ARM_SPD, 150);
      break;
    case 2:
      digitalWrite(ARM_DIR, LOW);
      digitalWrite(ARM_DIR2, HIGH);
      analogWrite(ARM_SPD, 150);
      break;
    case 1:
      digitalWrite(ARM_DIR, LOW);
      digitalWrite(ARM_DIR2, HIGH);
      analogWrite(ARM_SPD, 0);
      break;
    case 3: 
    digitalWrite(ARM_DIR, LOW);
    digitalWrite(ARM_DIR2, HIGH);
    analogWrite(ARM_SPD, 0);
    break;
  }
}

/*
Digitally writes IN1 to be high when the speed is above 0, elsewise it's low and IN2 is high, and if the speed is less than 20, we'll use 20 as a deadzone to prevent motor humming.
*/
void controlMotor(int speed, int in1, int in2, int spdPin, bool isFlipped) {
  if (abs(speed) < 20) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(spdPin, 0);
    return;
  }
  speed = constrain(speed, -255, 255);
  if (speed > 0) {
    digitalWrite(in1, isFlipped?HIGH:LOW);
    digitalWrite(in2, isFlipped?LOW:HIGH);
    analogWrite(spdPin, speed);
  } else {
    digitalWrite(in1, isFlipped?LOW:HIGH);
    digitalWrite(in2, isFlipped?HIGH:LOW);
    analogWrite(spdPin, abs(speed));
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent1() {
  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
    inputString += inChar;
    if (inChar == '\n') stringComplete = true;
  }
}
