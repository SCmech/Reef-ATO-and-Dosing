/* 
* doesn't display "purging" or "mixing"
* change actions of button on first screen?
* timing issues with the encoder. some variables change together
* 
* add max dosing - not started
* 
* save some variables to eeprom
*/

#define FIRMWARE "2.0.14"
#define LASTEDIT "2020.06.05"

#include "timers.h"
#include <nhOLED.h>
#include <SPI.h>
#include <Wire.h>
#include "i2cEncoderLib.h"
#include "RTClib.h"
RTC_PCF8523 rtc;

const byte ROW_N = 4  ;                       // Number of display rows
const byte COLUMN_N = 20;                     // Number of display columns
const byte CS = 2;                            // Arduino's pin assigned to the /CS line
const byte RES = NO_PIN;                      // Arduino's pin assigned to the Reset line (optional, can be always high)
const byte SCLK = 13;                         // Arduino's pin assigned to the SCLK line
const byte SDIN = 11;                         // Arduino's pin assigned to the SID line
byte new_line[4] = {0x80, 0xA0, 0xC0, 0xE0};  // DDRAM address for each line of the display
nhOLED oled(ROW_N, COLUMN_N, SDIN, SCLK, CS, RES);

//encoder variables and setup
const byte IntPin = A0 ; //A6 cant be used
i2cEncoderLib encoder(0x30); //Initialize the I2C encoder class with the I2C address 0x30 is the address with all the jumper open
int32_t counter;
uint8_t encoder_status;

//pins
const byte lowInPin = A2; 
const byte highInPin = 8; 
const byte maxInPin = 7; 
const byte out1Pin = 6; 
const byte out2Pin = 5; 
const byte out3Pin = 4; 
const byte out4Pin = 3; 

boolean levelLowState = 0;
boolean levelHighState = 0; //should this be a boolean?
boolean levelMaxState = 0;

byte refillSpeed = 150; //0-255
const byte min1Speed = 150;
const byte max1Speed = 255;
byte dosingSpeed = 200; //2g per 1000ms 
byte mixSpeed = 255; 
const byte min2Speed = 100;
const byte max2Speed = 255;

//new variables 2020.05.05
byte maxRefillSeconds = 30; //in seconds
boolean maxRefillState = LOW;
byte maxRefillCounter = 0;
byte refillDelaySeconds = 50;
boolean refillDelayState = LOW;
byte refillDelayCounter = 0;
boolean previousFeedState = LOW;

//counters for refill
int refillSeconds = 0;
int refillCounter = 0; //records seconds per day 
int lastRefillCounter = 0; //yesterdays counter
boolean refillCounterState = LOW;
//maxRefill = 15000; not sure about this yet

byte mixDelayHours = 8; //how often to mix
byte mixDelayCounter = 0;

int mixDurationSeconds = 50; //how long to mix for
int mixDurationCounter = 0;
boolean mixState = LOW;

int mixDelayStateMinutes = 20; //how long to block feeding
int mixDelayStateCounter = 0;
boolean mixDelayState = LOW; //do not feed if HIGH

boolean feedState = LOW;
int feedDurationCounter = 0; //track time feed is on

int purgeDuration = 10; //seconds to run during purge event
int purgeDurationCounter = 0; 
boolean purgeState = LOW;

int currentMinute = 0, currentHour = 0, currentSecond = 0, currentDay = 0; 
int previousMinute = -1, previousHour = -1, previousSecond = -1, previousDay = -1;

int timeLimitSeconds = 60; //Before screen reverts to main menu
int timeLimitCounter = 0;

byte displayStatus = 0; //tracks the state of the device. Needs some thought
byte displayStatusOld = 0;
byte currentLine = 0; //active row in the change menu
byte minLine = 0; //in a given menu, this is the first line that can be active
byte maxLine = 0; //in a given menu, this is the last line'
byte screenPosition = 0;
byte menu = 0; //status screen or active screen
boolean incrementVariable = 0; //switches between moving the cursor and changing a variable
boolean screenUpdate = HIGH; 

