/******************************************************************************/
/*
  PCF8574 chip uses I2C bus to communicate, 2 pins are required to  
  interface.

  Connect PCF8574 to pins :  SDA     SCL
  Uno, Mini, Pro:            A4      A5
  Mega2560, Due:             20      21
  Leonardo:                  2       3
  ATtiny85:                  0(5)    2/A1(7) (TinyWireM   - https://github.com/SpenceKonde/TinyWireM & ATTinyCore - https://github.com/SpenceKonde/ATTinyCore)
  ESP8266 ESP-xx:            ANY     ANY     (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:               ANY     ANY     (D1 & D2 by default)
*/
/******************************************************************************/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

uint8_t lcd_colums = 20;
uint8_t lcd_rows   = 4;

uint8_t l_channel = A0;
uint8_t r_channel = A1;

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  Serial.begin(115200);
  Serial.flush();

  if (lcd.begin(lcd_colums, lcd_rows) != 1)
  {
    Serial.println("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.");
  }
  else
  {
    Serial.println("PCF8574 is OK...");
  }
}

void loop()
{
  lcd.printHorizontalGraph('L', 0, analogRead(l_channel), 1023); //name of the bar, first  row, current value, max. value
  lcd.printHorizontalGraph('R', 1, analogRead(r_channel), 1023); //name of the bar, second row, current value, max. value
}
