//Changes for AVR32DA32- EMonlib removed, LCD Warning


//#include <EmonLib.h>
#include <EEPROM.h>

int voltage;
int switch_flag = 0;
char eepcheck;


int adc_12v_min, adc_12v_max, adc_9v_min, adc_9v_max, adc_6v_min, adc_6v_max, adc_3v_min, adc_3v_max;
int32_t frequency = 1000;//hz
int duty_cycle;  //   (10% 25.5)||(16% 40.8)||(25% 63.75)||(30% 76.5)||(40% 102)||(50% 127.5)||(100% 255) ,if above 800us or 80% duty cycle rule changes

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

#define pwm_1k 2
#define voltage_read 18

#define relay_live 3
#define relay_neutral 2
#define current_ip_pin A3
//EnergyMonitor emon1;
String temp;
String s1;

void start_detect()                          //Switch ON button
{
  if (start_flag == false)
  {
    if (   digitalRead(start_pin) == HIGH)
    {
      delay(50);
      if (  digitalRead(start_pin) == HIGH)
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
  TCA0.SINGLE.CMP0 = 0x3E;

  //Compare 1
  TCA0.SINGLE.CMP1 = 0x00;

  //Compare 2
  TCA0.SINGLE.CMP2 = 0x00;

  //Count
  TCA0.SINGLE.CNT = 0x00;

  //CMP2EN disabled; CMP1EN disabled; CMP0EN enabled; ALUPD disabled; WGMODE SINGLESLOPE;
  TCA0.SINGLE.CTRLB = 0x13;

  //CMP2OV disabled; CMP1OV disabled; CMP0OV disabled;
  TCA0.SINGLE.CTRLC = 0x00;

  //SPLITM disabled;
  TCA0.SINGLE.CTRLD = 0x00;

  //CMD NONE; LUPD disabled; DIR disabled;
  TCA0.SINGLE.CTRLECLR = 0x00;

  //CMD NONE; LUPD disabled; DIR UP;
  TCA0.SINGLE.CTRLESET = 0x00;

  //CMP2BV disabled; CMP1BV disabled; CMP0BV disabled; PERBV disabled;
  TCA0.SINGLE.CTRLFCLR = 0x00;

  //CMP2BV disabled; CMP1BV disabled; CMP0BV disabled; PERBV disabled;
  TCA0.SINGLE.CTRLFSET = 0x00;

  //DBGRUN disabled;
  TCA0.SINGLE.DBGCTRL = 0x00;

  //EVACTB NONE; CNTBEI disabled; EVACTA CNT_POSEDGE; CNTAEI disabled;
  TCA0.SINGLE.EVCTRL = 0x00;

  //CMP2 disabled; CMP1 disabled; CMP0 disabled; OVF disabled;
  TCA0.SINGLE.INTCTRL = 0x00;

  //CMP2 disabled; CMP1 disabled; CMP0 disabled; OVF disabled;
  TCA0.SINGLE.INTFLAGS = 0x00;

  //Period
  TCA0.SINGLE.PER = 0x7C;

  //Temporary data for 16-bit Access
  TCA0.SINGLE.TEMP = 0x00;

  //RUNSTDBY disabled; CLKSEL DIV64; ENABLE enabled;
  TCA0.SINGLE.CTRLA = 0x0B;

  analogWrite(pwm_1k, 128);
  delay(1000);
  analogWrite(pwm_1k, 63);
  delay(1000);
  analogWrite(pwm_1k, 128);

    TCA0.SINGLE.CMP0 = 0x3E;

  //Compare 1
  TCA0.SINGLE.CMP1 = 0x00;

  //Compare 2
  TCA0.SINGLE.CMP2 = 0x00;

  //Count
  TCA0.SINGLE.CNT = 0x00;

  //CMP2EN disabled; CMP1EN disabled; CMP0EN enabled; ALUPD disabled; WGMODE SINGLESLOPE;
  TCA0.SINGLE.CTRLB = 0x13;

  //CMP2OV disabled; CMP1OV disabled; CMP0OV disabled;
  TCA0.SINGLE.CTRLC = 0x00;

  //SPLITM disabled;
  TCA0.SINGLE.CTRLD = 0x00;

  //CMD NONE; LUPD disabled; DIR disabled;
  TCA0.SINGLE.CTRLECLR = 0x00;

  //CMD NONE; LUPD disabled; DIR UP;
  TCA0.SINGLE.CTRLESET = 0x00;

  //CMP2BV disabled; CMP1BV disabled; CMP0BV disabled; PERBV disabled;
  TCA0.SINGLE.CTRLFCLR = 0x00;

  //CMP2BV disabled; CMP1BV disabled; CMP0BV disabled; PERBV disabled;
  TCA0.SINGLE.CTRLFSET = 0x00;

  //DBGRUN disabled;
  TCA0.SINGLE.DBGCTRL = 0x00;

  //EVACTB NONE; CNTBEI disabled; EVACTA CNT_POSEDGE; CNTAEI disabled;
  TCA0.SINGLE.EVCTRL = 0x00;

  //CMP2 disabled; CMP1 disabled; CMP0 disabled; OVF disabled;
  TCA0.SINGLE.INTCTRL = 0x00;

  //CMP2 disabled; CMP1 disabled; CMP0 disabled; OVF disabled;
  TCA0.SINGLE.INTFLAGS = 0x00;

  //Period
  TCA0.SINGLE.PER = 0x7C;

  //Temporary data for 16-bit Access
  TCA0.SINGLE.TEMP = 0x00;

  //RUNSTDBY disabled; CLKSEL DIV64; ENABLE enabled;
  TCA0.SINGLE.CTRLA = 0x0B;


  Serial.begin(9600);
  pinMode(start_pin , INPUT);
  pinMode(led_ready , OUTPUT);
  pinMode(led_charging , OUTPUT);
  pinMode(led_fault , OUTPUT);
  pinMode(relay_live, OUTPUT);
  pinMode(relay_neutral, OUTPUT);
  digitalWrite(relay_live, LOW);
  digitalWrite(relay_neutral, LOW);
  // emon1.current(current_ip_pin, 60.1);
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
}

void loop()
{
  eepcheck = EEPROM.read(50);
  Serial.println(eepcheck);
  if (eepcheck != '1') {
    hardcode();
    Serial.println("Entering hardcode loop");
  }
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
      if ((adc_12v_min < voltage) && (voltage < adc_12v_max)) //12v
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

          Serial.println("In 9V Loop");
          //stop_now();                            //HE UNCOMMENT KARAYCHA NA
        }
      }
      else if ((adc_6v_min < voltage) && (voltage < adc_6v_max))//6v
      {
        Serial.println("In charging loop");
        // double Irms = emon1.calcIrms(1480);  // Calculate Irms only
        //        Serial.print("Power: "); Serial.print(Irms * 230.0); Serial.print("  Current: "); Serial.println(Irms);

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

  delay(5);
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
  if ( switch_flag == 1) {
    if (   digitalRead(start_pin) == HIGH)
    {
      delay(50);
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
      delay(5);
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

    else if (ss == '1')
    {
      writeThresh(0);
      Serial.println("12 V min threshold");
      temp = read_String(0);
      adc_12v_min = temp.toInt();
    }

    else if (ss == '2')
    {
      writeThresh(5);
      Serial.println("12 V max threshold");
      temp = read_String(5);
      adc_12v_max = temp.toInt();
    }

    else if (ss == '3')
    {
      writeThresh(10);
      Serial.println("9 V min threshold");
      temp = read_String(10);
      adc_9v_min = temp.toInt();
    }

    else if (ss == '4')
    {
      writeThresh(15);
      Serial.println("9 V max threshold");
      temp = read_String(15);
      adc_9v_max = temp.toInt();
    }

    else if (ss == '5')
    {
      writeThresh(20);
      Serial.println("6 V min threshold");
      temp = read_String(20);
      adc_6v_min = temp.toInt();
    }

    else if (ss == '6')
    {
      writeThresh(25);
      Serial.println("6 V max threshold");
      temp = read_String(25);
      adc_6v_max = temp.toInt();
    }

    else if (ss == '7')
    {
      writeThresh(30);
      Serial.println("3 V min threshold");
      temp = read_String(30);
      adc_3v_min = temp.toInt();
    }

    else if (ss == '8')
    {
      writeThresh(35);
      Serial.println("3 V max threshold");
      temp = read_String(35);
      adc_3v_max = temp.toInt();

    }

    else if (ss == 'p')
    {
      writeThresh(40);
      Serial.println("Duty Cycle");
      temp = read_String(40);
      duty_cycle = temp.toInt();
    }

    else if (ss == 'h')
    {
      Serial.println("Hardcode bit");
      EEPROM.write(50, '0');

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


void hardcode() {
  s1 = "#10600";
  writeThresh(0);
  s1 = "#20950";
  writeThresh(5);
  s1 = "#30450";
  writeThresh(10);
  s1 = "#40550";
  writeThresh(15);
  s1 = "#50250";
  writeThresh(20);
  s1 = "#60350";
  writeThresh(25);
  s1 = "#70100";
  writeThresh(30);
  s1 = "#80230";
  writeThresh(35);
  s1 = "#p0063";
  writeThresh(40);

  EEPROM.write(50, '1');
  Serial.println("hardcoding values");


}
