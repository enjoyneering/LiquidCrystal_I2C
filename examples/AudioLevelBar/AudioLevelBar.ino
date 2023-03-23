/***************************************************************************************************/
/*
   This is an Arduino sketch for LiquidCrystal_I2C library

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
#pragma GCC optimize ("O3") //code optimisation controls - "O2" & "O3" code performance, "Os" code size

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define COLUMS    20 //LCD columns
#define ROWS      4  //LCD rows
#define L_CHANNEL A0 //left audio input pin
#define R_CHANNEL A1 //right audio input pin


LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  Serial.begin(115200);

  while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1) //colums, rows, characters size
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);   
  }

  lcd.print(F("PCF8574 is OK...")); //(F()) saves string to flash & keeps dynamic memory free
  delay(2000);

  lcd.clear();
}

void loop()
{
  lcd.printHorizontalGraph('L', 0, analogRead(L_CHANNEL), 1023); //bar name, 1-st  row, current value, maximum value
  lcd.printHorizontalGraph('R', 1, analogRead(R_CHANNEL), 1023); //bar name, 2-nd  row, current value, maximum value
}
