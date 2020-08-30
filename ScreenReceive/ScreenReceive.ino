
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <CircularBuffer.h>

CircularBuffer<int, 20> queue_pixelX;
CircularBuffer<int, 20> queue_pixelY;


#define BLACK   0x0000
#define WHITE   0xFFFF

String inputString = "";         // a String to hold incoming data
void setup(void)
{
    Serial.begin(115200);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    tft.begin(ID);
    tft.setRotation(1);            //LANDSCAPE
    tft.fillScreen(WHITE);
}

void loop(void)
{
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
  delay(1);
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
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
        queue_pixelX.push(pixel_x);
        queue_pixelY.push(pixel_y);
      }
    }
  }
}
