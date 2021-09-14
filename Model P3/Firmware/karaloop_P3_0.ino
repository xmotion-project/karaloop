// Copyright (C) 2021 University of Geneva

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//    http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Libraries
#include <Wire.h>
#include <MLX90393.h>
#include <stdlib.h>
#include <math.h>
#include <bluefruit.h>
#include <SPI.h>
#include "Bitcraze_PMW3901.h"

// Inputs / Outputs
const int L1 = 11;
const int MagSwitchButton =  30;
const int MouseSwitchButton =  27;
const int ledPin =  17;
const int chipSelectPin = 7;    //Chip Select Pin
const int dataINPin = 14;       //MISO
const int dataOUTPin = 13;      //MOSI
const int serialClockPin = 12;  //CSK
#define Led_Mag_right   A0
#define Led_Mag_left    A1

// Objects
MLX90393 mlx_left, mlx_right;
MLX90393::txyz data_left, data_right;
BLEDis bledis;
BLEHidAdafruit blehid;
Bitcraze_PMW3901 flow(chipSelectPin);

// Global variables
// Buttons
int reading_MouseSwitch;
int reading_MagSwitch;
bool ButtonMagState  = false;
bool ButtonMouseState  = false;
bool oldStatusMag  = false;
bool oldStatusMouse  = false;
unsigned long lastDebounceTimeMag  = 0;
unsigned long lastDebounceTimeMouse  = 0;
unsigned long debounceDelay  = 50;
bool SwitchMouseON = true;
bool SwitchMagON = true;

double current_value_right;
double current_value_left;
int valueStateRight;
int valueStateLeft;
int OldvalueStateRight;
int OldvalueStateLeft;
int current_state_right;
int current_state_left;
int old_state_right;
int old_state_left;
int highpassRight = 0;
int highpassLeft = 0;

boolean DispMag = false;
boolean DispBoth = false;
boolean ChangeSpeed = false;
boolean Callonce = false;
boolean MouseToKeyboard = true;

boolean MouseMove = false;
int MOVE_STEP = 10;
int MoveMax = 5;

float init_mlx_left;
float init_mlx_right;

int16_t deltaX, deltaY;

//Threshold Mag
int Tmin = 2000;
int Tmax = 7000;
int Ttolerance = 1000;
int Toffset = 600;
int Tinit_min;
int Tinit_max;

//Right Threshold
float max_T1_right = 0;
float max_T2_right = 0;

//Left Threshold
float max_T3_left = 0;
float max_T4_left = 0;

void Init_BLE(void);
void Init_mlx(void);
void Init_optical(void);
void OpticalSensor(void);
float ExponentialSpeedMouse(float currentValue, boolean Direction, int speedMax);
void FrontButtons(void);


