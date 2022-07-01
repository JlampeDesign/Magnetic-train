#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include "SSD1306Wire.h"

#define SS_PIN 26
#define RST_PIN 25
#define DEMO_DURATION 7000
// wifi parameters
const char* ssid              = "hbo-ict-lab-2.4GHz";    
const char* password          = "hboictlab2018"; 
const char* postLogin         = "http://145.89.192.130/api/auth/login";
const char* postNewAccount    = "http://145.89.192.130/api/register";

// rfid & oled parameters
MFRC522 mfrc522(SS_PIN, RST_PIN);
SSD1306Wire display(0x3c, SDA, SCL);
// string content
String content;
String response;

// int
int j = 0;
int counter = 1;
int progress;
long timeSinceLastModeSwitch = 0;
int httpResponseCode;
// bool
bool progressStat = true;
bool keyFob = false;
bool fobText = false;

//---------------------------------------------------- function list -----------------------------------
void wifi();
// oled
void textDisplay();
void drawProgressBarDemo();
void starterText(String inputText);
void scanning();
// rifd
void rifdStats();
// post & get
void httpPostrequest(const char* endpoint);

void setup() {
  // ------------------- start-up sequence -----------
 Serial.begin(115200);
  //--------------------------------------------------   
  SPI.begin();      
  mfrc522.PCD_Init();   
  //--------------------------------------------------
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  // wifi setup
  wifi();
  delay(500);
}

void loop() {
  rifdStats();

  if(keyFob){
    starterText("keyfob mode");
      delay(500);
  } else if(fobText){
     for(int i = 0; i < response.length(); i++){
      if(response[i] == '\"'){
          response[i] = ' ';
        }
       }
    starterText(response);
    delay(8000);  
    fobText = false;
  }else {
    starterText("Place  card");
  }
}

//-------------------------------------------------- rifd & keyfob ----------------------------------------
void rifdStats(){
  // put your main code here, to run repeatedly:
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  for (int i = 0; i < content.length(); i++){
    if(content[i] == ' '){
        content[i] = '_';
      }
    }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "65_46_DC_D9" && keyFob == false) {
    Serial.println("Authorized access");
    keyFob = true;
    scanning();
    delay(2000);
  }
  else if(keyFob && content.substring(1)!= "65_46_DC_D9" ) {
    Serial.println("add pass");
    keyFob = false;
    fobText = true;
    scanning();
    delay(500);
    httpPostrequest(postNewAccount);
    delay(2000);
  }
  else if (content.substring(1) == "65_46_DC_D9"){
      keyFob = false;
      scanning();
      delay(2000); 
    }
  else {
    Serial.println(content.substring(1));
    scanning();
    delay(500);
    httpPostrequest(postLogin);
    delay(2000);
  }
}
//-------------------------------------------------- OLED ----------------------------------------
void scanning(){
  starterText("Scanning..");
  }
void drawProgressBarDemo() {
  progress = (counter / 5) % 100;
  display.drawProgressBar(0, 32, 120, 10, progress);

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

void textDisplay(){
  display.clear();
    drawProgressBarDemo();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);

  display.display();
  counter++;
  delay(10);
}

void starterText(String inputText) {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(10, 25, inputText);
  display.display();
}
// ---------------------------- wifi connection + post
void wifi () {
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
    starterText("connecting");
  }
}

void httpPostrequest(const char* endpoint){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    Serial.println("posting...");

    StaticJsonDocument<200> buff;
    String jsonPara ;
    buff["rfid"]= String(content);

    serializeJson(buff, jsonPara);
    Serial.println(jsonPara);

    http.begin(endpoint);

      http.addHeader("Content-Type", "application/json");
      int postRfid = http.POST(jsonPara);
      response = http.getString();
        
        if(endpoint == postLogin){
          for(int i = 0; i < response.length(); i++){
            if(response[i] == '\"'){
            response[i] = ' ';}
            delay(100);
            starterText(response);
            delay(5000);
            }
        } else if(postRfid == 200){
          Serial.print("Received response");
          Serial.println(response);
          starterText(response);
          delay(10000);
        } else {
          Serial.print("Post request failed");
        }
  
    http.end();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("ESP32 is not connected to a network");
    delay(5000);
  }
}