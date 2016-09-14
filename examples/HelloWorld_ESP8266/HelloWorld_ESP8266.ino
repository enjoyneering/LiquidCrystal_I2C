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

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  Serial.begin(115200);
  Serial.flush();

  while (lcd.begin(20, 4, LCD_5x8DOTS, D1, D2) != 1)  //colums - 20, rows - 4, pixels - LCD_5x8DOTS, SDA - D1, SCL - D2
  {
    Serial.println("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.");
    delay(5000);
  }
  lcd.print("PCF8574 is OK...");
}


void loop()
{
   lcd.setCursor(0, 1);         //set first colum & second row. NOTE: first colum & row started at zero
   lcd.print("Hello world!");

   lcd.setCursor(0, 2);         //set first colum & third  row. NOTE: first colum & row started at zero
   lcd.print("Random number:");
   lcd.print(random(10, 100));

   delay(1000);
}