//--------------------------------------------------------------------------------------//
void setup(void)
{
  DispMag = true;           //DispMag = True --> Activate Magnetometer and Disactivate Optical ; DispMag = false --> Disactivate Magnetometer and Activate Optical
  DispBoth = true;
  Wire.begin();             // join i2c bus (address optional for master)

  Serial.begin(115200);

  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  // set the input & output:
  pinMode(MouseSwitchButton, INPUT_PULLUP);
  pinMode(MagSwitchButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(L1, OUTPUT);
  pinMode(dataINPin, INPUT);
  pinMode(dataOUTPin, OUTPUT);
  pinMode(chipSelectPin, OUTPUT);
  pinMode(serialClockPin, OUTPUT);
  pinMode(Led_Mag_right, OUTPUT);
  pinMode(Led_Mag_left, OUTPUT);


  analogWrite(L1, 100);

  Init_BLE();

  if (DispBoth == true) {
    Init_mlx();
    Init_optical();
  }
  else
  {
    if (DispMag == true) {
      Init_mlx();
    }
    else {
      Init_optical();
    }
  }
}

//--------------------------------------------------------------------------------------//
// Button 1 (UP) : Magnetometer mode
// ON : Adjustement on the central position --> LED ON when magnetometer is inside the
// tolerance (switchMode = true)
// OFF : Visual verficication of the mouse --> LED ON when magnetometer is hiting extremity
// of the guide (switchMode = false)
// Push more than one seconde : change mode between MOUSE and KEYBOARD
//--------------------------------------------------------------------------------------//
void MagSwitch() {

  reading_MagSwitch = digitalRead(MagSwitchButton);
  static boolean mode = false;

  if (reading_MagSwitch != oldStatusMag) {
    lastDebounceTimeMag = millis();
    mode = true;
  }
  if ((millis() - lastDebounceTimeMag) >= debounceDelay) {
    if (reading_MagSwitch != ButtonMagState) {
      ButtonMagState = reading_MagSwitch;
      if (ButtonMagState == false) {
        SwitchMagON = ! SwitchMagON;
      }
    }
  }
  // Change mode between MOUSE to KEYBOARD
  if (((millis() - lastDebounceTimeMag) >= 1000) && reading_MagSwitch == 0 && mode == true) {
    MouseToKeyboard = !MouseToKeyboard;
    //Serial.println("test");
    mode = false;
  }
  oldStatusMag = reading_MagSwitch;
  //Serial.println(reading_MagSwitch);
}

//--------------------------------------------------------------------------------------//
// Button 2 (DOWN) : Mouse ON/OFF
// ON : Activate the mouse function
// OFF : Desactivate the mouse function
//--------------------------------------------------------------------------------------//
void MouseSwitch() {

  reading_MouseSwitch = digitalRead(MouseSwitchButton);

  if (reading_MouseSwitch != oldStatusMouse) {
    lastDebounceTimeMouse = millis();
  }
  if ((millis() - lastDebounceTimeMouse) >= debounceDelay) {
    if (reading_MouseSwitch != ButtonMouseState) {
      ButtonMouseState = reading_MouseSwitch;
      if (ButtonMouseState == false) {
        SwitchMouseON = ! SwitchMouseON;
      }
    }
  }
  oldStatusMouse = reading_MouseSwitch;
}

//--------------------------------------------------------------------------------------//
// Display signals on the serial plotter such as magnetic signals and thresholds
//--------------------------------------------------------------------------------------//
void DisplayMag(void)
{
  Serial.print(current_value_right);
  Serial.print(" ");
  Serial.print(current_value_left);
  Serial.print(" ");
  Serial.print(Tmin);
  Serial.print(" ");
  Serial.print(Tmax);
  Serial.print(" ");
  Serial.print(Tinit_min);
  Serial.print(" ");
  Serial.println(Tinit_max);
}

//--------------------------------------------------------------------------------------//
// Adjustement MODE (switchMode = true) : turn ON LEDs when magnetic signals are inside
// the tolerance range
// Verification MODE : (switchMode = false) : turn ON LEDs when magnetic signals are outside
// of Tmin and Tmax which are thresholds for detection
//--------------------------------------------------------------------------------------//
void Mag_adjustement(boolean switchMode)
{
  boolean switchModeInvers = ! switchMode;

  //Right Adjustement
  if (current_value_right > Tinit_min && current_value_right < Tinit_max) {
    digitalWrite(Led_Mag_right, switchMode);
  } else if (current_value_right < Tmin || current_value_right > Tmax) {
    digitalWrite(Led_Mag_right, switchModeInvers);
  }
  else {
    digitalWrite(Led_Mag_right, 0);
  }

  //Left Adjustement
  if (current_value_left > Tinit_min && current_value_left < Tinit_max) {
    digitalWrite(Led_Mag_left, switchMode);
  } else if (current_value_left < Tmin || current_value_left > Tmax) {
    digitalWrite(Led_Mag_left, switchModeInvers);
  }
  else {
    digitalWrite(Led_Mag_left, 0);
  }
}

//--------------------------------------------------------------------------------------//
// 1st button : Change mode between : adjustement and verification of both magnetometer
// 2nd button : Activate/disactivate mouse function --> 1st time with a lower max speed
// (5) and 2nd time with higer max speed (7.5)
// When both button are push at the same time --> Change mode between mouse and keyboard
//--------------------------------------------------------------------------------------//
void FrontButtons()
{

  // Is a changing state on buttons of the right Hub ?
  MagSwitch();    //-- > 1st button
  MouseSwitch();  //-- > 2nd button

  //Adjustement MODE
  if (SwitchMagON) {
    if (DispMag) {
      Acquisition();
      Mag_adjustement(true);

      //MOUSE MODE ACTIVATE
      if (SwitchMouseON == false) {
        if (ChangeSpeed == 1 && Callonce == 1) {
          MoveMax = 5;
          Callonce = 0;
        }
        if (ChangeSpeed == 0 && Callonce == 1) {
          MoveMax = 7.5;
          Callonce = 0;
        }
        OpticalSensor();
        Decision();
        DisplayMag();
      }

      //MOUSE MODE DISACTIVATE
      else {
        DisplayMag();
        if (Callonce == 0) {
          ChangeSpeed = !ChangeSpeed;
          Callonce = 1;
        }
      }
    }
  }

  //Verification MODE
  else {
    Acquisition();
    Mag_adjustement(false);

    //MOUSE MODE ACTIVATE
    if (SwitchMouseON == false) {
      if (ChangeSpeed == 1 && Callonce == 1) {
        MoveMax = 5;
        Callonce = 0;
      }
      if (ChangeSpeed == 0 && Callonce == 1) {
        MoveMax = 7.5;
        Callonce = 0;
      }
      OpticalSensor();
      Decision();
      //DisplayMag();
    }

    //MOUSE MODE DISACTIVATE
    else {
      //DisplayMag();
      if (Callonce == 0) {
        ChangeSpeed = !ChangeSpeed;
        Callonce = 1;
      }
    }
  }
}

//--------------------------------------------------------------------------------------//
void loop(void)
{
  FrontButtons();

  delay(3);
}
