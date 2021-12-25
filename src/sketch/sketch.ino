/*
 * --------------------------------------------------------------------------------------
 * Team FreshBOTS submittion to BUILDABOT Contest.
 * 
 * This is a project to build a Events Manager/Reminder Bot using Arduino
 * Features include alarms for classes, reminders to stay hydrated, take a walk/break
 * Every 15 mins it goes through all the upcoming events so youre refreshed every moment
 * and structure your day on the fly.
 * 
 * Read Full Documentation here:
 * Github Repository:
 * ---------------------------------------------------------------------------------------
 * 
 * ==============================================================================
 *   CREDITS for Tune Playing Program                                      
 *   Original Melody from All Star, by Smash Mouth
 *   Inspiration from https://workshops.hackclub.com/coffin_dance_arduino/
 *   Music code from bansalrohan18
 *   https://www.tinkercad.com/things/fMHcz0D6nag
 * ==============================================================================
 * 
 * HOW TO USE
 * ==========
 * 
 * Use generate_token.py to integrate your google calender and generate a summary of all events today.
 * You can also use this python script to set reminder durations for reccuring events like StayHydrated
 * Read full descriptions of functions in generate_token.py
 * 
 * Copy the token generated by the above file and replace it with the token variable here.
 * Compile and Upload to Arduino.
 * 
 * ======================================================================================================
 * NOTE: If your Arduino Board loses power, it will reset the time to when you last uploaded the sketch
 * You need to generate a new token as it contains time and upload it again to the arduino.
 * ======================================================================================================
 * 
 */
// ==============================================================================
// ==============================================================================

// TOKEN HERE PLEASE

long time[3] = {8,59,58};
int events = 3;
String eventName[3] = {"IC152: Lecture", "IC160: Lecture", "IC140: Lecture"};
long eventStartTime[3] = {(long)9*3600, (long)10*3600, (long)11*3600};
long eventEndTime[3] = {(long)10*3600, (long)11*3600, (long)12*3600};
int eventScrollingSpeed = 4;
long waterReminder = (long)3*3600;
long breakReminder = (long)4*3600;
long eveningBreak = (long)17*3600;
String userMode = "manual";


// TOKEN ENDS HERE
// =============================================================================
// =============================================================================

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
int currentSpeaker = 4;
int currentEvent = 0;
long timeWhileAlarmOn1;
bool recordingTimeAlarmOn1 = false;
long timeWhileAlarmOn2;
bool recordingTimeAlarmOn2 = false;
int eventScrollingSpeedCounter = 0;
bool notHourZero = true;
bool notMinZero = true;
bool notSecZero = true;
String mode = userMode;
int passedEvents = 0;
int breakPassedEvents = 1;
int waterPassedEvents = 1;
long timeForSetup;
bool alarmOn1 = false;
bool alarmOn2 = false;
bool alarmHealthEvents = true; 
bool alarmNormalEvents = true;
long waterBreak[8];
long breakBreak[6];
int currentHealthEvent = 0;
LiquidCrystal lcd(9,8,13,12,11,10);
LiquidCrystal lcd2(9,7,13,12,11,10);

void setup() {
  timeForSetup = millis();
  lcd.begin(16,2);
  lcd.begin(16,2);



  pinMode(pushButton, INPUT);
  pinMode(mutePin, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd2.begin(16,2);

  long cur_time = getLongTime();
  int var = 0;
  while(cur_time + var*waterReminder < 86399){
    waterBreak[var] = cur_time + var*waterReminder;
    var++;
  }

  var = 0;
  while(cur_time + var*breakReminder < 86399){
    breakBreak[var] = cur_time + var*breakReminder;
    var++;
  }
  
  if(mode == "events"){alarmHealthEvents = false;}
  else if(mode == "health"){alarmNormalEvents = false;}
  else if(mode == "silent"){alarmHealthEvents = false; alarmNormalEvents = false;}
  
  if(digitalRead(mutePin) == LOW){mode = "muted";}
  attachInterrupt(digitalPinToInterrupt(mutePin), muteDevice, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pushButton), buttonFunction, FALLING);

  updateTimeBySeconds((millis()-timeForSetup)/1000);
}

