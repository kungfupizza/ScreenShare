
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <CircularBuffer.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "shhhh.h"


CircularBuffer<int, 20> queue_pixelX;
CircularBuffer<int, 20> queue_pixelY;

#define BLACK   0x0000
#define WHITE   0xFFFF
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

String inputString = "";         // a String to hold incoming data
void setup(void)
{
    Serial.begin(115200);
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.begin(SECRET_SSID, SECRET_PASS) != WL_CONNECTED) {
      // failed, retry
      Serial.print(".");
      delay(5000);
    }
  
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    tft.begin(ID);
    tft.setRotation(1);            //LANDSCAPE
    tft.fillScreen(WHITE);

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);
  
    if (!mqttClient.connect(broker)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
      while (1);
    }
  
    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
  
    Serial.print("Subscribing to topic: ");
    Serial.println(topic);
    Serial.println();
  
    // subscribe to a topic
    mqttClient.subscribe(topic);
 
  
    Serial.print("Waiting for messages on topic: ");
    Serial.println(topic);
    Serial.println();
}

void loop(void)
{
  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    consumeMessage(messageSize);
  }
  if (queue_pixelX.size() || queue_pixelY.size()) {
    int pixel_x = queue_pixelX.shift();
    int pixel_y = queue_pixelY.shift();
    if(pixel_x == 1024 || pixel_y == 1024){ // Reset command
      tft.fillScreen(WHITE);
      queue_pixelX.clear();
      queue_pixelY.clear();
    }
    else{
      tft.drawPixel(pixel_x,pixel_y,BLACK);
    }    
    inputString = "";
  }
}

void consumeMessage(int messageSize) {
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    char inChar = (char)mqttClient.read();
    // add it to the inputString:
    inputString += inChar;
    // Wait until end of msg
    if (inChar == '\n') {
      char *p = inputString.c_str();
      char *str;
      String pixel[2];
      int index = 0;
      while ((str = strtok_r(p, ",", &p)) != NULL){ // delimiter is the comma
        pixel[index++]= String(str);
      }
      int pixel_x = pixel[0].toInt();
      int pixel_y = pixel[1].toInt();
      if(pixel_x>320 || pixel_y>240 || pixel_x<0 || pixel_y<0){
        //Do some error handling
      }
      else{
        queue_pixelX.push(pixel_x); //Better handling with q queue
        queue_pixelY.push(pixel_y);
      }
    }
  }
}
