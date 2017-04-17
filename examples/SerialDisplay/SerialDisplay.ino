/***************************************************************************************************/
/*
  PCF8574 chip uses I2C bus to communicate, 2 pins are required to interface

  Connect PCF8574 to pins :  SDA     SCL
  Uno, Mini, Pro:            A4      A5
  Mega2560, Due:             20      21
  Leonardo:                  2       3
  ATtiny85:                  0(5)    2/A1(7) (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                              & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-xx:            ANY     ANY     (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:               ANY     ANY     (D2 & D1 by default)
*/
/***************************************************************************************************/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define COLUMS 20
#define ROWS   4
#define PAGE   COLUMS * ROWS

uint8_t char_counter = 0;

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  Serial.begin(115200);

  while (lcd.begin(COLUMS, ROWS) != 1) //colums - 20, rows - 4
  {
    Serial.println("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.");
    delay(5000);   
  }
  lcd.print("PCF8574 is OK...");
  delay(2000);
  lcd.clear();

  lcd.blink();
 
}

void loop()
{
  while (Serial.available() > 0)
  {
    char_counter += Serial.available();

    if (char_counter == PAGE)
    {
      char_counter = 0;
      lcd.clear();
    }
    
    lcd.write(Serial.read());
  }
}
