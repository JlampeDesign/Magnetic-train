// ------------------------------------------- libraries ---------------------------------

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <OneButton.h>

// ------------------------------------------ RGB include --------------------------------

#define buttonCount   25
int PatternCounter =  0;

OneButton btn = OneButton(buttonCount,true,true);

// ---------------------------------------- define data send -----------------------------

uint8_t macAdress1[] = {0x0C, 0xDC, 0x7E, 0x63, 0x14, 0x8C};

typedef struct struct_message {int y;} 
struct_message;
struct_message message;

esp_now_peer_info_t peerInfo;

int sendControll = 1; 

// ---------------------------------------- void callback -----------------------------

void buttonPush   ();
void indESP       ();
void espSendCheck ();
void NextPattern  ();
void adressCheck  ();
void sendData     ();
void stopSend     ();

static void handleClick() {PatternCounter++; Serial.println("button pressed");}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


// ---------------------------------------------- setup --------------------------------

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, macAdress1, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


}
// ---------------------------------------------- loops ---------------------------------

void loop() {

  // test button push
  buttonPush  ();
  // esp now placement sendf
  sendData    ();
  // sending stopped
  stopSend();
}

// ---------------------------------------------- void ---------------------------------

void adressCheck(){
  memcpy(peerInfo.peer_addr, macAdress1, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
void sendData(){
  if(message.y != sendControll){
    esp_err_t result = esp_now_send(macAdress1, (uint8_t *) &message, sizeof(message));
   
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    Serial.println(PatternCounter);
  }
}

// ------------------------------------------- button push ---------------------------------

void buttonPush(){
  btn.attachClick(handleClick);
  btn.tick();
  
  if (PatternCounter == 4) {
    PatternCounter = 0;
  }

  switch(PatternCounter){
    case 1:
      message.y = 1;
      delay(100);
    break;
    case 2:
      message.y = 2;
      delay(100);
    break;
    case 3:
      message.y = 3;
      delay(100);
    break;
    case 0:
      message.y = 0;
      delay(100);
    break;
  }
}

void stopSend(){
  btn.attachClick(handleClick);
  btn.tick();

  switch(PatternCounter){
    case 1:
      sendControll = 1;
      btn.attachClick(handleClick);
    break;
    case 2:
      sendControll = 2;
      btn.attachClick(handleClick);
    break;
    case 3:
      sendControll = 3;
      btn.attachClick(handleClick);
    break;
    case 0:
      sendControll = 0;
      btn.attachClick(handleClick);
    break;
  }
}