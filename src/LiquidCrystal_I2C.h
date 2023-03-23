/***************************************************************************************************/
/*
   This is an Arduino library for HD44780, S6A0069, KS0066U, NT3881D, LC7985, ST7066, SPLC780,
   WH160xB, AIP31066, GDM200xD, ADM0802A LCD displays

   written by : enjoyneering
   sourse code: https://github.com/enjoyneering/

   NOTE:
   - screens are operated in 4-bit mode over I2C bus with 8-bit I/O expander PCF8574x
   - typical displays sizes: 8x2, 16x1, 16x2, 16x4, 20x2, 20x4 & etc


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

#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

#include <Arduino.h>
#include <Wire.h>

#if defined (ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>          //for Arduino AVR PROGMEM support
#elif defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
#include <pgmspace.h>              //for Arduino ESP8266 PROGMEM support
#elif defined (ARDUINO_ARCH_STM32) || defined (ARDUINO_ARCH_SAMD)
#include <avr/pgmspace.h>          //for Arduino STM32 & SAMD21 PROGMEM support
#endif



/* 
   LCD main register commands
   NOTE: all commands formated as RS=(0:IR write & BF read, 1:DR write/read), RW=(0:write, 1:read), E=1, DB7=0, DB6=0, DB5=0, DB4=0, BCK_LED=0
*/
#define LCD_INSTRUCTION_WRITE    0x20   //writes command to Instruction Register (IR)
#define LCD_DATA_WRITE           0xA0   //writes data/text to Data Register (DR)
#define LCD_DATA_READ            0xE0   //reads data(text) from Data Register (DR)/DB7..DB0 bits value
#define LCD_BUSY_FLAG_READ       0x60   //reads Busy Flag (BF) & address counter/cursor position


/*
   "LCD_INSTRUCTION_WRITE" instructions
   NOTE: all instructions formated as DB7=(set DDRAM), DB6=(set CGRAM), DB5=(function set), DB4=(cursor shift), DB3=(disp. control), DB2=(entry mode set), DB1, DB0
*/
#define LCD_CLEAR_DISPLAY        0x01   //clears display & move cursor to home position
#define LCD_RETURN_HOME          0x02   //moves cursor position to home position
#define LCD_ENTRY_MODE_SET       0x04   //sets cursor type, text direction (I/D) & display shift direction (S)
#define LCD_DISPLAY_CONTROL      0x08   //sets display on/off (D), cursor on/off (C) & cursor blinking on/off (B)
#define LCD_CURSOR_DISPLAY_SHIFT 0x10   //cursor or display/text shifts without changing DDRAM contents
#define LCD_FUNCTION_SET         0x20   //sets data length (DL), number of lines (N) & font size (F)
#define LCD_CGRAM_ADDR_SET       0x40   //sets CGRAM address. CGRAM data is sent & received after this setting
#define LCD_DDRAM_ADDR_SET       0x80   //sets DDRAM address/cursor position


/*
   "LCD_ENTRY_MODE_SET" controls
   NOTE: all controls formated as DB7, DB6, DB5, DB4, DB3, DB2, DB1=(I/D), DB0=(S)
*/
#define LCD_ENTRY_RIGHT          0x00   //sets text direction decrement/"right to left" (I/D)
#define LCD_ENTRY_LEFT           0x02   //sets text direction increment/"left to right" (I/D)
#define LCD_ENTRY_SHIFT_ON       0x01   //text shifts when byte written & cursot stays (S)
#define LCD_ENTRY_SHIFT_OFF      0x00   //text stays & cursor moves when byte written  (S)


/*
   "LCD_DISPLAY_CONTROL" controls
   NOTE: all controls formated as DB7, DB6, DB5, DB4, DB3, DB2=(D), DB1=(C), DB0=(B)
*/
#define LCD_DISPLAY_ON           0x04   //turns display ON/retrive text (D)
#define LCD_DISPLAY_OFF          0x00   //turns display OFF/clears text (D)
#define LCD_UNDERLINE_CURSOR_ON  0x02   //turns ON  underline cursor (C)
#define LCD_UNDERLINE_CURSOR_OFF 0x00   //turns OFF underline cursor (C)
#define LCD_BLINK_CURSOR_ON      0x01   //turns ON  blinking  cursor (B)
#define LCD_BLINK_CURSOR_OFF     0x00   //turns OFF blinking  cursor (B)


/*
   "LCD_CURSOR_DISPLAY_SHIFT" controls
   NOTE: all controls formated as DB7, DB6, DB5, DB4, DB3=(SC), DB2=(RL), DB1=*, DB0=*
*/
#define LCD_DISPLAY_SHIFT        0x08   //display/text shifts after char print (SC)
#define LCD_CURSOR_SHIFT         0x00   //cursor shifts after char print       (SC)
#define LCD_SHIFT_RIGHT          0x04   //cursor or display/text shifts to right (RL)
#define LCD_SHIFT_LEFT           0x00   //cursor or display/text shifts to left  (RL)


