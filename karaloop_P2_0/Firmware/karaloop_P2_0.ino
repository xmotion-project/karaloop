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

// Librairies
#include <bluefruit.h>
#define MOVE_STEP 10

// Defining input
const int Wheel_left = A1;
const int Wheel_right = A0;
const int switch_left = 15;
const int switch_right = 16;
const int ledPin_left =  17;
const int ledPin_right =  19;

// Global variables
const int numReadings = 10;
int readingsA0[numReadings];      // the readings from the analog input
int readingsA1[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int IndexOffset = 0;
int totalA0 = 0;                  // the running total
int totalA1 = 0;                  // the running total
int averageA0 = 0;                // the average
int averageA1 = 0;                // the average

float EMA_a = 0;    //initialization of EMA alpha
int EMA_A0 = 0;
int EMA_A1 = 0;

int highpass0 = 0;
int highpass1 = 0;
int highpass0_cut = 0;
int highpass1_cut = 0;
int Tlim = 0;

unsigned int  MemTimeSerial;
unsigned int  tempTime = 0;

int VThigh = 0;
int VTlow = 0;
int HPthreshold = 0;
int Th_EMAhigh = 0;
int Th_EMAlow = 0;

float periode_pos_right = 0.0;
int periode_neg_right = 0;
int wheel_dir_right = 0;
boolean zero_right = true;
int OuptutWheel_right = 0;
int OuptutWheel_right_old = 0;

int periode_pos_left = 0;
int periode_neg_left = 0;
int wheel_dir_left = 0;
boolean zero_left = true;
int OuptutWheel_left = 0;
int OuptutWheel_left_old = 0;

int periode_value_right = 1;
int periode_value_left = 1;

int ValidState_right = 0;
int ValidState_left = 0;

boolean state = false;
boolean stateDelay = false;
unsigned int  MemTimeDelay;
unsigned int  delayTime = 0;

int switchState_left = 0;
int switchState_right = 0;

int counter = 0;

// Objects
BLEDis bledis;
BLEHidAdafruit blehid;

void setup() {
  Serial.begin(115200);
  Init();
}

// ----------------------------------------------------------------------------------------
// Init Bluefruit
// ----------------------------------------------------------------------------------------
void Init()
{
  Bluefruit.begin();
  // HID Device can have a min connection interval of 9*1.25 = 11.25 ms
  Bluefruit.Periph.setConnInterval(9, 16); // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("xMotionP2");

  // Configure and Start Device Information Service
  bledis.setManufacturer("xMotion");
  bledis.setModel("xMotion P2");
  bledis.begin();

  // BLE HID
  blehid.begin();

  // Set up and start advertising
  startAdv();

  // Initialize all the readings to 0:
  for (int thisReadingA0 = 0; thisReadingA0 < numReadings; thisReadingA0++) {
    readingsA0[thisReadingA0] = 0;
  }
  for (int thisReadingA1 = 0; thisReadingA1 < numReadings; thisReadingA1++) {
    readingsA0[thisReadingA1] = 0;
  }

  //Set input/output
  pinMode(ledPin_left, OUTPUT);
  pinMode(ledPin_right, OUTPUT);
  pinMode(switch_left, INPUT);
  pinMode(switch_right, INPUT);
}

// ----------------------------------------------------------------------------------------
// BT Advertising
// ----------------------------------------------------------------------------------------
void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_MOUSE);

  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for 'Name' in the advertising packet
  Bluefruit.Advertising.addName();

  /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

// ----------------------------------------------------------------------------------------
// Smooth potentiometers readings
// ----------------------------------------------------------------------------------------

void Potentiometers_Smooth()
{

  // Subtract the last reading:
  totalA0 = totalA0 - readingsA0[readIndex];
  totalA1 = totalA1 - readingsA1[readIndex];

  // Read from the sensor:
  readingsA0[readIndex] = analogRead(Wheel_left);
  readingsA1[readIndex] = analogRead(Wheel_right);

  // Add the reading to the total:
  totalA0 = totalA0 + readingsA0[readIndex];
  totalA1 = totalA1 + readingsA1[readIndex];

  // Advance to the next position in the array:
  readIndex = readIndex + 1;

  // If we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // Calculate the average and remove the offset to center around 0:
  averageA0 = (totalA0 / numReadings);
  averageA1 = (totalA1 / numReadings);

}
// ----------------------------------------------------------------------------------------
// Potentiomerters HP filter
// ----------------------------------------------------------------------------------------
void Potentiometers_HPfilter()
{

  // Coefficient to modifiy the hp filter
  EMA_a = 0.3;

  EMA_A0 = (EMA_a * averageA0) + ((1 - EMA_a) * EMA_A0); //run the EMA
  EMA_A1 = (EMA_a * averageA1) + ((1 - EMA_a) * EMA_A1); //run the EMA

  highpass0 = averageA0 - EMA_A0;                   //calculate the high-pass signal
  highpass1 = averageA1 - EMA_A1;                   //calculate the high-pass signal

  // Removing noise around 0 when doing no movement
  Tlim = 4;

  if (highpass0 < Tlim && highpass0 > -Tlim ) {
    highpass0_cut = 0;
  }
  else {
    highpass0_cut = highpass0;
  }
  if (highpass1 < Tlim && highpass1 > -Tlim ) {
    highpass1_cut = 0;
  }
  else {
    highpass1_cut = highpass1;
  }
}