//boolean errorState1 = 0; //over the timelimit

void setup(void)
{
  SPI.begin();
  oled.begin();        // Setup control pins and initialize OLED controller

  pinMode(IntPin, INPUT_PULLUP);
  pinMode(out1Pin, OUTPUT);
  pinMode(out2Pin, OUTPUT);
  pinMode(out3Pin, OUTPUT);
  pinMode(out4Pin, OUTPUT);    
  pinMode(lowInPin, INPUT);
  pinMode(highInPin, INPUT);
  pinMode(maxInPin, INPUT_PULLUP);
     
  Serial.begin(57600);
  //Serial1.begin(57600);
  Serial.println(FIRMWARE);
  oled.setCursor(1,0);
  oled.print("ATO ");
  oled.print(FIRMWARE);
  oled.setCursor(1,3);
  oled.print(LASTEDIT);

  encoder.begin((INTE_ENABLE | LEDE_DISABLE | WRAP_DISABLE | DIRE_RIGHT | IPUP_DISABLE | RMOD_X1 ));
  encoder.writeCounter(0);
  
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //sets time to match compiled time
  DateTime now = rtc.now();
  currentMinute = now.minute();
  currentHour = now.hour();

  Serial.print("start up time: ");
  Serial.print(currentHour);
  Serial.print(":");
  Serial.println(currentMinute);
  Serial.println(); 
  delay(2000);

  setMenuLimits(menu);
}

void jumpScreen(byte m){
  Serial.println("reseting display 0,1");
  menu = m;
  setMenuLimits(menu);
  incrementVariable = 0;
  screenPosition = 0;
  currentLine = 0;
  screenUpdate = HIGH;
}

void updateStates(){
  levelHighState = digitalRead(highInPin);
  if (levelHighState == HIGH){
    maxRefillState = LOW;
    //refillDelayState = LOW;    
  }
  levelLowState = digitalRead(lowInPin);
  levelMaxState = digitalRead(maxInPin);   
}

void purgeEvent() {
  //first check for overflow condition.
  //Serial.println("purge event");
  purgeState = HIGH;
  purgeDurationCounter = 0;
  jumpScreen(0);
}

void mixEvent(){
  mixState = HIGH;
  mixDelayState = HIGH;  
  mixDelayStateCounter = 0;
  mixDurationCounter = 0;
  mixDelayHours = 0; 
  jumpScreen(0);
}

void motorStates(){
  previousFeedState = feedState;
  displayStatusOld = displayStatus;
  
  if (levelMaxState == HIGH){ //can add water 
    if (levelHighState == LOW && mixDelayState == LOW && refillDelayState == LOW){
      feedState = HIGH;  
      maxRefillState = HIGH;
      //then determine which displayState
      if (purgeState != HIGH){
        displayStatus = 1; //refilling    
      }
      else {
        displayStatus = 5; //purging        
      }
    }
    else { //do not run and update the display state
      feedState = LOW;
     
      //then determine which displayState
      if (refillDelayState == HIGH) {
        displayStatus = 7; //Mixing
      }
      else if (mixDelayState == HIGH){
        displayStatus = 2; //mixDelayState
      }
      else if (mixState == HIGH) {
        displayStatus = 3; //Mixing
      }
      else {
        displayStatus = 0; //idle
      }  
     }
  }   
  else {
      feedState = LOW;
      purgeState = LOW;
      displayStatus = 4; //overflow
  }

  if (previousFeedState == HIGH && feedState == LOW){ //are we transitioning from feeding?
    Serial.print("pump on for: ");
    Serial.println(feedDurationCounter);
    feedDurationCounter = 0;      
  }
 
  if (displayStatus != displayStatusOld){
    //Serial.println(displayStatus);
    //Serial.println(displayStatusOld);
    screenUpdate = HIGH;  
  }  
  
  //Turn the feed motor on or off
  if (feedState == HIGH || purgeState == HIGH){
    analogWrite (out1Pin, refillSpeed);   

    if (previousFeedState == LOW){
      //refillSeconds = 0; 
      refillCounterState = HIGH; //turn on counter
    }
  }
  else{
    analogWrite (out1Pin, 0);
    if (previousFeedState == HIGH){
      //turn on counter
      refillCounter = refillCounter + refillSeconds;
      refillSeconds = 0; 
      refillCounterState = LOW;
    }  
  }

  //turn the mix motor on or off
  if (mixState == HIGH){
    analogWrite (out2Pin, mixSpeed);   
  }
  else{
    analogWrite (out2Pin, 0);   
  }  
}

