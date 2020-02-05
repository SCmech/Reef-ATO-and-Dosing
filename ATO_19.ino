/* 
* 
* 
* 
* send info to the LCD controller
* save some variables to eeprom
*/

#include <nhOLED.h>
#include <SPI.h>
#include <Wire.h>
#include "i2cEncoderLib.h"
#include "RTClib.h"
RTC_PCF8523 rtc;

const byte ROW_N = 4  ;                 // Number of display rows
const byte COLUMN_N = 20;             // Number of display columns
const byte CS = 2;                    // Arduino's pin assigned to the /CS line
const byte RES = NO_PIN;              // Arduino's pin assigned to the Reset line (optional, can be always high)
const byte SCLK = 13;                 // Arduino's pin assigned to the SCLK line
const byte SDIN = 11;                 // Arduino's pin assigned to the SID line
byte new_line[4] = {0x80, 0xA0, 0xC0, 0xE0};               // DDRAM address for each line of the display
nhOLED oled(ROW_N, COLUMN_N, SDIN, SCLK, CS, RES);

//encoder variables and setup
const byte IntPin = A0 ; //A6 cant be used
i2cEncoderLib encoder(0x30); //Initialize the I2C encoder class with the I2C address 0x30 is the address with all the jumper open
int32_t counter;
uint8_t encoder_status;

const byte lowInPin = 9; 
const byte highInPin = 8; 
const byte maxInPin = 7; 
const byte out1Pin = 6; 
const byte out2Pin = 5; 
const byte out3Pin = 4; 
const byte out4Pin = 3; 

boolean levelLowState = 0;
byte levelHighState = 0;
boolean levelMaxState = 0;
byte out1Speed = 200; //0-255
const byte min1Speed = 150;
const byte max1Speed = 255;

const byte min2Speed = 100;
const byte max2Speed = 150;
byte out2Speed = 125; 

byte mixDelayHours = 8; //how often to mix
byte mixDelayCounter = 0;

int mixDurationSeconds = 50; //how long to mix for
int mixDurationCounter = 0;
boolean mixState = LOW;

int feedDelayMinutes = 20; //how long to block feeding
int feedDelayCounter = 0;
boolean feedDelay = LOW; //do not feed if HIGH

boolean feedState = LOW;
int feedDurationCounter = 0; //track time feed is on

int purgeDuration = 10; //seconds to run during purge event
int purgeDurationCounter = 0; 
boolean purgeState = LOW;


int currentMinute = 0, currentHour = 0, currentSecond = 0;
int previousMinute = -1, previousHour = -1, previousSecond = -1;

int timeLimitSeconds = 40; //Before screen reverts to main menu
int timeLimitCounter = 0;

byte statusVariable = 0; //tracks the state of the device. Needs some thought
byte statusVariableOld = 0;
byte currentLine = 0; //active row in the change menu
byte minLine = 0; //in a given menu, this is the first line that can be active
byte maxLine = 0; //in a given menu, this is the last line'
byte screenPosition = 0;
boolean menu = 0; //status screen or active screen
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
  //Serial.print("ATO_19_2020.01.11");
  oled.print("ATO v19 2020.01.11");

  encoder.begin((INTE_ENABLE | LEDE_ENABLE | WRAP_DISABLE | DIRE_LEFT | IPUP_ENABLE | RMOD_X1 ));
  //encoder.writeCounter(0);
  //encoder.writeMax(10); //Set maximum threshold
  //encoder.writeMin(-10); //Set minimum threshold
  //encoder.writeLEDA(0x00);
  //encoder.writeLEDB(0x00);  

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

void setMenuLimits(byte i){
  switch(i){
    case 0:
      minLine = 1;
      maxLine = 3;
      return;
    case 1:
      minLine = 0;
      maxLine = 6;          
  }
}

void updateStates(){
  levelHighState = digitalRead(highInPin);
  levelLowState = digitalRead(lowInPin);
  levelMaxState = digitalRead(maxInPin);   
}


void purgeEvent() {
  //first check for overflow condition.
  //Serial.println("purge event");
  purgeState = HIGH;
  purgeDurationCounter = 0;
  menu == 0;
  motorStates(); 
  writeScreen();
}

void mixEvent(){
  mixState = HIGH;
  feedDelay = HIGH;

  feedDelayCounter = 0;
  mixDurationCounter = 0;
  mixDelayHours = 0;  
  menu == 0;
  motorStates(); 
  writeScreen();
}

void motorStates(){
  if (levelMaxState == HIGH){ //can add water 
    if (levelHighState == LOW && feedDelay == LOW){
      //analogWrite(out1Pin, out1Speed);
      statusVariable = 1; //refilling
      feedState = HIGH;  
    }
    else {
      if (mixState == HIGH) {
        statusVariable = 3; //Mixing
      }
      else if (feedDelay == HIGH){
        statusVariable = 2; //feedDelay
      }
      else {
        statusVariable = 0; //idle
      }
      feedState = LOW;
    } 
    if (purgeState == HIGH){
      statusVariable = 5; //purging     
    } 
  }
  else { //LOW do not add water but mixing the kalk is ok
    statusVariable = 4; //overflow!  

    if (feedState == HIGH){ //are we transitioning from feeding?
      Serial.print("pump on for: ");
      Serial.println(feedDurationCounter);
      feedDurationCounter = 0;      
    }
    feedState = LOW; 
    purgeState = LOW;
  }

  if (statusVariable != statusVariableOld){
    screenUpdate = HIGH;  
  }
  statusVariable = statusVariableOld;

  //Turn the feed motor on or off
  if (feedState == HIGH || purgeState == HIGH){
    analogWrite (out1Pin, out1Speed);   
  }
  else{
    analogWrite (out1Pin, 0);
  }

  //turn the mix motor on or off
  if (mixState == HIGH){
    analogWrite (out2Pin, out2Speed);   
  }
  else{
    analogWrite (out2Pin, 0);   
  }  
}

void loop(void)
{
  DateTime now = rtc.now(); //check the time and fill in variables
  currentSecond = now.second(), DEC;
  currentMinute = now.minute(), DEC;
  currentHour = now.hour(), DEC;
  readEncoder();
  updateStates();
 
  if (previousSecond != currentSecond)  {
    previousSecond = currentSecond;
    
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
      menu = 0; //return to status menu
    } 
    //updateStates(); //update input states. will work on debouncing later...   
    if (menu == 0){
      writeScreen();
    }
  }

  if (previousMinute != currentMinute)  {
    previousMinute = currentMinute;
    if (feedDelay == HIGH){
      feedDelayCounter ++;  
      if (feedDelayCounter >= feedDelayMinutes){
        feedDelayCounter = 0;
        feedDelay = LOW;
      }
    }
  }  

  if (previousHour != currentHour)  {
    previousHour = currentHour;
    mixDelayCounter ++;
    if (mixDelayCounter >= mixDelayHours){
      mixDelayCounter = 0;
      mixState = HIGH;
      feedDelay = HIGH;
    }
  }
  
  //set state
  motorStates();  

  if (screenUpdate == HIGH){
    if (menu == 0){
      writeScreen();  
    }
    if(menu == 1){
      writeDynamicScreen();
    }
    screenUpdate = LOW;
  }  
}
