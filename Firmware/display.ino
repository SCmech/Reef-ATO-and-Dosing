
//first menu, then line
void writeScreenLine(byte i){
  switch(menu){
    case 0:
      switch(i){
        case 0:
          //oled.print(" ");
          writeStatus();
          lcdPrint24time(currentHour, currentMinute); //then time
          return;
        case 1:
          oled.print("Hi: ");
          if (levelHighState == HIGH){
            oled.print("0"); 
          }
          else {
            oled.print("X"); 
          }
          oled.setCursor(7,i);
          oled.print("Lo: ");
          if (levelLowState == HIGH){
            oled.print("0"); 
          }
          else {
            oled.print("X"); 
          } 
          oled.setCursor(13,i);
          oled.print("Fs: ");
          if (levelMaxState == HIGH){
            oled.print("0"); 
          }
          else {
            oled.print("X"); 
          } 
          return;
        case 2:
          oled.setCursor(1,i);  
          oled.print("Cnt: ");
          oled.print(refillCounter); 
          oled.print(", ");
          oled.print(lastRefillCounter); 
          return;
        case 3:
          oled.print("Mg: xxx ");
          return;                              
      }
      return;
    case 1:
      switch(i){
        case 0:
          oled.print("Purge Event");
          return;
        case 1:
          oled.print("Mix Event ");
          return;
        case 2:
          oled.print("Fill Speed: ");
          oled.print(refillSpeed);
          return;
        case 3:
          oled.print("Mix Speed: ");
          oled.print(mixSpeed);
          return;
        case 4:
          oled.print("Mix Delay(H): ");
          oled.print(mixDelayHours);
          return;
        case 5:
          oled.print("Mix Duration(S): ");
          oled.print(mixDurationSeconds);
          return;
        case 6:
          oled.print("RETURN");
          return;                                        
      }
      //return;
  }
}

//need new case for each new menu.  Or make it a struct
void setMenuLimits(byte i){ 
  switch(i){ //menu
    case 0:
      minLine = 0;
      maxLine = 3;
      return;
    case 1:
      minLine = 0;
      maxLine = 6;          
  }
}

void writeDynamicScreen(){
  //Serial.print("line, position, min, max ");
  //Serial.print(currentLine);
  //Serial.print(screenPosition);
  //Serial.print(minLine);
  //Serial.println(maxLine);
  oled.clear();

  if (menu != 0){
    writeCursor();  
    oled.setCursor(1,0);  
  }
  /*
  writeCursor();  
  oled.setCursor(1,0);  
  */
  
  switch(screenPosition){
    case 0:
      writeScreenLine(currentLine);
      oled.setCursor(1,1);
      writeScreenLine(currentLine + 1);
      oled.setCursor(1,2);
      writeScreenLine(currentLine + 2);
      oled.setCursor(1,3);
      writeScreenLine(currentLine + 3);
      return;
    case 1:
      writeScreenLine(currentLine - 1);
      oled.setCursor(1,1);
      writeScreenLine(currentLine);
      oled.setCursor(1,2);
      writeScreenLine(currentLine + 1);
      oled.setCursor(1,3);
      writeScreenLine(currentLine + 2);
      return;                      
    case 2:
      writeScreenLine(currentLine - 2);
      oled.setCursor(1,1);
      writeScreenLine(currentLine - 1);
      oled.setCursor(1,2);
      writeScreenLine(currentLine);
      oled.setCursor(1,3);
      writeScreenLine(currentLine + 1);
      return; 
    case 3:
      writeScreenLine(currentLine - 3);
      oled.setCursor(1,1);
      writeScreenLine(currentLine - 2);
      oled.setCursor(1,2);
      writeScreenLine(currentLine - 1);
      oled.setCursor(1,3);
      writeScreenLine(currentLine);
      return;         
  }  
}

void writeCursor(){
  oled.setCursor(0,0);
  oled.print(" ");
  oled.setCursor(0,1);
  oled.print(" ");
  oled.setCursor(0,2);
  oled.print(" ");
  oled.setCursor(0,3);
  oled.print(" ");
  switch(screenPosition){
    case 0:
      oled.setCursor(0,0);
      oled.write(45);  //42 = *
      return;
    case 1:
      oled.setCursor(0,1);
      oled.write(45);
      return;
    case 2:
      oled.setCursor(0,2);
      oled.write(45);
      return;
    case 3:
      oled.setCursor(0,3);
      oled.write(45);
      return;            
  }
}

void writeStatus(){
  switch (displayStatus) {
    case 0:
      oled.print("Idle.....");
      break;  
    case 1:
      oled.print("Refilling");
      break;  
    case 2:
      oled.print("Mix Delay");
      break;  
    case 3:
      oled.print("Stirring ");
      break;     
    case 4:
      oled.print("OVERFLOW ");
      break;  
    case 5:
      oled.print("PURGE    ");
      break; 
    case 6:
      oled.print("UNDERFILL");
      break; 
    case 7:  
      oled.print("SLOW    "); //refillDelay state is ON
      break;                
  }
}

void lcdPrint24time(int hours, int minutes ) {
  oled.setCursor(15,0); //starting location of the time
  if (hours >= 0 && hours < 10) {
    oled.write('0');
  }
  oled.print(hours);
  oled.print(F(":"));
  if (minutes >= 0 && minutes < 10) {
    oled.print("0");
  }
  oled.print(minutes);
  oled.print(F(" "));
}
