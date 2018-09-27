/***************************************************************************************************/
/*
   This is an Arduino library for HD44780, S6A0069, KS0066U, NT3881D, LC7985, ST7066, SPLC780,
   WH160xB, AIP31066, GDM200xD, ADM0802A LCD displays.

   Screens are operated in 4 bit mode over i2c bus with 8-bit I/O expander PCF8574x.
   Typical displays sizes: 8x2, 16x1, 16x2, 16x4, 20x2, 20x4 & etc.

   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/


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

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#ifndef LiquidCrystal_i2c_h
#define LiquidCrystal_i2c_h

#if defined(ARDUINO) && ((ARDUINO) >= 100) //arduino core v1.0 or later
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>                  //use for PROGMEM Arduino AVR
#elif defined(ESP8266)
#include <pgmspace.h>                      //use for PROGMEM Arduino ESP8266
#elif defined(_VARIANT_ARDUINO_STM32_)
#include <avr/pgmspace.h>                  //use for PROGMEM Arduino STM32
#endif

#include <Wire.h>
#include <inttypes.h>
#include <Print.h>


/* 
   lcd main register commands
   NOTE: all commands formated as RS=(0:IR write & BF read, 1:DR write/read), RW=(0:write, 1:read), E=1, DB7=0, DB6=0, DB5=0, DB4=0, BCK_LED=0
*/
#define LCD_INSTRUCTION_WRITE    0x20  //writes command to Instruction Register (IR)
#define LCD_DATA_WRITE           0xA0  //writes data/text to Data Register (DR)
#define LCD_DATA_READ            0xE0  //reads data(text) from Data Register (DR)/DB7..DB0 bits value
#define LCD_BUSY_FLAG_READ       0x60  //reads Busy Flag (BF) & address counter/cursor position

/*
   LCD_INSTRUCTION_WRITE instructions
   NOTE: all instructions formated as DB7=(set DDRAM), DB6=(set CGRAM), DB5=(function set), DB4=(cursor shift), DB3=(disp. control), DB2=(entry mode set), DB1, DB0
*/
#define LCD_CLEAR_DISPLAY        0x01  //clears display & move cursor to home position
#define LCD_RETURN_HOME          0x02  //moves cursor position to home position
#define LCD_ENTRY_MODE_SET       0x04  //sets cursor type, text direction (I/D) & display shift direction (S)
#define LCD_DISPLAY_CONTROL      0x08  //sets display on/off (D), cursor on/off (C) & cursor blinking on/off (B)
#define LCD_CURSOR_DISPLAY_SHIFT 0x10  //cursor or display/text shifts without changing DDRAM contents
#define LCD_FUNCTION_SET         0x20  //sets data length (DL), number of lines (N) & font size (F)
#define LCD_CGRAM_ADDR_SET       0x40  //sets CGRAM address. CGRAM data is sent & received after this setting
#define LCD_DDRAM_ADDR_SET       0x80  //sets DDRAM address/cursor position

/*
   LCD_ENTRY_MODE_SET controls
   NOTE: all controls formated as DB7, DB6, DB5, DB4, DB3, DB2, DB1=(I/D), DB0=(S)
*/
#define LCD_ENTRY_RIGHT          0x00  //sets text direction decrement/"right to left" (I/D)
#define LCD_ENTRY_LEFT           0x02  //sets text direction increment/"left to right" (I/D)
#define LCD_ENTRY_SHIFT_ON       0x01  //text shifts when byte written & cursot stays (S)
#define LCD_ENTRY_SHIFT_OFF      0x00  //text stays & cursor moves when byte written  (S)

/*
   LCD_DISPLAY_CONTROL controls
   NOTE: all controls formated as DB7, DB6, DB5, DB4, DB3, DB2=(D), DB1=(C), DB0=(B)
*/
#define LCD_DISPLAY_ON           0x04  //turns display ON/retrive text (D)
#define LCD_DISPLAY_OFF          0x00  //turns display OFF/clears text (D)
#define LCD_UNDERLINE_CURSOR_ON  0x02  //turns ON  underline cursor (C)
#define LCD_UNDERLINE_CURSOR_OFF 0x00  //turns OFF underline cursor (C)
#define LCD_BLINK_CURSOR_ON      0x01  //turns ON  blinking  cursor (B)
#define LCD_BLINK_CURSOR_OFF     0x00  //turns OFF blinking  cursor (B)