/*
   "LCD_FUNCTION_SET" controls
   NOTE: all controls formated as DB7, DB6, DB5, DB4=(DL), DB3=(N), DB2=(F), DB1=*, DB0=*
*/
#define LCD_8BIT_MODE            0x10   //select 8-bit interface            (DL)
#define LCD_4BIT_MODE            0x00   //select 4-bit interface            (DL)
#define LCD_1_LINE               0x00   //selects one line display          (N)
#define LCD_2_LINE               0x08   //selects two or more lines display (N)


/* LCD misc */
#define LCD_HOME_CLEAR_DELAY     2      //duration of home & clear commands, in milliseconds
#define LCD_COMMAND_DELAY        43     //duration of command, HD44780 & clones delay varies 37usec..43usec
#define LCD_CMD_LENGTH_8BIT      8      //8-bit command length
#define LCD_CMD_LENGTH_4BIT      4      //4-bit command length
#define LCD_COLUMNS_SIZE         16     //default number of columns
#define LCD_ROWS_SIZE            2      //default number of rows
#define LCD_I2C_SPEED            100000 //default I2C speed 100KHz..400KHz, in Hz
#define LCD_I2C_ACK_STRETCH      1000   //default I2C stretch time, in microseconds


/* PCF8574 misc controls */
#define LCD_BACKLIGHT_ON         0x01
#define LCD_BACKLIGHT_OFF        0x00
#define PCF8574_PORTS_LOW        0x00   //sets PCF8574 pins to RS=0,RW=0,E=0,DB7=0,DB6=0,DB5=0,DB4=0,BCK_LED=0
#define PCF8574_LCD_DATA_HIGH    0x3E   //sets PCF8574 pins to RS=0,RW=0,E=1,DB7=1,DB6=1,DB5=1,DB4=1,BCK_LED=0


typedef enum : uint8_t
{
  LCD_5x10DOTS                 = 0x04,  //5x9+1 dots charecter+cursor font (F), 5x10 dots total
  LCD_5x8DOTS                  = 0x00   //5x7+1 dots charecter+cursor font (F), 5x8  dots total
}
lcdFontSize;


/* PCF8574 & PCF8574A addresses */
typedef enum : uint8_t
{
  PCF8574_ADDR_A21_A11_A01     = 0x27,  //I2C address A2 = 1, A1 = 1, A0 = 1 (by default)
  PCF8574_ADDR_A21_A11_A00     = 0x26,  //I2C address A2 = 1, A1 = 1, A0 = 0
  PCF8574_ADDR_A21_A10_A01     = 0x25,  //I2C address A2 = 1, A1 = 0, A0 = 1
  PCF8574_ADDR_A21_A10_A00     = 0x24,  //I2C address A2 = 1, A1 = 0, A0 = 0
  PCF8574_ADDR_A20_A11_A01     = 0x23,  //I2C address A2 = 0, A1 = 1, A0 = 1
  PCF8574_ADDR_A20_A11_A00     = 0x22,  //I2C address A2 = 0, A1 = 1, A0 = 0
  PCF8574_ADDR_A20_A10_A01     = 0x21,  //I2C address A2 = 0, A1 = 0, A0 = 1
  PCF8574_ADDR_A20_A10_A00     = 0x20,  //I2C address A2 = 0, A1 = 0, A0 = 0

  PCF8574A_ADDR_A21_A11_A01    = 0x3F,  //I2C address A2 = 1, A1 = 1, A0 = 1
  PCF8574A_ADDR_A21_A11_A00    = 0x3E,  //I2C address A2 = 1, A1 = 1, A0 = 0
  PCF8574A_ADDR_A21_A10_A01    = 0x3D,  //I2C address A2 = 1, A1 = 0, A0 = 1
  PCF8574A_ADDR_A21_A10_A00    = 0x3C,  //I2C address A2 = 1, A1 = 0, A0 = 0
  PCF8574A_ADDR_A20_A11_A01    = 0x3B,  //I2C address A2 = 0, A1 = 1, A0 = 1
  PCF8574A_ADDR_A20_A11_A00    = 0x3A,  //I2C address A2 = 0, A1 = 1, A0 = 0
  PCF8574A_ADDR_A20_A10_A01    = 0x39,  //I2C address A2 = 0, A1 = 0, A0 = 1
  PCF8574A_ADDR_A20_A10_A00    = 0x38   //I2C address A2 = 0, A1 = 0, A0 = 0
}
pcf8574Address;


/* PCF8574 backlight controls */
typedef enum : uint8_t
{
  POSITIVE                     = 0x01,  //backlight switching polarity
  NEGATIVE                     = 0x00   //backlight switching polarity
}
backlightPolarity;



