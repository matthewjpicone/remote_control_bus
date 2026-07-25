/*
RC VOLVO B12B
Master Controller.

Microcontroller: ESP8266 WEMOS R1 D1
Sends commands via IIC.

Author: Matthew Picone
Date: 25/Mar/2021

Description: Controls the Slave Micro Controller, 6 Oled Screens,
1 Relay, and PCA9685 PWM Servo Driver connected to 8 Servos
*/

#include <Wire.h>    //Wire Library
#include "RCVOLVODISPLAY.h" //DESTO DISPLAYS

// ESP WIFI CONFIGURATIONS
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "credentials.h"
#define OTA_HOSTNAME "RC_VOLVO_B12B_BLEA"

//LCU SLAVE ADDRESS & SEND FUNCTION
#define SLAVE_ADDR 0x75
void LCU(int COMMAND) {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write((byte)COMMAND);
  Wire.endTransmission();
}

// PS2 CONTROLLER CONFIGURATIONS
#include <PS2X_lib.h>
#define pressures   false
#define rumble      false
PS2X ps2x;
int error = 0;
byte type = 0;
byte vibrate = 0;
#define PS2_DAT         D12
#define PS2_CMD         D11
#define PS2_SEL         D10
#define PS2_CLK         D13

// PWM SERVO DRIVER CONFIGURATIONS & FUNCTION
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN          150
#define SERVOMAX          600
#define USMIN             600
#define USMAX             2400
#define SERVO_FREQ        60
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  pulselength = 1000000;
  pulselength /= SERVO_FREQ;
  pulselength /= 4096;
  pulse *= 1000000;
  pulse /= pulselength;
  pwm.setPWM(n, 0, pulse);
}
//SERVO MAPPING
int SERVOPOS(int DESIREDPOS) {
  int pulse = map(DESIREDPOS, 0, 180, SERVOMIN, SERVOMAX);
  return pulse;
}

// VEHICLE PARAMETERS
#define FORWARDRELAY          D0  //DRIVETRAIN RELAYS
#define REVERSERELAY          D1

#define NS_TURN_MIN           80  //STEERING MAXIMUM TURN MAP VALUES
#define NS_TURN_MAX         120
#define OS_TURN_MIN           70
#define OS_TURN_MAX         115

#define ODD_DOOR_CLOSE      125 //MAXIMUM DOOR OPEN/CLOSE VALUES
#define ODD_DOOR_OPEN       25
#define EVEN_DOOR_CLOSE     35
#define EVEN_DOOR_OPEN        135

int DOOR_1_POS = ODD_DOOR_OPEN;   //DOOR STATES & INITIAL STARTUP VALUES (ALL OPEN)
int DOOR_2_POS = EVEN_DOOR_OPEN;
int DOOR_3_POS = ODD_DOOR_OPEN;
int DOOR_4_POS = EVEN_DOOR_OPEN;
int DOOR_5_POS = ODD_DOOR_OPEN;
int DOOR_6_POS = EVEN_DOOR_OPEN;
int DOOR_STATE = 1;

// COMMAND ASSIGNMENTS TO LCU
#define LCU_REVERSE           20
#define LCU_REVERSE_OFF       21
#define LCU_WIGWAG_1          22
#define LCU_WIGWAG_2          23
#define LCU_NSIND             24
#define LCU_NSIND_OFF         25
#define LCU_OSIND             26
#define LCU_OSIND_OFF         27
#define LCU_HEADLIGHTS        28
#define LCU_HEADLIGHTS_OFF    29
#define LCU_HIGHBEAM          30
#define LCU_HIGHBEAM_OFF      31
#define PLAYSOUND_1           32  //DOORS CLOSING
#define PLAYSOUND_2           33  //FREEEK
#define PLAYSOUND_3           34  //HORN
#define PLAYSOUND_4           35  //HANDBRAKE
#define PLAYSOUND_5           36
#define PLAYSOUND_6           37
#define PLAYSOUND_7           38
#define PLAYSOUND_8           39
#define PLAYSOUND_9           40
#define PLAYSOUND_10          41
#define PLAYSOUND_11          42
#define PLAYSOUND_12          43
#define PLAYSOUND_13          44
#define PLAYSOUND_14          45
#define PLAYSOUND_15          46
#define PLAYSOUND_16          47
#define PLAYSOUND_17          48
#define PLAYSOUND_18          49
#define PLAYSOUND_19          50
#define PLAYSOUND_20          51
#define LCU_RESET             100 //LCU RESET FUNCTION (TO PREVENT UNWANTED REPETITIONS OF CERTAIN COMMANDS)
int HEADLIGHTS_STATE = false;
int HIGHBEAM_STATE = false;
int NSIND_STATE = false;
int OSIND_STATE = false;
int HAZARD_STATE = false;
int DESTO_STATE = 0;
int TRACK_STATE = 0;
bool otaReady = false;


