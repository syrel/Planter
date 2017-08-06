#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int temperaturePin = A3;      // pin of temperature sensor

const int B = 3975;                  // B value of the thermistor

const int MOISTURE_MIN = 400;
const int MOISTURE_MAX = 800;
const float HUE_MAX = 240;
const float HUE_MIN = 0;

const char* MESSAGE_DRY = "dry";
const char* MESSAGE_OK = " ok";
const char* MESSAGE_WET = "wet";

const int moistureSensors[] = { A0, A1, A2 };

typedef struct {
  int red;
  int green;
  int blue;
} Color;

typedef struct {
  bool isPressed;
  int pin;
} Button;

int moistureSensor = 0;
unsigned long lastRenderTime;
bool requestRender;
Button moistureButton;


void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Welcome");
  delay(1000);
  lcd.setRGB(255,255,255);

  moistureButton.pin = 8;
  // Configure the button's pin for input signals.
  pinMode(moistureButton.pin, INPUT);
}

bool is_pressed(Button aButton) {
  return digitalRead(aButton.pin);
}

void switch_moisture() {
  moistureSensor++;
  moistureSensor = moistureSensor % (sizeof(moistureSensors) / sizeof(moistureSensors[0]));
}

float read_temperature()
{
  int value = analogRead(temperaturePin);                              // get analog value
  float resistance = (float)(1023-value)*10000/value;                  // get resistance
  float temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;     // calc temperature
  return temperature;
}

int read_moisture() {
  int value = analogRead(moistureSensors[moistureSensor]);
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
  float value = max(min(moisture, MOISTURE_MAX),MOISTURE_MIN);
  return (value - MOISTURE_MIN) / (MOISTURE_MAX - MOISTURE_MIN) * HUE_MAX + HUE_MIN;
}

const char* moisture_to_text(float moisture) {
  if (moisture <= 450) {
    return MESSAGE_DRY;
  }
  else if(moisture <= 650) {
    return MESSAGE_OK;
  }
  else return MESSAGE_WET;
}

void process_events() {
  if (is_pressed(moistureButton)) {
    if (!moistureButton.isPressed) {
      switch_moisture();
      requestRender = true;
    }
    moistureButton.isPressed = true;
  }
  else {
    moistureButton.isPressed = false;
  }
}

void process_render() {
  if (requestRender || (abs(millis() - lastRenderTime) > 5000)) {
    requestRender = false;
    render();
    lastRenderTime = millis();
  }
}

void render() {
  lcd.clear();
 
  int moisture = read_moisture();
  float temperature = read_temperature();
  
  Color c = hsv_to_rgb(moisture_to_hue(moisture));
  lcd.setRGB(c.red,c.green,c.blue);
  
  lcd.print("Temp:  ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1); 
  lcd.print("Moist(");
  lcd.print(moistureSensor + 1);
  lcd.print(") ");
  lcd.print(moisture);
  lcd.print(" ");
  lcd.setCursor(13, 1);
  lcd.print(moisture_to_text(moisture));
}

void loop() {
  process_events();
  process_render();
  delay(1);          // delay 1ms
}
