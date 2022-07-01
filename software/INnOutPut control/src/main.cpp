#include <Arduino.h>
// --------------------------------------------------
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Adafruit_ADS1X15.h>
// --------------------------------------------------
// define + values
#define number_of_74hc595s 2
#define numOfRegisterPins number_of_74hc595s * 8

#define SER_Pin 25
#define RCLK_Pin 27
#define SRCLK_Pin 26

Adafruit_ADS1115 ads[4];
int16_t adsCycle[16];

// pinOUT values
char serialOutput;
boolean registers [numOfRegisterPins];

// wifi + getrequest values
const char* ssid      =  "hbo-ict-lab-2.4GHz";
const char* password  =  "hboictlab2018";

String getPreset    = "http://145.89.192.130/api/location/getNewLocation/home";
String getActivity  = "http://145.89.192.130/api/presets/getactivepreset";

String ifData = "";

// voids wifi connect + get/post request
void wifi           ();
void httpGetrequest ();
void presetTester   ();
void outputResponse ();
// voids pinOUT
void clearRegisters ();
void writeRegisters ();
void writeGrpRelay(int port,int delayTime);
void ledRoll (int Activi, int delayTime);
// voids pinIN
void adsReadCycle();
void adsStartUp(){
  ads[0].begin(0x48);
  ads[1].begin(0x49);
  ads[2].begin(0x4B);
  ads[3].begin(0x4A);
}
void printloop(int16_t numb);
void printValue();
// pre-set movement
void setupOne();
void setupTwo();
void setupThree();
// --------------------------------------------------
// voids Setup + Loop
void setup(){

Serial.begin(9600);

// wifi setup
  wifi            ();
  httpGetrequest  ();
// Pinout setup
  pinMode (SER_Pin, OUTPUT);
  pinMode (RCLK_Pin, OUTPUT);
  pinMode (SRCLK_Pin, OUTPUT);
// pinout startup
  clearRegisters  ();
  writeRegisters  ();
  delay(500);
// pinIN startup
  adsStartUp();
  delay(500);

}

void loop(){
  
  httpGetrequest();
  delay(500);

}

// --------------------------------------------------
// input PIN setup
void adsReadCycle(){
  for(int j=0; j<4; j++){
      adsCycle[j] = ads[0].readADC_SingleEnded(j);
    delay(5); 
      adsCycle[j + 4] = ads[1].readADC_SingleEnded(j);
    delay(5);
      adsCycle[j + 8] = ads[2].readADC_SingleEnded(j);
    delay(5);
      adsCycle[j + 12] = ads[3].readADC_SingleEnded(j);
    delay(5);
  }
}

void printValue(){
  for(int i=0; i<16; i++){
    if(adsCycle[i] < 17500){
        adsCycle[i] = 0;
    }
    if(i<9){
      Serial.println("DP " + String(i + 1) + "   : " + String(adsCycle[i]));
      delay(5);
    } else {
      Serial.println("DP " + String(i + 1) + "  : " + String(adsCycle[i]));
      delay(5);
    }
  }
  Serial.println("--------------------- end input --------------------");
}
// --------------------------------------------------
// pinout setup + startup + write state
void clearRegisters(){
  int i;
  for(i = 0; i <  numOfRegisterPins; i++){
    registers[i] = LOW;
  }
}

void writeRegisters(){
  digitalWrite(RCLK_Pin, LOW);
    for(int i = 0; i <  numOfRegisterPins; i++){
      digitalWrite(SRCLK_Pin, LOW); int val = registers[i];
      digitalWrite(SER_Pin, val);
      digitalWrite(SRCLK_Pin, HIGH);
    }
  digitalWrite(RCLK_Pin, HIGH);
}

void writeGrpRelay(int port, int delayTime){
  registers[port] = HIGH;
  writeRegisters();
  delay(delayTime);
}

void ledRoll (int Activi, int delayTime){
  for(int i = numOfRegisterPins-1; i >=  0; i--){
    Serial.print(F("Relay "));Serial.print(i);Serial.println(F(" Active"));
      writeGrpRelay(i,Activi);
    Serial.print(F("Relay "));Serial.print(i);Serial.println(F(" De-act"));
      writeGrpRelay(i,delayTime); 
  }
}
// --------------------------------------------------
// wifi + get request pre-sets
void wifi () {
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  delay(500);
  Serial.println("Connecting to WiFi...");
}

void httpGetrequest(){

  HTTPClient http;
  String response;

  http.begin(getPreset);
  http.GET();
  response = http.getString();
  
  http.end();

  int labelStart = response.indexOf("potmeter_cam\":");

  int contentStart = response.indexOf(":", labelStart);

  int contentEnd = response.indexOf("}", labelStart);
  String content = response.substring(contentStart + 1, contentEnd);

  Serial.println("data is: " + content);
  int conv = content.toInt();
  Serial.print("int is:  ");
  Serial.println(conv);
}

// --------------------------------------------------
// pre-set placements
void setupOne(){
  
}

void setupTwo(){

}

void setupThree(){

}
