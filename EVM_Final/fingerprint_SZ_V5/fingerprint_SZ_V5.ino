#include "TRIGGER_WIFI.h"
#include "TRIGGER_GOOGLESHEETS.h"
#include <Adafruit_Fingerprint.h>
#include<ESP8266WiFi.h>
#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(D3, D4);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int votingdone[127];
int sizev = 0;
int buttonState = 0;
int voteflag = 0; //
int enrollkey = 0;// unnecessary
uint8_t id, count;


/*Google Sheets Definations*/
char column_name_in_sheets[ ][6] = {"pb1", "pb2", "pb3"};                      /*1. The Total no of column depends on how many value you have created in Script of Sheets;2. It has to be in order as per the rows decided in google sheets*/
String Sheets_GAS_ID = "AKfycbwgsODCxfj0bYvSgEB3kumGH3P1KVFZf16_Kflzji2zMBGH0gGAeP5zS1FzvHVrZBb4";                                         /*This is the Sheets GAS ID, you need to look for your sheets id*/
int No_of_Parameters = 3;                                                                /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/
/*EVM Start Here*/



void setup()
{
  bool j;


  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  lcd.clear();



  while (1)
  {

    j = WIFI_Connect("ASUS", "home1234");


    if (j == false) {
      lcd.clear();
      lcd.print("No Wifi found");
      delay(500);
    }
    else {
      Serial.println("Wifi Connected");
      break;
    }

  }


  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
  pinMode(D5, INPUT_PULLUP);      // Can1 / UP Dual Function Button
  pinMode(D6, INPUT_PULLUP);      // Can2 / Down Dual Function Button
  pinMode(D7, INPUT_PULLUP);      // Can3 / OK Dual Function Button
  pinMode(D0, INPUT_PULLDOWN_16); //Confirm Button

  lcd.setCursor(0, 0);  lcd.print("SMART EVM with");
  lcd.setCursor(0, 1);  lcd.print("IOT&Fingerprint");
  delay(2000);


  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword())
  {
    Serial.println("Found Module!");
  }
  else
  {
    Serial.println("Did not find fingerprint sensor");
    lcd.clear();
    lcd.print("Fingerprint module not Found");
    lcd.setCursor(0, 1);  lcd.print("Check Connections");
    while (1) {
      delay(1);
    }
  }
  finger.getTemplateCount();
  if (finger.templateCount == 0)
  {
    Serial.print("Sensor doesn't contain any fingerprint data. Please enroll fingeprints.");
  }
  else
  {
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
  int i;
  for (i = 0; i < 50; i++)
  {
    votingdone[i] = -10;
  }
}

void loop()
{
  lcd.clear();
  lcd.setCursor(0, 0);  lcd.print("Vote | Enroll?");
  lcd.setCursor(0, 1);  lcd.print("Press Button");
  delay(1000);
  Serial.println("Vote | Enroll?");
  if (analogRead(A0) > 1000)        //enters enroll mode
    enrollID();
  else if (digitalRead(D0))
  {
    while (1)
    {
      delay(1);
      int result = getFingerprintIDez();
      int flag = 0, i;
      delay(50);
      Serial.println("Scanning");
      lcd.clear();
      lcd.setCursor(0, 0);  lcd.print("Scanning");

      if (result >= 0)
      {
        Serial.print("Matched FPID is:  "); Serial.println(result);
        flag = 0;
        for (i = 0; i < sizev; i++)
        {
          if (result == votingdone[i])
          {
            flag = 1;
            Serial.println("Duplicate Voter");
            lcd.clear();
            lcd.print("DuplicateID #"); lcd.print(result);
            lcd.setCursor(0, 1);  lcd.print("Already Voted");
            delay(5000);
            break;
          }
        }
        if (flag == 0)
        {
          lcd.clear();
          lcd.setCursor(0, 0);  lcd.print("MatchedID #"); lcd.print(result);
          lcd.setCursor(0, 1);  lcd.print("Please Vote");
          buttonpress();
          //      Serial.print("VF = ");
          //      Serial.println(voteflag);
          if (!voteflag) {
            votingdone[sizev] = result;
            sizev += 1;
          }
          voteflag = 0;
        }
      }
      if (analogRead(A0) > 1000)        //Exits Vote Mode
        break;
    }
  }
}

