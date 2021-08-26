unsigned long startTime = 0;
unsigned long endTime= 0;
unsigned long latency = 0;

float voltage1=0;
float voltage2=0;
float voltage3=0;


void setup() {
  // put your setup code here, to run once:
pinMode(A0, OUTPUT);
pinMode(A1, OUTPUT);
pinMode(A2, OUTPUT);

Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

voltage1=analogRead(14);
Serial.print("V1=");
Serial.print(voltage1*5/1.024);
Serial.print(",   ");
delay(100);

voltage2=analogRead(15);
Serial.print("V2=");
Serial.print(voltage2*5/1.024);
Serial.print(",   ");
delay(100);

voltage3=analogRead(16);
Serial.print("V3=");
Serial.println(voltage3*5/1.024);

delay(100);

//*5/1.024

//if (voltage > 100)
//{
//  startTime = 0;
//  startTime = millis();
//  
//  }
//else if (voltage<50)
//{
//
//latency = millis() - startTime;
//Serial.println(latency);
//
// }

//la valeur en [mV] du canal 3 est
}
