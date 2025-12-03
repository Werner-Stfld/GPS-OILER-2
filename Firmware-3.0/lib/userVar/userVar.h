#pragma once

#include "eepromStorage.h"

int noIntBoundsCheck(int);

class IntUserVar {
  String name;
  IntEepromVar value;
  int (*checkBounds) (int v);

public:
  IntUserVar(String n, int initial, eepromAddr a, int (*cb) (int v)=noIntBoundsCheck);

  int get();
  
  void set(int v);

  bool evaluate(String &input);

  void status();

  int read();

  void write(int v);

  void flush();
};

float noFloatBoundsCheck(float);

class FloatUserVar {
  String name;
  FloatEepromVar value;
  float (*checkBounds) (float v);
public:
  FloatUserVar(String n, float initial, eepromAddr a, float (*floatCheckBounds) (float v)= noFloatBoundsCheck);

  float get();
  
  void set(float v);

  bool evaluate(String &input);

  void status();

  float read();

  void write(float v);

  void flush();
};

typedef char char20[20];

class Char20UserVar {
  String name;
  Char20EepromVar value;

public:
  Char20UserVar(String n, const char *initial, eepromAddr a);

  const char *get();
  
  void set(const char *v, int length);

  void status();

  const char *read();

  void write(const char *v, int length);

  void flush();
};
