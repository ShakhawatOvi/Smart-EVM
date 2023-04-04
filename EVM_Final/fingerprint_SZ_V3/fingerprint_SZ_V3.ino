#include "TRIGGER_WIFI.h"
#include "TRIGGER_GOOGLESHEETS.h"
#include<EEPROM.h>
#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(D3, D4);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
int votingdone[127];
int sizev = 0;
int buttonState = 0;
int voteflag = 0; //
int enrollkey = 0;

/**********Google Sheets Definations***********/
char column_name_in_sheets[ ][6] = {"pb1", "pb2", "pb3"};                      /*1. The Total no of column depends on how many value you have created in Script of Sheets;2. It has to be in order as per the rows decided in google sheets*/
String Sheets_GAS_ID = "AKfycbwgsODCxfj0bYvSgEB3kumGH3P1KVFZf16_Kflzji2zMBGH0gGAeP5zS1FzvHVrZBb4";                                         /*This is the Sheets GAS ID, you need to look for your sheets id*/
int No_of_Parameters = 3;                                                                /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/
/*********************************************/

#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 16, 2);

void setup()
{
  WIFI_Connect("ASUS", "home1234");                                                    /*Provide you Wi-Fi SSID and password to connect to Wi-Fi*/
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
  Serial.begin(115200);
  EEPROM.begin(32);
  lcd.begin();
  lcd.backlight();


  Serial.println("");
  //while (!Serial);
  //delay(100);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D0, INPUT_PULLDOWN_16); //Confirm Button

  //Serial.println("\n\nAdafruit finger detect test");

  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found Module!");
    lcd.clear();
    lcd.print("Found Module ");
    delay(1000);
  }
  else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.clear();
    lcd.print("module not Found");
    lcd.setCursor(0, 1);
    lcd.print("Check Connections");
    while (1) {
      delay(1);
    }
  }


  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
  lcd.clear();
  lcd.print("Voting Machine");
  lcd.setCursor(0, 1);
  lcd.print("by Fingerprint");
  delay(2000);
  lcd.clear();

  int i;

  for (i = 0; i < 127; i++) {
    
    votingdone[i] = -10);
  }
}

void loop()
{
  lcd.print("Press Any Button");
  //Serial.println("Press a button");
  if (analogRead(A0) > 1000)  //enroll
  {
    while (1) {
      delay(1);

      Serial.println("MODE 1");

      if (digitalRead(D0))
        break;
    }

  }


  else if (digitalRead(D0))
  {
    while (1)
    {
      delay(1);
      int result = getFingerprintIDez();
      int flag = 0, i;
      delay(50);
      lcd.clear();
      Serial.println("Scanning");
      lcd.print("Scanning");
      lcd.setCursor(0, 0);
      if (result >= 0)
      {
        Serial.print("Matched FPID is:  "); Serial.println(result);
        lcd.clear();
        //lcd.print("Matched FPID #");lcd.print(result);


        //  delay(500);


        flag = 0;
        for (i = 0; i < sizev; i++)
        {
          if (result == votingdone[i])
          {
            flag = 1;
            Serial.println("Duplicate Voter");
            lcd.clear();
            lcd.print("DuplicateID #"); lcd.print(result);
            lcd.setCursor(0, 1);
            lcd.print("Already Voted");
            delay(5000);

            break;
          }
        }
        if (flag == 0)
        {

          lcd.clear();
          lcd.print("MatchedID #");
          lcd.print(result);

          lcd.setCursor(0, 1);
          lcd.print("Please Vote");
          //      Serial.println("Place Votes");
          //      lcd.println("Place Votes");
          buttonpress();
          //      Serial.print("VF = ");
          //      Serial.println(voteflag);
          if (!voteflag) {
            votingdone[sizev] = result;   //Online protection
            EEPROM.write(sizev, votingdone[sizev]); //Offline Protection

            Serial.print(sizev); Serial.print("ROM--->");
            Serial.println(EEPROM.read(sizev));
            sizev += 1;
          }
          voteflag = 0;


        }

      }
      if (analogRead(A0) > 1000) {
        break;
      }

    }
  }
}


