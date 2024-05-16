//timer interrupts
//https://www.instructables.com/Arduino-Timer-Interrupts/

//storage variables
int toggle0 = 0;



void setup(){

  Serial.begin(9600);
  
  //set pins as outputs
  pinMode(5, OUTPUT);


cli();//stop interrupts

//set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 30;
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);



sei();//allow interrupts

}//end setup

ISR(TIMER0_COMPA_vect){
  if (toggle0==0){
    digitalWrite(5,HIGH);
    toggle0 = 1;
  }
  else{
    digitalWrite(5,LOW);
    toggle0++;
    if(toggle0==4){toggle0=0;}
  }
}




void loop(){
  
  //do other things here
  Serial.println("WORKING");
  Serial.println(millis());
  
}
