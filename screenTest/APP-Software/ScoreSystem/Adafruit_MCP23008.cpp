/*************************************************** 
  Rewritten by Jonas Agger Jørgensen 
  05/12-2017

  This is a library for the MCP23008 i2c port expander for the Raspberry Pi
  using the ASE RPI kernel image. This requires you to have i2C enabled.
  By default, this is using the node /dev/i2c-1 for i2C operations.

  These displays use I2C to communicate, 2 pins are required to  
  ScoreSystem
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/



#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>
#include <osapi/ScoreSystem/Adafruit_MCP23008.hpp>
#include <osapi/ScoreSystem/I2C_reg.hpp>


////////////////////////////////////////////////////////////////////////////////
// RTC_DS1307 implementation

void Adafruit_MCP23008::begin(uint8_t addr) {

    I2C_reg::getInstance().displayInit();
}

void Adafruit_MCP23008::begin(void) {
  begin(0);
}

void Adafruit_MCP23008::pinMode(uint8_t p, uint8_t d) {
  uint8_t iodir;
  

  // only 8 bits!
  if (p > 7)
    return;
  
  iodir = read8(MCP23008_IODIR);

  // set the pin and direction
  if (d == 0) {
    iodir |= 1 << p; 
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  write8(MCP23008_IODIR, iodir);
}

uint8_t Adafruit_MCP23008::readGPIO(void) {
  // read the current GPIO input
  return read8(MCP23008_GPIO);
}

void Adafruit_MCP23008::writeGPIO(uint8_t gpio) {
  write8(MCP23008_GPIO, gpio);
}


void Adafruit_MCP23008::digitalWrite(uint8_t p, uint8_t d) {
  uint8_t gpio;
  
  // only 8 bits!
  if (p > 7)
    return;

  // read the current GPIO output latches
  gpio = readGPIO();

  // set the pin and direction
  if (d == 1) {
    gpio |= 1 << p; 
  } else {
    gpio &= ~(1 << p);
  }

  // write the new GPIO
  writeGPIO(gpio);
}

void Adafruit_MCP23008::pullUp(uint8_t p, uint8_t d) {
  uint8_t gppu;
  
  // only 8 bits!
  if (p > 7)
    return;

  gppu = read8(MCP23008_GPPU);
  // set the pin and direction
  if (d == 1) {
    gppu |= 1 << p; 
  } else {
    gppu &= ~(1 << p);
  }
  // write the new GPIO
  write8(MCP23008_GPPU, gppu);
}

uint8_t Adafruit_MCP23008::digitalRead(uint8_t p) {
  // only 8 bits!
  if (p > 7)
    return 0;

  // read the current GPIO
  return (readGPIO() >> p) & 0x1;
}

uint8_t Adafruit_MCP23008::read8(uint8_t addr) {
    return I2C_reg::getInstance().displayRead(addr);
}


void Adafruit_MCP23008::write8(uint8_t addr, uint8_t data) {
  I2C_reg::getInstance().displayWrite(addr, data);
}