int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  lcd.clear();
  lcd.print("Found ID #"); lcd.print(finger.fingerID);
  lcd.setCursor(0, 1);
  lcd.print("Press To Vote");
  lcd.clear();
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void buttonpress() {
  unsigned long canceldelay = 3000;
  int pb1, pb2, pb3;
  int data1 = 0, data2 = 0, data3 = 0;
  while ((!data1) && (!data2) && (!data3)) {
    delay(1);
    pb1 = digitalRead(D6);
    pb2 = digitalRead(D7);
    pb3 = digitalRead(D5);

    data1 = pb1 ? 0 : 1;
    data2 = pb2 ? 0 : 1;
    data3 = pb3 ? 0 : 1;


    if ((data1 == 1) && (data2 == 0) && (data3 == 0)) {

      lcd.clear();
      lcd.print("Can1 , Sure?");
      delay(1);
      buttonState = digitalRead(D0); //Low if not pressed, High if pressed
      unsigned long entrypoint = millis();
      while (!buttonState)
      {
        delay(1);
        buttonState = digitalRead(D0);
        Serial.println(buttonState);

        if (((millis() - entrypoint) > canceldelay)) {
          voteflag = 1;
          break;
        }
        else if (((millis() - entrypoint) > 2000))
        {
          lcd.print("1...");
          Serial.println("1...");
        }
        else if (((millis() - entrypoint) > 1000))
        {
          lcd.print("2...");
          Serial.println("2...");
        }
        else if (((millis() - entrypoint) > 0))
        {
          lcd.print("3...");
          Serial.println("3...");
        }

      }


      if (buttonState) {

        Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2, (float) data3);
        lcd.clear();
        lcd.print("Can1 Voted");
        Serial.println("Can1 Voted");
        delay(3000);
        lcd.clear();
      }
      else {
        lcd.print("Vote Cancelled");
        Serial.println("Vote Cancelled");
      }
    }
    else if ((data1 == 0) && (data2 == 1) && (data3 == 0)) {
      lcd.clear();
      lcd.print("Can2 , Sure?");
      delay(1);
      buttonState = digitalRead(D0); //Low if not pressed, High if pressed
      unsigned long entrypoint = millis();
      while (!buttonState)
      {
        delay(1);
        buttonState = digitalRead(D0);
        Serial.println(buttonState);

        if (((millis() - entrypoint) > canceldelay)) {
          voteflag = 1;
          break;
        }
        else if (((millis() - entrypoint) > 2000))
        {
          lcd.print("1...");
          Serial.println("1...");
        }
        else if (((millis() - entrypoint) > 1000))
        {
          lcd.print("2...");
          Serial.println("2...");
        }
        else if (((millis() - entrypoint) > 0))
        {
          lcd.print("3...");
          Serial.println("3...");
        }

      }


      if (buttonState) {

        Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2, (float) data3);
        lcd.clear();
        lcd.print("Can2 Voted");
        Serial.println("Can2 Voted");
        delay(3000);
        lcd.clear();
      }
      else {
        lcd.print("Vote Cancelled");
        Serial.println("Vote Cancelled");
      }
    }


    else if ((data1 == 0) && (data2 == 0) && (data3 == 1)) {
      lcd.clear();
      lcd.print("Can1 , Sure?");
      delay(1);
      buttonState = digitalRead(D0); //Low if not pressed, High if pressed
      unsigned long entrypoint = millis();
      while (!buttonState)
      {
        delay(1);
        buttonState = digitalRead(D0);
        Serial.println(buttonState);

        if (((millis() - entrypoint) > canceldelay)) {
          voteflag = 1;
          break;
        }
        else if (((millis() - entrypoint) > 2000))
        {
          lcd.print("1...");
          Serial.println("1...");
        }
        else if (((millis() - entrypoint) > 1000))
        {
          lcd.print("2...");
          Serial.println("2...");
        }
        else if (((millis() - entrypoint) > 0))
        {
          lcd.print("3...");
          Serial.println("3...");
        }

      }


      if (buttonState) {

        Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2, (float) data3);
        lcd.clear();
        lcd.print("Can3 Voted");
        Serial.println("Can3 Voted");
        delay(3000);
        lcd.clear();
      }
      else {
        lcd.print("Vote Cancelled");
        Serial.println("Vote Cancelled");
      }
    }


    if ((data1 == 1) || (data2 == 1) || (data3 == 1)) {
      delay(50);
      return;

    }

  }


}
