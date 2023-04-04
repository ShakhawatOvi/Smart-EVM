#include "TRIGGER_WIFI.h"               /*Includes ESP8266WiFi.h and WiFiClientSecure.h, just have these two libraries downloaded before*/
#include "TRIGGER_GOOGLESHEETS.h"       /*Library file for Google Sheets, has to be used after Wi-Fi Client Secure declaration, here everything is in Trigger_WIFI.h, so using it after Trigger_WIFI.h*/ 

/**********Google Sheets Definations***********/
char column_name_in_sheets[ ][6] = {"pb1","pb2","pb3"};                        /*1. The Total no of column depends on how many value you have created in Script of Sheets;2. It has to be in order as per the rows decided in google sheets*/
String Sheets_GAS_ID = "AKfycbwgsODCxfj0bYvSgEB3kumGH3P1KVFZf16_Kflzji2zMBGH0gGAeP5zS1FzvHVrZBb4";                                         /*This is the Sheets GAS ID, you need to look for your sheets id*/
int No_of_Parameters = 3;                                                                /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/
/*********************************************/

long debouncing_time = 500; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;

void setup() {
  Serial.begin(115200);
  
  while (!Serial);

  WIFI_Connect("samin","26121998");                                                     /*Provide you Wi-Fi SSID and password to connect to Wi-Fi*/
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
  
  pinMode(D6,INPUT_PULLUP);
  pinMode(D7,INPUT_PULLUP);
  pinMode(D5,INPUT_PULLUP);

  //attachInterrupt(digitalPinToInterrupt(D6), vote1, FALLING);
  //attachInterrupt(digitalPinToInterrupt(D7), vote2, FALLING);
  //attachInterrupt(digitalPinToInterrupt(D5), vote3, FALLING);

}

void loop() {


  int pb1,pb2,pb3;
  float data1,data2,data3;
  
  pb1 = digitalRead(D6);
  pb2 = digitalRead(D7);
  pb3 = digitalRead(D5);

  data1 = pb1? 0:1;
  data2 = pb2? 0:1;
  data3 = pb3? 0:1;

  if ((data1==1)&(data2==0)&&(data3==0)){
    Data_to_Sheets(No_of_Parameters,  data1,  data2, data3);
    delay(10000);
  }
  else if((data1==0)&(data2==1)&&(data3==0)){
    Data_to_Sheets(No_of_Parameters,  data1,  data2, data3);
    delay(10000);
  }
  else if((data1==0)&(data2==0)&&(data3==1)){
    Data_to_Sheets(No_of_Parameters,  data1,  data2, data3);
    delay(10000);
  }

  

  
}
