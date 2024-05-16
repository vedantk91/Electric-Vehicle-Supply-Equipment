//12/07/2022 stable and working on Rev 2

#include <EmonLib.h>

byte cMCUSR;
#include<avr/wdt.h>

int voltage;
int switch_flag = 0;
int duty_flag = 0;

unsigned long prevmillis;
unsigned long currentmillis;
unsigned long custommillis = 0;
unsigned long chargeduration = 15000;

#define adc_12v_min 650
#define adc_12v_max 900
#define adc_9v_min 351
#define adc_9v_max 600
#define adc_6v_min 250
#define adc_6v_max 350
#define adc_3v_min 100
#define adc_3v_max 230

int32_t frequency = 1000;//hz
#define duty_cycle 63.75  //   (10% 25.5)||(16% 40.8)||(25% 63.75)||(30% 76.5)||(40% 102)||(50% 127.5)||(100% 255) ,if above 800us or 80% duty cycle rule changes

unsigned long voltagetotal = 0;
unsigned long voltageavg = 0;
int counterval = 0;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);


uint8_t      volt6_flag = false;
uint8_t      volt9_flag = false;
uint8_t      volt12_flag = false;
uint8_t count = 0;
uint8_t Irms;

#define start_pin 10
uint8_t start_flag = false; // MAKE IT FALSE FOR DEVELOPMENT
uint8_t green_flag=false;

#define led_blue 8       //Dickpak
#define led_green 7
#define led_red 6

#define pwm_1k 5
#define voltage_read A2

#define relay_live 3
#define relay_neutral 2
#define current_ip_pin A3
EnergyMonitor emon1;
String temp;
int toggle0;

void start_detect()                          //Switch ON button
{
  if (start_flag == false)
  {
    if (   digitalRead(start_pin) == HIGH)
    {
      _delay_ms(500);
      if (  digitalRead(start_pin) == HIGH)
      {
        _delay_ms(1000);
        if (  digitalRead(start_pin) == LOW) {
          TIMSK0 &= ~(1 << OCIE0A);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Starting...");
          _delay_ms(2000);
          Serial.println(F("buttonpresss instance"));
          start_flag = true;
          analogWrite(pwm_1k, 255);
          switch_flag = 1;
        }
        else if (digitalRead(start_pin) == HIGH) {
          Serial.println(F("Mode change"));
          if (duty_flag == 0) {
            duty_flag = 1;
            Serial.println(F("Changed to 32A")  );
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Changed to 32A");
          }
          else if (duty_flag == 1) {
            duty_flag = 0;
            Serial.println(F("Changed to 25% duty cycle"));
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Changed to 16A");
          }
        }
      }
    }
  }
}
void setup() {
  cMCUSR = MCUSR;
  MCUSR = 0;
  wdt_disable();
  Serial.begin(9600);
  pinMode(start_pin , INPUT);
  pinMode(led_blue , OUTPUT);
  pinMode(led_green , OUTPUT);
  pinMode(led_red , OUTPUT);
  pinMode(relay_live, OUTPUT);
  pinMode(relay_neutral, OUTPUT);
  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);
  // emon1.current(current_ip_pin, 17);
  emon1.current(current_ip_pin, 5.45); // For 5111
 // emon1.current(current_ip_pin, 5); // For 5014
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  _delay_ms(500);
  digitalWrite(13, LOW);
  Serial.println(F("X`"));
  if (cMCUSR & (1 << PORF ))
    Serial.println(F("Power-on reset.\n"));
  if (cMCUSR & (1 << EXTRF))
    Serial.println(F("External reset!\n"));
  if (cMCUSR & (1 << BORF ))
    Serial.println(F("Brownout reset!\n"));
  if (cMCUSR & (1 << WDRF )){
    Serial.println(F("Watchdog reset!\n"));
    //start_flag=true;
  }
  Wire.begin();
  Wire.beginTransmission(0x27);
  uint8_t err = Wire.endTransmission();
  Serial.println(err);
  if (err == 0)
  {
    Serial.println(F("LCD Ready"));
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EVSE Ready");
  }
  else
  {
    Serial.println(F("LCDfault "));
  }
  wdt_enable(WDTO_8S);
  cli();//stop interrupts
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
  TIMSK0 &= ~(1 << OCIE0A);        //need to check this
}