// ----------------------------------------------------------------------------------------
// Detecting wheel movements
// ----------------------------------------------------------------------------------------
void Potentiometers_Detection()
{
  HPthreshold = Tlim;

  // Thresholds for detecting wheel movements
  VThigh = Tlim + 1 ;
  VTlow = -VThigh;

  // No detection Right
  if ((highpass0_cut < VThigh) && (ValidState_right == 0) && (highpass0_cut > VTlow) && (zero_right == true)) {
    periode_pos_right = 0 ;
    periode_neg_right = 0 ;
    OuptutWheel_right = 0;
    ValidState_right = 1;
  }

  // No detection Left
  if ((highpass1_cut < VThigh) && (ValidState_left == 0) && (highpass1_cut > VTlow) && (zero_left == true)) {
    periode_pos_left = 0 ;
    periode_neg_left = 0 ;
    OuptutWheel_left = 0;
    ValidState_left = 1;
  }

  // Positive direction Right
  if ((highpass0_cut > VThigh) && (ValidState_right == 1) && (zero_right == true) && (periode_neg_right == 0)) {
    periode_pos_right = periode_value_right ;
    wheel_dir_right = 1;
    OuptutWheel_right = periode_pos_right * wheel_dir_right;
    zero_right = false;
  }

  if ((highpass0_cut < VThigh) && (ValidState_right == 1) && (highpass0_cut > VTlow) && (periode_pos_right == periode_value_right) && (periode_neg_right == 0)) {
    OuptutWheel_right = periode_pos_right * wheel_dir_right;
//    OuputWheel_right = 0;
    zero_right = false;
  }

  if ((highpass0_cut < VTlow) && (ValidState_right == 1) && (periode_pos_right == periode_value_right) && (periode_neg_right == 0)) {
    OuptutWheel_right = 0;
    zero_right = true;
    ValidState_right = 0;
  }

  // Negative direction Right
  if ((highpass0_cut < VTlow) && (ValidState_right == 1) && (zero_right == true) && (periode_pos_right == 0)) {
    periode_neg_right = periode_value_right ;
    wheel_dir_right = -1;
    OuptutWheel_right = periode_neg_right * wheel_dir_right;
    zero_right = false;
  }

  if ((highpass0_cut < VThigh) && (ValidState_right == 1) && (highpass0_cut > VTlow) && (periode_neg_right == periode_value_right) && (periode_pos_right == 0)) {
    OuptutWheel_right = periode_neg_right * wheel_dir_right;
//    OuptutWheel_right = 0;
    zero_right = false;
  }

  if ((highpass0_cut > VThigh) && (ValidState_right == 1) && (periode_neg_right == periode_value_right) && (periode_pos_right == 0)) {
//        OuptutWheel_right = periode_neg_right * wheel_dir_right;
    OuptutWheel_right = 0;
    zero_right = true;
    ValidState_right = 0;
  }

  // Positive direction Left
  if ((highpass1_cut > VThigh) && (ValidState_left == 1) && (zero_left == true) && (periode_neg_left == 0)) {
    periode_pos_left = periode_value_left ;
    wheel_dir_left = 1;
    OuptutWheel_left = periode_pos_left * wheel_dir_left;
    zero_left = false;
  }

  if ((highpass1_cut < VThigh) && (ValidState_left == 1) && (highpass1_cut > VTlow) && (periode_pos_left == periode_value_left) && (periode_neg_left == 0)) {
    OuptutWheel_left = periode_pos_left * wheel_dir_left;
//    OuptutWheel_left = 0;
    zero_left = false;
  }

  if ((highpass1_cut < VTlow) && (ValidState_left == 1) && (periode_pos_left == periode_value_left) && (periode_neg_left == 0)) {
//    OuptutWheel_left = periode_pos_left * wheel_dir_left;
    OuptutWheel_left = 0;
    zero_left = true;
    ValidState_left = 0;
  }

  // Negative direction Left
  if ((highpass1_cut < VTlow) && (ValidState_left == 1) && (zero_left == true) && (periode_pos_left == 0)) {
    periode_neg_left = periode_value_left;
    wheel_dir_left = -1;
    OuptutWheel_left = periode_neg_left * wheel_dir_left;
    zero_left = false;
  }

  if ((highpass1_cut < VThigh) && (ValidState_left == 1) && (highpass1_cut > VTlow) && (periode_neg_left == periode_value_left) && (periode_pos_left == 0)) {
    OuptutWheel_left = periode_neg_left * wheel_dir_left;
//    OuptutWheel_left = 0;
    zero_left = false;
  }

  if ((highpass1_cut > VThigh) && (ValidState_left == 1) && (periode_neg_left == periode_value_left) && (periode_pos_left == 0)) {
//    OuptutWheel_left = periode_neg_left * wheel_dir_left;
    OuptutWheel_left = 0;
    zero_left = true;
    ValidState_left = 0;
  }
}

