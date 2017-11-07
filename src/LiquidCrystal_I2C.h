/***************************************************************************************************/
/*
  This is an Arduino library for HD44780, S6A0069, KS0066U, NT3881D, LC7985, ST7066, SPLC780,
  WH160xB, AIP31066, GDM200xD, ADM0802A LCD displays.
  Screens are operated in 4 bit mode over I2C bus with 8-bit I/O expander PCF8574.
  Typical displays resolutions are: 8x2, 16x1, 16x2, 16x4, 20x2, 20x4 and etc.

  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  This chip uses I2C bus to communicate, specials pins are required to interface
  Connect chip to pins:    SDA        SCL
  Uno, Mini, Pro:          A4         A5
  Mega2560, Due:           20         21
  Leonardo:                2          3
  ATtiny85:                0(5)       2/A1(7)   (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                                 & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-01:          GPIO0/D5   GPIO2/D3  (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:             GPIO4/D2   GPIO5/D1
  WeMos D1 Mini:           GPIO4/D2   GPIO5/D1

  BSD license, all text above must be included in any redistribution
*/
/***************************************************************************************************/

#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#if defined(__AVR_ATtinyX4__) || defined(__AVR_ATtinyX5__) || defined(__AVR_ATtinyX313__)
#include <TinyWireM.h>
#define  Wire TinyWireM
#else
#include <Wire.h>
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#include <inttypes.h>
#include <Print.h>

/* !!! NOTE: all commands formated as: RS, RW, E=1, DB7=0, DB6=0, DB5=0, DB4=0, BCK_LED=0 */
/* lcd main register commands */
#define LCD_INSTRUCTION_WRITE    0x20 //Writes command to Instruction Register (IR). Such as: data length (DL), number of lines (N), font size (F).
#define LCD_DATA_WRITE           0xA0 //Writes data/text to Data Register (DR)
#define LCD_DATA_READ            0xE0 //Reads data(text) from Data Register (DR) - values from pins DB7..DB0
#define LCD_BUSY_FLAG_READ       0x60 //Reads Busy Flag (BF) - indicating internal operation is being performed and reads address counter(row, column) content.

/* !!! NOTE: all controls formated as: DB7, DB6, DB5, DB4, DB3, DB2, DB1, DB0 */
/* lcd main register commands controls */
#define LCD_CLEAR_DISPLAY        0x01 //Clears entire display (fill with spacers) and sets DDRAM address (cursor position) to 0 in the address counter.
#define LCD_RETURN_HOME          0x02 //Sets DDRAM address 0 in address counter. Returns display to original position. DDRAM contents remain unchanged.
#define LCD_ENTRY_MODE_SET       0x04 //Sets cursor type, moving and display shift direction. These operations are performed during data write and read.
#define LCD_DISPLAY_CONTROL      0x08 //Sets display on/off (D), cursor on/off (C), cursor blinking on/off (B).
#define LCD_CURSOR_SHIFT         0x10 //Moves cursor and shifts display without changing DDRAM contents.
#define LCD_SET_CGRAM_ADDR       0x40 //Sets CGRAM address. CGRAM data is sent and received after this setting.
#define LCD_SET_DDRAM_ADDR       0x80 //Sets DDRAM address (cursor position). DDRAM data is sent and received after this setting.

/* LCD_ENTRY_MODE_SET controls */
#define LCD_ENTRY_RIGHT          0x00 //Sets entry/text direction decrement/"right to left" (I/D).
#define LCD_ENTRY_LEFT           0x02 //Sets entry/text direction increment/"left to right" (I/D).
#define LCD_ENTRY_SHIFT_ON       0x01 //Entry/text shifts when the byte written, cursot stays (S).
#define LCD_ENTRY_SHIFT_OFF      0x00 //Entry/text stays, cursor moves when the byte written  (S).