void loop() {
  long currentTime = getLongTime();
  if(alarmNormalEvents && !alarmOn2){
    if(!digitalRead(mutePin) && !recordingTimeAlarmOn1 && alarmOn1){
      recordingTimeAlarmOn1 = true;
      timeWhileAlarmOn1 = millis();
    }
    if(currentTime == eventStartTime[passedEvents] && !alarmOn1){
      timeForSetup = millis();
      alarmOn1 = true;
      lcd.clear(); lcd2.clear();
      lcd.print("ALARM"); lcd.setCursor(0,1); lcd.print("Hurry UP !!!");
      lcd2.print(eventName[passedEvents]); lcd2.setCursor(0,1);
      lcd2.print(getShortTime(eventStartTime[passedEvents])); lcd2.print(" - "); lcd2.print(getShortTime(eventEndTime[passedEvents]));
    }
    
    if(alarmOn1 && digitalRead(mutePin)){alarm();}
  }
  if(alarmHealthEvents && !alarmOn1){
    if(!digitalRead(mutePin) && !recordingTimeAlarmOn2 && alarmOn2){
      recordingTimeAlarmOn2 = true;
      timeWhileAlarmOn2 = millis();
    }
    if(!alarmOn2 && currentTime == waterBreak[waterPassedEvents]){
      timeForSetup = millis();
      alarmOn2 = true;
      currentHealthEvent = 0;
      lcd.clear(); lcd2.clear();
      lcd.print("Water Reminder"); lcd.setCursor(0,1); lcd.print("Stay Hydrated!");
      lcd2.print("Last Drink At"); lcd2.setCursor(0,1);
      lcd2.print(getShortTime(waterBreak[waterPassedEvents-1]));
    }
    if(!alarmOn2 && currentTime == breakBreak[breakPassedEvents]){
      timeForSetup = millis();
      alarmOn2 = true;
      currentHealthEvent = 1;
      lcd.clear(); lcd2.clear();
      lcd.print("Break Reminder"); lcd.setCursor(0,1); lcd.print("Rest your eyes!");
      lcd2.print("Last Break At"); lcd2.setCursor(0,1);
      lcd2.print(getShortTime(breakBreak[breakPassedEvents-1]));
    }
    if(!alarmOn2 && currentTime == eveningBreak){
      timeForSetup = millis();
      alarmOn2 = true;
      currentHealthEvent = 2;
      lcd.clear(); lcd2.clear();
      lcd.print("Evening Break"); lcd.setCursor(0,1); lcd.print("Go for a walk?");
      lcd2.print("Some exercise?"); lcd2.setCursor(0,1); lcd2.print("Some coffee?");
    }
    if(alarmOn2 && digitalRead(mutePin)){alarm();}
  }
  if(!alarmOn1 && !alarmOn2){
  
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
  delay(975-(millis()-timeForSetup)/1000);
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
  if(alarmOn1){
    alarmOn1 = false;
    passedEvents++;
    lcd.clear(); lcd2.clear();
    lcd.print("Alarm Muted!"); lcd.setCursor(0,1);
    lcd.print("Next Alarm -->");
    lcd2.print(eventName[passedEvents]); lcd2.setCursor(0,1);
    lcd2.print(getShortTime(eventStartTime[passedEvents])); lcd2.print(" : "); lcd2.print(getShortTime(eventEndTime[passedEvents]));
    if(recordingTimeAlarmOn1){updateTimeBySeconds((millis()-timeWhileAlarmOn1)/1000); recordingTimeAlarmOn1 = false;}
  }
  else if(alarmOn2){
    alarmOn2 = false;
    if(currentHealthEvent == 0){waterPassedEvents++;}
    else if(currentHealthEvent == 1){breakPassedEvents++;}
    lcd.clear(); lcd2.clear();
    lcd.print("Alarm Muted!"); lcd.setCursor(0,1);
    if(recordingTimeAlarmOn2){updateTimeBySeconds((millis()-timeWhileAlarmOn2)/1000); recordingTimeAlarmOn2 = false;}
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
    if(alarmOn1 || alarmOn2){play(hertz[i], wait[i]);}
  }
  updateTimeBySeconds((millis()-alarmTime)/1000);
  
}