// ----------------------------------------------------------------------------------------
// Mapping potentiometers to mouse translational function
// ----------------------------------------------------------------------------------------
void Potentiometers_classification()
{
//  int delayValue = 50;
//  delayTime = millis();
//
//  //Changing state detection
//  if (OuptutWheel_left != OuptutWheel_left_old || OuptutWheel_right != OuptutWheel_right_old)
//  {
//    state = true;
//    stateDelay = false;
//    MemTimeDelay = delayTime;
//  }
//
//  // Waiting a delay if both movements are not synchronized
//  if ((delayTime - MemTimeDelay > delayValue) && state == true)
//  {
//    state = false;
//    stateDelay = true;
//  }

  // Lower Lips right --> Mouse X+
  if (OuptutWheel_left == 1 && OuptutWheel_right == 0)// && stateDelay == true)
  {
      blehid.mouseMove(-MOVE_STEP, 0);
  }

  // Lower Lips left  --> Mouse X-
  if (OuptutWheel_left == 0 && OuptutWheel_right == -1)// && stateDelay == true)
  {
    blehid.mouseMove(MOVE_STEP, 0);
  }

  // Pursing Lips     --> Mouse Y+
  if (OuptutWheel_left == 1 && OuptutWheel_right == -1)// && stateDelay == true)
  {
    blehid.mouseMove(0, MOVE_STEP);
  }

  // Compression Lips --> Mouse Y-
  if (OuptutWheel_left == -1 && OuptutWheel_right == 1)// && stateDelay == true)
  {
    blehid.mouseMove(0, -MOVE_STEP);
  }

}

// ----------------------------------------------------------------------------------------
// Mapping switches to mouse wheel function
// ----------------------------------------------------------------------------------------
void Switches()
{
  switchState_left = digitalRead(switch_left);
  switchState_right = digitalRead(switch_right);

  if (switchState_left == LOW) {
    digitalWrite(ledPin_left, HIGH);
    //    blehid.mouseScroll(-MOVE_STEP);
  }

  if (switchState_left == HIGH) {
    digitalWrite(ledPin_left, LOW);
    blehid.mouseScroll(0);
  }

  if (switchState_right == LOW) {
    digitalWrite(ledPin_right, HIGH);
    //    blehid.mouseScroll(MOVE_STEP);
  }

  if (switchState_right == HIGH) {
    digitalWrite(ledPin_right, LOW);
    blehid.mouseScroll(0);
  }
}

// ----------------------------------------------------------------------------------------
// Main loop
// ----------------------------------------------------------------------------------------
void loop() {

  tempTime = millis();

  if ((tempTime - MemTimeSerial) > 20)
  {
    MemTimeSerial = tempTime;

    OuptutWheel_right_old = OuptutWheel_right;
    OuptutWheel_left_old = OuptutWheel_left;

    Potentiometers_Smooth();
    Potentiometers_HPfilter();
    Potentiometers_Detection();
    Potentiometers_classification();
    Switches();

    // Show signals

//    Serial.print(OuptutWheel_left);
//    Serial.print("\t");
//    Serial.print(OuptutWheel_right);
//    Serial.println("\t");
//    Serial.print(counter);
//    Serial.print("\t");
//    Serial.println(displacement);
//    Serial.print("\t");
//    Serial.println(averageA1);
//    Serial.print("\t");
//    Serial.println(EMA_A0);
//    Serial.print("\t");
//    Serial.println(EMA_A1);
//    Serial.print(highpass0);
//    Serial.print("\t");
//    Serial.println(highpass1);
//    Serial.print("\t");
//    Serial.print(highpass1_cut);
//    Serial.print("\t");
//    Serial.println(highpass0_cut);

    delay(3);        // delay in between reads for stability
  }
}
