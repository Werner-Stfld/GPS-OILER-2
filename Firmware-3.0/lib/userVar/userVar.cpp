
#include <Arduino.h>
#include "userVar.h"

int noIntBoundsCheck(int v)
{
  return v;
}

IntUserVar::IntUserVar(String n, int initial, eepromAddr a, int (*cb) (int)) : name(n), value(a), checkBounds(cb) {
  value.set(initial);
}

int IntUserVar::get() {
  return value.get();
}

void IntUserVar::set(int v) {
  value.set(checkBounds(v));
}
bool IntUserVar::evaluate(String &input) {
  if (input.startsWith(name))
  {
    input.replace(name, "");
    int value = input.toInt();
    write(value);
    status();
    return true;
  }
  return false;
}

void IntUserVar::status() {
  Serial.print(name);
  Serial.println(value.get());
}

int IntUserVar::read() {
  value.init();
  return value.get();
}

void IntUserVar::write(int v) {
  value.set(v);
  flush();
}

void IntUserVar::flush() {
  value.flush();
}

float noFloatBoundsCheck(float v) {
    return v;
}

FloatUserVar::FloatUserVar(String n, float initial, eepromAddr a, float (*floatCB) (float v)) : name(n), value(a), checkBounds(floatCB) {
  value.set(initial);
}

float FloatUserVar::get() {
  return value.get();
}

void FloatUserVar::set(float v) {
  value.set( checkBounds(v));
}

bool FloatUserVar::evaluate(String &input) {
  if (input.startsWith(name))
  {
    input.replace(name, "");
    float value = input.toDouble();
    write(value);
    status();
    return true;
  }
  return false;
}
void FloatUserVar::status() {
  Serial.print(name);
  Serial.println(value.get());
}

float FloatUserVar::read() {
  value.init();
  return value.get();
}

void FloatUserVar::write(float v) {
  set(v);
  flush();
}

void FloatUserVar::flush() {
  value.flush();
}

Char20UserVar::Char20UserVar(String n, const char *initial, eepromAddr a) : name(n), value(a) {
  value.set(initial, 20);
}

const char *Char20UserVar::get() {
  return value.get();
}

void Char20UserVar::set(const char *v, int length) {
  value.set(v, length);
}

void Char20UserVar::status() {
  Serial.print(name);
  Serial.println(value.get());
}

const char *Char20UserVar::read() {
  value.init();
  return get();
}

void Char20UserVar::write(const char *v, int length) {
  set(v, length);
  flush();
}

void Char20UserVar::flush() {
  value.flush();
}
