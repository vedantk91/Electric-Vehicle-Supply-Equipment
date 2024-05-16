int analogpin = 2;


//internal 16 
void setup() {
  pinMode(analogpin, OUTPUT);
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
 
 
  analogWrite(analogpin, 128);

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
 
}

void loop() {



}
