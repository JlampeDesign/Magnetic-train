#include <Arduino.h>

#include <WiFi.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "";
const char* password =  "";
String postEndpoint = "http://145.89.192.130/api/auth/login";
String getEndpoint = "";

String message = "dsfs";
// wifi connect + get/post request
void wifi           ();
void pingReq        ();
void httpGetrequest ();
void httpPostrequest();
 
void setup() {
  Serial.begin(115200);
  // wifi setup
  wifi            ();
  pingReq         ();
  // get request api
  // httpGetrequest();
}
 
void loop() { 
    httpPostrequest();
    delay(10000);
}
// ----------------- a
void wifi () {
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
}

void pingReq () {
  bool success = Ping.ping("www.google.com", 3);
  if(!success){
    Serial.println("Ping failed");
    return;
  }
  Serial.println("Ping succesful.");
}

void httpPostrequest(){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String postResponse;
    Serial.println("posting...");

    StaticJsonDocument<200> buff;
    String jsonPara ;
    buff["rfid"]="dsfs";

    serializeJson(buff, jsonPara);
    Serial.println(jsonPara);

    http.begin(postEndpoint);

      http.addHeader("Content-Type", "application/json");
      int postRfid = http.POST(jsonPara);
      postResponse = http.getString();

        if(postRfid == 200){
          Serial.print("Received response");
          Serial.println(postResponse);
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
void httpGetrequest(){
  HTTPClient http;
  String response;

  http.begin(getEndpoint);
  http.GET();

  response = http.getString();
  Serial.println(response);

  StaticJsonDocument<1024> doc;
  deserializeJson(doc, response);
  JsonObject obj = doc.as<JsonObject>();

  String dataObj = obj[String("data ")];
  String support = obj[String("Support ")];

  Serial.println(dataObj);
  Serial.println(support);

  http.end();
}