#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int moisturePin = A0;
const int temperaturePin = A1;      // pin of temperature sensor
const int B = 3975;                  // B value of the thermistor

const int MOISTURE_MIN = 0;
const int MOISTURE_MAX = 950;
const float HUE_MAX = 240;

typedef struct {
  int red;
  int green;
  int blue;
} Color;

void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Welcome");
  delay(1000);
  lcd.setRGB(255,255,255);
}

float read_temperature()
{
  int value = analogRead(temperaturePin);                              // get analog value
  float resistance = (float)(1023-value)*10000/value;                  // get resistance
  float temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;     // calc temperature
  return temperature;
}

int read_moisture() {
  int value = analogRead(moisturePin);
  return value;
}

Color hsv_to_rgb(float degrees) {
  float saturation = 1.0;
  float value = 1.0;

  float c = saturation * value;
  float x = c * ( 1 - fabs(fmod((degrees / 60.0), 2) - 1));
  float m = value - c;

  float red = 0;
  float green = 0;
  float blue = 0;
  
  if (0 <= degrees && degrees < 60) {
    red = c;
    green = x;
    blue = 0;
  }
  else if (60 <= degrees && degrees < 120) {
    red = x;
    green = c;
    blue = 0;
  }
  else if (120 <= degrees && degrees < 180) {
    red = 0;
    green = c;
    blue = x;
  }
  else if (180 <= degrees && degrees < 240) {
    red = 0;
    green = x;
    blue = c;
  }
  else if (240 <= degrees && degrees < 300) {
    red = x;
    green = 0;
    blue = c;
  }
  else if (300 <= degrees && degrees < 360) {
    red = c;
    green = 0;
    blue = x;
  }

  Color color;

  color.red = (int)((red + m) * 255);
  color.green = (int)((green + m) * 255);
  color.blue = (int)((blue + m) * 255);
  
  return color;
}

float moisture_to_hue(float moisture) {
  return moisture / (MOISTURE_MAX - MOISTURE_MIN) * HUE_MAX;
}

const char* moisture_to_text(float moisture) {
  if (moisture <= 300) {
    return "dry";
  }
  else if(moisture <= 700) {
    return "ok";
  }
  else return "wet";
}

void loop() {
  lcd.clear();
 
  int moisture = read_moisture();
  float temperature = read_temperature();
  
  Color c = hsv_to_rgb(moisture_to_hue(moisture));
  lcd.setRGB(c.red,c.green,c.blue);
  
  lcd.print("Temp:  ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1); 
  lcd.print("Moist: ");
  lcd.print(moisture);
  lcd.print(" (");
  lcd.print(moisture_to_text(moisture));
  lcd.print(")");
  delay(5000);          // delay 5s
}
