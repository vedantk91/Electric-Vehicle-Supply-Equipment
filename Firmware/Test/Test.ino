#define duty_cycle 63
#define pwm_1k 5
#define voltage_read A2

void setup() {
  pinMode(pwm_1k, OUTPUT);
  pinMode(voltage_read, INPUT);
  analogWrite(pwm_1k, duty_cycle);

}

void loop() {

//  int voltage = analogRead(voltage_read);
//  //ADCSRA = (ADCSRA & 0xf8) | 0x04; // set 16 times division
//  Serial.println(ADCSRA);

}
