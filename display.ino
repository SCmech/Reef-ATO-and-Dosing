void writeScreenLine(byte i){
  switch(menu){
    case 0:
      switch(i){
        case 0:
          oled.print("line 0-0");
          return;
        case 1:
          oled.print("line 0-1");
          return;
        case 2:
          oled.print("line 0-2");
          return;
        case 3:
          oled.print("line 0-3");
          return;
        case 4:
          oled.print("line 0-4");
          return;
        case 5:
          oled.print("line 0-5");
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
          oled.print(out1Speed);
          return;
        case 3:
          oled.print("Mix Speed: ");
          oled.print(out2Speed);
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
      return;
  }
}


void writeScreen(){ //update whole screen
  //Serial.print(levelLowState);
  //Serial.print(",");
  //Serial.print(levelHighState);
  //Serial.print(",");
  //Serial.println(levelMaxState);  
  
  oled.clear();
  //oled.print("STATUS..."); //
  writeStatus();
  lcdPrint24time(currentHour, currentMinute);
  oled.setCursor(1,1);
  oled.print("High Sense: ");
  if (levelHighState == 1){
    oled.print("0"); 
  }
  else {
    oled.print("X"); 
  }
  oled.setCursor(1,2);
  oled.print("Low Sense: ");
  if (levelLowState == 1){
    oled.print("0"); 
  }
  else {
    oled.print("X"); 
  } 
  oled.setCursor(1,3);
  oled.print("Float SW: ");
  if (levelMaxState == 1){
    oled.print("0"); 
  }
  else {
    oled.print("X"); 
  }    
}

void writeDynamicScreen(){
  Serial.print("currentLine = ");
  Serial.print(currentLine);
  Serial.print(" , screePostion = ");
  Serial.println(screenPosition);
  oled.clear();
  writeCursor();
  oled.setCursor(1,0);
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
      oled.write(45);
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
  switch (statusVariable) {
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
