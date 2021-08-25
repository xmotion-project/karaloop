// xMotion P1.V1
// 09.12.2019

// Librairies
#include <Arduino.h>
#include <movingAvg.h>
#include <SoftwareSerial.h> 

// Objects
movingAvg avgTemp(5);               // define the moving average object

// Globales variables
int ADC_res = 12;     //ADC resolution, Can be 8, 10, 12 or 14
int A0_output;
int A1_output;
int A2_output;
int A5_output;

// Pins assignment
#define led 19 
#define ADC0 A0 
#define ADC1 A1 
#define ADC2 A2 
#define ADC5 A5 

void setup() {

  Serial.begin(9600);
  InititalisationADC ();
  
  // Set the led in output
  pinMode(19, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  
A0_output = analogRead(ADC0);
A1_output = analogRead(ADC1);
A2_output = analogRead(ADC2);
A5_output = analogRead(ADC5);

Serial.print(A0_output);
Serial.print(";");
Serial.print(A1_output);
Serial.print(";");
Serial.print(A2_output);
Serial.print(";");
Serial.println(A5_output);

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