/* LCD_DISPLAY_CONTROL controls */
#define LCD_DISPLAY_ON           0x04 //Turns display ON  & retrive text from the DDRAM (D).
#define LCD_DISPLAY_OFF          0x00 //Turns display OFF & clears text from the screen & remains it in DDRAM (D).
#define LCD_UNDERLINE_CURSOR_ON  0x02 //Turns ON  the underline cursor (C).
#define LCD_UNDERLINE_CURSOR_OFF 0x00 //Turns OFF the underline cursor (C).
#define LCD_BLINK_CURSOR_ON      0x01 //Turns ON  the blinking  cursor (B).
#define LCD_BLINK_CURSOR_OFF     0x00 //Turns OFF the blinking  cursor (B).

/* LCD_CURSOR_SHIFT controls */
#define LCD_DISPLAY_MOVE         0x08 //Display/text shifts after char print (SC).
#define LCD_CURSOR_MOVE          0x00 //Cursor shifts after char print       (SC).
#define LCD_MOVE_RIGHT           0x04 //Cursor or Display shifts to right    (RL).
#define LCD_MOVE_LEFT            0x00 //Cursor or Display shifts to left     (RL).

/* LCD_INSTRUCTION_WRITE controls */
#define LCD_8BIT_MODE            0x30 //Select 8 bit interface            (DL).
#define LCD_4BIT_MODE            0x20 //Select 4 bit interface            (DL).
#define LCD_1_LINE               0x00 //Selects one line display          (N).
#define LCD_2_LINE               0x08 //Selects two or more lines display (N).

/* lcd misc. */
#define LCD_HOME_CLEAR_DELAY     2000 //Duration of the home & clear commands, in microseconds. Must be > 1.53 .. 1.64ms
#define LCD_COMMAND_DELAY        50   //Duration of command, in microseconds. Must be > 37 .. 43us
#define LCD_EN_PULSE_DELAY       1    //Duration of the En pulse, in microseconds. Must be > 450ns
#define LCD_CMD_LENGTH_8BIT      8    //8bit command length
#define LCD_CMD_LENGTH_4BIT      4    //4bit command length
#define LCD_POLL_LIMIT           8    //i2c retry limit

/* LCD_INSTRUCTION_WRITE controls */
typedef enum
{
  LCD_5x10DOTS = 0x04,                //5x9 dots for charecter, plus 5x1 dots for cursor. 5x10 total.
  LCD_5x8DOTS  = 0x00                 //5x7 dots for charecter, plus 5x1 dots for cursor. 5x8  total.
}
lcd_font_size;

/* PCF8574 controls */
#define LCD_BACKLIGHT_ON   0x01
#define LCD_BACKLIGHT_OFF  0x00
#define PCF8574_ALL_LOW    0x00       //Sets all PCF8574 pins to LOW
#define PCF8574_DATA_HIGH  0x3E       //Sets lcd pins E, DB7, DB6, DB5, DB4 to HIGH

/* PCF8574 addresses */
typedef enum
{
  PCF8574_ADDR_A21_A11_A01 = 0x27,    //I2C address. A2 = 1, A1 = 1, A0 = 1 (by default)
  PCF8574_ADDR_A21_A11_A00 = 0x26,    //I2C address. A2 = 1, A1 = 1, A0 = 0
  PCF8574_ADDR_A21_A10_A01 = 0x25,    //I2C address. A2 = 1, A1 = 0, A0 = 1
  PCF8574_ADDR_A21_A10_A00 = 0x24,    //I2C address. A2 = 1, A1 = 0, A0 = 0
  PCF8574_ADDR_A20_A11_A01 = 0x23,    //I2C address. A2 = 0, A1 = 1, A0 = 1
  PCF8574_ADDR_A20_A11_A00 = 0x22,    //I2C address. A2 = 0, A1 = 1, A0 = 0
  PCF8574_ADDR_A20_A10_A01 = 0x21,    //I2C address. A2 = 0, A1 = 0, A0 = 1
  PCF8574_ADDR_A20_A10_A00 = 0x20,    //I2C address. A2 = 0, A1 = 0, A0 = 0

  PCF8574A_ADDR_A21_A11_A01 = 0x3F,   //I2C address. A2 = 1, A1 = 1, A0 = 1
  PCF8574A_ADDR_A21_A11_A00 = 0x3E,   //I2C address. A2 = 1, A1 = 1, A0 = 0
  PCF8574A_ADDR_A21_A10_A01 = 0x3D,   //I2C address. A2 = 1, A1 = 0, A0 = 1
  PCF8574A_ADDR_A21_A10_A00 = 0x3C,   //I2C address. A2 = 1, A1 = 0, A0 = 0
  PCF8574A_ADDR_A20_A11_A01 = 0x3B,   //I2C address. A2 = 0, A1 = 1, A0 = 1
  PCF8574A_ADDR_A20_A11_A00 = 0x3A,   //I2C address. A2 = 0, A1 = 1, A0 = 0
  PCF8574A_ADDR_A20_A10_A01 = 0x39,   //I2C address. A2 = 0, A1 = 0, A0 = 1
  PCF8574A_ADDR_A20_A10_A00 = 0x38    //I2C address. A2 = 0, A1 = 0, A0 = 0
}
PCF8574_address;