/*Enroll Main Function*/
void enrollID()
{
  while (1)
  {
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    lcd.clear();
    lcd.setCursor(0, 0);  lcd.print("Enter New FP-ID");
    lcd.setCursor(0, 1);  lcd.print("1");
    delay(500);
    id = mycount();
    if (id == 0)
      return;
    Serial.print("Enrolling ID #");
    Serial.println(id);
    if ((getFingerprintEnroll()) != FINGERPRINT_ENROLLMISMATCH)
      return;
  }
}
/* ~Enroll Main Function */

/* Enroll by Push Button */
uint8_t mycount(void) {
  count = 1;
  Serial.print("Enter New FP-ID");
  while (1) {
    delay(1);

    if (digitalRead(D5) == 0) // if Up button is pressed
    {
      count++;
      if (count > 25)
        count = 1;
      Serial.print("+++>"); Serial.println(count);
      lcd.clear();
      lcd.setCursor(0, 0);  lcd.print("Enter New FP-ID");
      lcd.setCursor(0, 1);  lcd.print(count);
      delay(500);
    }
    else if (digitalRead(D6) == 0)
    {
      count--;
      if (count < 1)
        count = 25;
      Serial.print("--->"); Serial.println(count);
      lcd.clear();
      lcd.setCursor(0, 0);  lcd.print("Enter New FP-ID");
      lcd.setCursor(0, 1);  lcd.print(count);
      delay(500);
    }
    else if (digitalRead(D7) == 0)
    {
      Serial.print("Scanning for ID: ");
      Serial.println(count);
      lcd.clear();
      lcd.setCursor(0, 0);  lcd.print("Scanning for ");
      lcd.setCursor(0, 1);  lcd.print("ID:");
      lcd.setCursor(4, 1);  lcd.print(count);
      delay(500);
      return count;
    }
  }
}

/*~Enroll ID by Push Button */

