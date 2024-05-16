int analogpin=2;
int readpin=18;
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//#define PERIOD_EXAMPLE_VALUE      (0xFFFF)
//#define DUTY_CYCLE_EXAMPLE_VALUE    (0x8000)

void setup() {
  Serial.begin(9600);
  pinMode(analogpin,OUTPUT);
  pinMode(readpin,INPUT);


  delay(100);

          lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Starting...");
delay(500);
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

//     TCA1.SINGLE.CTRLB = TCA_SINGLE_CMP2EN_bm                    /* enable compare channel 2 */
//                      | TCA_SINGLE_WGMODE_SINGLESLOPE_gc ;    /* single-slope PWM mode */
//          
//  TCA1.SINGLE.PER = PERIOD_EXAMPLE_VALUE;     /* set PWM frequency*/
//  TCA1.SINGLE.CMP2 = DUTY_CYCLE_EXAMPLE_VALUE;  /* set PWM duty cycle*/
//  
//  TCA1.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc  /* set clock source (main_clk/16) */
//            | TCA_SINGLE_ENABLE_bm;   /* start timer */
}

void loop() {
  
analogWrite(analogpin,128);

float data1=analogRead(readpin);
Serial.println(data1);

}
