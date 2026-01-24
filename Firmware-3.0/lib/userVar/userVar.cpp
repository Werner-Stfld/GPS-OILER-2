
#include <Arduino.h>
#include "userVar.h"

// flush vars to persistant storage
void VarContainer::flush() {
  Var *tmp = vars;
  while (tmp != nullptr) {
    tmp->flush();
    tmp = tmp->next;
  }
}

// restore vars from persistant storage
void VarContainer::restore() {
  Var *tmp = vars;
  while (tmp != nullptr) {
    tmp->restore();
    tmp = tmp->next;
  }
}

// Create chain of vars
void VarContainer::add(Var *var) {
  var->next=nullptr;
  if (vars == nullptr) {
    vars = var;
    return;
  }
  Var *tmp = vars;
  while (tmp->next != nullptr) { // go to var with .next == nullptr
    tmp=tmp->next;
  }
  tmp->next = var;
}

int noIntBoundsCheck(int v)
{
  return v;
}

IntVar::IntVar(int initial, PrefKey a, int (*cb) (int)) : value(a), checkBounds(cb) {
  value.set(initial);
}

int IntVar::get() {
  return value.get();
}

void IntVar::set(int v, SetMode mode) {
  value.set(checkBounds(v));
  if (mode == SetMode::flush)
    value.flush();
}

void IntVar::restore() {
  value.restore();
}

void IntVar::flush() {
  value.flush();
}

float noFloatBoundsCheck(float v) {
    return v;
}

FloatVar::FloatVar(float initial, PrefKey a, float (*floatCB) (float v)) : value(a), checkBounds(floatCB) {
  value.set(initial);
}

float FloatVar::get() {
  return value.get();
}

void FloatVar::set(float v, SetMode mode) {
  value.set(checkBounds(v));
  if (mode == SetMode::flush)
    value.flush();
}

void FloatVar::restore() {
  value.restore();
}

void FloatVar::flush() {
  value.flush();
}

StringVar::StringVar(const char *initial, PrefKey a) : value(a) {
  value.set(initial);
}

const char *StringVar::get() {
  return value.get();
}

void StringVar::set(const char *v, SetMode mode) {
  value.set(v);
  if (mode == SetMode::flush)
    value.flush();
}

void StringVar::restore() {
  value.restore();
}

void StringVar::flush() {
  value.flush();
}
