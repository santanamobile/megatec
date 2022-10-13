#define SERBAUD       2400
#define MegatecSerial Serial

#define ACINPUT_PIN   A0
#define ACOUTPUT_PIN  A1
#define DCVOLT_PIN    A2

#define PIN_BEEP      13

#define AC_MIN 90
#define AC_MAX 235

#define BATTERY_MAX   13
#define BATTERY_MIN   11

/*
  
  UPS Information Data
  
  Company_Name 15 characters, leave space if less than 15 characters
  UPS_Model 10 characters, leave space if less than 10 characters
  Version 10 characters, leave space if less than 10 characters
 
*/
const String INFO_COMPANY = "Megatec        ";
const String INFO_MODEL   = "Emulator  ";
const String INFO_VERSION = "v1,1      ";

/*

  UPS Rating Information
  
  Rating Voltage : MMM.M
  Rating Current : QQQ
  Battery Voltage: SS.SS or SSS.S
  Frequency      : RR.R

*/
const float RatingVoltage  = 13.2;
const float RatingCurrent  = 3.5;
const float BatteryVoltage = 12.5;
const float UPSFrequency   = 0.0;

bool invalidCommand = false;

byte StatusBit7 = 0,
     StatusBit6 = 0,
     StatusBit5 = 0,
     StatusBit4 = 0,
     StatusBit3 = 0,
     StatusBit2 = 0,
     StatusBit1 = 0,
     StatusBit0 = 0;

int beepActive = 0;

unsigned long timeTick;
int timer = 0;

String inputString = "";
boolean stringComplete = false;
boolean tenSecondsTest = false;
int tenSecondsTestCounter = 0;

double inputVoltage;
double faultVoltage;
double outputVoltage;
double batteryVoltage;
double frequencyHz;
double temperature;
int loadPercent;

void protocolParser();
void StatusDataStream();
void heartBeat();