/*Enroll Process Start*/
uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");

        //        lcd.clear();
        //        lcd.setCursor(0, 0);
        //        lcd.print("Finger Found");
        //        delay(2000);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");

      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Print Taken");
  lcd.setCursor(0, 1);
  lcd.print("Remove Finger");
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Place Finger ");
  lcd.setCursor(0, 1); lcd.print("Again");
  //  delay(500);
  while (p != FINGERPRINT_OK) {
    if (digitalRead(D0))
      break;
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    lcd.clear();
    lcd.setCursor(0, 0);  lcd.print("Saved ID:");
    lcd.setCursor(12, 0); lcd.print(id);
    lcd.setCursor(0, 1);  lcd.print("Success");
    delay(3000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    lcd.clear();
    lcd.setCursor(0, 0);  lcd.print("No Match");
    lcd.setCursor(0, 1);  lcd.print("Fail, Try Again");
    delay(3000);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  return true;
}
/*Enroll Process End*/

/*Finger Matching Start*/
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  //  lcd.clear();
  //  lcd.print("Found ID #"); lcd.print(finger.fingerID);
  //  lcd.setCursor(0, 1);
  //  lcd.print("Press To Vote");

  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
/*Finger Matching End*/

/*Send of Sheets Start*/
void buttonpress() {
  unsigned long canceldelay = 3000;
  int pb1, pb2, pb3;
  int data1 = 0, data2 = 0, data3 = 0;
  bool success = false;
  while ((!data1) && (!data2) && (!data3)) {
    delay(1);
    pb1 = digitalRead(D5);
    pb2 = digitalRead(D6);
    pb3 = digitalRead(D7);

    data1 = pb1 ? 0 : 1;
    data2 = pb2 ? 0 : 1;
    data3 = pb3 ? 0 : 1;

    if ((data1 == 1) && (data2 == 0) && (data3 == 0)) {

      lcd.clear();
      lcd.print("Can1 , Sure?");
      delay(1000);
      buttonState = digitalRead(D0); //Low if not pressed, High if pressed
      unsigned long entrypoint = millis();
      while (!buttonState)
      {
        delay(1);
        buttonState = digitalRead(D0);
        //Serial.println(buttonState);

        if (((millis() - entrypoint) > canceldelay)) {
          voteflag = 1;
          lcd.clear();
          lcd.print("NoVote-TryAgain");
          Serial.println("NoVote-TryAgain");
          delay(2000);
          break;
        }
        else if (((millis() - entrypoint) > 2000))
        {
          // lcd.clear();
          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("1 Sec");
          Serial.println("Time Remaining 1 Sec");
        }
        else if (((millis() - entrypoint) > 1000))
        {
          // lcd.clear();
          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("2 Sec");
          Serial.println("Time Remaining 2 Sec");
        }
        else if (((millis() - entrypoint) > 0))
        {

          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("3 Sec");
          Serial.println("Time Remaining 3 Sec");
        }

      }
      if (buttonState) {
        lcd.clear();
        lcd.setCursor(0, 0);  lcd.print("Can1 Vote");
        success = Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2, (float) data3);
        if (success == true) {
          lcd.setCursor(0, 1);  lcd.print("Vote Successful!");
          Serial.println("Vote Successful!");
          delay(3000);
        }
        else {
          lcd.setCursor(0, 1);  lcd.print("Vote Failed!");
          Serial.println("Vote Failed!");
          delay(3000);

        }

      }

    }
    else if ((data1 == 0) && (data2 == 1) && (data3 == 0)) {
      lcd.clear();
      lcd.print("Can2 , Sure?");
      delay(1000);
      buttonState = digitalRead(D0); //Low if not pressed, High if pressed
      unsigned long entrypoint = millis();
      while (!buttonState)
      {
        delay(1);
        buttonState = digitalRead(D0);

        if (((millis() - entrypoint) > canceldelay)) {
          voteflag = 1;
          lcd.clear();
          lcd.print("NoVote-TryAgain");
          Serial.println("NoVote-TryAgain");
          delay(2000);
          break;
        }
        else if (((millis() - entrypoint) > 2000))
        {
          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("1 Sec");
          Serial.println("Time Remaining 1 Sec");
        }
        else if (((millis() - entrypoint) > 1000))
        {
          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("2 Sec");
          Serial.println("Time Remaining 2 Sec");
        }
        else if (((millis() - entrypoint) > 0))
        {
          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("3 Sec");
          Serial.println("Time Remaining 3 Sec");
        }
      }
      if (buttonState) {
        lcd.clear();
        lcd.setCursor(0, 0);  lcd.print("Can2 Vote");
        Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2, (float) data3);
        lcd.setCursor(0, 1);  lcd.print("Vote Successful!");
        Serial.println("Vote Successful!");
        delay(3000);
      }
    }
    else if ((data1 == 0) && (data2 == 0) && (data3 == 1)) {
      lcd.clear();
      lcd.print("Can3 , Sure?");
      delay(1000);
      buttonState = digitalRead(D0); //Low if not pressed, High if pressed
      unsigned long entrypoint = millis();
      while (!buttonState)
      {
        delay(1);
        buttonState = digitalRead(D0);

        if (((millis() - entrypoint) > canceldelay)) {
          voteflag = 1;
          lcd.clear();
          lcd.print("NoVote-TryAgain");
          Serial.println("NoVote-TryAgain");
          delay(2000);
          break;
        }
        else if (((millis() - entrypoint) > 2000))
        {
          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("1 Sec");
          Serial.println("Time Remaining 1 Sec");
        }
        else if (((millis() - entrypoint) > 1000))
        {
          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("2 Sec");
          Serial.println("Time Remaining 2 Sec");
        }
        else if (((millis() - entrypoint) > 0))
        {
          lcd.setCursor(0, 0);  lcd.print("Time Remaining:");
          lcd.setCursor(0, 1);  lcd.print("3 Sec");
          Serial.println("Time Remaining 3 Sec");
        }
      }
      if (buttonState) {
        lcd.clear();
        lcd.setCursor(0, 0);  lcd.print("Can3 Vote");
        Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2, (float) data3);
        lcd.setCursor(0, 1);  lcd.print("Vote Successful!");
        Serial.println("Vote Successful!");
        delay(3000);
      }
    }
    if ((data1 == 1) || (data2 == 1) || (data3 == 1)) {
      delay(50);
      return;
    }
  }
}
/*Send of Sheets End*/
