/***************************************************************************************************/
/*
  This is an Arduino sketch for LiquidCrystal_I2C library.

  PCF8574 chip uses I2C bus to communicate, specials pins are required to interface
  Board:                                    SDA                    SCL
  Uno, Mini, Pro........................... A4                     A5
  Mega2560, Due............................ 20                     21
  Leonardo................................. 2                      3
  Trinket/ATtiny85......................... 0/physical pin no.5    2/physical pin no.7
  Blue Pill/STM32F103xxxx boards........... B7                     B6 with 5v->3v logich converter
  ESP8266 ESP-01:.......................... GPIO0/D5               GPIO2/D3
  NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1

  Frameworks & Libraries:
  ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
  ATtiny I2C Master lib - https://github.com/SpenceKonde/TinyWireM
  ESP8266 Core          - https://github.com/esp8266/Arduino
  ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
  STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

  GNU GPL license, all text above must be included in any redistribution, see link below for details
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_SPACE_SYMBOL 0x20  //space symbol from the LCD ROM, see p.9 of GDM2004D datasheet
 
LiquidCrystal_I2C lcd_01(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE); //all three pads on the PCF8574 shield are open
LiquidCrystal_I2C lcd_02(PCF8574_ADDR_A20_A10_A00, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE); //all three pads on the PCF8574 shield are shorted
 

void setup()
{
  Serial.begin(115200);

  while (lcd_01.begin(20, 4) != 1 || lcd_02.begin(16, 2) != 1) //lcd1: colums - 20, rows - 4, lcd2: colums - 16, rows - 2
  {
    Serial.println("PCF8574_01 or PCF8574_02 is not connected or lcd pins declaration is wrong. Only pins numbers 4,5,6,16,11,12,13,14 are legal.");
    delay(5000);
  }

  lcd_01.print("PCF8574_01 is OK...");
  lcd_02.print("PCF8574_02 is OK...");
  delay(2000);

  lcd_01.clear();
  lcd_02.clear();

  /* prints static text */
  lcd_01.setCursor(0, 1);              //set 1-st colum & 2-nd row, 1-st colum & row started at zero
  lcd_01.print("LCD1 Hello world");
  lcd_01.setCursor(0, 2);
  lcd_01.print("Random number:");

  lcd_02.setCursor(0, 0);
  lcd_02.print("LCD2 Hello world");
  lcd_02.setCursor(0, 1);
  lcd_02.print("Random number:");
}

void loop()
{
  /* prints dynamic text */
  lcd_01.setCursor(14, 2);             //set 15-th colum & 3-rd  row. NOTE: 1-st colum & row started at zero
  lcd_01.print(random(10, 1000));
  lcd_01.write(LCD_SPACE_SYMBOL);

  lcd_02.setCursor(14, 1);             //set 15-th colum & 2-rd  row. NOTE: 1-st colum & row started at zero
  lcd_02.print(random(10, 1000));
  lcd_02.write(LCD_SPACE_SYMBOL);

  delay(1000);
}
