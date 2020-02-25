#include "command.h"
#include <Arduino.h>

AltSoftSerial Command::altSerial;

Command::Command(SerialPort receivePort) {
  rxPort = receivePort;
  valid = true;
  if (rxPort == SOFTWARE) {
    txPort = HARDWARE;
    if (altSerial.available() < COMMAND_LENGTH) {
      valid = false;
    }
    else {
      for (int i=0; i < COMMAND_LENGTH; i++) {
        command[i] = altSerial.read();
      }
    }
  }
  else {
    txPort = SOFTWARE;
    if (Serial.available() < COMMAND_LENGTH) {
      valid = false;
    }
    else {
      for (int i=0; i < COMMAND_LENGTH; i++) {
        command[i] = Serial.read();
      }
    }
  }
  cellNumber = (int) command[0];
  commandType = (CommandType) command[1];
  commandData = ((int) command[3]) + 256*((int) command[2]);
}

Command::Command(int number, CommandType type, int data, SerialPort tx) {
  cellNumber = number;
  commandType = type;
  commandData = data;
  txPort = tx;
  command[0] = cellNumber;
  command[1] = (byte) commandType;
  command[2] = (byte) (data / 256);
  command[3] = (byte) (data % 256);
}

void Command::setupSerial() {
  Serial.begin(9600);
  altSerial.begin(9600);
}

bool Command::available(SerialPort rxPort) {
  if (rxPort == HARDWARE) return Serial.available() >= COMMAND_LENGTH;
  else return altSerial.available() >= COMMAND_LENGTH;
}

bool Command::isValid() {
  return valid;
}

void Command::send() {
  //if (txPort == SOFTWARE) Serial.println("Writing to SOFTWARE");
  //else Serial.println("Writing to HARDWARE");
  command[0] = cellNumber;
  command[1] = (byte) commandType;
  command[2] = (byte) (commandData / 256);
  command[3] = (byte) (commandData % 256);
  for (int i = 0; i < COMMAND_LENGTH; i++) {
    if (txPort == SOFTWARE) altSerial.write(command[i]);
    else Serial.write(command[i]);
  }
}

int Command::numAvailable(SerialPort p) {
  if (p == HARDWARE) return Serial.available();
  else return altSerial.available();
}

void Command::flushSerial(SerialPort p) {
  if (p == HARDWARE) while (Serial.available()) { Serial.read(); }
  else while (altSerial.available()) { altSerial.read(); }
}