/* PCF8574 backlight controls - transistor switching polarity */
typedef enum
{
  POSITIVE = 0x01,
  NEGATIVE = 0x00
}
switchPolarity;

class LiquidCrystal_I2C : public Print 
{
  public:
   LiquidCrystal_I2C(PCF8574_address = PCF8574_ADDR_A21_A11_A01, uint8_t P0 = 4, uint8_t P1 = 5, uint8_t P2 = 6, uint8_t P3 = 16, uint8_t P4 = 11, uint8_t P5 = 12, uint8_t P6 = 13, uint8_t P7 = 14, switchPolarity = POSITIVE);
 
   #if defined(ESP8266)
   bool begin(uint8_t lcd_colums = 16, uint8_t lcd_rows = 2, lcd_font_size = LCD_5x8DOTS, uint8_t sda = SDA, uint8_t scl = SCL);
   #else
   bool begin(uint8_t lcd_colums = 16, uint8_t lcd_rows = 2, lcd_font_size = LCD_5x8DOTS);
   #endif
   void clear(void);
   void home(void);
   void noDisplay(void);
   void display(void);
   void noBlink(void);
   void blink(void);
   void noCursor(void);
   void cursor(void);
   void scrollDisplayLeft(void);
   void scrollDisplayRight(void);
   void printLeft(void);
   void printRight(void);
   void leftToRight(void);
   void rightToLeft(void);
   void shiftIncrement(void);
   void shiftDecrement(void);
   void noBacklight(void);
   void backlight(void);
   void autoscroll(void);
   void noAutoscroll(void); 
   void createChar(uint8_t, uint8_t[]);
   void setCursor(uint8_t, uint8_t);

   /* Arduino class "Print" calls func. "write()" to send characters to the LCD */
   #if defined(ARDUINO) && ARDUINO >= 100
   size_t write(uint8_t value);
   size_t print(uint8_t value);
   #else
   void write(uint8_t value);
   void print(uint8_t value);
   #endif
   using  Print::write;
   using  Print::print;

   /* Arduino Unsupported API functions */
   void printHorizontalGraph(char name, uint8_t row, uint16_t currentValue, uint16_t maxValue);
   void printVerticalGraph(uint8_t colum, uint8_t row, uint16_t currentValue, uint16_t maxValue);
   void setBrightness(uint8_t pin, uint8_t value, switchPolarity);
	 
  private:
   uint8_t _displayFunction;
   uint8_t _displayControl;
   uint8_t _displayMode;
   uint8_t _lcd_colums;
   uint8_t _lcd_rows;
   uint8_t _backlightValue;
   uint8_t _LCD_TO_PCF8574[8];
   bool    _PCF8574_initialisation;

   PCF8574_address _PCF8574_address;
   lcd_font_size   _lcd_font_size;
   switchPolarity  _switchPolarity;

   void    initialization(void);
   void    send(uint8_t mode, uint8_t value, uint8_t length);
   uint8_t portMapping(uint8_t value);
   void    writePCF8574(uint8_t value);
   uint8_t readPCF8574(void);
};

#endif
