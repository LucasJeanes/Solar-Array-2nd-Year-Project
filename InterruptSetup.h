#define timerUpdate 20000000     //Timing interval of timer interrupt trigger (In microseconds) - currently 5 seconds
hw_timer_t *Timer0_Cfg = NULL;
bool timeUpdated = true;
bool motorUpdateTimer = true;
bool gps_TimeOutFlag = false;
int timeOutReset = 0;
int motorUpdateCount = 0;

void TimerSetup();

void IRAM_ATTR timeUpdate_ISR() {    //Manual clock update via Timer Interrupt, also updates timing flag to control serial print frequency
  timeUpdated = true;
  motorUpdateCount+=20;
  if(motorUpdateCount >= 240) {     //4 minutes for motors to update ie. 1 degree
    motorUpdateTimer = true;
    motorUpdateCount = 0;
  }
  if(gps_TimeOutFlag == true) {
    gps_SecondValue+=20;
    if(gps_SecondValue == 60) {
      gps_MinuteValue++;
      gps_SecondValue = 0;
    }
    if(gps_MinuteValue == 60) {
      gps_HourValue++;
      gps_MinuteValue = 0;
    }
    if(gpsFlag == false) {
      timeOutReset+=20;
    }
  }
}
//Initiate and setup timer interrupt
void TimerSetup() {
  Timer0_Cfg = timerBegin(0,80,true);                         //Setup Timer0 with prescalar of 80, Counts Up
  timerAttachInterrupt(Timer0_Cfg, &timeUpdate_ISR, true);    //Attach ISR (timeUpdate_ISR) to timer0 
  timerAlarmWrite(Timer0_Cfg,timerUpdate,true);
  timerAlarmEnable(Timer0_Cfg);
}
