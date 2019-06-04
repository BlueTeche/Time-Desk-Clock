//*******************************************************************************************************************
// 								                                         Main Loop 
//*******************************************************************************************************************
void loop()
{

  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  
  // Test for Sleep ------------------------------------------------*

  currentMillis = millis();
  OptionModeFlag = false;

  if(((currentMillis - SleepTimer) > SleepLimit) && SleepEnable)
  {
    
   if(STATE= 1)                                                    // New for ST Desk Clock - goto Time vs Sleep
  {
    SUBSTATE = 1;
     blinkON = true;
    blinkFlag = false;
    blinkMin = false;
    blinkHour = false;
  }else
  {
       STATE= 1; // was STATE= 99; 
       SUBSTATE = 0;
  }
    
    SleepTimer = millis();

  }

  // Test for Mode Button Press ------------------------------------* 

  bval = !digitalRead(MODEBUTTON);
  if(bval)
  {
    if(ALARMON)
    {
    CheckAlarm();
    }

    if(ALARM1FLAG)
    {
      ALARM1FLAG = false;
      ALARMON = false;
      EnableAlarm1(false);
      STATE = 90;
      JustWokeUpFlag = false;
    }
    else
    {
      if(JustWokeUpFlag)
      {
        JustWokeUpFlag = false;
        JustWokeUpFlag2 = true;                                    // Used to supress "Time" text from showing when waking up.
      }
      else
      {
      NextStateRequest = true;
      }
   //   SUBSTATE = 99;

      while(bval)
      {
          bval = !digitalRead(SETBUTTON);
          if(bval)
           {
            OptionModeFlag = true;
            NextStateRequest = false;
            NextSUBStateRequest = false;
            displayString("SPEC");
            delay(300);
           }      
        bval = !digitalRead(MODEBUTTON);
      }

      delay(100);
      SleepTimer = millis();
    }
  }

  // Test for SET Button Press ------------------------------------*
  
  bval = !digitalRead(SETBUTTON);
  if(bval && !OptionModeFlag)
  {
    NextSUBStateRequest = true;

    while(bval)
    {
      
          bval = !digitalRead(MODEBUTTON);
          if(bval)
           {
            OptionModeFlag = true;
            NextStateRequest = false;
            NextSUBStateRequest = false;
            displayString("SPEC");
            delay(300);
           }   
      
      
      bval = !digitalRead(SETBUTTON);
    }
    delay(100);
    SleepTimer = millis();
  }
 
   // Running Blink counter ------------------------------------* 
  if(blinkFlag)
  {
    blinkCounter = blinkCounter +1;
    if(blinkCounter >blinkTime)                                        // was 150
    {
      blinkON = !blinkON;
      blinkCounter = 0;
    }
  }
  else
  {
    blinkON = true;                                                    // Not blinking, just leave the LEDs lit
  }  
  
//*******************************************************************************************************************
// 								                        Main Loop - State Machine 
//*******************************************************************************************************************

  switch (STATE) 
  {
  case 0:                                                                  // Set-Up
    STATE = 1;
    break;
  
  /*case 1:                                                                  // Temperture
    Temperture();
    break;

  case 2:                                                                  // humidity
    humidity();
    break;*/

  case 3:                                                                  // Display Time
    DisplayTimeSub(); 
    break;

  case 4:                                                                  // Set Time
    setTimeSub();
    break; 

  case 5:                                                                  // Config Alarm
   setAlarmSub();
    break;
 
  case 6:                                                                 // Stop Watch
    StopWatch();
    break;
  
  case 7:                                                                 // Serial Display                                 
    DisplaySerialData();
    break;

  case 8:                                                                 // Graphic Demo                                 
    graphican();
    break;

  

   // ---------------------------------------------------------------   
  
  case 90:                                                                  // Alarm Triggered
  
    blinkFlag = true;
    displayString("Beep");

  if(blinkON)
  {
    pinMode(SETBUTTON, OUTPUT);
    tone(SETBUTTON,4000) ;
    delay(100);
    noTone(SETBUTTON);
    digitalWrite(SETBUTTON, HIGH);
  }

    #if ARDUINO >= 101 
    pinMode(SETBUTTON, INPUT_PULLUP);
//    digitalWrite(SETBUTTON, HIGH);
     #else
//    digitalWrite(SETBUTTON, HIGH);
    pinMode(SETBUTTON, INPUT);
     #endif
    delay(250);

//    bval = !digitalRead(SETBUTTON);
    if(NextSUBStateRequest | NextStateRequest)
    {
      STATE = 0;
      SUBSTATE = 0;
 //     NextStateFlag = true;
      NextStateRequest = false;
      NextSUBStateRequest = false;      
      blinkFlag = false;
    }    
    break;

    // --------------------------------------------------------------- 

  case 99:                                                                    // Sleep
    displayString("Nite");
    delay(500);
    clearmatrix();
    GoToSleep();
    SleepTimer = millis();
    STATE = 0;
    SUBSTATE = 0;
    break;

    // --------------------------------------------------------------- 

  }

    char c; 
  
if(Serial.available())  
  {  
   c = Serial.read();  
   if(c=='t')
   readSensor();
  } 

}

//*******************************************************************************************************************
// 								                                 End of Main Loop
//*******************************************************************************************************************

/////////////////////////////////////////////TEMP HUMI//////////////////////////
void readSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
}
