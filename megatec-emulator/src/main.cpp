/*

  Megatec Protocol Emulator (ng)

*/
#include <Arduino.h>
#include "settings.h"

void setup()
{
  MegatecSerial.begin(SERBAUD);
  pinMode(PIN_BEEP, OUTPUT);
}

void loop()
{

  if (MegatecSerial.available() > 0) {
    inputString = MegatecSerial.readStringUntil('\r');
    inputString.toUpperCase();
    stringComplete = true;
  }

  if (millis() > timeTick + 1000) {
    timeTick = millis();
    timer = timer + 1;
    heartBeat();
  } else if (timeTick > millis()) {
    timeTick = 0;
  }

  if (stringComplete) {
    protocolParser();
    inputString = "";
    stringComplete = false;
  }

}

void protocolParser()
{

  char cmd = inputString.charAt(0);
  int cmdLen = inputString.length();

  switch (cmd) {
    case 'D':                           /* Status Inquiry *disabled */
      MegatecSerial.print("@\r");
      break;
    case 'Q':
      if (cmdLen == 1) {                /* Turn beep On/Off */
        StatusBit0 = !StatusBit0;
        digitalWrite(PIN_BEEP, StatusBit0);
        invalidCommand = false;
      } else if (cmdLen == 2) {         /* Status Inquiry */
        if (inputString.equals("Q1")) {
          StatusDataStream();
          invalidCommand = false;
        }
      } else  {
        invalidCommand = true;
      }
      break;

    case 'T':                           /* Unity Tests */
      if (cmdLen == 1) {                /* 10 Seconds Test */
        StatusBit2 = true;
        tenSecondsTest = true;
      } else {
        if (cmdLen == 2) {              /* Test until Battery Low: Not implemented */
          if (inputString.charAt(1) == 'L') {
            MegatecSerial.print("@\r");
          }
        }
        if (cmdLen == 3) {              /* Test for Specified Time Period */
          if (inputString.charAt(1) == 'L') {
            invalidCommand = true;
            MegatecSerial.print("@\r");
          }
        }
      }
      break;

    
    case 'S':                           /* Shut Down Command */
      if (cmdLen == 1) {                /* TODO - Not implemented */
        MegatecSerial.print("@\r");
        StatusBit1 = true;
      } else {
        if (cmdLen >= 2) {              /* TODO - Not implemented */
          MegatecSerial.print("@\r");
          StatusBit2 = false;
        }
      }
      break;

    case 'C':                           /* Cancel Shut Down Command */
      if (cmdLen == 1) {                /* Cancel the SN<n><cr> and SN<n>R<m><cr> command. */
        StatusBit1 = false;
      } else {
        if (cmdLen == 2) {              /* Cancel all test activity */
          if (inputString.charAt(1) == 'T') {
            StatusBit2 = false;
          }
        }
      }
      break;

    case 'I': /* UPS Information Command */
      if (cmdLen == 1) {
        MegatecSerial.print("#" + INFO_COMPANY + " " + INFO_MODEL + " " + INFO_VERSION + "\r");
      } else {
        invalidCommand = true;
      }
      break;

    case 'F':                           /* UPS Rating Information */
      if (cmdLen == 1) {                /* Format: #MMM.M QQQ SS.SS RR.R */
        String strDataStream = "#";
        strDataStream += String(RatingVoltage) + " ";
        strDataStream += String(RatingCurrent) + " ";
        strDataStream += String(BatteryVoltage) + " ";
        strDataStream += String(UPSFrequency) + " ";
        MegatecSerial.print(strDataStream);
        MegatecSerial.print("\r");
      } else {
        invalidCommand = true;
      }
      break;

    default:                            /* Flag invalid command */
      invalidCommand = true;
      break;
  }

/*

  If the UPS receives any command that it could not handle,
  the UPS should echo the received command back to the computer.
  The host should check if the command send to UPS been echo or not.

*/

  if (invalidCommand) {
    MegatecSerial.print(inputString);
    MegatecSerial.print("\r");
  }

  invalidCommand = false;

}

void StatusDataStream()
{
  /* Format: (MMM.M NNN.N PPP.P QQQ RR.R S.SS TT.T  b7b6b5b4b3b2b1b0 */
  String strDataStream = "(";
  String stringStatusByte = String(StatusBit7);
  stringStatusByte += String(StatusBit6);
  stringStatusByte += String(StatusBit5);
  stringStatusByte += String(StatusBit4);
  stringStatusByte += String(StatusBit3);
  stringStatusByte += String(StatusBit2);
  stringStatusByte += String(StatusBit1);
  stringStatusByte += String(StatusBit0);

  strDataStream += String(inputVoltage, 1) + " ";
  strDataStream += String(faultVoltage, 1) + " ";
  strDataStream += String(outputVoltage, 1) + " ";
  strDataStream += String("025") + " ";
  strDataStream += String(frequencyHz, 1) + " ";

  if (!StatusBit3)
    strDataStream += String(batteryVoltage, 2) + " ";
  else
    strDataStream += String(batteryVoltage, 1) + " ";

  strDataStream += String(temperature, 1) + " ";
  strDataStream += String(stringStatusByte);

  MegatecSerial.print(strDataStream);
  MegatecSerial.print("\r");

}

void heartBeat()
{
  inputVoltage = map(analogRead(ACINPUT_PIN), 0, 1023, AC_MIN, AC_MAX);
  outputVoltage = map(analogRead(ACOUTPUT_PIN), 0, 1023, AC_MIN, AC_MAX);
  batteryVoltage = map(analogRead(DCVOLT_PIN), 0, 1023, 10, 15);

  if (batteryVoltage < BATTERY_MIN)
    StatusBit6 = true;
  else
    StatusBit6 = false;

  faultVoltage = inputVoltage - random(18, 20);
  frequencyHz = random(580.0, 620.0) / 10;
  loadPercent = random(100, 1000) / 10;
  temperature = random(180, 250) / 10;

/*
    TODO LIST

  StatusBit7 - Utility Fail (Immediate)
  StatusBit5 - Bypass/Boost or Buck Active
  StatusBit4 - UPS Failed
  StatusBit3 - UPS Type is Standby (0 is On_line)
  StatusBit1 - Shutdown Active

*/

  if (StatusBit2 && tenSecondsTest) {   /* Test in Progress */
    tenSecondsTestCounter++;
    StatusBit0 = true;
    if (tenSecondsTestCounter == 10) {
      StatusBit0 = false;
      StatusBit2 = false;
      tenSecondsTest = false;
      tenSecondsTestCounter = 0;
    }
  } else {
    tenSecondsTestCounter = 0;
  }

  if (StatusBit0) {                     /* Beeper On */
    beepActive ^= 1;
    digitalWrite(PIN_BEEP, beepActive);
  }

}
