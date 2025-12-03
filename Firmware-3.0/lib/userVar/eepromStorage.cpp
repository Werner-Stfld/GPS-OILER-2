#include <Arduino.h>
#include <wire.h>
#include "eepromStorage.h"

boolean stop_write_eeprom = true; //TODO: Replace by references verhindert das Beschreiben des EEPRON Speichers

void disableEepromWriting() {
    stop_write_eeprom = true;
}

#define EEPROM_I2C_ADDRESS 0x50

#pragma pack(push, 1)
union convert4 {
    int i;
    float f;
    char c[4];
};
#pragma pack(pop)

void read_char_array(eepromAddr addr, int length, char *rcvData)
{
  int address = static_cast<int>(addr);
  for (int i = 0; i < length; i++)
  {
    // byte charByte;

    // Begin transmission to I2C EEPROM
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);

    // Send memory address as two 8-bit bytes
    Wire.write((int)(address >> 8));   // MSB
    Wire.write((int)(address & 0xFF)); // LSB

    // End the transmission
    Wire.endTransmission();

    // Request one byte of data at current memory address
    Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);

    // Read the data and assign to variable
    rcvData[i] = Wire.read();

    address++;
  }
}

void write_char_array(eepromAddr addr, const char *value, int length)
{
  if (stop_write_eeprom || addr == eepromAddr::Noeeprom)
    return;
  int address = static_cast<int>(addr);
  for (int i = 0; i < length; i++)
  {
    byte charByte = (value[i]);

    // Begin transmission to I2C EEPROM
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);
    // Send memory address as two 8-bit bytes
    Wire.write((int)(address >> 8));   // MSB
    Wire.write((int)(address & 0xFF)); // LSB

    // Send data to be stored
    Wire.write(charByte);

    // End the transmission
    Wire.endTransmission();

    // Add 5ms delay for EEPROM
    delay(5);
    address++;
  }
}

int read_int(eepromAddr addr)
{
  convert4 convert;
  convert.i=0;
  read_char_array(addr, 4,  convert.c);
  return convert.i;
}

void write_int(eepromAddr addr, unsigned int value)
{
  convert4 convert;
  convert.i = value;
  write_char_array(addr,convert.c,4);
}

float read_float(eepromAddr addr)
{
  convert4 convert;
  read_char_array(addr, 4, convert.c);
  return convert.f;
}

void write_float(eepromAddr addr, float value)
{
  convert4 convert;
  convert.f = value;
  write_char_array(addr,convert.c,4);
}

IntEepromVar::IntEepromVar(eepromAddr a) : address(a),dirty(true) {}
FloatEepromVar::FloatEepromVar(eepromAddr a) : address(a),dirty(true) {}
Char20EepromVar::Char20EepromVar(eepromAddr a) : address(a),dirty(true) {}

int IntEepromVar::get() {
  return cache;
}

void IntEepromVar::set(int v) {
  if (cache != v) {
    dirty = true;
    cache = v;
  }
}

void IntEepromVar::init() {
  if (dirty) {
    cache = read_int(address);
    dirty = false;
  }
}

void IntEepromVar::flush() {
  if (dirty) {
    write_int(address, cache);
    dirty=false;
  }
}


float FloatEepromVar::get() {
  return cache;
}

void FloatEepromVar::set(float v) {
  if (cache != v) {
    dirty = true;
    cache = v;
  }
}

void FloatEepromVar::init() {
  if (dirty) {
    cache = read_float(address);
    dirty = false;
  }
}

void FloatEepromVar::flush() {
  if (dirty) {
    write_float(address, cache);
    dirty=false;
  }
}

void Char20EepromVar::set(const char *value, int length){
  if (length > 20) {
    length = 20;
  }
  for (int i = 0; i<length;i++) 
  {
    if (cache[i] != value[i]) {
      dirty = true;
      cache[i] = value[i];
    }
  }
  for (int i=length;i<20;i++) {
    if (cache[i] != 0) {
      dirty = true;
      cache[i] = 0;
    }
  }
}

const char *Char20EepromVar::get(){
  return cache;
}

void Char20EepromVar::init() {
  if (dirty) {
    ::read_char_array(address, 20, cache);
    dirty = false;
  }
}

void Char20EepromVar::flush() {
  if (dirty) {
    ::write_char_array(address, cache, 20);
    dirty=false;
  }
}
