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
#include <Wire.h>              //for ESP8266 use bug free i2c driver https://github.com/enjoyneering/ESP8266-I2C-Driver
#include <LiquidCrystal_I2C.h>

#define LCD_SPACE_SYMBOL 0x20  //space symbol from the lcd ROM, see p.9 of GDM2004D datasheet
 
LiquidCrystal_I2C lcd_01(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE); //all three address pads on the PCF8574 shield are open
LiquidCrystal_I2C lcd_02(PCF8574_ADDR_A20_A10_A00, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE); //all three address pads on the PCF8574 shield are shorted
 

void setup()
{
  Serial.begin(115200);

  while (lcd_01.begin(20, 4) != 1 || lcd_02.begin(16, 2) != 1) //lcd1: colums - 20, rows - 4, lcd2: colums - 16, rows - 2
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);
  }

  lcd_01.print(F("PCF8574_01 is OK...")); //(F()) saves string to flash & keeps dynamic memory free
  lcd_02.print(F("PCF8574_02 is OK..."));
  delay(2000);

  lcd_01.clear();
  lcd_02.clear();

  /* prints static text */
  lcd_01.setCursor(0, 1);                 //set 1-st colum & 2-nd row, 1-st colum & row started at zero
  lcd_01.print(F("LCD1 Hello world"));
  lcd_01.setCursor(0, 2);
  lcd_01.print(F("Random number:"));

  lcd_02.setCursor(0, 0);
  lcd_02.print(F("LCD2 Hello world"));
  lcd_02.setCursor(0, 1);
  lcd_02.print(F("Random number:"));
}

void loop()
{
  /* prints dynamic text */
  lcd_01.setCursor(14, 2);                //set 15-th colum & 3-rd  row, 1-st colum & row started at zero
  lcd_01.print(random(10, 1000));
  lcd_01.write(LCD_SPACE_SYMBOL);

  lcd_02.setCursor(14, 1);                //set 15-th colum & 2-rd  row, 1-st colum & row started at zero
  lcd_02.print(random(10, 1000));
  lcd_02.write(LCD_SPACE_SYMBOL);

  delay(1000);
}
