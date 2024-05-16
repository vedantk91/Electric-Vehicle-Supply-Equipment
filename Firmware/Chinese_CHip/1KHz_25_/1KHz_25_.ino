int voltage=0;
int counterval=0;
unsigned long voltagetotal = 0;
unsigned long voltageavg = 0;
int volt9_flag=1;
int volt6_flag=1;
#define voltage_read A2
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(5, OUTPUT);
  pinMode(A2,INPUT);
  Serial.begin(9600);
  Serial.println("BC");

}

// the loop function runs over and over again forever
void loop() {

analogWrite(5,63.75);
Serial.println(analogRead(voltage_read));

for (int i = 0; i < 100; i++)
   {
  voltage = analogRead(voltage_read);
        if (voltage > 100)
        {
          voltagetotal += voltage;
          counterval++;
        }
    voltageavg = voltagetotal / counterval;
    voltage = voltageavg;
    Serial.print("Counter val: "); Serial.print(counterval);
    Serial.print("/100\tVoltage val: "); Serial.println(voltage);
    

////read
//
//
//
//
// for (int i = 0; i < 100; i++)
//    {
//      voltage = analogRead(voltage_read);
//      if (volt9_flag == 1)
//      {
//        if (voltage > 100)
//        {
//          voltagetotal += voltage;
//          counterval++;
//        }
//      }
//      else if (volt6_flag == 1)
//      {
//        if (voltage > 100)
//        {
//          voltagetotal += voltage;
//          counterval++;
//        }
//      }
//      else
//      {
//        if (voltage > 50)
//        {
//          voltagetotal += voltage;
//          counterval++;
//        }
//      }
//    }
//      
//    voltageavg = voltagetotal / counterval;
//    voltage = voltageavg;
//    Serial.print("Counter val: "); Serial.print(counterval);
//    Serial.print("/100\tVoltage val: "); Serial.println(voltage);
//    

}
counterval=0;
voltageavg=0;
voltagetotal=0;}
