#include <osapi/ScoreSystem/LCD.hpp>

#define BV(bit) (1 <<(bit))

//! Default constructor, sets up registers
LCD::LCD()
{
    displayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

    // the I/O expander pinout
    _rs_pin = 1;
    _rw_pin = 255;
    _enable_pin = 2;
    _data_pins[0] = 3;  // really d4
    _data_pins[1] = 4;  // really d5
    _data_pins[2] = 5;  // really d6
    _data_pins[3] = 6;  // really d7
}

//! Sets up the display to the correct modes according to the datasheet for the HD44780
void LCD::begin()
{
    i2c_.begin(0);

    i2c_.pinMode(7, 1); // Backlight pin as output
    i2c_.digitalWrite(7, 1); // Backlight pin set high

    // Setting our data pins as outputs
    i2c_.pinMode(_data_pins[0], 1);
    i2c_.pinMode(_data_pins[1], 1);
    i2c_.pinMode(_data_pins[2], 1);
    i2c_.pinMode(_data_pins[3], 1);

    i2c_.pinMode(_rs_pin, 1);
    i2c_.pinMode(_enable_pin, 1);

    displayFunction |= LCD_2LINE;
    numLines = 4;

    usleep(50000);

    i2c_.digitalWrite(_rs_pin, 0);
    i2c_.digitalWrite(_enable_pin, 0);

    lcdWrite_four_bits(0x03);
    usleep(5000);
    lcdWrite_four_bits(0x03);
    usleep(5000);
    lcdWrite_four_bits(0x03);
    usleep(150);
    lcdWrite_four_bits(0x02);

    command(LCD_FUNCTIONSET | displayFunction);

    displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;

    display();

    clear();

    displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    command(LCD_ENTRYMODESET | displayMode);
}

//! Writing a four bit value to the screen. Making several I2C requests meanwhile
void LCD::lcdWrite_four_bits(uint8_t value)
{
	uint8_t out = 0;
	out = i2c_.readGPIO();

	for(int i = 0; i < 4; i++)
	{
		out &= ~BV(_data_pins[i]);
      	out |= (((value >> i) & 0x1) << _data_pins[i]);
	}

	out &= ~BV(_enable_pin);

	i2c_.writeGPIO(out);

	// pulse enable

	//usleep(1);
	out |= BV(_enable_pin);
	i2c_.writeGPIO(out);
	//usleep(1);
	out &= ~BV(_enable_pin);
	i2c_.writeGPIO(out);
	usleep(100);
}

//! Writes a string to the screen.
void LCD::stringWrite(string str)
{
    if(str.length() > 20) return;
    for(uint8_t i = 0; i < str.length(); i++)
    {
        charWrite(str[i]);
    }
}

//! send a command to the screen. Used to set blink, cursor etc.
void LCD::command(uint8_t value)
{
	send(value, 0);
}

//! Writes a char on the screen.
void LCD::charWrite(uint8_t value)
{
	send(value, 1);
}

//! Wrapper function, so that it can be passed an 8bit value and send it using 2x write_four_bits
void LCD::send(uint8_t value, uint8_t mode)
{
    I2C_reg::getInstance().lockI2C();
	i2c_.digitalWrite(_rs_pin, mode);
	lcdWrite_four_bits((value>>4));
	lcdWrite_four_bits(value);
    i2c_.digitalWrite(7, 1); // Backlight pin set high
    I2C_reg::getInstance().unlockI2C();
}

//! Set cursor position
void LCD::setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > numLines ) {
	row = numLines-1;    // we count rows starting w/0
	}

	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

//! clears the display
void LCD::clear()
{
    command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    usleep(2000);  // this command takes a long time!
}

//! returns cursor to the home position
void LCD::home()
{
    command(LCD_RETURNHOME);  // set cursor position to zero
    usleep(2000);  // this command takes a long time!
}

//! turn display on
void LCD::display() {
    displayControl |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | displayControl);
}

//! turn display off
void LCD::noDisplay() {
    displayControl = ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | displayControl);
}

//! enables the buttom cursor
void LCD::cursor()
{
	displayControl |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | displayControl);
}

//! disables the buttom cursor 
void LCD::noCursor()
{
	displayControl &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | displayControl);
}

//! stops the current position from blinking
void LCD::noBlink() {
    displayControl &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | displayControl);
}

//! starts blinking at the current position
void LCD::blink() {
    displayControl |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | displayControl);
}

//! read the current GPIO values. 
uint8_t LCD::readReg()
{
    return i2c_.readGPIO();
}
