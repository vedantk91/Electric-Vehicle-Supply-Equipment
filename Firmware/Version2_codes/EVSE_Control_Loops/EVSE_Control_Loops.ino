#include <EmonLib.h>

#include <Wire.h>
#include <PWM.h>
#include <LiquidCrystal_I2C.h>

bool      volt6_flag = false;
bool      volt9_flag = false;
bool      volt12_flag = false;
uint8_t count = 0;
#define start_pin 11
#define stop_pin 2            //interupt

bool start_flag = false; // MAKE IT FALSE FOR DEVELOPMENT

#define led_ready 5
#define led_charging 3
#define led_fault 4

#define pwm_1k 9
int32_t frequency = 1000;//hz
#define duty_cycle 63.75  //   (10% 25.5)||(16% 40.8)||(25% 63.75)||(30% 76.5)||(40% 102)||(50% 127.5)||(100% 255) ,if above 800us or 80% duty cycle rule changes
#define voltage_read A0

#define relay_live 12
#define relay_neutral 13
#define current_ip_pin A1
EnergyMonitor emon1;



void start_detect()
{

  if (start_flag == false)
  {
    if (   digitalRead(start_pin) == HIGH)
    {
      _delay_ms(50);
      if (  digitalRead(start_pin) == HIGH)
      {
        start_flag = true;
        pwmWrite(pwm_1k, 255);

      }
    }

  }

}


void setup() {
  Serial.begin(9600);
  pinMode(start_pin , INPUT);
  attachInterrupt(digitalPinToInterrupt(stop_pin), stop_now, CHANGE);
  //  pinMode(stop_pin ,INPUT);
  pinMode(led_ready , OUTPUT);
  pinMode(led_charging , OUTPUT);
  pinMode(led_fault , OUTPUT);
  pinMode(relay_live, OUTPUT);
  pinMode(relay_neutral, OUTPUT);


  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);


  emon1.current(current_ip_pin, 60.1);
  InitTimersSafe();
  SetPinFrequencySafe( pwm_1k , frequency);
  //  pwmWrite(pwm_1k, 0);
  //  bool success = SetPinFrequencySafe( pwm_1k , frequency);//if the pin frequency was set successfully, turn pin 13 on
  //  if(success) {
  //  pinMode(13, OUTPUT);
  //  digitalWrite(13, HIGH);
  delay(2000);
  pwmWrite(pwm_1k, 68);//same as analog write,starts the square wave.12v signal will come from opamp.,,keep 255
  //  delay(5000);
  //  pwmWrite(pwm_1k, 64);//same as analog write,starts the square wave.12v signal will come from opamp.,,keep 255
  //  delay(5000);
  //  pwmWrite(pwm_1k, 255);//same as analog write,starts the square wave.12v signal will come from opamp.,,keep 255
  //  delay(5000);
  //  pwmWrite(pwm_1k, 30);//same as analog write,starts the square wave.12v signal will come from opamp.,,keep 255
  Serial.println("yyyyyyyyyyyyyyy");

}

void loop()
{

  if (start_flag == true)
  {

    int voltage = analogRead(voltage_read);
    Serial.println(voltage);
    Serial.print(volt12_flag); Serial.print(volt9_flag); Serial.println(volt6_flag);

    if ((740 < voltage) && (voltage < 900)) //12v
    {
      digitalWrite(relay_live, LOW);
      digitalWrite(relay_neutral, LOW);
      Serial.println("on standby i.e no car connected ");
      volt12_flag = true;
      volt9_flag = false;
      volt6_flag = false;
    }
    else if ((545 < voltage) && (voltage < 681))//9v
    {
      if (volt12_flag == true)
      {
        digitalWrite(relay_live, LOW);
        digitalWrite(relay_neutral, LOW);
        Serial.println("vehicle detected ..sending 1khz signal ");    Serial.println(voltage);
        //      pwmWrite(pwm_1k, duty_cycle);//same as analog write,starts the square wave.
        digitalWrite(led_ready, HIGH);
        digitalWrite(led_fault, LOW);
        digitalWrite(led_charging, LOW);
        delay(5000);
        volt9_flag = true;
        volt12_flag = false;
        volt6_flag = false;

      }
      else
      {
        Serial.println("Charge interrupted ");
        stop_now();

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
    else if ((341 < voltage) && (voltage < 476))//6v
    {
      Serial.println("charging");

      if (volt9_flag == true)
      { delay(5000);
        Serial.print("69");
        //start charging
        Serial.println("CHARGING STARTED");
        digitalWrite(relay_live, HIGH);
        digitalWrite(relay_neutral, HIGH);
        digitalWrite(led_charging, HIGH);
        digitalWrite(led_ready, LOW);
        digitalWrite(led_fault, LOW);
        volt9_flag = false;
        volt6_flag = true;


      }

    }
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

  }


  else
  {
    start_detect();
  }


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
