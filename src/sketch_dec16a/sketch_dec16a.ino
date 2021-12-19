#include <LiquidCrystal.h>

#define pushButton 3
#define mutePin 2

String getShortTime(long eventTime);
long getLongTime();
void updateTimeBySeconds(int x);
void printTime();
void updateTime();
void muteDevice();
void buttonFunction();
void play(float hz, int wait);
void alarm();

// =============================================================================

//TOKEN HERE PLEASE

long time[3] = {8,59,58};
int events = 3;
String eventName[3] = {"IC152: Lecture", "IC160: Lecture", "IC140: Lecture"};
long eventStartTime[3] = {(long)9*3600, (long)10*3600, (long)11*3600};
long eventEndTime[3] = {(long)10*3600, (long)11*3600, (long)12*3600};
int eventScrollingSpeed = 4;
long waterReminder = (long)3*3600;
long breakReminder = (long)4*3600;
long skippingBreak = (long)17*3600;
String userMode = "manual";


//TOKEN ENDS HERE

// =============================================================================
int currentSpeaker = 4;
int currentEvent = 0;
int timeWhileAlarmOn;
bool recordingTimeAlarmOn = false;
int eventScrollingSpeedCounter = 0;
bool notHourZero = true;
bool notMinZero = true;
bool notSecZero = true;
String mode = userMode;
int passedEvents = 0;
int timeForSetup;
bool alarmOn = false;
LiquidCrystal lcd(9,8,13,12,11,10);
LiquidCrystal lcd2(9,7,13,12,11,10);

void setup() {
  timeForSetup = millis();
  lcd.begin(16,2);
  lcd.begin(16,2);



  pinMode(pushButton, INPUT);
  pinMode(mutePin, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd2.begin(16,2);

  //generate Events list
  int t = getLongTime();
  long healthEvents[3] = {t+waterReminder, t+breakReminder, skippingBreak};
  if(digitalRead(mutePin) == LOW){mode = "muted  ";}
  attachInterrupt(digitalPinToInterrupt(mutePin), muteDevice, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pushButton), buttonFunction, FALLING);

  updateTimeBySeconds((millis()-timeForSetup)/1000);
}

void loop() {
  if(!digitalRead(mutePin) && !recordingTimeAlarmOn && alarmOn){
    recordingTimeAlarmOn = true;
    timeWhileAlarmOn = millis();
  }
  if(getLongTime() == eventStartTime[passedEvents] && !recordingTimeAlarmOn){
    timeForSetup = millis();
    alarmOn = true;
    lcd.clear(); lcd2.clear();
    lcd.print("ALARM"); lcd.setCursor(0,1); lcd.print("Hurry UP !!!");
    lcd2.print(eventName[passedEvents]); lcd2.setCursor(0,1);
    lcd2.print(getShortTime(eventStartTime[passedEvents])); lcd2.print(" - "); lcd2.print(getShortTime(eventEndTime[passedEvents]));
  }
  
  if(getLongTime() >= eventStartTime[passedEvents]){
  if(digitalRead(mutePin)){alarm();}
  }else{
  
  timeForSetup = millis();
  lcd.clear(); lcd2.clear();
  lcd.setCursor(0,0); lcd.print("mode: ");lcd.print(mode); printTime();
  lcd2.setCursor(0,0); lcd2.print(eventName[currentEvent]); lcd2.setCursor(0,1); 
  lcd2.print(getShortTime(eventStartTime[currentEvent])); lcd2.print(" - "); lcd2.print(getShortTime(eventEndTime[currentEvent]));
  eventScrollingSpeedCounter++;
  
  if(eventScrollingSpeedCounter == eventScrollingSpeed){
    currentEvent++; eventScrollingSpeedCounter = 0;
    if(currentEvent == events){currentEvent = 0 + passedEvents;}
    }
    
  updateTime();
  delay(1000-(millis()-timeForSetup)/1000);
  }
}

long getLongTime(){
  return time[0]*3600 + time[1]*60 + time[2];
}
  
String getShortTime(long eventTime){
  String s1 = String(eventTime/3600);
  if(s1.length() == 1){s1 = "0"+s1;}
  String s2 = String((eventTime%3600)/60);
  if(s2.length() == 1){s2 = "0"+s2;}
  return s1 + ":" + s2;
}

void updateTimeBySeconds(int x){
  long temp = getLongTime() + x;
  time[0] = temp / 3600;
  time[1] = (temp%3600)/60;
  time[2] = temp%60;
}

void printTime(){
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  for(int i = 0; i<3; i++){
    if(time[i]<10){lcd.print(0);}
    lcd.print(time[i]);
    if(i != 2){lcd.print(":");}
  }
}

void updateTime(){ 
  if(time[2]>58){
    if(time[1]>58){
      if(time[0]>22){
        time[0] = 0;
        notHourZero = false;
      }
      if(notHourZero){time[0]++;}
      notHourZero = true;
      time[1] = 0;
      notMinZero = false;
    }
    if(notMinZero){time[1]++;}
    notMinZero = true;
    time[2] = 0;
    notSecZero = false;
  }
  if(notSecZero){time[2]++;}
  notSecZero = true;
}

void muteDevice(){
  if(digitalRead(mutePin) == LOW){
    mode = "muted  ";}
  else if(digitalRead(mutePin) == HIGH){
    mode = userMode;}
}
  
void buttonFunction(){
  if(alarmOn == true){
    alarmOn = false;
    passedEvents++;
    lcd.clear(); lcd2.clear();
    lcd.print("Alarm Muted!"); lcd.setCursor(0,1);
    lcd.print("Next Alarm -->");
    lcd2.print(eventName[passedEvents]); lcd2.setCursor(0,1);
    lcd2.print(eventStartTime[passedEvents]); lcd2.print(" : "); lcd2.print(eventEndTime[passedEvents]);
    if(recordingTimeAlarmOn){updateTimeBySeconds((millis()-timeWhileAlarmOn)/1000); recordingTimeAlarmOn = false;}
  }
}

/*
 * Credits for Tune Playing Code
 * Original Melody from All Star, by Smash Mouth
 * Inspiration from https://workshops.hackclub.com/coffin_dance_arduino/
 * Music code from bansalrohan18
 * https://www.tinkercad.com/things/fMHcz0D6nag
 */
void play(float hz, int wait) {

  tone(currentSpeaker, hz, 1000);
  delay(200);
  noTone(currentSpeaker);
  delay(wait);
  
  if(currentSpeaker == 4) {
    currentSpeaker += 1;
  } else {
      currentSpeaker -= 1;
  }    
}

void alarm() {
  int alarmTime = millis();
  float hertz[] = {392, 587.33, 493.88, 493.88, 440, 392, 392, 523.25, 493.88, 493.88, 440, 440, 392, 392, 523.25, 493.88, 493.88, 440, 440, 392, 440, 329.63};
  float wait[] = {200, 70, 100, 300, 100, 200, 130, 200, 70, 70, 70, 70, 200, 70, 70, 70, 70, 70, 70, 70, 70, 1000};
  for(int i = 0; i < 22; i++){
    play(hertz[i], wait[i]);
    }
  updateTimeBySeconds((millis()-alarmTime)/1000);
  
}