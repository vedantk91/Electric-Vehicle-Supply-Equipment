int Number_of_Samples=1;
double offsetI,sampleI,Irms,sumI,sqI,filteredI;
void setup() {
Serial.begin(9600);
#define CalibConstant 60.1

}

void loop() 

{
// value1=analogRead(A3);
// voltage= (value1*5)/1023;
// voltage = CalibConstant / voltage; 

 offsetI = 1024>>1;

   for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleI = analogRead(18);
    Serial.print("Analog read value");
    Serial.println(sampleI);

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset,
    //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI-offsetI)/1024);
    filteredI = sampleI - offsetI;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum
    sumI += sqI;
  }

  double I_RATIO = CalibConstant *((5/1000.0) / (1024));
  Irms = I_RATIO * sqrt(sumI / Number_of_Samples);
 Serial.print("IRMS  ");
 Serial.println(Irms);
 delay(1000);
 digitalWrite(2,HIGH);
 delay(500);
  digitalWrite(2,LOW);
delay(500);
}