void setup() {
  Wire.begin();     //BEGIN WIRE LIBRARY
  pinMode(REVERSERELAY, OUTPUT);    //SET RELAY PINS TO OUTPUT
  pinMode(FORWARDRELAY, OUTPUT);

  DESTOSELECT(2);     //INITIALISE DISPLAYS
  BUS2.init();
  DESTOSELECT(3);
  BUS3.init();
  DESTOSELECT(4);
  BUS4.init();
  DESTOSELECT(5);
  BUS5.init();
  DESTOSELECT(6);
  BUS6.init();
  DESTOSELECT(7);
  BUS7.init();

  pwm.begin();      // INITIALISE PWW CONTROLLERS
  pwm.setOscillatorFrequency(27000000);
  delay(50);
  pwm.setPWMFreq(SERVO_FREQ);

  DISPLAYFUNC("WAITING FOR WIFI");  //OTA UPDATER
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  const unsigned long wifiStartedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStartedAt < 10000) {
    delay(250);
  }
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.onStart([]() {
      DISPLAYFUNC("UPLOAD IN PROGRESS");
    });
    ArduinoOTA.onEnd([]() {
      DISPLAYFUNC("UPLOAD END");
    });
    ArduinoOTA.onError([](ota_error_t error) {
      if (error == OTA_AUTH_ERROR) DISPLAYFUNC("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) DISPLAYFUNC("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) DISPLAYFUNC("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) DISPLAYFUNC("Receive Failed");
      else if (error == OTA_END_ERROR) DISPLAYFUNC("End Failed");
    });
    ArduinoOTA.begin();
    otaReady = true;
    DISPLAYFUNC("BOOTING - OTA COMPLETE");
  } else {
    DISPLAYFUNC("NO WIFI - OFFLINE MODE");
  }

  // ATTEMPT CONNECT WITH CONTROLLER
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  if (error == 0) {
    DISPLAYFUNC("Found Controller, configured successful ");
  }    else if (error == 1) {
    DISPLAYFUNC("NO CONTROLLER FOUND - REBOOTING");
    ESP.restart();
  }
  if (error == 2) {
    DISPLAYFUNC("NOT ACCEPTING COMMANDS - REBOOTING");
    ESP.restart();
  }
  else if (error == 3)
    DISPLAYFUNC("ERROR 3 - REBOOTING");
  type = ps2x.readType();
  switch (type) {
    case 0:
      { ESP.restart();
        DISPLAYFUNC("UNKNOWN CONTROLLER FOUND");
      }
      break;
    case 1:
      DISPLAYFUNC("CONTROLLER FOUND - DUALSHOCK");
      break;
    case 2:
      DISPLAYFUNC("GUITAR HERO CONTROLLER FOUND");
      break;
    case 3:
      DISPLAYFUNC("WIRELESS DUALSHOCK CONTROLLER FOUND");
      break;
  }
  delay(50);
  DISPLAYFUNC("  1:18 RC 2009 VOLVO CAD V73 SOFTWARE V7");  //ALL SUCCESS IF DISPLAYED
}
//******************************************************************************************************************************
void loop() {
  if (otaReady) {
    ArduinoOTA.handle();
  }

  if (error == 1)         //CONTROLLER INPUT DETECTION
    return;
  if (type == 2) { //Guitar Hero Controller
   return;
  }
  else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate);
    //CONTROLLER COMMANDS BUTTON PRESSED, BUTTON RELEASED, NEW STATE
    if (ps2x.ButtonPressed(PSB_START)) {        //RESTART ESP
      ESP.restart();
    }
    if (ps2x.ButtonPressed(PSB_SELECT)) {
      LCU(LCU_RESET);
    }
    if (ps2x.ButtonPressed(PSB_PAD_UP)) {
      if (HIGHBEAM_STATE == false){
        HIGHBEAM_STATE = true;
        LCU(LCU_HIGHBEAM);
      }else{
        HIGHBEAM_STATE = false;
        LCU(LCU_HIGHBEAM_OFF);
    }
    }
    if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {
      if (HEADLIGHTS_STATE == false){
        HEADLIGHTS_STATE = true;
        LCU(LCU_HEADLIGHTS);
      }else{
        HEADLIGHTS_STATE = false;
        LCU(LCU_HEADLIGHTS_OFF);
    }
    }
    if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {
      if (DESTO_STATE == 0){
        DESTO_STATE = 1;
        DESTO333();
      }else if (DESTO_STATE == 1){
        DESTO_STATE = 2;
        DESTO32();
      }else{
        DESTO_STATE = 0;
      //  DESTO_SET("373", "CIRCULAR QUAY", "CITY");
      DESTO373();
      
    }
    }
    if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {
      DESTO_FAREWELL();
    }
    if (ps2x.ButtonPressed(PSB_TRIANGLE)) {
      LCU(LCU_RESET);
    }

