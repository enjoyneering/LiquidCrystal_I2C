/***************************************************************************************************/
/*
   This is an Arduino sketch for LiquidCrystal_I2C library

   Single character printing speed test results:
   ESP8266   at 80MHz....................... 1155us*, 274us**
   STM32     at 72MHz....................... 902us*                    
   ATmega328 at 16MHz....................... 1088us*
                                            *I2C speed 100KHz, stretch 1000usec
                                           **I2C speed 400KHz, stretch 1000usec


   This device uses I2C bus to communicate, specials pins are required to interface
   Board                                     SDA              SCL              Level
   Uno, Mini, Pro, ATmega168, ATmega328..... A4               A5               5v
   Mega2560................................. 20               21               5v
   Due, SAM3X8E............................. 20               21               3.3v
   MKR Zero, XIAO SAMD21, SAMD21xx.......... PA08             PA09             3.3v
   Leonardo, Micro, ATmega32U4.............. 2                3                5v
   Digistump, Trinket, Gemma, ATtiny85...... PB0/D0           PB2/D2           3.3v/5v
   Blue Pill*, STM32F103xxxx boards*........ PB7/PB9          PB6/PB8          3.3v/5v
   ESP8266 ESP-01**......................... GPIO0            GPIO2            3.3v/5v
   NodeMCU 1.0**, WeMos D1 Mini**........... GPIO4/D2         GPIO5/D1         3.3v/5v
   ESP32***................................. GPIO21/D21       GPIO22/D22       3.3v
                                             GPIO16/D16       GPIO17/D17       3.3v
                                            *hardware I2C Wire mapped to Wire1 in stm32duino
                                             see https://github.com/stm32duino/wiki/wiki/API#I2C
                                           **most boards has 10K..12K pullup-up resistor
                                             on GPIO0/D3, GPIO2/D4/LED & pullup-down on
                                             GPIO15/D8 for flash & boot
                                          ***hardware I2C Wire mapped to TwoWire(0) aka GPIO21/GPIO22 in Arduino ESP32

   Supported frameworks:
   Arduino Core - https://github.com/arduino/Arduino/tree/master/hardware
   ATtiny  Core - https://github.com/SpenceKonde/ATTinyCore
   ESP8266 Core - https://github.com/esp8266/Arduino
   ESP32   Core - https://github.com/espressif/arduino-esp32
   STM32   Core - https://github.com/stm32duino/Arduino_Core_STM32
   SAMD    Core - https://github.com/arduino/ArduinoCore-samd


   GNU GPL license, all text above must be included in any redistribution,
   see link for details - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define COLUMS        20      //LCD columns
#define ROWS          4       //LCD rows
#define I2C_BUS_SPEED 400000  //I2C bus speed 400000Hz
#define ALPHABET_SIZE 26      //size of english alphabet array

uint32_t startTimer;
char     letter;
char     alphabet[ALPHABET_SIZE] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
  
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  #if defined(ESP8266)
  WiFi.persistent(false);                            //disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin();                            //disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep
  #endif
  
  Serial.begin(115200);

  while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1)  //colums, rows, characters size
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);   
  }

//Wire.setClock(I2C_BUS_SPEED);                      //change I2C bus speed

  lcd.print(F("PCF8574 is OK..."));                  //(F()) saves string to flash & keeps dynamic memory free
  delay(2000);

  lcd.clear();
}

void loop()
{
  lcd.setCursor(random(0, COLUMS), random(0, ROWS)); //set random column & row

  letter = alphabet[random(0, ALPHABET_SIZE)];       //copy random char from array

  startTimer = micros();                             //start timer, in usec

  lcd.print(letter);                                 //print char

  Serial.println(micros() - startTimer);             //stop timer, in usec
  
  delay(200);
}