/*
   LCD_CURSOR_DISPLAY_SHIFT controls
   NOTE: all controls formated as DB7, DB6, DB5, DB4, DB3=(SC), DB2=(RL), DB1=*, DB0=*
*/
#define LCD_DISPLAY_SHIFT        0x08  //display/text shifts after char print (SC)
#define LCD_CURSOR_SHIFT         0x00  //cursor shifts after char print       (SC)
#define LCD_SHIFT_RIGHT          0x04  //cursor or display/text shifts to right (RL)
#define LCD_SHIFT_LEFT           0x00  //cursor or display/text shifts to left  (RL)

/*
   LCD_FUNCTION_SET controls
   NOTE: all controls formated as DB7, DB6, DB5, DB4=(DL), DB3=(N), DB2=(F), DB1=*, DB0=*
*/
#define LCD_8BIT_MODE            0x10  //select 8-bit interface            (DL)
#define LCD_4BIT_MODE            0x00  //select 4-bit interface            (DL)
#define LCD_1_LINE               0x00  //selects one line display          (N)
#define LCD_2_LINE               0x08  //selects two or more lines display (N)

typedef enum : uint8_t
{
  LCD_5x10DOTS                 = 0x04, //5x9+1 dots charecter+cursor font (F), 5x10 dots total
  LCD_5x8DOTS                  = 0x00  //5x7+1 dots charecter+cursor font (F), 5x8  dots total
}
lcd_font_size;


/* lcd misc. */
#define LCD_HOME_CLEAR_DELAY     2     //duration of home & clear commands, in milliseconds
#define LCD_COMMAND_DELAY        43    //duration of command, in microseconds
#define LCD_CMD_LENGTH_8BIT      8     //8-bit command length
#define LCD_CMD_LENGTH_4BIT      4     //4-bit command length

/* PCF8574 misc. controls */
#define LCD_BACKLIGHT_ON         0x01
#define LCD_BACKLIGHT_OFF        0x00
#define PCF8574_ALL_LOW          0x00  //sets PCF8574 pins to RS=0,RW=0,E=0,DB7=0,DB6=0,DB5=0,DB4=0,BCK_LED=0
#define PCF8574_DATA_HIGH        0x3E  //sets PCF8574 pins to RS=0,RW=0,E=1,DB7=1,DB6=1,DB5=1,DB4=1,BCK_LED=0

/* PCF8574 & PCF8574A addresses */
typedef enum : uint8_t
{
  PCF8574_ADDR_A21_A11_A01     = 0x27, //i2c address A2 = 1, A1 = 1, A0 = 1 (by default)
  PCF8574_ADDR_A21_A11_A00     = 0x26, //i2c address A2 = 1, A1 = 1, A0 = 0
  PCF8574_ADDR_A21_A10_A01     = 0x25, //i2c address A2 = 1, A1 = 0, A0 = 1
  PCF8574_ADDR_A21_A10_A00     = 0x24, //i2c address A2 = 1, A1 = 0, A0 = 0
  PCF8574_ADDR_A20_A11_A01     = 0x23, //i2c address A2 = 0, A1 = 1, A0 = 1
  PCF8574_ADDR_A20_A11_A00     = 0x22, //i2c address A2 = 0, A1 = 1, A0 = 0
  PCF8574_ADDR_A20_A10_A01     = 0x21, //i2c address A2 = 0, A1 = 0, A0 = 1
  PCF8574_ADDR_A20_A10_A00     = 0x20, //i2c address A2 = 0, A1 = 0, A0 = 0

  PCF8574A_ADDR_A21_A11_A01    = 0x3F, //i2c address A2 = 1, A1 = 1, A0 = 1
  PCF8574A_ADDR_A21_A11_A00    = 0x3E, //i2c address A2 = 1, A1 = 1, A0 = 0
  PCF8574A_ADDR_A21_A10_A01    = 0x3D, //i2c address A2 = 1, A1 = 0, A0 = 1
  PCF8574A_ADDR_A21_A10_A00    = 0x3C, //i2c address A2 = 1, A1 = 0, A0 = 0
  PCF8574A_ADDR_A20_A11_A01    = 0x3B, //i2c address A2 = 0, A1 = 1, A0 = 1
  PCF8574A_ADDR_A20_A11_A00    = 0x3A, //i2c address A2 = 0, A1 = 1, A0 = 0
  PCF8574A_ADDR_A20_A10_A01    = 0x39, //i2c address A2 = 0, A1 = 0, A0 = 1
  PCF8574A_ADDR_A20_A10_A00    = 0x38  //i2c address A2 = 0, A1 = 0, A0 = 0
}
PCF8574_address;

