#include "nxtservo.h"
#include <PID_v1.h>
#include <Arduino.h>

NXTServo::NXTServo(int MotorFwd, int MotorRev, int MotorEnable) {
  pinMotorFwd = MotorFwd;
  pinMotorRev = MotorRev;
  pinMotorEnable = MotorEnable;
  
  lastEncoded = 0;
  encoderValue = 0;
  
  input = 0;
  output = 0;
  setpoint = 0;
  
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(1);
  myPID.SetOutputLimits(-150, 150);
  
  pinMode(pinMotorFwd, OUTPUT);
  pinMode(pinMotorRev, OUTPUT);
  pinMode(pinMotorEnable, OUTPUT);
  pinMode(NXT_YELLOW_WIRE, INPUT_PULLUP);
  pinMode(NXT_BLUE_WIRE, INPUT_PULLUP);
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  
  TCCR1B = TCCR1B & 0b11111000 | 1;
}

void NXTServo::goTo(int angle) {
  int desiredPulses = map(angle, 0, MAX_ANGLE, 0, PULSES_PER_ROTATION);
  setpoint = desiredPulses;
  unsigned long startMillis = millis();
  while (abs(desiredPulses - encoderValue) > 0 and millis() - startMillis < (MAX_MILLIS_FOR_MOVE-2000)) {
    input = encoderValue;
    myPID.Compute();
    //Serial.println(abs(encoderValue));
    pwmOut(output);
  }
  startMillis = millis();
  while (millis() - startMillis < 2000) {
    input = encoderValue;
    myPID.Compute();
    //Serial.println(abs(encoderValue));
    pwmOut(output);
  }
  finish();
}

void NXTServo::moveRelative(int relAngle) {
  int currentAngle = map(encoderValue, 0, PULSES_PER_ROTATION, 0, MAX_ANGLE);
  int desiredAngle = currentAngle + relAngle;
  goTo(desiredAngle);
}

void NXTServo::changeSpeed(int newSpeed) {
  myPID.SetOutputLimits(-1 * newSpeed, newSpeed);
}

void NXTServo::pwmOut(int out) {                               
  if (out > 0) {
    analogWrite(pinMotorEnable, out);
    forward();
  }
  else {
    analogWrite(pinMotorEnable, abs(out));
    reverse();
  }
}

void NXTServo::updateEncoder() {
  int MSB = digitalRead(NXT_YELLOW_WIRE); //MSB = most significant bit
  int LSB = digitalRead(NXT_BLUE_WIRE); //LSB = least significant bit
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
  lastEncoded = encoded; //store this value for next time
}

void NXTServo::forward() {
  digitalWrite(pinMotorFwd, HIGH);
  digitalWrite(pinMotorRev, LOW);
}

void NXTServo::reverse() {
  digitalWrite(pinMotorFwd, LOW);
  digitalWrite(pinMotorRev, HIGH);
}

void NXTServo::finish() {
  digitalWrite(pinMotorFwd, LOW);
  digitalWrite(pinMotorRev, LOW);
}

volatile int NXTServo::lastEncoded;
volatile long NXTServo::encoderValue;
