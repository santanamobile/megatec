/*
   Megatec Protocol Simulator
*/

#define SERBAUD       2400

#define ACINPUT_PIN   A0
#define ACOUTPUT_PIN  A1
#define DCVOLT_PIN    A2
#define PIN_BEEP      13
#define BATTERY_LOW   11

/*
  UPS Information Data

  Company_Name 15 characters, leave space if less than 15 characters
  UPS_Model 10 characters, leave space if less than 10 characters
  Version 10 characters, leave space if less than 10 characters
*/

#define INFO_COMPANY "Megatec        "
#define INFO_MODEL   "Simulator "
#define INFO_VERSION "v1.0.0    "

/*
  UPS Rating Information

  Rating Voltage : MMM.M
  Rating Current : QQQ
  Battery Voltage: SS.SS or SSS.S
  Frequency      : RR.R
*/

#define RATING_VOLTAGE 13.2
#define RATING_CURRENT  3.5
#define BATTERY_VOLTAGE 12.5
#define UPS_FREQUENCY   60.0

byte StatusBit7 = 0, // {1: Battery operation, 0: mains operation}
     StatusBit6 = 0, // {1: Low battery level, 0: battery normal}
     StatusBit5 = 0, // {1: Bypass enabled, 0: bypass disabled (online UPS only)}
     StatusBit4 = 0, // {1: UPS alarm, 0: UPS ok}
     StatusBit3 = 0, // {1: Line-interactive UPS, 0: online-UPS}
     StatusBit2 = 0, // {1: UPS test started}
     StatusBit1 = 0, // {1: UPS is off, 0: UPS is on}
     StatusBit0 = 0; // {1: Beep on, 0: off}

int beepActive = 0;

unsigned long timeTick;
int timer = 0;

String inputString = "";
boolean tenSecondsTest = false;
int tenSecondsTestCounter = 0;

double inputVoltage;
double faultVoltage;
double outputVoltage;
double batteryVoltage;
double frequencyHz;
double temperature;
int loadPercent;

void setup()
{
  Serial.begin(SERBAUD);
  pinMode(PIN_BEEP, OUTPUT);
}

