
void readEncoder(){
  if (digitalRead(IntPin) == LOW) {
    Serial.println("encoder interrupt"); 
    if (menu == 0){
      if (encoder.updateStatus()) {
        if (encoder.readStatus(E_PUSH)) {
          Serial.println("Encoder Pushed!");
          menu = !menu;
          Serial.println("to change menu");
          currentLine = 0;
          setMenuLimits(menu);
          screenPosition = 0;
          //writeDynamicScreen();     
        }
      }
    }
    if (menu == 1){
      if (encoder.updateStatus()) {
        if (encoder.readStatus(E_PUSH)) {
          Serial.println("push");  
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
              menu = !menu;
              //writeScreen();
              return; 
          }
        }
        if (encoder.readStatus(E_INCREMENT)) {
          if (incrementVariable == 0){
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
              //writeDynamicScreen();
              //writeCursor();
            }           
          }
          else {
            changeVariable(0);  
          }
          //writeDynamicScreen();
        }
        if (encoder.readStatus(E_DECREMENT)) {
          if (incrementVariable == 0){
            if (currentLine > 0){
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
              //writeDynamicScreen();
              //writeCursor();
            }
          }   
          else if (incrementVariable == 1) {
            changeVariable(1);   
          }
          //writeDynamicScreen();               
        } 
      }
    }
    screenUpdate= HIGH;
  }
}

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
        if (out1Speed < max1Speed){
          out1Speed ++;                   
        }        
      }
      else {
        if (out1Speed > min1Speed){
          out1Speed --;            
        }                
      }
      return;
    case 3:
      if (i == LOW){
        if (out2Speed < max2Speed){
          out2Speed ++; 
        }
      }  
      else {
        if (out2Speed > min2Speed){
          out2Speed --;    
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
  writeDynamicScreen();
}
