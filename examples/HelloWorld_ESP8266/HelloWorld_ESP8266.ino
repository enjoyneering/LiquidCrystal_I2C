/***************************************************************************************************/
/*
  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  This chip uses I2C bus to communicate, 2 pins are required to interface

  Connect chip to pins:    SDA     SCL
  Uno, Mini, Pro:          A4      A5
  Mega2560, Due:           20      21
  Leonardo:                2       3
  ATtiny85:                0(5)    2/A1(7) (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                            & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-xx:          ANY     ANY     (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:             ANY     ANY     (D2 & D1 by default)

*/
/***************************************************************************************************/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
 
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


  lcd_01.setCursor(0, 1);              //set 1-st colum & 2-nd row. NOTE: 1-st colum & row started at zero
  lcd_01.print("LCD1 Hello world");

  lcd_02.setCursor(0, 1);              //set 1-st colum & 2-nd row. NOTE: 1-st colum & row started at zero
  lcd_02.print("LCD2 Hello world");
}

void loop()
{
  lcd_01.setCursor(0, 2);              //set 1-st colum & 3-rd  row. NOTE: 1-st colum & row started at zero
  lcd_01.print("Random number:");
  lcd_01.print(random(10, 100) + " ");
  delay(4000);
}