ISR(TIMER0_COMPA_vect) {
  if (duty_flag == 0)
  {
    if (toggle0 == 0) {
      digitalWrite(5, HIGH);
      //PORTD|=_BV(5);
      toggle0 = 1;
    }
    else {
      digitalWrite(5, LOW);
      //PORTD = (~(1 << 5) & PORTD);
      //PORTB ~=
      toggle0++;
      if (toggle0 == 4) {
        toggle0 = 0;
        custommillis++;
      }
    }
  }

  else if (duty_flag == 1) {
    if (toggle0 <= 2) {
      digitalWrite(5, HIGH);
      toggle0++;
      if (toggle0 == 2) {
        toggle0 = 3;
      }
    }
    else {
      digitalWrite(5, LOW);
      toggle0++;
      if (toggle0 == 5) {
        toggle0 = 0;
         custommillis++;
      }
    }
  }
}
void loop()
{

   digitalWrite(13, HIGH);
  _delay_ms(50);
  digitalWrite(13, LOW);
  _delay_ms(50);
  wdt_reset();
  stop_detect();
  if (start_flag == true)
  {
    voltagetotal = 0;
    voltageavg = 0;
    counterval = 0;
    for (int i = 0; i < 100; i++)
    {
      voltage = analogRead(voltage_read);
      if (volt9_flag == 1)
      {
        if (voltage > 100)
        {
          voltagetotal += voltage;
          counterval++;
        }
      }
      else if (volt6_flag == 1)
      {
        if (voltage > 100)
        {
          voltagetotal += voltage;
          counterval++;
        }
      }
      else
      {
        if (voltage > 50)
        {
          voltagetotal += voltage;
          counterval++;
        }
      }
    }
    wdt_reset();
    voltageavg = voltagetotal / counterval;
    voltage = voltageavg;
    Serial.print(F("Counter val: ")); Serial.print(counterval);
    Serial.print(F("/100\tVoltage val: ")); Serial.println(voltage);

    Serial.print(volt12_flag); Serial.print(volt9_flag); Serial.println(volt6_flag);
    if (voltage > 100) {
      if ((adc_12v_min < voltage) && (voltage < adc_12v_max)) //12v
      {
        TIMSK0 &= ~(1 << OCIE0A);
        digitalWrite(pwm_1k, HIGH);
        digitalWrite(relay_live, LOW);
        digitalWrite(relay_neutral, LOW);
        digitalWrite(led_blue, HIGH);
        digitalWrite(led_red, LOW);
        digitalWrite(led_green, HIGH);
        Serial.println(F("on standby i.e no car connected "));
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("Standby");
        lcd.setCursor(0, 1);
        lcd.print("No Car Connected");
        _delay_ms(200);
        volt12_flag = true;
        volt9_flag = false;
        volt6_flag = false;
      }
      else if ((adc_9v_min < voltage) && (voltage < adc_9v_max )) //9v
      {
        volt9_flag = true;
        if (volt12_flag == true && volt9_flag == true && volt6_flag==false)          //check if it was 12V first
        {
          TIMSK0 |= (1 << OCIE0A);
          digitalWrite(relay_live, LOW);
          digitalWrite(relay_neutral, LOW);
          Serial.println(F("vehicle detected ..sending 1khz signal "));
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Vehicle Detected");
          lcd.setCursor(1, 1);
          lcd.print("Sending 1KHz");
          Serial.println(voltage);
          analogWrite(pwm_1k, duty_cycle);
          READY_BLINK();
          digitalWrite(13, HIGH);
          volt9_flag = true;
          volt12_flag = false;
          volt6_flag = false;
        }
        else
        {
          if (volt6_flag == true)                   //wasnt 12V first directly 9V
          {
            stop_now();                            //HE UNCOMMENT KARAYCHA NA
          }
          Serial.println(F("In 9V Loop"));
        }
      }
      else if ((adc_6v_min < voltage) && (voltage < adc_6v_max))//6v
      {
         TIMSK0 |= (1 << OCIE0A);
       
        Serial.println(F("In charging loop"));
        Serial.println(custommillis);
        wdt_reset();
        Irms = emon1.calcIrms(100); // Calculate Irms only  100 or 1480
        Irms=sqrt(Irms);
        Serial.println(custommillis);
        Serial.println(F("Calculating"));      
        wdt_reset();
        Serial.print("Power: "); Serial.print(Irms * 230.0); Serial.print("  Current: "); Serial.println(Irms);
        //green blinking
        currentmillis=custommillis;
        if((currentmillis-prevmillis)>2000){
          if(green_flag==1){digitalWrite(led_green, LOW);}
          else if(green_flag==0{digitalWrite(led_green, HIGH);}
          prevmillis=currentmillis;
          
          }
        if(Irms>0){
        lcd.setCursor(0, 1);
        lcd.print("Current: ");
        lcd.setCursor(10, 1);
        lcd.print(Irms);
        }
        else{Serial.println(F("error value"));
        }
                if (volt6_flag == true) {
        //
        //          currentmillis = custommillis;
        //          Serial.println(currentmillis);
        //          Serial.println(prevmillis);
        //          if (currentmillis - prevmillis >= chargeduration) {
        //            stop_now();
        //            switch_flag = 0;
        //
        //          }
                  Serial.println(F("In 6V"));
                }
        else if (volt6_flag == false)
        {
          digitalWrite(13, LOW);
          Serial.println(F("CHARGING STARTED"));
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Vehicle Charging");
          _delay_ms(500);
          digitalWrite(relay_live, HIGH);
          digitalWrite(relay_neutral, HIGH);
          digitalWrite(led_green, LOW);
          digitalWrite(led_blue, HIGH);
          digitalWrite(led_red, HIGH);
          volt9_flag = false;
          volt6_flag = true;
          currentmillis=prevmillis=0
        }
      }
      else if ((adc_3v_min < voltage) && (voltage < adc_3v_max))//3v
      {
        digitalWrite(relay_live, LOW);
        digitalWrite(relay_neutral, LOW);
        digitalWrite(led_blue, LOW);
        digitalWrite(led_green, HIGH);
        digitalWrite(led_red, HIGH);
        //      start_flag = false;
        Serial.println(F("open surrounding needed X overheat "));
        volt6_flag = false;
        volt9_flag = false;
        volt12_flag = false;
      }
      else
      {
        Serial.println(F("No if conditions"));
        stop_now();
      }
    }                //>100 else ?
  }
  else                  //switch still OFF
  {
    digitalWrite(13, LOW);
    digitalWrite(led_blue, HIGH);
    digitalWrite(led_green, HIGH);
    digitalWrite(led_red, LOW);
    Serial.println(F("Everything OFF"));
    start_detect();
  }
//  _delay_ms(100);
}

void stop_now()
{
  wdt_reset();
  TIMSK0 &= ~(1 << OCIE0A);
  Serial.println(F("Stop now"));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EVSE Ready");

  _delay_ms(50);
  start_flag = false;
  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);
  digitalWrite(led_green, HIGH);
  digitalWrite(led_blue, HIGH);
  digitalWrite(led_red, LOW);

  analogWrite(pwm_1k, 255);

  volt6_flag = false;
  volt9_flag = false;
  volt12_flag = false;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EVSE Ready");
}

void stop_detect()
{
  if ( switch_flag == 1) {
    if (   digitalRead(start_pin) == HIGH)
    {
      _delay_ms(1000);
      wdt_reset();
      if (  digitalRead(start_pin) == HIGH)
      {
        Serial.println(F("In stop detect function"));
        stop_now();
        switch_flag = 0;
      }
    }
  }
}

void READY_BLINK()  // required integer specifies, Delay time in sec
{
  wdt_reset();
  for (int k = 0; k < 10 ; k++)
  {
    wdt_reset();
    digitalWrite(led_green, LOW);
    digitalWrite(led_blue, HIGH);
    digitalWrite(led_red, HIGH);
    _delay_ms(50);
    digitalWrite(led_green, HIGH);
    digitalWrite(led_blue, HIGH);
    digitalWrite(led_red, HIGH);
    _delay_ms(50);
  }
}