/* PCF8574 backlight controls */
typedef enum : uint8_t
{
  POSITIVE                     = 0x01, //backlight switching polarity
  NEGATIVE                     = 0x00  //backlight switching polarity
}
backlightPolarity;

class LiquidCrystal_I2C : public Print 
{
  public:
   LiquidCrystal_I2C(PCF8574_address = PCF8574_ADDR_A21_A11_A01, uint8_t P0 = 4, uint8_t P1 = 5, uint8_t P2 = 6, uint8_t P3 = 16, uint8_t P4 = 11, uint8_t P5 = 12, uint8_t P6 = 13, uint8_t P7 = 14, backlightPolarity = POSITIVE);
 
   #if defined(ESP8266)
   bool begin(uint8_t lcd_colums = 16, uint8_t lcd_rows = 2, lcd_font_size = LCD_5x8DOTS, uint8_t sda = SDA, uint8_t scl = SCL);
   #else
   bool begin(uint8_t lcd_colums = 16, uint8_t lcd_rows = 2, lcd_font_size = LCD_5x8DOTS);
   #endif
   void clear(void);
   void home(void);
   void setCursor(uint8_t colum, uint8_t row);
   void noDisplay(void);
   void display(void);
   void noBlink(void);
   void blink(void);
   void noCursor(void);
   void cursor(void);
   void scrollDisplayLeft(void);
   void scrollDisplayRight(void);
   void leftToRight(void);
   void rightToLeft(void);
   void autoscroll(void);
   void noAutoscroll(void); 
   void createChar(uint8_t CGRAM_address,       uint8_t *char_pattern);
   #if defined (PROGMEM)
   void createChar(uint8_t CGRAM_address, const uint8_t *char_pattern);
   #endif
   void noBacklight(void);
   void backlight(void);

   /* "write()" replacement in Arduino "Print" class */
   #if defined(ARDUINO) && ((ARDUINO) >= 100)
   size_t write(uint8_t value);
   #else
   void write(uint8_t value);
   #endif

   /*************** !!! arduino not standard API functions !!! ***************/
   void printHorizontalGraph(char name, uint8_t row, uint16_t currentValue, uint16_t maxValue);
   void displayOff(void);
   void displayOn(void);  
   void setBrightness(uint8_t pin, uint8_t value, backlightPolarity polarity);
	 
  private:
   uint8_t _displayControl = 0; //DO NOT CHANGE!!! default bits value: DB7, DB6, DB5, DB4, DB3, DB2=(D), DB1=(C),   DB0=(B)
   uint8_t _displayMode    = 0; //DO NOT CHANGE!!! default bits value: DB7, DB6, DB5, DB4, DB3, DB2,     DB1=(I/D), DB0=(S)
   uint8_t _lcd_colums;
   uint8_t _lcd_rows;
   uint8_t _backlightValue;
   uint8_t _LCD_TO_PCF8574[8];
   bool    _PCF8574_initialisation;

   PCF8574_address   _PCF8574_address;
   lcd_font_size     _lcd_font_size;
   backlightPolarity _backlightPolarity;

          void    initialization(void);
          void    send(uint8_t mode, uint8_t value, uint8_t length);
   inline uint8_t portMapping(uint8_t value);
          bool    writePCF8574(uint8_t value);
          uint8_t readPCF8574(void);
          bool    readBusyFlag(void);
          uint8_t getCursorPosition(void);
};

#endif
