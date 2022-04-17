// TO DO
// implement display sleep
// Add readme/diagram
//
//
//
//If using a Chinese Arduino Nano. Install the baords drivers and the set `Change Tools > Processor > old bootloader`
//https://forum.arduino.cc/t/cant-upload-to-arduino-nano/347718/2
//http://www.wch.cn/download/CH341SER_ZIP.html
/*
                                                                                                      +-------------+
                                                                                                      |Relay Board  |
                           +---------------------------------------------------------+                |             |
                           |                                                         |                |             |
                           | +-----------------------------------------------------+ |                |             |
                           | |                                                     | |                |             |
                           | |                                                     | |                |             |
                           | |                                                     | +----------------+[] GND       |
                           | |                                                     | +----------------|[] IN1       |
                           | |        +---+[PWR]+------------------+ USB +--+      | | +--------------+[] IN2       |
                           | |        |                            +-----+  |      | | |              |[] IN3       |
                           | |        |         GND/RST2  [ ][ ]            |      | | |              |[] IN4       |
                           | |        |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] |      +---|--------------+[] VOC       |
                           | |        |          5V/MISO2 [ ][ ]  A4/SDA[ ] |        | |              |             |
                           | |        |                             AREF[ ] |        | |              |             |
                           | |        |                              GND[ ] |        | |              +-------------+
                           | |        | [ ]N/C                    SCK/13[ ] |        | |
                           | |        | [ ]IOREF                 MISO/12[ ] |        | |
                           | |        | [ ]RST                   MOSI/11[ ]~|        | |
                           | |        | [ ]3V3    +---+               10[ ]~|        | |
                           | +--------+ [ ]5v    ++ A ++               9[ ]~|        | |
                           +----------+ [ ]GND   || R ||               8[ ] |        | |
                           | |        | [ ]GND   || D ||                    |        | |
                           | |        | [ ]Vin   || U ||               7[ ] |        | |
+--------------+           | |        |          || I ||               6[ ]~+--------+ |
|LCD SSD1306   |           | |        | [ ]A0    || N ||               5[ ]~+----------+
|        GND []+-----------+ |        | [ ]A1    ++ O ++               4[ ] |
|        VDD []|-------------+        | [ ]A2     +---+           INT1/3[ ]~|
|        SCK []|---------------+      | [ ]A3                     INT0/2[ ] |
|        SDA []+---------------|------+ [ ]A4/SDA  RST SCK MISO     TX^1[ ] |
+--------------+               +------+ [ ]A5/SCL  [ ] [ ] [ ]      RXv0[ ] |
                                      |            [ ] [ ] [ ]              +
                                      +  UNO_R3    GND MOSI 5V  ____________/
                                       \_______________________/


                                                                                       +   +      +   +
                                                                                      ++---++    ++---++
                                                                                      |     |    |     |
                                                                                      |     |    |     |
                                                                                      |     |    |     |
                                                                                      ++---++    ++---++
                                                                                       +   +      +   +
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int Relay1Pin = 10;    //1st Relay Pin
const int Relay2Pin = 9;    //2nd Relay Pin
const int Relay3Pin = 6;    //3rd Relay Pin
const int Relay4Pin = 5;    //4th Relay Pin

const int button1Pin = 2;   // Pushbutton pin
const int button2Pin = 4;  // Pushbutton pin

int button1State = 0;         // variable for reading the pushbutton status
int button1Toggle = 0;        // variable to only change each time button is pressed

int button2State = 0;         // variable for reading the pushbutton status
int button2Toggle = 0;        // variable to only change each time button is pressed

int selection = 10; // determine which item to highlight
int timer1 = 0; //Timer for relay 1 in seconds
int timer2 = 0; //Timer for relay 2 in seconds
int timer3 = 0; //Timer for relay 3 in seconds
int timer4 = 0; //Timer for relay 4 in seconds

// These variables are for the subTime function to be called every second without using delays
unsigned long previousMillis = 0;
unsigned long interval = 1000;
//int a = 60;

void setup() {
  // Set RelayPin as an output pin
  pinMode(Relay1Pin, OUTPUT);
  pinMode(Relay2Pin, OUTPUT);

  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);

  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(1,1);
  display.println("STARTING..");

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();

}

void cursorCycle(){
  selection += 10;
  if (selection > 40){
    selection = 10;
  }
//  if (selection == 10){
//    selection = 20;
//  } else {
//    selection = 10;
//  }
}

void drawCursor(){
  display.drawTriangle(0, selection, 5, selection +3, 0, selection + 6, WHITE);
}

void relayState(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  if (timer1 > 0){
    // Turn on relay
    digitalWrite(Relay1Pin, LOW);
    display.setCursor(10,10);
    display.print("Zone1:ON ");
    display.print(timer1/60);
    display.print("min");
  } else {
    // Turn off relay
    digitalWrite(Relay1Pin, HIGH);
    display.setCursor(10,10);
    display.print("Zone1:OFF");
  }
  if (timer2 > 0){
    // Turn on relay
    digitalWrite(Relay2Pin, LOW);
    display.setCursor(10,20);
    display.print("Zone2:ON ");
    display.print(timer2/60);
    display.print("min");
  } else {
    // Turn off relay
    digitalWrite(Relay2Pin, HIGH);
    display.setCursor(10,20);
    display.print("Zone2:OFF");
  }
    if (timer3 > 0){
    // Turn on relay
    digitalWrite(Relay3Pin, LOW);
    display.setCursor(10,30);
    display.print("Zone3:ON ");
    display.print(timer3/60);
    display.print("min");
  } else {
    // Turn off relay
    digitalWrite(Relay3Pin, HIGH);
    display.setCursor(10,30);
    display.print("Zone3:OFF");
  }
    if (timer4 > 0){
    // Turn on relay
    digitalWrite(Relay4Pin, LOW);
    display.setCursor(10,40);
    display.print("Zone4:ON ");
    display.print(timer4/60);
    display.print("min");
  } else {
    // Turn off relay
    digitalWrite(Relay4Pin, HIGH);
    display.setCursor(10,40);
    display.print("Zone4:OFF");
  }
}

void addTime(){
  if (selection == 10){
    timer1 += 300;
  }else if (selection == 20){
    timer2 += 300;
  }else if (selection == 30){
    timer3 += 300;
  }else if (selection == 40){
    timer4 += 300;
  }
}

void subTime(){
  static const unsigned long REFRESH_INTERVAL = 1000; // ms
  static unsigned long lastRefreshTime = 0;
  if (timer1 > 0){
    timer1 -= 1;
  }
  if (timer2 > 0){
    timer2 -= 1;
  }
}

void resTime(){
  if (selection == 10){
    timer1 = 0;
  }else if (selection == 20){
    timer2 = 0;
  }else if (selection == 30){
    timer3 = 0;
  }else if (selection == 40){
    timer4 = 0;
  }
}

void loop() {
  // read the state of the pushbuttons:
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);

  display.clearDisplay();
  drawCursor();
  relayState();

  // check if the pushbutton is pressed. HIGH = yes, LOW = no
  if (button1State == HIGH) {
    if (button1Toggle == 0){
      cursorCycle();
      drawCursor();
    }
    button1Toggle = 1;
  } else {
    button1Toggle = 0;
  }
  
  // check if the pushbutton is pressed. HIGH = yes, LOW = no
  if (button2State == HIGH) {
    if (button2Toggle == 0){
      addTime();
    }
    button2Toggle += 1;
    if (button2Toggle > 50){
      resTime();
      button2Toggle = 1;
    }
  } else {
    button2Toggle = 0;
  }

  display.display();

  // Call subTime function every second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
   previousMillis = currentMillis;
    subTime();
  }
}
