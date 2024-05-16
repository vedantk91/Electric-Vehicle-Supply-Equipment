#include <EmonLib.h>
int voltage;
#define adc_12v_min 600
#define adc_12v_max 950
#define adc_9v_min 450
#define adc_9v_max 550
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

#define pwm_1k 5
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
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Starting...");
        delay(2000);
        start_flag = true;
        analogWrite(pwm_1k, 255);
      }
    }
  }
}
void setup() {
  TCCR0B = TCCR0B & B11111000 | B00000011;
  Serial.begin(9600);
  pinMode(start_pin , INPUT);
  pinMode(stop_pin , INPUT);
  pinMode(led_ready , OUTPUT);
  pinMode(led_charging , OUTPUT);
  pinMode(led_fault , OUTPUT);
  pinMode(relay_live, OUTPUT);
  pinMode(relay_neutral, OUTPUT);
  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);
  emon1.current(current_ip_pin, 60.1);
  //  emon1.c urrent(current_ip_pin, 111.1);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  Serial.println("STARTING");
  Wire.begin();
  Wire.beginTransmission(0x27);
  int   err = Wire.endTransmission();
  Serial.println(err);
  if (err == 0)
  {
    Serial.println("LCD Ready");
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
  }
}

void loop()
{
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
        //        if (voltage > 420)
        //        {
        //          stop_now();
        //        }
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
    voltageavg = voltagetotal / counterval;
    voltage = voltageavg;
    Serial.print("Counter val: "); Serial.print(counterval);
    Serial.print("/100\tVoltage val: "); Serial.println(voltage);

    Serial.print(volt12_flag); Serial.print(volt9_flag); Serial.println(volt6_flag);
    if (voltage > 100) {
      if ((adc_12v_min < voltage) && (voltage < adc_12v_max)) //12v
      {
        if (volt6_flag == true)
        {
          stop_now();
        }
        else
        {
          digitalWrite(relay_live, LOW);
          digitalWrite(relay_neutral, LOW);
          Serial.println("on standby i.e no car connected ");
          lcd.clear();
          lcd.setCursor(5, 0);
          lcd.print("Standby");
          lcd.setCursor(0, 1);
          lcd.print("No Car Connected");
          volt12_flag = true;
          volt9_flag = false;
          volt6_flag = false;
        }
      }
      else if ((adc_9v_min < voltage) && (voltage < adc_9v_max )) //9v
      {
        if (volt12_flag == true)          //check if it was 12V first
        {
          digitalWrite(relay_live, LOW);
          digitalWrite(relay_neutral, LOW);
          Serial.println("vehicle detected ..sending 1khz signal ");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Vehicle Detected");
          lcd.setCursor(1, 1);
          lcd.print("Sending 1KHz");
          Serial.println(voltage);
          analogWrite(pwm_1k, duty_cycle);
          digitalWrite(led_ready, HIGH);
          digitalWrite(led_fault, LOW);
          digitalWrite(led_charging, LOW);
          digitalWrite(13, HIGH);
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Waiting for 6V");
          volt9_flag = true;
          volt12_flag = false;
          volt6_flag = false;
        }
        else                    //wasnt 12V first directly 9V
        {
          if (volt6_flag == true)
          {
            stop_now();
          }
          Serial.println("In 9V Loop");
          //stop_now();                            //HE UNCOMMENT KARAYCHA NA
        }
      }
      else if ((adc_6v_min < voltage) && (voltage < adc_6v_max))//6v
      {
        Serial.println("In charging loop");
        double Irms = emon1.calcIrms(1480);  // Calculate Irms only
        Serial.print("Power: "); Serial.print(Irms * 230.0); Serial.print("  Current: "); Serial.println(Irms);

        if (volt9_flag == true)              //check if it was 9V first
        {
          digitalWrite(13, LOW);
          Serial.println("CHARGING STARTED");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Vehicle Charging");
          delay(3000);
          digitalWrite(relay_live, HIGH);
          digitalWrite(relay_neutral, HIGH);
          digitalWrite(led_charging, HIGH);
          digitalWrite(led_ready, LOW);
          digitalWrite(led_fault, LOW);
          volt9_flag = false;
          volt6_flag = true;
        }
      }
      else if ((adc_3v_min < voltage) && (voltage < adc_3v_max))//3v
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
      }
    }                //>100 else ?
  }
  else                  //switch still OFF
  {
    digitalWrite(13, LOW);
    digitalWrite(led_ready, HIGH);
    Serial.println("Everything OFF");
    start_detect();
  }
  delay(100);
}

void stop_now()
{
  Serial.println("Stop now");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EVSE Ready");

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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EVSE Ready");
}

void stop_detect()
{
  if (   digitalRead(stop_pin) == HIGH)
  {
    _delay_ms(50);
    if (  digitalRead(stop_pin) == HIGH)
    {
      stop_now();
    }
  }
}
