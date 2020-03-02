#include "nxtservo.h"
#include "command.h"
//#include <AltSoftSerial.h>

#define INITIAL_CELL_NUMBER 97
#define MAX_ACKNOWLEDGEMENT_TIME 1000
#define INCLUDE_LOG 0

#define BUTTON_PIN 5
#define BUTTON_DEBOUNCE_TIME 50

NXTServo m = NXTServo(4,7,6);

//AltSoftSerial altSerial;

int thisCell = INITIAL_CELL_NUMBER;

void setup() {
  Command::setupSerial();
  pinMode(13, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {

//  while(Serial.available()> 1)
//  {
//    int val  = Serial.parseInt();
//    m.goTo(val);
//  }
  
  if (Command::available(HARDWARE)) {
    logln("newHW");
    Command hardwareCommand = Command(HARDWARE);
    processNewCommand(hardwareCommand);
  }
  else if (Command::available(SOFTWARE)) {
    logln("newSW");
    Command softwareCommand = Command(SOFTWARE);
    processNewCommand(softwareCommand);
  }
  else if (digitalRead(BUTTON_PIN) == LOW) {
    delay(BUTTON_DEBOUNCE_TIME);
    if (digitalRead(BUTTON_PIN) == LOW) {
      Command buttonPressCmd = Command(0, BUTTON_PRESS, thisCell, HARDWARE);
      buttonPressCmd.send();
      while (digitalRead(BUTTON_PIN) == LOW) delay(10);
    }
  }
  delay(10);
}

void processNewCommand(Command c) {
  if (c.cellNumber == thisCell || c.cellNumber == INITIAL_CELL_NUMBER) {
    handleCommand(c);
  }
  else {
    if (c.cellNumber == 255) {
      handleCommand(c);
    }
    log("Passing command: ");
    logint((int)c.commandType);
    log(", destination port: ");
    logint((int)c.txPort);
    log(", sender port: ");
    logint((int)c.rxPort);
    logln("");
    c.send(); // forward command to next cell in chain
  }
}

int newPIDp = KP;
int newPIDi = KI;
int newPIDd = KD;

void handleCommand(Command c) {
  log("Cmd type: ");
  logint((int)c.commandType);
  logln("");
  Command pong = Command(c.commandData, PONG, thisCell, HARDWARE); // putting the line here works
  switch (c.commandType) {
    case PING:
      logln("PINGU");
      //Command pong = Command(c.commandData, PONG, thisCell, HARDWARE); // putting it here doesn't
      pong.send();
      break;
    case LED_ON:
      logln("LEDON");
      digitalWrite(13, HIGH);
      break;
    case LED_OFF:
      logln("LEDOFF");
      digitalWrite(13, LOW);
      break;
    case MOTOR_FWD:
      logln("MOTORFWD");
      m.moveRelative(c.commandData);
      break;
    case MOTOR_REV:
      logln("MOTORREV");
      m.moveRelative(c.commandData * -1);
      break;
    case CHANGE_SPEED:
      logln("CHANGE_SPEED");
      m.changeSpeed(c.commandData);
      break;
    case DISCOVERY:
      logln("DISCO");
      thisCell = c.commandData % 256;
      Command discoveryAcknowledge = Command(thisCell - 1, ACKNOWLEDGE, 0, HARDWARE);
      discoveryAcknowledge.send();
      c.commandData ++;
      Command::flushSerial(SOFTWARE);
      Command::flushSerial(HARDWARE);
      c.send(); // forward discovery down the chain

      logln("Reached loop...");
      unsigned long startTime = millis();
      while ( (! Command::available(SOFTWARE)) && millis() - startTime < MAX_ACKNOWLEDGEMENT_TIME) delay(5);
      logln("Passed loop...");
      log("Command is available:");
      logint((int)Command::available(SOFTWARE));
      logln("");

      /*while (1) {
        if (Command::altSerial.available()) {
          byte b = Command::altSerial.read();
          Serial.write(b);
          delay(10);
        }
      }*/

      /*Command cc = Command(SOFTWARE);
      log("New command:");
      //logint(Command::altSerial.read());
      logint((int)cc.command[0]);
      log(",");
      //logint(Command::altSerial.read());
      logint((int)cc.command[1]);
      log(",");
      //logint(Command::altSerial.read());
      logint((int)cc.command[2]);
      log(",");
      //logint(Command::altSerial.read());
      logint((int)cc.command[3]);
      logln("");
      log("Still available bytes: ");
      logint(Command::altSerial.available());
      logln("");*/
      
      
      if (! Command::available(SOFTWARE)) {
        logln("no ack");
        // no acknowledgement - must be the end of the line
        endOfTheLine();
      }
      else {
        Command hwCommand = Command(SOFTWARE);
        if (hwCommand.commandType == ACKNOWLEDGE) {
          // success! don't need to do anything else
          logln("sleeping now...");
        }
        else {
          log("wrong cmd: ");
          logint((int)hwCommand.commandType);
          logln("");
          //endOfTheLine();
        }
      }
      break;
    default:
      log("unknown cmd: ");
      logint((int)c.commandType);
      logln("");
  }
}

void endOfTheLine() {
  logln("end of the line...");
  Command discoveryTotal = Command(0, DISCOVERY_COMPLETE, thisCell, HARDWARE);
  discoveryTotal.send();
}

void log(String s) {
  if (INCLUDE_LOG) Serial.print(s);
}

void logint(int i) {
  if (INCLUDE_LOG) Serial.print(i);
}

void logln(String s) {
  if (INCLUDE_LOG) Serial.println(s);
}
