#ifndef LCD_H_
#define LCD_H_
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <osapi/ScoreSystem/Adafruit_MCP23008.hpp>

using std::string;

/* DEFINES */
#define HIGH 1
#define INPUT 1
#define LOW 0
#define OUTPUT 0
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00



class LCD
{
public:
    LCD();
    void begin();
    void lcdWrite_four_bits(uint8_t command);
    void command(uint8_t value);
    void setCursor(uint8_t col, uint8_t row);
    void cursor();
    void noCursor();
    void display();
    void noDisplay();
    void blink();
    void noBlink();
    void clear();
    void home();
    void send(uint8_t value, uint8_t mode);
    uint8_t readReg();

    void stringWrite(string str);
    void charWrite(uint8_t value);

private:
    uint8_t En = 0b00000100; // Enable bit
    uint8_t Rw = 0b00000010; // Read/lcdWrite bit
    uint8_t Rs = 0b00000001; // Register select bit

    uint8_t _rs_pin = 1;
    uint8_t _rw_pin = 255;
    uint8_t _enable_pin = 2;
    uint8_t _data_pins[4];

    uint8_t displayFunction = 0x00;
    uint8_t displayControl = 0x00;
    uint8_t displayMode = 0x00;

    uint8_t numLines = 0;
    uint8_t currentLine = 0;

    Adafruit_MCP23008 i2c_;
};

#endif