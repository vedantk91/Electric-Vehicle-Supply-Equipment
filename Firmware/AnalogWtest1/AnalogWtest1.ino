void setup() {

 TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz (The DEFAULT)
pinMode(5,OUTPUT);

}

void loop() {

analogWrite(5,51);

}
