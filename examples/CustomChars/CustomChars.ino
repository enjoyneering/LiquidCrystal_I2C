/***************************************************************************************************/
/*
  This is an Arduino sketch for LiquidCrystal_I2C library.

  This chip uses I2C bus to communicate, specials pins are required to interface
  Board:                                    SDA                    SCL                    Level
  Uno, Mini, Pro, ATmega168, ATmega328..... A4                     A5                     5v
  Mega2560................................. 20                     21                     5v
  Due, SAM3X8E............................. 20                     21                     3.3v
  Leonardo, Micro, ATmega32U4.............. 2                      3                      5v
  Digistump, Trinket, ATtiny85............. 0/physical pin no.5    2/physical pin no.7    5v
  Blue Pill, STM32F103xxxx boards.......... PB7                    PB6                    3.3v/5v
  ESP8266 ESP-01........................... GPIO0/D5               GPIO2/D3               3.3v/5v
  NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1               3.3v/5v
  ESP32.................................... GPIO21/D21             GPIO22/D22             3.3v

  Frameworks & Libraries:
  ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
  ESP32 Core            - https://github.com/espressif/arduino-esp32
  ESP8266 Core          - https://github.com/esp8266/Arduino
  ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
  STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

  GNU GPL license, all text above must be included in any redistribution, see link below for details:
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#pragma GCC optimize ("O2")    //code optimisation controls - "O2" & "O3" code performance, "Os" code size

#include <Wire.h>              //for ESP8266 use bug free i2c driver https://github.com/enjoyneering/ESP8266-I2C-Driver
#include <LiquidCrystal_I2C.h>

/* 
   Some usefull icons located in the LCD ROM, see p.9 of GDM2004D datasheet for details
   NOTE: your LCD could be different, use "romPattern.ino" example to find out what is in your ROM
*/
#define ARROW_LEFT  0x7E
#define ARROW_RIGHT 0x7F
#define DEGREE      0xDF
#define ALFA        0xE0
#define OMEGA       0xF4
#define SUM         0xF6
#define PI_SIGN     0xF7
#define DIVISION    0xFD
#define MICRO       0xE4
#define SQ_ROOT     0xE8
#define SPACE       0x20

#define COLUMS      20
#define ROWS        4

const uint8_t bell[8]    PROGMEM = {0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x00, 0x04, 0x00}; //PROGMEM saves variable to flash & keeps dynamic memory free
const uint8_t note[8]    PROGMEM = {0x01, 0x03, 0x05, 0x09, 0x0B, 0x1B, 0x18, 0x00};
const uint8_t watch[8]   PROGMEM = {0x00, 0x0E, 0x15, 0x17, 0x11, 0x0E, 0x00, 0x00};
const uint8_t heart[8]   PROGMEM = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};
const uint8_t duck[8]    PROGMEM = {0x00, 0xCC, 0x1D, 0x0F, 0x0F, 0x06, 0x00, 0x00};
const uint8_t check[8]   PROGMEM = {0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00, 0x00};
const uint8_t lock[8]    PROGMEM = {0x0E, 0x11, 0x11, 0x1F, 0x1B, 0x1B, 0x1F, 0x00};
      uint8_t battery[8]         = {0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00}; //saves variable to dynamic memory
      uint8_t temp[8]            = {0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x1F, 0x1F, 0x0E};

uint8_t icon = 0;
  
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  Serial.begin(115200);

  while (lcd.begin(COLUMS, ROWS) != 1) //colums - 20, rows - 4
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);   
  }

  lcd.print(F("PCF8574 is OK...")); //(F()) saves string to flash & keeps dynamic memory free
  delay(2000);

  lcd.clear();
  
  lcd.createChar(0, bell);    //variable stored in flash
  lcd.createChar(1, note);    //variable stored in flash
  lcd.createChar(2, watch);   //variable stored in flash
  lcd.createChar(3, heart);   //variable stored in flash
  lcd.createChar(4, duck);    //variable stored in flash
  lcd.createChar(5, check);   //variable stored in flash
  lcd.createChar(6, lock);    //variable stored in flash
  lcd.createChar(7, battery); //variable stored in dynamic memory
}

void loop()
{
  lcd.setCursor(random(0, COLUMS), random(0, ROWS));

  icon = random(0, 19);

  switch(icon)
  {
    case 0:
      lcd.write(icon);
      break;
    case 1:
      lcd.write(icon);
      break;
    case 2:
      lcd.write(icon);
      break;
    case 3:
      lcd.write(icon);
      break;
    case 4:
      lcd.write(icon);
      break;
    case 5:
      lcd.write(icon);
      break;
    case 6:
      lcd.write(icon);
      break;
    case 7:
      lcd.write(icon);
      break;
    case 8:
      lcd.write(ARROW_LEFT);
      break;
    case 9:
      lcd.write(ARROW_RIGHT);
      break;
    case 10:
      lcd.write(DEGREE);
      break;
    case 11:
      lcd.write(ALFA);
      break;
    case 12:
      lcd.write(OMEGA);
      break;
    case 13:
      lcd.write(SUM);
      break;
    case 14:
      lcd.write(PI_SIGN);
      break;
    case 15:
      lcd.write(DIVISION);
      break;
    case 16:
      lcd.write(MICRO);
      break;
    case 17:
      lcd.write(SQ_ROOT);
      break;
    case 18:
      lcd.write(SPACE);
      break;
  }
  delay(200);
}
