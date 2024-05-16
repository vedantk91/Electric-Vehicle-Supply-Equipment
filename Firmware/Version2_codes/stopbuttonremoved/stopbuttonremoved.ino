#include <EmonLib.h>
#include <EEPROM.h>

int voltage;
int switch_flag = 0;

#define adc_12v_min  750
#define adc_12v_max  950
#define adc_9v_min  550
#define adc_9v_max  650
#define adc_6v_min 250
#define adc_6v_max  450
#define adc_3v_min  100
#define adc_3v_max  230

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
String temp;
String s1;

void start_detect()                          //Switch ON button
{
  if (start_flag == false)
  {
    if (digitalRead(start_pin) == HIGH)
    {
      _delay_ms(50);
      if (digitalRead(start_pin) == HIGH)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Starting...");
        delay(2000);
        start_flag = true;
        analogWrite(pwm_1k, 255);
        switch_flag = 1;
      }
    }
  }
}
void setup() {
  TCCR0B = TCCR0B & B11111000 | B00000011;
  Serial.begin(9600);
  pinMode(start_pin , INPUT);
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
  /*
    temp = read_String(0);
    adc_12v_min = temp.toInt();
    temp = read_String(5);
    adc_12v_max = temp.toInt();
    temp = read_String(10);
    adc_9v_min = temp.toInt();
    temp = read_String(15);
    adc_9v_max = temp.toInt();
    temp = read_String(20);
    adc_6v_min = temp.toInt();
    temp = read_String(25);
    adc_6v_max = temp.toInt();
    temp = read_String(30);
    adc_3v_min = temp.toInt();
    temp = read_String(35);
    adc_3v_max = temp.toInt();
    temp = read_String(40);
    duty_cycle = temp.toInt();
  */
}

void loop()
{
  send1_string();

  if (start_flag == true)
  {
    stop_detect();
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
      if ((adc_12v_min > voltage) && (voltage < adc_12v_max)) //12v
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
          digitalWrite(led_ready, LOW);
          digitalWrite(led_fault, HIGH);
          digitalWrite(led_charging, HIGH);
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

          Serial.println("In 9V Loop");
          //stop_now();                            //HE UNCOMMENT KARAYCHA NA
        }
      }
      else if ((adc_6v_min < voltage) && (voltage < adc_6v_max))//6v
      {
        Serial.println("In charging loop");
        double Irms = emon1.calcIrms(1480);  // Calculate Irms only
        Serial.print("Power: "); Serial.print(Irms * 230.0); Serial.print("  Current: "); Serial.println(Irms);

        if (volt6_flag == false)              //check if it was 9V first
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
          digitalWrite(led_ready, HIGH);
          digitalWrite(led_fault, HIGH);
          volt9_flag = false;
          volt6_flag = true;
        }
      }
      else if ((adc_3v_min > voltage) && (voltage < adc_3v_max))//3v
      {
        digitalWrite(relay_live, LOW);
        digitalWrite(relay_neutral, LOW);
        digitalWrite(led_ready, HIGH);
        digitalWrite(led_charging, HIGH);
        digitalWrite(led_fault, HIGH);
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
    digitalWrite(led_ready, LOW);
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
  digitalWrite(led_charging, HIGH);
  digitalWrite(led_ready, LOW);
  digitalWrite(led_fault, HIGH);
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
      _delay_ms(50);
      if (  digitalRead(start_pin) == HIGH)
      {
        Serial.println("In stop detect function");
        stop_now();
        switch_flag = 0;
      }
    }
  }
}


void send1_string()
{


  if (Serial.available() > 0)
  {
    s1 = Serial.readStringUntil('\n');
    int len = s1.length();

    Serial.print("Length of scanned code is: ");
    Serial.println(len);
    char ss = s1.charAt(1);
    Serial.print("Char at 1 is: ");
    Serial.println(ss);
    //    Serial2.print("Char at 0 is: ");
    //    Serial2.println(ss);
    if (s1 == "#STRT")
    {
      start_flag = true;
      analogWrite(pwm_1k, 255);
    }

    else if (s1 == "#STOP")
    {
      _delay_ms(5);
      start_flag = false;
      digitalWrite(relay_live, LOW);
      digitalWrite(relay_neutral, LOW);
      digitalWrite(led_charging, HIGH);
      digitalWrite(led_ready, LOW);
      digitalWrite(led_fault, HIGH);
      volt6_flag = false;
      volt9_flag = false;
      volt12_flag = false;
    }

    else if (ss == '1')
    {
      writeThresh(0);
      Serial.println("12 V min threshold");
    }

    else if (ss == '2')
    {
      writeThresh(5);
      Serial.println("12 V max threshold");
    }

    else if (ss == '3')
    {
      writeThresh(10);
      Serial.println("9 V min threshold");
    }

    else if (ss == '4')
    {
      writeThresh(15);
      Serial.println("9 V max threshold");
    }

    else if (ss == '5')
    {
      writeThresh(20);
      Serial.println("6 V min threshold");
    }

    else if (ss == '6')
    {
      writeThresh(25);
      Serial.println("6 V max threshold");
    }

    else if (ss == '7')
    {
      writeThresh(30);
      Serial.println("3 V min threshold");
    }

    else if (ss == '8')
    {
      writeThresh(35);
      Serial.println("3 V max threshold");
    }

    else if (ss == 'p')
    {
      writeThresh(40);
      Serial.println("Duty Cycle");
    }

  }
}

void writeThresh(int addrw)
{
  Serial.println("Entering writeString");

  String  data1 = s1.substring(3, 6);
  Serial.println(data1);
  int _size = data1.length();

  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(addrw + i, data1[i]);
    delay(10);
  }
  EEPROM.write(addrw + _size, '\0');
  delay(20);


  Serial.println("Exiting writeString");
}

String read_String(int addrr)
{
  int i = 0;
  char datas[20];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(addrr);
  while (i < 5)
  {
    k = EEPROM.read(addrr + len);
    if (k == '\0')
    {
      Serial.println("Null Character");
      //      datas[len] = '\0';r
      break;
    }
    //    else
    //    {
    //      datas[len] = k;
    //    }
    datas[len] = k;
    Serial.print("IN read_String loop: \t");
    Serial.println(datas[len]);
    len++;
    i++;
    delay(50);
  }
  Serial.println(len);
  String finalstring = String(datas);
  finalstring[len] = '\0';
  finalstring = finalstring.substring(0, len);
  Serial.println(finalstring);
  //  return String(datas);
  return finalstring;
}
