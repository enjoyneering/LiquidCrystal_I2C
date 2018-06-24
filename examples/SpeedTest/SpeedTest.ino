/***************************************************************************************************/
/*
  This is an Arduino sketch for LiquidCrystal_I2C library.

  PCF8574 chip uses I2C bus to communicate, specials pins are required to interface
  Board:                                    SDA                    SCL
  Uno, Mini, Pro, ATmega168, ATmega328..... A4                     A5
  Mega2560, Due............................ 20                     21
  Leonardo, Micro, ATmega32U4.............. 2                      3
  Digistump, Trinket, ATtiny85............. 0/physical pin no.5    2/physical pin no.7
  Blue Pill, STM32F103xxxx boards.......... PB7*                   PB6*
  ESP8266 ESP-01:.......................... GPIO0/D5               GPIO2/D3
  NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1

                                           *STM32F103xxxx pins B7/B7 are 5v tolerant, but
                                            bi-directional logic level converter is recommended

  Frameworks & Libraries:
  ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
  ESP8266 Core          - https://github.com/esp8266/Arduino
  ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
  STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

  GNU GPL license, all text above must be included in any redistribution, see link below for details
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define COLUMS      20
#define ROWS        4

char alphabet[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

char     letter     = '0';
uint32_t startTimer =  0;
  
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  #if defined(ESP8266)
  WiFi.persistent(false);               //disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin();               //disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep
  #endif
  
  Serial.begin(115200);

  while (lcd.begin(COLUMS, ROWS) != 1) //colums - 20, rows - 4
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);   
  }

  lcd.print(F("PCF8574 is OK..."));    //(F()) saves string to flash & keeps dynamic memory free
  delay(2000);

  lcd.clear();
}

void loop()
{
  lcd.setCursor(random(0, COLUMS), random(0, ROWS));

  letter = alphabet[random(0, 26)];

  startTimer = micros();

  lcd.print(letter);

  Serial.println(micros() - startTimer);
  
  delay(200);
}
