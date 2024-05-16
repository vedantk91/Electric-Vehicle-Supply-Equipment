void setup() {
  //pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22);
  //OCR2A = 180;    // commentthis for 5
  OCR0B = 50;    // for pin 5 OC0B
}

void loop() {}
