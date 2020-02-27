
#ifndef __NXTSERVO_LIB_H
#define __NXTSERVO_LIB_H

#include <PID_v1.h>

#define KP 40
#define KI 0
#define KD 4

#define NXT_YELLOW_WIRE 2
#define NXT_BLUE_WIRE 3

#define MAX_ANGLE 360
#define PULSES_PER_ROTATION 720

#define MAX_MILLIS_FOR_MOVE 7000

class NXTServo{
public:
  NXTServo(int MotorFwd, int MotorRev, int MotorEnable);

  void goTo(int angle);

  void moveRelative(int relAngle);

  void changeSpeed(int newSpeed);

  PID myPID = PID(&input, &output, &setpoint, KP, KI, KD, DIRECT);

private:
  int pinMotorFwd;
  int pinMotorRev;
  int pinMotorEnable;
  static volatile int lastEncoded;
  static volatile long encoderValue;
  double input;
  double output;
  double setpoint;
  
  void pwmOut(int out);
  
  static void updateEncoder();
  
  void forward();
  
  void reverse();
  
  void finish();

};

#endif
