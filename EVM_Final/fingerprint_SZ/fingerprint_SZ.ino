#include "TRIGGER_WIFI.h"  
#include "TRIGGER_GOOGLESHEETS.h" 
#include<EEPROM.h>
#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(D3, D4);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
int votingdone[127];
int sizev=0;
int buttonState = 0; 


/**********Google Sheets Definations***********/
char column_name_in_sheets[ ][6] = {"pb1","pb2","pb3"};                        /*1. The Total no of column depends on how many value you have created in Script of Sheets;2. It has to be in order as per the rows decided in google sheets*/
String Sheets_GAS_ID = "AKfycbwgsODCxfj0bYvSgEB3kumGH3P1KVFZf16_Kflzji2zMBGH0gGAeP5zS1FzvHVrZBb4";                                         /*This is the Sheets GAS ID, you need to look for your sheets id*/
int No_of_Parameters = 3;                                                                /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/
/*********************************************/
//int stage = 2;
#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 16, 2);
//#define buzzer D8
//long debouncing_time = 500; //Debouncing Time in Milliseconds
//volatile unsigned long last_micros;

void setup()
{
  int i;
  for (i=0;i<127;i++){
    votingdone[i]=-10;
  }
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  while (!Serial);  
  delay(100);

  WIFI_Connect("ASUS","home1234");                                                     /*Provide you Wi-Fi SSID and password to connect to Wi-Fi*/
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
  
  pinMode(D6,INPUT_PULLUP); 
  pinMode(D7,INPUT_PULLUP);
  pinMode(D5,INPUT_PULLUP);
//  pinMode(buzzer, OUTPUT);
  pinMode(D0, INPUT_PULLDOWN_16); //Confirm Your Vote
  
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.print("Found Module ");
    delay(1000);
  } 
  else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.clear();
    lcd.print("module not Found");
    lcd.setCursor(0,1);
    lcd.print("Check Connections");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

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

    lcd.setCursor(0,1);

    lcd.print("by Fingerprint");

    delay(3000);

    lcd.clear();
  
}

void loop()                     // run over and over again
{
   int result=getFingerprintIDez();
   int flag =0,i;
   delay(50); 
   Serial.println("Scanning");
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Scanning");
   

   
    if(result>=0)
    {
      Serial.print("Matched FPID is:  ");Serial.println(result);
      lcd.clear();
      //lcd.print("Matched FPID #");lcd.print(result);
      

    //  delay(500);
      
 
      flag=0;
      for (i=0;i<sizev;i++)
      {
          if (result==votingdone[i])
          {
             flag = 1;
             Serial.println("Duplicate Voter");
             lcd.clear();
      lcd.print("DuplicateID #");lcd.print(result);
      lcd.setCursor(0,1);
      lcd.print("Already Voted");
              delay(5000);
              
             break;
          }
      }
      if(flag==0)
      {
      
      lcd.clear();
      lcd.print("MatchedID #");
     lcd.print(result);
     
      lcd.setCursor(0,1);
      lcd.print("Please Vote");
//      Serial.println("Place Votes");
//      lcd.println("Place Votes");
      buttonpress();
      votingdone[sizev]=result;
      sizev+=1; 
      
      }
     
    }
  
//   getFingerprintID();
//  delay(50);  
//  int x=getFingerprintID();
//  
//  Serial.println(x);
            //don't ned to run this at full speed.

  
}

//uint8_t getFingerprintID() {
//  uint8_t p = finger.getImage();
//  switch (p) {
//    case FINGERPRINT_OK:
//      Serial.println("Image taken");
//      break;
//    case FINGERPRINT_NOFINGER:
//      Serial.println("No finger detected");
//      return p;
//    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
//      return p;
//    case FINGERPRINT_IMAGEFAIL:
//      Serial.println("Imaging error");
//      return p;
//    default:
//      Serial.println("Unknown error");
//      return p;
//  }
//
//  // OK success!
//
//  p = finger.image2Tz();
//  switch (p) {
//    case FINGERPRINT_OK:
//      Serial.println("Image converted");
//      break;
//    case FINGERPRINT_IMAGEMESS:
//      Serial.println("Image too messy");
//      return p;
//    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
//      return p;
//    case FINGERPRINT_FEATUREFAIL:
//      Serial.println("Could not find fingerprint features");
//      return p;
//    case FINGERPRINT_INVALIDIMAGE:
//      Serial.println("Could not find fingerprint features");
//      return p;
//    default:
//      Serial.println("Unknown error");
//      return p;
//  }
//
//  // OK converted!
//  p = finger.fingerSearch();
//  if (p == FINGERPRINT_OK) {
//    Serial.println("Found a print match!");
//  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//    Serial.println("Communication error");
//    return p;
//  } else if (p == FINGERPRINT_NOTFOUND) {
//    Serial.println("Did not find a match");
//    return p;
//  } else {
//    Serial.println("Unknown error");
//    return p;
//  }
//
//  // found a match!
//  Serial.print("Found ID #"); Serial.print(finger.fingerID);
//  Serial.print(" with confidence of "); Serial.println(finger.confidence);
//
//  return finger.fingerID;
//  stage=2;
//}
//
// returns -1 if failed, otherwise returns ID #
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
  lcd.setCursor(0,1);
  lcd.print("Press To Vote");
  lcd.clear();
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void buttonpress(){
//  unsigned long canceldelay=3000;
//  int count,flag;
  int pb1,pb2,pb3;
  int data1 = 0,data2 = 0,data3 =0;
   while((!data1)&&(!data2)&&(!data3)){
    delay(1);
  pb1 = digitalRead(D6);
  pb2 = digitalRead(D7);
  pb3 = digitalRead(D5);

  data1 = pb1? 0:1;
  data2 = pb2? 0:1;
  data3 = pb3? 0:1;


  
  if((data1==1)&&(data2==0)&&(data3==0)){
    Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2,(float) data3);
    lcd.clear();
    lcd.print("Can1 Voted");
    delay(3000);
    lcd.clear();
    
    }
  else if((data1==0)&&(data2==1)&&(data3==0)){
    Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2,(float) data3);
    lcd.clear();
    lcd.print("Can2 Voted");
    delay(3000);
    lcd.clear();
    
    }
  else if((data1==0)&&(data2==0)&&(data3==1)){
    Data_to_Sheets(No_of_Parameters, (float) data1, (float) data2, (float) data3);
    lcd.clear();
    lcd.print("Can3 Voted");
    delay(3000);
    lcd.clear();
      
    }

  if((data1==1)||(data2==1)||(data3==1)){
     delay(50);
    return;
   
    }
  
  


    }
}