void checkTimers(){
  for (byte i = 0; i < 12; i++){
    if (allEvents[i].hour == currentHour && allEvents[i].minute == currentMinute){
    timedEvent(allEvents[i].pump, allEvents[i].duration);    
    }
  }
}

void timedEvent(byte p, int d){
  analogWrite(out1Pin, 0);
  analogWrite(out2Pin, 0);
  analogWrite(out3Pin, 0);
  analogWrite(out4Pin, 0);
  analogWrite(p, dosingSpeed); 
  oled.clear();
  oled.print("Dispense ");
  if (p == 3){
    oled.print("Calcium");
  }
  else{
    oled.print("Magnesium");
  }
  delay(d);
  analogWrite(p, 0);
  purgeEvent();
}

void loop(void)
{
  DateTime now = rtc.now(); //check the time and fill in variables
  currentSecond = now.second(), DEC;
  currentMinute = now.minute(), DEC;
  currentHour = now.hour(), DEC;
  currentDay = now.day(), DEC;
  readEncoder();  
  updateStates();

  if (previousDay != currentDay)  {
    previousDay = currentDay;    

    lastRefillCounter = refillCounter;
    refillCounter = 0;
  }

  if (previousMinute != currentMinute)  {
    previousMinute = currentMinute;    
    checkTimers();
  }
  
  if (previousSecond != currentSecond)  {
    previousSecond = currentSecond;

    if (refillCounterState == HIGH){
      refillSeconds++;
    }

    if (maxRefillState == HIGH){
      maxRefillCounter ++;   
      //Serial.println(maxRefillCounter);   
      if (maxRefillCounter >= maxRefillSeconds){
        //
        maxRefillCounter = 0;
        maxRefillState = LOW;
        refillDelayState = HIGH;       
      }
    }
    if (refillDelayState == HIGH){
      refillDelayCounter ++;
      if (refillDelayCounter >= refillDelaySeconds){
        refillDelayCounter = 0;
        refillDelayState = LOW;
      }
    }
    
    if (feedState == HIGH){
      feedDurationCounter ++;
    }
    if (purgeState == HIGH){
      purgeDurationCounter ++;
      if (purgeDurationCounter >= purgeDuration){
        purgeDurationCounter = 0;       
        purgeState = LOW;
      }
    }
    //turn off mix motor
    if (mixState == HIGH){
      mixDurationCounter ++;
      if (mixDurationCounter >= mixDurationSeconds){
        mixDurationCounter = 0;
        mixState = LOW;
      }      
    }

    ////return to status menu
    timeLimitCounter ++;
    if (timeLimitCounter >= timeLimitSeconds){
      timeLimitCounter = 0;
      Serial.println("time limit counter");
      jumpScreen(0);
    } 
  }

  if (previousMinute != currentMinute)  {
    previousMinute = currentMinute;
    if (mixDelayState == HIGH){
      mixDelayStateCounter ++;  
      if (mixDelayStateCounter >= mixDelayStateMinutes){
        mixDelayStateCounter = 0;
        mixDelayState = LOW;
      }
    }
  }  

  if (previousHour != currentHour)  {
    previousHour = currentHour;
    mixDelayCounter ++;
    if (mixDelayCounter >= mixDelayHours){
      mixDelayCounter = 0;
      mixState = HIGH;
      mixDelayState = HIGH;
    }
  }
  
  //set state
  motorStates();  

  //update screen
  if (screenUpdate == HIGH){
    writeDynamicScreen();
    screenUpdate = LOW;
  }   
}
