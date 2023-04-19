[![license-badge][]][license] ![version] [![stars][]][stargazers] ![hit-count] [![github-issues][]][issues]

# LiquidCrystal_I2C

This is small and fast Arduino library for LCD HD44780 and its clones (S6A0069, KS0066U, NT3881D, LC7985, ST7066, SPLC780, WH160xB, AIP31066, GDM200xD). The display operates in 4-bit mode over I²C bus with 8-bit I/O expander PCF8574/PCF8574A.

The 99.9% of all PCF8574(A) I²C backpacks available on the market are connected like this:

| PCF8574 ports | LCD pins |
| ---- | ------- |
| P0 | 4/RS |
| P1 | 5/RW |
| P2 | 6/En |
| P3 | 16/BACKLIGHT LED- (turn-on level HIGH/POSITIVE) |
| P4 | 11/DB4 |
| P5 | 12/DB5 |
| P6 | 13/DB6 |
| P7 | 14/DB7 |

The initialization string for this connection:
```C++
//                    A2=HIGH, A1=HIGH, A0=HIGH P0 P1 P2 P3  P4  P5  P6  P7  backlight turn-on level
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
```
But what if your I²C backpack is different? For example, LCD pin **4/RS** is connected **to port P6** and LCD's pin **13/DB6** is connected **to port P0**:

| PCF8574 ports | LCD pins |
| ---- | ------- |
| **P0** | **13/DB6** :warning:|
| P1 | 5/RW |
| P2 | 6/En |
| P3 | 16/BACKLIGHT LED- (turn-on level HIGH/POSITIVE) |
| P4 | 11/DB4 |
| P5 | 12/DB5 |
| **P6** | **4/RS** :warning:|
| P7 | 14/DB7 |

The initialization string in this case:
```C++
//                    A2=HIGH, A1=HIGH, A0=HIGH P0 P1 P2  P3  P4  P5  P6 P7  backlight turn-on level
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 13, 5, 6, 16, 11, 12, 4, 14, POSITIVE);
```

Supports:

- Arduino AVR
- Arduino ESP8266
- Arduino ESP32
- Arduino STM32
- Arduino SAMD21

[license-badge]: https://img.shields.io/badge/License-GPLv3-blue.svg
[license]:       https://choosealicense.com/licenses/gpl-3.0/
[version]:       https://img.shields.io/badge/Version-1.4.0-green.svg
[stars]:         https://img.shields.io/github/stars/enjoyneering/LiquidCrystal_I2C.svg
[stargazers]:    https://github.com/enjoyneering/LiquidCrystal_I2C/stargazers
[hit-count]:     https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fenjoyneering%2FLiquidCrystal_I2C&count_bg=%2379C83D&title_bg=%23555555&icon=&icon_color=%23E7E7E7&title=hits&edge_flat=false
[github-issues]: https://img.shields.io/github/issues/enjoyneering/LiquidCrystal_I2C.svg
[issues]:        https://github.com/enjoyneering/LiquidCrystal_I2C/issues/
