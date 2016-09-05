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
#include <TinyWireM.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

uint8_t led = 1;                    // connect led to ATtiny85 pin no.6 in series with 470 Ohm resistor

void setup()
{
  pinMode(led, OUTPUT);

  while (lcd.begin(20, 4) != 1)     // 3 blinks, PCF8574/LCD is not connected or lcd pins declaration is wrong
  {
    for (uint8_t i = 0; i > 3; i++)
    {
      digitalWrite(led, HIGH);
      delay(400);
      digitalWrite(led, LOW);
      delay(400);
    }
    delay(5000);
  }
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