//    if (ps2x.ButtonPressed(PSB_SQUARE)) {
//      LCU(LCU_RESET);
//    }

    if (ps2x.ButtonPressed(PSB_L1)) {         //DOOR CONTROL
      if (DOOR_STATE == 0) {        //IF DOORS CLOSED OPEN ALL, START WIGWAG, PLAY DOORS CLOSING
        DOOR_STATE = 1;
        pwm.setPWM(2, 0, SERVOPOS(EVEN_DOOR_OPEN));
        pwm.setPWM(3, 0, SERVOPOS(ODD_DOOR_OPEN));
        delay(50);
        pwm.setPWM(4, 0, SERVOPOS(EVEN_DOOR_OPEN));
        pwm.setPWM(5, 0, SERVOPOS(ODD_DOOR_OPEN));
        delay(50);
        pwm.setPWM(6, 0, SERVOPOS(EVEN_DOOR_OPEN));
        pwm.setPWM(7, 0, SERVOPOS(ODD_DOOR_OPEN));
        pwm.setPWM(10, 4096, 0);
        pwm.setPWM(11, 4096, 0);
        pwm.setPWM(12, 4096, 0);
        LCU(LCU_WIGWAG_1);
        LCU(LCU_RESET);
      }
      else if (DOOR_STATE == 1) {       //IF ALL DOORS OPEN CLOSE REAR
        DOOR_STATE = 2;
        LCU(PLAYSOUND_1);
        LCU(LCU_RESET);
        pwm.setPWM(6, 0, SERVOPOS(EVEN_DOOR_CLOSE));
        pwm.setPWM(7, 0, SERVOPOS(ODD_DOOR_CLOSE));
        pwm.setPWM(12, 0, 4096);
      }
      else if (DOOR_STATE == 2) {        //IF FRONT TWO DOORS OPEN CLOSE CENTRE
        pwm.setPWM(4, 0, SERVOPOS(EVEN_DOOR_CLOSE));
        pwm.setPWM(5, 0, SERVOPOS(ODD_DOOR_CLOSE));
        pwm.setPWM(10, 0, 4096);
        DOOR_STATE = 3;
      }
      else if (DOOR_STATE == 3) {        //IF FRONT DOOR OPEN REAR TWO CLOSED, CLOSE FRONT, START WIGWAG OFF TIMER
        DOOR_STATE = 0;
        LCU(LCU_WIGWAG_2);
        LCU(LCU_RESET);
        pwm.setPWM(2, 0, SERVOPOS(EVEN_DOOR_CLOSE));
        pwm.setPWM(3, 0, SERVOPOS(ODD_DOOR_CLOSE));
        pwm.setPWM(11, 0, 4096);
      }
    }
    if (ps2x.ButtonPressed(PSB_L2)) {       //NEARSIDE INDICATOR ON
      if (NSIND_STATE == false){
        NSIND_STATE = true;
        LCU(LCU_NSIND);
      }else{
        NSIND_STATE = false;
        LCU(LCU_NSIND_OFF);
      }
    }
    if (ps2x.ButtonPressed(PSB_L3)) {
      LCU(LCU_RESET);
    }

    if (ps2x.ButtonPressed(PSB_R1)) {
      LCU(PLAYSOUND_3);                     //HORN TRACK
      LCU(LCU_RESET);
    }
    if (ps2x.ButtonPressed(PSB_R2)) {       //OFFSIDE INDICATOR ON
      if (OSIND_STATE == false){
        OSIND_STATE = true;
        LCU(LCU_OSIND);
      }else{
        OSIND_STATE = false;
        LCU(LCU_OSIND_OFF);
      }
    }
    if (ps2x.ButtonPressed(PSB_R3)) {
      LCU(LCU_RESET);
    }
