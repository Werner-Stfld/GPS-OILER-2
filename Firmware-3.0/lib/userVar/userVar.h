#pragma once

#include "prefs.h"

int noIntBoundsCheck(int);

// mode controlling the set operation.
// cache "only" updates the value in memory, setting the "dirty flagm" if the value is modified.
// flush updates the value in memory and in the persistent store which is used during restore. restore is typically used to set the var during next boot
typedef enum {
  cache = false,
  flush = true
} SetMode;

class Var {
  public:
  virtual void restore() = 0;
  virtual void flush() = 0;
  Var *next;
};

class VarContainer {
protected:
  Var *vars = nullptr;
  public:

  // flush vars to persistant storage
  void flush();

  // restore vars from persistant storage
  void restore();

  // Create chain of vars
  void add(Var *var);
};

class IntVar : public Var {
  String name;
  IntPrefVar value;
  int (*checkBounds) (int v);

public:
  IntVar(String n, int initial, PrefKey a, int (*cb) (int v)=noIntBoundsCheck);

  int get();
  
  void set(int v, SetMode mode = cache);

  bool evaluate(String &input);

  void status();

  void restore();

  void flush();
};

float noFloatBoundsCheck(float);

class FloatVar : public Var {
  String name;
  FloatPrefVar value;
  float (*checkBounds) (float v);
public:
  FloatVar(String n, float initial, PrefKey a, float (*floatCheckBounds) (float v)= noFloatBoundsCheck);

  float get();
  
  void set(float v, SetMode mode = cache);

  bool evaluate(String &input);

  void status();

  void restore();

  void flush();
};

class StringVar : public Var {
  String name;
  StringPrefVar value;

public:
  StringVar(String n, const char *initial, PrefKey a);

  const char *get();
  
  void set(const char * v, SetMode mode = cache);

  void status();

  void restore();

  void flush();
};
