// Copyright (C) 2021 University of Geneva

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License, Version 3, as 
// published by the Free Software Foundation.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Librairies
#include <bluefruit.h>
#include <movingAvg.h>

// Objects
movingAvg avgTemp(5);               // define the moving average object

// Globales variables
int ADC_res = 12;     //ADC resolution, Can be 8, 10, 12 or 14
int A0_output;
int A1_output;
int A2_output;
int ThresholdA0 = 3000;
int ThresholdA1 = 3000;
int ThresholdA2 = 3000;

// Pins assignment 
#define ADC0 A0 
#define ADC1 A1 
#define ADC2 A2 

// Switch
const int LED_PIN = 17;
const int BUTTON_PIN= 12;
bool blinkState = false;
int lastButtonState=LOW;
int currentButtonState=LOW;
boolean outputOn=false;
unsigned long lastButtonClickTime=0;

// HID
BLEDis bledis;
BLEHidAdafruit blehid;

// Report Keystrokes
char keyUp = 'q'; // See https://www.arduino.cc/en/Reference/ASCIIchart
char keyDown = 'a';
char keySelect = 'z';

void setup() {

  Serial.begin(9600);
  InititalisationADC ();
  
  // Set the led in output
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT); 
  
  Bluefruit.begin();
  // HID Device can have a min connection interval of 9*1.25 = 11.25 ms
  // Bluefruit.Periph.setConnInterval(9, 16); // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("xMotion");

  // Configure and Start Device Information Service
  bledis.setManufacturer("xMotion Project");
  bledis.setModel("xMotion P1");
  bledis.begin();

  // BLE HID
  blehid.begin();

  // Set up and start advertising
  startAdv();  
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for 'Name' in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

// the loop function runs over and over again forever
void loop() {
  
  A0_output = analogRead(ADC0);
  A1_output = analogRead(ADC1);
  A2_output = analogRead(ADC2);
  
  Serial.print(A0_output);
  Serial.print(";");
  Serial.print(A1_output);
  Serial.print(";");
  Serial.println(A2_output);

  // TURNING HID FUNCTION ON AND OFF. WHEN OFF, STABLE POSITION WILL BE RESET
  currentButtonState = digitalRead(BUTTON_PIN);
  if ((currentButtonState!=lastButtonState && ((millis()-lastButtonClickTime)>100)) ){
    if(currentButtonState==LOW ){
      outputOn=!outputOn;
      lastButtonClickTime=millis();
      if(outputOn){
        digitalWrite(LED_PIN, HIGH);   // TURN LED ON (HIGH is the voltage level)
      } 
      else {
        digitalWrite(LED_PIN, LOW);   // TURN LED ON (HIGH is the voltage level)
      }
    }
    lastButtonState=currentButtonState;
  }
  
  if(outputOn){  
     
    KeyPress();
    
  }
  
}

// --------------------------------------------------------------------------
// --------- Initialisation ADC ---------------------------------------------
// --------------------------------------------------------------------------
void InititalisationADC ()
{
// Set the analog reference to 3.0V (default = 3.6V)
analogReference(AR_INTERNAL_3_0);

// Set the resolution to 12-bit (0..4095)
analogReadResolution(ADC_res); 
}

// --------------------------------------------------------------------------
// --------- Acquisition et conversion des capteurs -------------------------
// --------------------------------------------------------------------------
int sensors_ADC (int AnalogueInput)
{
  int val;
  float Aref = 3.313;   // Aref for conversion to Analogue
  float Uout;           // Ouput force sensor (V) 
  float Uout_AVG;       // Ouput force sensor - mean
  float Fout;           // Ouput force sensor (N)
  
  val = analogRead(AnalogueInput);        // Analogue acquisition 
  Uout_AVG = avgTemp.reading(val);        // mobile mean
  
  return Uout_AVG;  
}

void KeyPress ()
{      
    if (A0_output > ThresholdA0)
    {  
      blehid.keyPress(keyUp);
      delay(5);
      blehid.keyRelease();
      delay(500);
      Serial.println(keyUp);     
    }
      
    if (A1_output > ThresholdA1)
    {
      blehid.keyPress(keyDown);
      delay(5);
      blehid.keyRelease();
      delay(500);
      Serial.println(keyDown); 
    }
      
    if (A2_output > ThresholdA2)
    {
      blehid.keyPress(keySelect);
      delay(5);
      blehid.keyRelease();
      delay(500);
      Serial.println(keySelect); 
    } 
}

  
