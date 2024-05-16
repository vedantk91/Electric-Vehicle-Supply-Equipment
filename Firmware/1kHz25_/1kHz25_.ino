int pwmpin=5;
int pot=A5;
void setup()
{ // change pwm frequency of pwm pin 5 and 6 .....from 1 kHz to 62.5 kHz

   TCCR0B = TCCR0B & B11111000 | B00000010;


//  change pwm frequency of pwm pin 9 and 10 .....from 490 Hz to 32kHz
//  TCCR1B = TCCR1B & B11111000 | B00000001;

//  change pwm frequency of pwm pin 3 and 11 .....from 490 Hz to 32 kHz
//  TCCR2B = TCCR2B & B11111000 | B00000001;

 
  pinMode(pwmpin,OUTPUT);
  
   
}
void loop()
{
  int pwmvalue=63;
  analogWrite(pwmpin,pwmvalue);
}