if (ps2x.ButtonPressed(PSB_L2) && ps2x.ButtonPressed(PSB_R2)) {       //HAZARD LIGHTS
      if (HAZARD_STATE == false){
        HAZARD_STATE = true;
        LCU(LCU_NSIND);
        LCU(LCU_OSIND);
      }else{
        HAZARD_STATE = false;
        LCU(LCU_NSIND_OFF);
        LCU(LCU_OSIND_OFF);
      } }
      
    //STEERING CONTROL
    pwm.setPWM(0, 0, SERVOPOS((map(ps2x.Analog(PSS_LX), 0, 255, NS_TURN_MAX, NS_TURN_MIN))));
    pwm.setPWM(1, 0, SERVOPOS((map(ps2x.Analog(PSS_LX), 0, 255, OS_TURN_MAX, OS_TURN_MIN))));
    delay(50);

    //FORWARD/BACKWARD CONTROL
    if (ps2x.ButtonPressed(PSB_CROSS)) {
      if (DOOR_STATE == 0) {
        digitalWrite(FORWARDRELAY, HIGH);
        digitalWrite(REVERSERELAY, LOW);
      } else {
        LCU(PLAYSOUND_4);
        LCU(LCU_NSIND);
        LCU(LCU_OSIND);
        HAZARD_STATE = true;
      }
    }
    else if (ps2x.ButtonPressed(PSB_CIRCLE)) {
      if (DOOR_STATE == 0) {
        digitalWrite(REVERSERELAY, HIGH);
        digitalWrite(FORWARDRELAY, LOW);
        LCU(LCU_NSIND);
        LCU(LCU_OSIND);
        HAZARD_STATE = true;
        LCU(LCU_REVERSE);
        
      } else {
        LCU(PLAYSOUND_4);
        HAZARD_STATE = true;
        LCU(LCU_NSIND);
        LCU(LCU_OSIND);
        
      }
    } else if (ps2x.ButtonReleased(PSB_CIRCLE) || ps2x.ButtonReleased(PSB_CROSS)) { 
      digitalWrite(FORWARDRELAY, LOW);
      digitalWrite(REVERSERELAY, LOW);
      LCU(LCU_REVERSE_OFF);
      if (HAZARD_STATE == true) {
        HAZARD_STATE = false;
        LCU(LCU_NSIND_OFF);
        LCU(LCU_OSIND_OFF);
      }
    }
  }
}
