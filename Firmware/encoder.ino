void readEncoder(){
  if (digitalRead(IntPin) == LOW) {
    timeLimitCounter = 0;
    //Serial.println("readEncoder");
    if (encoder.updateStatus()) {
      if (encoder.readStatus(E_PUSH)) {
        encoderPush();
        screenUpdate = HIGH;
        return;       
      }
      if (encoder.readStatus(E_INCREMENT)) {
        encoderIncrement();
        screenUpdate = HIGH;
        return;          
      }
      if (encoder.readStatus(E_DECREMENT)) {
        encoderDecrement();
        screenUpdate = HIGH;
        return;          
      }
    }
  }
}

void encoderPush(){
  Serial.println("encoderPush");
  if (menu == 0){
    Serial.println("encoder push and menu 0");
    jumpScreen(1);
    return;  
  }
  if (menu == 1){
    switch(currentLine){
      case 0:
        purgeEvent();
        return;
      case 1:
        mixEvent();
        return;
      case 2:
        incrementVariable = !incrementVariable;
        return;
      case 3:
        incrementVariable = !incrementVariable;
        return;
      case 4:
        incrementVariable = !incrementVariable;            
        return;              
      case 5:
        incrementVariable = !incrementVariable;            
        return; 
      case 6:
        jumpScreen(0);
        return; 
    }    
  }
}

void encoderIncrement(){
  if (incrementVariable == LOW){
    if (currentLine < maxLine){
      currentLine ++; 
        if (screenPosition == 0){
          screenPosition = 1;
        }
        else if (screenPosition == 1){
          screenPosition = 2;             
        }              
        else if (screenPosition == 2){
          if (currentLine != (maxLine - 1)){
            screenPosition = 3;
          }
        }
        else if (screenPosition == 3){
          screenPosition = 3;
        }
        else {
          Serial.println("screen print error");
        }
      }           
    }
  else {
    changeVariable(HIGH);  
  }
}

void encoderDecrement(){
  if (incrementVariable == LOW){
    if (currentLine > minLine){
      currentLine --; 
      if (screenPosition == 0){
        screenPosition = 0;
      }
      else if (screenPosition == 1){
        screenPosition = 0;              
      }              
      else if (screenPosition == 2){
        screenPosition = 1;
      }
      else if (screenPosition == 3){
        screenPosition = 2;
      }
      else {
        Serial.println("screen print error");
      }
    }
  }   
  else if (incrementVariable == 1) {
    changeVariable(LOW);   
  }  
} //close ecoderDecrement

void changeVariable(boolean i) {
  switch(currentLine){
    case 0:
      Serial.println("error xxx");
      return;
    case 1:
      Serial.println("error xxx");
      return;
    case 2:
      if (i == LOW){
        if (refillSpeed < max1Speed){
          refillSpeed ++;                   
        }        
      }
      else {
        if (refillSpeed > min1Speed){
          refillSpeed --;            
        }                
      }
      return;
    case 3:
      if (i == LOW){
        if (mixSpeed < max2Speed){
          mixSpeed ++; 
          return;
        }
      }  
      else {
        if (mixSpeed > min2Speed){
          mixSpeed --;    
        }                     
      return;
      }  
    case 4:  
      if (i == LOW){
        if (mixDelayHours < 24){
          mixDelayHours ++; 
        }
      }  
      else {
        if (mixDelayHours > 0){
          mixDelayHours --;    
        } 
      }          
      return;
    case 5:
      if (i == LOW){
        if (mixDurationSeconds < 300){
          mixDurationSeconds ++; 
        }
      }  
      else {
        if (mixDurationSeconds > 5){
          mixDurationSeconds --;    
        }
      }                     
      return;
     case 6:
      Serial.println("error return");
      return;
  }
  screenUpdate = HIGH;
}