void loop()
{

  boolean stringComplete = false;

  if (Serial.available() > 0) {
    inputString = Serial.readStringUntil('\r');
    inputString.toUpperCase();
    stringComplete = true;
    while (Serial.available())
      Serial.read();
  }

  if (millis() > timeTick + 1000) { // Heartbeat timer
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
  boolean invalidCommand = false;

  switch (cmd) {
    case 'D':                                         // Status Inquiry - Old protocol (disabled)
      invalidCommand = true;
      break;
    case 'Q':
      if (cmdLen == 1) {                            // Turn beep On/Off
        StatusBit0 = !StatusBit0;
        digitalWrite(PIN_BEEP, StatusBit0);
        invalidCommand = false;
      } else if (cmdLen == 2) {
        if (inputString.equals("Q1")) {             // Status Inquiry
          StatusDataStream();
          invalidCommand = false;
        }
      } else  {
        invalidCommand = true;
      }
      break;

    case 'T': // Unity Tests
      if (cmdLen == 1) { // 10 Seconds Test
        StatusBit2 = true;
        tenSecondsTest = true;
      } else {
        if (cmdLen == 2) {                          // Test until Battery Low
          if (inputString.charAt(1) == 'L') {       // TODO - Not implemented
          }
        }
        if (cmdLen == 3) {                          // Test for Specified Time Period
          if (inputString.charAt(1) == 'L') {
            invalidCommand = true;
          }
        }
      }
      break;

    case 'S':                                       // Shut Down Command
      if (cmdLen == 1) {                            // TODO - Not implemented
        StatusBit1 = true;
      } else {
        if (cmdLen >= 2) {                          // TODO - Not implemented
          StatusBit2 = false;
        }
      }
      break;

    case 'C':                                       // Cancel Shutdown Command
      if (cmdLen == 1) {
        StatusBit1 = false;
      } else {
        if (cmdLen == 2) {
          if (inputString.charAt(1) == 'T') {       // Cancel all test activity
            StatusBit2 = false;
          }
        }
      }
      break;

    case 'I':                                       // UPS Information Command
      if (cmdLen == 1) {
        Serial.print("#");
        Serial.print(INFO_COMPANY);
        Serial.print(" ");
        Serial.print(INFO_MODEL);
        Serial.print(" ");
        Serial.print(INFO_VERSION);
        Serial.print(" \r");
      } else {
        invalidCommand = true;
      }
      break;

    case 'F':                                       // UPS Rating Information
      if (cmdLen == 1) {
        Serial.print("#");
        Serial.print(RATING_VOLTAGE);
        Serial.print(" ");
        Serial.print(RATING_CURRENT);
        Serial.print(" ");
        Serial.print(BATTERY_VOLTAGE);
        Serial.print(" ");
        Serial.print(UPS_FREQUENCY);
        Serial.print("\r");
      } else {
        invalidCommand = true;
      }
      break;

    default:                                        // Set command as invalid if not recognized
      invalidCommand = true;
      break;
  }

  /*
    If the UPS receives any command that it could not handle,
    the UPS should echo the received command back to the computer.
    The host should check if the command send to UPS been echo or not.

  */

  if (invalidCommand) {
    Serial.print(inputString);
    Serial.print("\r");
  }

  invalidCommand = false;

}

void StatusDataStream()
{
  Serial.print("(");
  Serial.print(inputVoltage);
  Serial.print(" ");
  Serial.print(faultVoltage, 1);
  Serial.print(" ");
  Serial.print(outputVoltage, 1);
  Serial.print(" ");

  if (loadPercent < 100)
    Serial.print("0");
  Serial.print(loadPercent);

  Serial.print(" ");
  Serial.print(frequencyHz, 1);
  Serial.print(" ");

  if (!StatusBit3)
    Serial.print(batteryVoltage, 2);
  else
    Serial.print(batteryVoltage, 1);

  Serial.print(" ");
  Serial.print(temperature, 1);
  Serial.print(" ");
  Serial.print(StatusBit7);
  Serial.print(StatusBit6);
  Serial.print(StatusBit5);
  Serial.print(StatusBit4);
  Serial.print(StatusBit3);
  Serial.print(StatusBit2);
  Serial.print(StatusBit1);
  Serial.print(StatusBit0);
  Serial.print("\r");

}

void heartBeat()
{
  /* Update Values */
  inputVoltage = map(analogRead(ACINPUT_PIN), 0, 1023, 90, 235);
  outputVoltage = map(analogRead(ACOUTPUT_PIN), 0, 1023, 90, 235);
  batteryVoltage = map(analogRead(DCVOLT_PIN), 0, 1023, 10, 14);
  /*
    inputVoltage = 127;
    outputVoltage = 127;
    batteryVoltage = 13;
  */
  if (batteryVoltage <= BATTERY_LOW)
    StatusBit6 = true;
  else
    StatusBit6 = false;

  faultVoltage = inputVoltage - random(18, 20);
  frequencyHz = random(580.0, 620.0) / 10;
  loadPercent = random(100, 1000) / 10;
  temperature = random(180, 250) / 10;

  /*
    TODO

    StatusBit7 - Utility Fail (Immediate)
    StatusBit5 - Bypass/Boost or Buck Active
    StatusBit4 - UPS Failed
    StatusBit3 - UPS Type is Standby (0 is On_line)
    StatusBit1 - Shutdown Active

  */

  if (StatusBit2 && tenSecondsTest) { // Test in Progress
    tenSecondsTestCounter++;
    StatusBit0 = true;
    if (tenSecondsTestCounter == 10) { // 10 seconds test
      StatusBit0 = false;
      StatusBit2 = false;
      tenSecondsTest = false;
      tenSecondsTestCounter = 0;
    }
  } else {
    tenSecondsTestCounter = 0;
  }

  if (StatusBit0) { // Change buzzer state
    beepActive ^= 1;
    digitalWrite(PIN_BEEP, beepActive);
  }

}
