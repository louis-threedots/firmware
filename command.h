#ifndef __COMMAND_LIB_H
#define __COMMAND_LIB_H

#define COMMAND_LENGTH 4

#include <Arduino.h>
#include <AltSoftSerial.h>

enum SerialPort {
  SOFTWARE,
  HARDWARE
};

enum CommandType {
  LED_ON = 97,                // a
  LED_OFF = 98,               // b
  DISCOVERY = 99,             // c
  ACKNOWLEDGE = 100,          // d
  DISCOVERY_COMPLETE = 101,   // e
  MOTOR_FWD = 102,            // f
  MOTOR_REV = 103,            // g
  CHANGE_SPEED = 104,         // h
  BUTTON_PRESS = 105,         // i
  PING = 106,                 // j
  PONG = 107,                 // k
};

class Command {
public:
  int cellNumber;

  byte command[4];

  SerialPort rxPort; //TODO: it was only made public for testing

  CommandType commandType;

  int commandData;

  SerialPort txPort;
  
  Command(SerialPort rxPort);

  Command(int cellNumber, CommandType type, int data, SerialPort txPort);
  
  static void setupSerial();

  static bool available(SerialPort rxPort);

  static int numAvailable(SerialPort p);

  static void flushSerial(SerialPort p);

  bool isValid();

  void send();

  static AltSoftSerial altSerial;

private:
  
  //byte command[4]; //TODO: it was only made public for testing

  bool valid;

  

};

#endif