class LiquidCrystal_I2C : public Print
{
  public:
   LiquidCrystal_I2C(pcf8574Address = PCF8574_ADDR_A21_A11_A01, uint8_t P0 = 4, uint8_t P1 = 5, uint8_t P2 = 6, uint8_t P3 = 16, uint8_t P4 = 11, uint8_t P5 = 12, uint8_t P6 = 13, uint8_t P7 = 14, backlightPolarity = POSITIVE);
 
  #if defined (ARDUINO_ARCH_AVR)
   bool begin(uint8_t columns = LCD_COLUMNS_SIZE, uint8_t rows = LCD_ROWS_SIZE, lcdFontSize = LCD_5x8DOTS, uint32_t speed = LCD_I2C_SPEED, uint32_t stretch = LCD_I2C_ACK_STRETCH);
  #elif defined (ARDUINO_ARCH_ESP8266)
   bool begin(uint8_t columns = LCD_COLUMNS_SIZE, uint8_t rows = LCD_ROWS_SIZE, lcdFontSize = LCD_5x8DOTS, uint8_t sda = SDA, uint8_t scl = SCL, uint32_t speed = LCD_I2C_SPEED, uint32_t stretch = LCD_I2C_ACK_STRETCH);
  #elif defined (ARDUINO_ARCH_ESP32)
   bool begin(uint8_t columns = LCD_COLUMNS_SIZE, uint8_t rows = LCD_ROWS_SIZE, lcdFontSize = LCD_5x8DOTS, int32_t sda = SDA, int32_t scl = SCL, uint32_t speed = LCD_I2C_SPEED, uint32_t stretch = LCD_I2C_ACK_STRETCH);
  #elif defined (ARDUINO_ARCH_STM32)
   bool begin(uint8_t columns = LCD_COLUMNS_SIZE, uint8_t rows = LCD_ROWS_SIZE, lcdFontSize = LCD_5x8DOTS, uint32_t sda = SDA, uint32_t scl = SCL, uint32_t speed = LCD_I2C_SPEED);
  #elif defined (ARDUINO_ARCH_SAMD)
   bool begin(uint8_t columns = LCD_COLUMNS_SIZE, uint8_t rows = LCD_ROWS_SIZE, lcdFontSize = LCD_5x8DOTS, uint32_t speed = LCD_I2C_SPEED);
  #else
   bool begin(uint8_t columns = LCD_COLUMNS_SIZE, uint8_t rows = LCD_ROWS_SIZE, lcdFontSize = LCD_5x8DOTS);
  #endif

   void clear();
   void home();
   void setCursor(uint8_t column, uint8_t row);

   void noDisplay();
   void display();

   void noBlink();
   void blink();
   void noCursor();
   void cursor();

   void scrollDisplayLeft();
   void scrollDisplayRight();
   void leftToRight();
   void rightToLeft();
   void autoscroll();
   void noAutoscroll(); 

   void createChar(uint8_t cgramAddress, uint8_t *cgramChar, uint8_t cgramCharSize = 8);
  #if defined (PROGMEM)
   void createChar(uint8_t cgramAddress, const uint8_t *cgramChar, uint8_t cgramCharSize = 8);
  #endif

   void noBacklight();
   void backlight();

   size_t write(uint8_t character);

   /************************* !!! bonus function !!! *************************/
   void printHorizontalGraph(char name, uint8_t row, uint16_t setValue, uint16_t maxValue);
   void displayOff();
   void displayOn();
  #if defined (ARDUINO_ARCH_ESP32)
   void setBrightness(uint8_t pin, uint8_t value, uint8_t channel);
  #else
   void setBrightness(uint8_t pin, uint8_t value);
  #endif
	 
  private:
   pcf8574Address    _pcf8574Address;
   lcdFontSize       _lcdFontSize;
   backlightPolarity _backlightPolarity;

   uint8_t _displayControl = 0; //DO NOT CHANGE!!! default bits value: DB7, DB6, DB5, DB4, DB3, DB2=(D), DB1=(C),   DB0=(B)
   uint8_t _displayMode    = 0; //DO NOT CHANGE!!! default bits value: DB7, DB6, DB5, DB4, DB3, DB2,     DB1=(I/D), DB0=(S)
   uint8_t _lcdColumns;
   uint8_t _lcdRows;
   uint8_t _backlightValue;
   uint8_t _lcdToPCF8574[8];
   bool    _pcf8574PortsMaping;

         void    _initialization();
         void    _send(uint8_t mode, uint8_t value, uint8_t cmdLength);
  inline uint8_t _portMapping(uint8_t value);
         void    _writePCF8574(uint8_t value);
         uint8_t _readPCF8574();
         bool    _readBusyFlag();
};

#endif
