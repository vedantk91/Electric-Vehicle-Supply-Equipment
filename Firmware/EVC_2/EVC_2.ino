
#include <EmonLib.h>

#include <Wire.h>
//#include <PWM.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);


bool      volt6_flag = false;
bool      volt9_flag = false;
bool      volt12_flag = false;
uint8_t count = 0;

#define start_pin 10
#define stop_pin 9            //interupt

bool start_flag = false; // MAKE IT FALSE FOR DEVELOPMENT

#define led_ready 8
#define led_charging 6
#define led_fault 7

//#define pwm_1k 5
int pwmpin = 5;
int pwmvalue = 63;

int32_t frequency = 1000;//hz
#define duty_cycle 63  //   (10% 25.5)||(16% 40.8)||(25% 63.75)||(30% 76.5)||(40% 102)||(50% 127.5)||(100% 255) ,if above 800us or 80% duty cycle rule changes
#define voltage_read A2

#define relay_live 3
#define relay_neutral 2
#define current_ip_pin A3
EnergyMonitor emon1;



void start_detect()                          //Switch ON button
{

  if (start_flag == false)
  {
    if (   digitalRead(start_pin) == HIGH)
    {
      _delay_ms(50);
      if (  digitalRead(start_pin) == HIGH)
      {
        start_flag = true;
        analogWrite(pwmpin, 255);
        //pwmWrite(pwm_1k, 255);
        //digitalWrite(pwm_1k, HIGH);
      }
    }
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.println("LED HIGH");
  delay(1000);
  digitalWrite(13, LOW);
  Serial.println("LED LOW");
  TCCR0B = TCCR0B & B11111000 | B00000011;  //external 16MHz
  //pinMode(pwm_1k, OUTPUT);
  pinMode(pwmpin, OUTPUT);

  delay(1000);
  digitalWrite(13, HIGH);
  Serial.println("LED HIGH");


  pinMode(start_pin , INPUT);
  pinMode(voltage_read, INPUT);
  //attachInterrupt(digitalPinToInterrupt(stop_pin), stop_now, CHANGE);
  //  pinMode(stop_pin ,INPUT);
  pinMode(led_ready , OUTPUT);
  pinMode(led_charging , OUTPUT);
  pinMode(led_fault , OUTPUT);
  pinMode(relay_live, OUTPUT);
  pinMode(relay_neutral, OUTPUT);


  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);



  emon1.current(current_ip_pin, 60.1);
  //InitTimersSafe();
  //SetPinFrequencySafe( pwm_1k , frequency);
  //  pwmWrite(pwm_1k, 0);
  //  bool success = SetPinFrequencySafe( pwm_1k , frequency);//if the pin frequency was set successfully, turn pin 13 on
  //  if(success) {

  //analogWrite(pwm_1k, 255);//same as analog write,starts the square wave.12v signal will come from opamp.,,keep 255
  //  Serial.println("255");
  //
  //  delay(10000);
  //  analogWrite(pwm_1k, 63);//same as analog write,starts the square wave.12v signal will come from opamp.,,keep 255
  //  Serial.println("63");
  //
  //  delay(10000);
  //  analogWrite(pwm_1k, 128);//same as analog write,starts the square wave.12v signal will come from opamp.,,keep 255
  //  Serial.println("128");
  //
  //  delay(10000);
  //  analogWrite(pwm_1k, 255 ); //same as analog write,starts the square wave.12v signal will come from opamp.,,keep 255
  //  Serial.println("255");

  Serial.println("yyyyyyyyyyyyyyy");
  Wire.begin();
  Wire.beginTransmission(0x27);
  int   err = Wire.endTransmission();
  Serial.println(err);
  if (err == 0)
  {
    Serial.println("gg");
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EVSE Ready");
    //rtcf = 1;
  }
  else
  {
    Serial.println(F("LCDfault "));
    //rtcf = 1;
  }
}

