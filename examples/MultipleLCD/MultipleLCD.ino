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
 
LiquidCrystal_I2C lcd_01(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE); //all three pads on the PCF8574 shield are open
LiquidCrystal_I2C lcd_02(PCF8574_ADDR_A20_A10_A00, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE); //all three pads on the PCF8574 shield are shorted
 

void setup()
{
  Serial.begin(115200);

  if (lcd_01.begin(20, 4) != 1) //colums - 20, rows - 4
  {
    Serial.println("PCF8574_01 is not connected or lcd pins declaration is wrong. Only pins numbers 4,5,6,16,11,12,13,14 are legal.");
  }
  else
  {
    lcd_01.print("PCF8574_01 is OK...");
  }

  if (lcd_02.begin(16, 2) != 1) //colums - 16, rows - 2
  {
    Serial.println("PCF8574_02 is not connected or lcd pins declaration is wrong. Only pins numbers 4,5,6,16,11,12,13,14 are legal.");
  }
  else
  {
    lcd_02.print("PCF8574_02 is OK...");
  }
}

void loop()
{
   lcd_01.setCursor(0, 1);           //set first colum & second row. NOTE: first colum & row started at zero
   lcd_01.print("LCD1 Hello world");

   lcd_01.setCursor(0, 2);           //set first colum & third  row. NOTE: first colum & row started at zero
   lcd_01.print("Random number:");
   lcd_01.print(random(10, 100));

   lcd_02.setCursor(0, 1);           //set first colum & second row. NOTE: first colum & row started at zero
   lcd_02.print("LCD2 Hello world");

   delay(4000);
}
