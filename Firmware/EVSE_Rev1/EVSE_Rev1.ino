#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <EEPROM.h>
#include <PWM.h>
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int voltflag = 0;
//voltflag = 1; //12V
//voltflag = 2; //9V
//voltflag = 3; //6V
//voltflag = 4; //3V

uint8_t final_chg_flag = 0;
uint8_t count = 0;
#define start_pin 11
#define stop_pin 2            //interupt
bool start_flag = false; // MAKE IT FALSE FOR real DEVELOPMENT
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
byte cMCUSR;

void setup() {
  // put your setup code here, to run once:
  cMCUSR = MCUSR;
  MCUSR = 0;
  wdt_disable();
  Serial.begin(9600);
  _delay_ms(200);
  if (cMCUSR & (1 << PORF ))
    Serial.println(F("POR"));
  if (cMCUSR & (1 << EXTRF))
    Serial.println(F("EXTR"));
  if (cMCUSR & (1 << BORF ))
    Serial.println(F("BOR"));
  if (cMCUSR & (1 << WDRF ))
  {
    Serial.println(F("WDR"));
  }
  pinMode(relay_live, OUTPUT);
  digitalWrite(relay_live, LOW);
  pinMode(relay_neutral, OUTPUT);
  digitalWrite(relay_neutral, LOW);
  pinMode(led_ready, OUTPUT);
  digitalWrite(led_ready, HIGH);
  pinMode(led_charging, OUTPUT);
  digitalWrite(led_ready, HIGH);
  pinMode(led_fault, OUTPUT);
  digitalWrite(led_ready, HIGH);
  pinMode(stop_pin, INPUT_PULLUP);
  pinMode(start_pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(stop_pin), stop_now, CHANGE);
  _delay_ms(500);
  digitalWrite(led_ready, LOW);
  digitalWrite(led_charging, LOW);
  digitalWrite(led_fault, LOW);
  Serial.println("EVSE Initialized Rev 1.1");
  InitTimersSafe();
  SetPinFrequencySafe( pwm_1k , 1000);
  emon1.current(0, 60.1);
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

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(stop_pin) == LOW)
  {
    Serial.println("In charge stop seq");
    _delay_ms(50);
    if (digitalRead(stop_pin) == LOW && voltflag == 1)
    {
      Serial.println("Charging Stopped");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Charging Stopped");
      stop_now();
      digitalWrite(13, LOW);
    }
  }
  Serial.print("Voltflag: ");
  Serial.println(voltflag);

  if (digitalRead(start_pin) == LOW)
  {
    Serial.println("In charge start seq");
    _delay_ms(50);
    if (digitalRead(start_pin) == LOW && (voltflag == 0))
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Charging Init");
      //chrgInit();
      digitalWrite(13, HIGH);
      start_flag = true;
      final_chg_flag = 0;
      voltflag = 0;
      pwmWrite(pwm_1k, 255);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CHRG INIT");
      delay(500);
      Serial.println("Charging Init");
    }
  }
  if (start_flag == true)
  {
    double Irms = 0;
    //    double Irms = emon1.calcIrms(1480);
    if (Irms > 18)
    {
      Serial.println("OverCurrent!!!!!!");
      close_all();
    }
    lcd.setCursor(0, 1);
    lcd.print("I: ");
    lcd.setCursor(3, 1);
    lcd.print(Irms);
    lcd.setCursor(14, 1);
    lcd.print("A");

    int voltage = analogRead(voltage_read);
    Serial.print("AnalogRead: ");
    Serial.println(voltage);

    // double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    if ((740 < voltage) && (voltage < 900)) //12v
    {
      check_errors();
      digitalWrite(relay_live, LOW);
      digitalWrite(relay_neutral, LOW);
      Serial.println("on standby i.e no car connected ");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No car connected");
      voltflag = 1;
    }
    else if ((545 < voltage) && (voltage < 681))//9v
    {
      check_errors();
      if (voltflag == 1)
      {
        digitalWrite(relay_live, LOW);
        digitalWrite(relay_neutral, LOW);
        Serial.println("vehicle detected ..sending 1khz signal ");
        Serial.println(voltage);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("vehicle detected .");
        pwmWrite(pwm_1k, duty_cycle);//same as analog write,starts the square wave.
        digitalWrite(led_ready, HIGH);
        digitalWrite(led_fault, LOW);
        digitalWrite(led_charging, LOW);
        delay(2000);
        voltflag = 2;
      }
      //      else
      //      {
      //        stop_now();
      //      }
    }
    else if ((341 < voltage) && (voltage < 476))//6v
    {
      Serial.println("6V Loop");
      if (voltflag == 2 && final_chg_flag == 0)
      {
        delay(500);
        Serial.print("69");
        //start charging
        Serial.println("CHARGING STARTED");
        digitalWrite(relay_live, HIGH);
        digitalWrite(relay_neutral, HIGH);
        digitalWrite(led_charging, HIGH);
        digitalWrite(led_ready, HIGH);
        digitalWrite(led_fault, LOW);
        voltflag = 3;
        final_chg_flag = 1;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CHARGING STARTED");
        delay(1000);
      }
    }
    else if ((136 < voltage) && (voltage < 272))//3v
    {
      Serial.println("High Temp!");
      close_all();
    }
    else
    {
      Serial.println("No if conditions");
    }
  }
  if (final_chg_flag == 1)
  {
    Serial.println("Vehicle is in charging mode now!!!!!");
  }
  else
  {
    Serial.println("Vehicle is not in charging mode");
  }

}

void stop_now()
{
  Serial.println("Stop now");
  //havent consider the arcs;we can send stop pwm first and then shut relay
  //  pwmWrite(pwm_1k, 0);
  _delay_ms(200);
  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);
  digitalWrite(led_charging, LOW);
  digitalWrite(led_ready, HIGH);
  digitalWrite(led_fault, LOW);
}
String read_String(int addrr)
{
  int i = 0;
  char datas[20];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(addrr);
  while (i < 8)
  {
    k = EEPROM.read(addrr + len);
    datas[len] = k;
    Serial.print("IN read_String loop: \t");
    Serial.println(datas[len]);
    len++;
    i++;
    delay(50);
  }
  datas[len] = '\0';
  Serial.println(len);

  return String(datas);
}

void writeString(int addrw, String data)
{
  Serial.println("Entering writeString");
  int _size = data.length();
  if (_size > 9)
  {
    Serial.println("Invalid Dev_id entry");
  }
  else
  {
    int i;
    for (i = 0; i < _size; i++)
    {
      EEPROM.write(addrw + i, data[i]);
      delay(10);
    }
    EEPROM.write(addrw + _size, '\0');
    delay(20);
    //EEPROM.commit();
  }
  Serial.println("Exiting writeString");
}

void check_errors()
{
  if (voltflag == 3 && final_chg_flag == 1)
  {
    Serial.println("Wrong loop: Exiting!!!!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EXITING");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EVSE Ready");
    digitalWrite(led_charging, LOW);
    digitalWrite(led_ready, HIGH);
    digitalWrite(led_fault, LOW);

    final_chg_flag = 0;
    voltflag = 0;
    start_flag = 0;
    pwmWrite(pwm_1k, 0);
  }
}
void close_all()
{
  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);
  digitalWrite(led_charging, LOW);
  digitalWrite(led_ready, LOW);
  digitalWrite(led_fault, HIGH);
  final_chg_flag = 0;
  voltflag = 0;
  start_flag = 0;
}