void loop()
{

  if (start_flag == true)
  {
    int voltage;
    unsigned long voltotal = 0;

    for (int i = 0; i < 1000; i++) {
      voltage = analogRead(voltage_read);
      //Serial.println(voltage);
      voltotal = voltotal + voltage;


    }
    int voltavg = voltotal / 1000;
    Serial.print("VOLTAGE TOTAL : ");
    Serial.println(voltotal);
    Serial.print("VOLTAGE AVERAGE : ");
    Serial.println(voltavg);
    voltage = voltavg;

    if (voltage > 50) {

      Serial.print(volt12_flag); Serial.print(volt9_flag); Serial.println(volt6_flag);

      if ((700 < voltage) && (voltage < 900)) //12v
      {
        digitalWrite(relay_live, LOW);
        digitalWrite(relay_neutral, LOW);
        Serial.println("on standby i.e no car connected ");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Standby");
        lcd.setCursor(0, 1);
        lcd.print("No Car Connected");
        volt12_flag = true;
        volt9_flag = false;
        volt6_flag = false;
      }
      else if ((400 < voltage) && (voltage < 580))//9v
      {
        if (volt12_flag == true)          //check if it was 12V first
        {
          digitalWrite(relay_live, LOW);
          digitalWrite(relay_neutral, LOW);
          Serial.println("vehicle detected ..sending 1khz signal ");
          analogWrite(pwmpin, pwmvalue);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Vehicle Detected");
          lcd.setCursor(0, 1);
          lcd.print("Sending 1KHz");
          Serial.println(voltage);
          // analogWrite(pwm_1k, duty_cycle);//same as analog write,starts the square wave.
          //tone(pwm_1k, 1000);
          //pwmWrite(pwm_1k, duty_cycle);//same as analog write,starts the square wave.
          digitalWrite(led_ready, HIGH);
          digitalWrite(led_fault, LOW);
          digitalWrite(led_charging, LOW);
          delay(5000);
          volt9_flag = true;
          volt12_flag = false;
          volt6_flag = false;

        }
        else                    //wasnt 12V first directly 9V
        {
          Serial.println("Charge interrupted ");
          //stop_now();                            //HE UNCOMMENT KARAYCHA NA

        }
      }
      //    else if (voltage == 0)
      //    {
      //
      //      count = count + 1;
      //      if (count > 20)
      //      {
      //        Serial.println("fault triggered");
      //        pwmWrite(pwm_1k, 0);
      //        _delay_ms(5);
      //        digitalWrite(relay_live, LOW);
      //        digitalWrite(relay_neutral, LOW);
      //        digitalWrite(led_charging, LOW);
      //        digitalWrite(led_ready, LOW);
      //        digitalWrite(led_fault, HIGH);
      //
      //        start_flag = false;
      //        volt9_flag = false;
      //
      //
      //      }
      //    }
      else if ((120 < voltage) && (voltage < 170))//6v
      {
        Serial.println("charging");


        if (volt9_flag == true)              //check if it was 9V first
        { delay(5000);
          Serial.print("69");
          //start charging
          Serial.println("CHARGING STARTED");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Vehicle Charging");
          digitalWrite(relay_live, HIGH);
          digitalWrite(relay_neutral, HIGH);
          digitalWrite(led_charging, HIGH);
          digitalWrite(led_ready, LOW);
          digitalWrite(led_fault, LOW);
          volt9_flag = false;
          volt6_flag = true;


        }

      }
      /*
        else if ((136 < voltage) && (voltage < 272))//3v
        {
        digitalWrite(relay_live, LOW);
        digitalWrite(relay_neutral, LOW);
        digitalWrite(led_ready, LOW);
        digitalWrite(led_charging, LOW);
        digitalWrite(led_fault, LOW);
        //      start_flag = false;
        Serial.println("open surrounding needed X overheat ");
        volt6_flag = false;
        volt9_flag = false;
        volt12_flag = false;
        }
      */
      else
      {
        Serial.println("No if conditions");
        //                    digitalWrite(relay_live,LOW);
        //                    digitalWrite(relay_neutral,LOW);
        //                    digitalWrite(led_charging,LOW);
        //                    digitalWrite(led_ready,LOW);
        //                    digitalWrite(led_fault,HIGH);
        //                    start_flag=false;
      }

    }                //>100 else ?
  }

  else                  //switch still OFF
  {
    Serial.println("Everything OFF");
    start_detect();
  }
  delay(100);


}

void stop_now()
{
  //havent consider the arcs;we can send stop pwm first and then shut relay
  //  pwmWrite(pwm_1k, 0);
  _delay_ms(5);
  start_flag = false;
  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);
  digitalWrite(led_charging, LOW);
  digitalWrite(led_ready, HIGH);
  digitalWrite(led_fault, LOW);
  volt6_flag = false;
  volt9_flag = false;
  volt12_flag = false;
}
