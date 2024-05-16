int analogpin = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(analogpin, OUTPUT);

   OCR0A = 0x09;

    //Compare B
    OCR0B = 0x09;

    //Count
    TCNT0 = 0x00;


    //TSM disabled; PSRSYNC disabled; 
    GTCCR = 0x00;

    //COMA 0; COMB 0; WGM 0; 
    TCCR0A = 0x00;

    //FOCA disabled; FOCB disabled; WGM disabled; CS VAL_0x02; 
    TCCR0B = 0x02;

    //OCIEB disabled; OCIEA disabled; TOIE disabled; 
    TIMSK0 = 0x00;
    
  analogWrite(analogpin, 128);

}

void loop() {
  // put your main code here, to run repeatedly:

}
