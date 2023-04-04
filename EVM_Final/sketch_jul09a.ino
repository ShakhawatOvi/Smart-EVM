#include "TRIGGER_WIFI.h"               /*Includes ESP8266WiFi.h and WiFiClientSecure.h, just have these two libraries downloaded before*/
#include "TRIGGER_GOOGLESHEETS.h"       /*Library file for Google Sheets, has to be used after Wi-Fi Client Secure declaration, here everything is in Trigger_WIFI.h, so using it after Trigger_WIFI.h*/ 

/**********Google Sheets Definations***********/
char column_name_in_sheets[ ][6] = {"pb1","pb2","pb3"};                        /*1. The Total no of column depends on how many value you have created in Script of Sheets;2. It has to be in order as per the rows decided in google sheets*/
String Sheets_GAS_ID = "AKfycbwgsODCxfj0bYvSgEB3kumGH3P1KVFZf16_Kflzji2zMBGH0gGAeP5zS1FzvHVrZBb4";                                         /*This is the Sheets GAS ID, you need to look for your sheets id*/
int No_of_Parameters = 3;                                                                /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/
/*********************************************/

void setup() 
{
  Serial.begin(9600);
  while (!Serial);

  WIFI_Connect("samin","26121998");                                                     /*Provide you Wi-Fi SSID and password to connect to Wi-Fi*/
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );
  
  /*Sets the column name for Google Sheets, the GAS ID, and the No of Parameter we want to send*/   

  pinMode(D6,INPUT_PULLUP);
  pinMode(D7,INPUT_PULLUP);
  pinMode(D5,INPUT_PULLUP);

}

void loop() 
{
  float a = 1, b = 2, c = 3;                           /*Demo values that has to be sent to google sheets, you can use sensor values*/

  int pb1,pb2,pb3;
  float data1,data2,data3;

  pb1 = digitalRead(D6);
  pb2 = digitalRead(D7);
  pb3 = digitalRead(D5);

  data1 = pb1? 0:1;
  data2 = pb2? 0:1;
  data3 = pb3? 0:1;

 
  Serial.print("Pb1:");
  Serial.print(data1);
  Serial.print("  ");

  Serial.print("Pb2:");
  Serial.print(data2);
  Serial.print("  ");

  Serial.print("Pb3:");
  Serial.print(data3);
  Serial.println("  ");
  
  Data_to_Sheets(No_of_Parameters,  data1,  data2, data3);  /*1. This function accepts multiple float parameter, here No_of_Parameters decides how many parameters you want to send to Google Sheets; 2. The values sent should be in order as per the column in Google Sheets*/ 
  
  delay(5000);                                       /*10 Sec Delay, Here 10 second delay is just used so that we can see the data pushed to sheets one by one
//                                                        There is nothing like a minimum delay between two data push*/
}
