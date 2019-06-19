#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "config.h"
#include <macros.h>

#ifdef USE_KEYBOARD2

class Keyboard
{
  public:  

  void begin(Macros * macros);
  void reset(bool reset_macros=false);
  void pause(bool paused);
  void process();
  bool is_paused();
  byte get_captured_key();
  void clear_captured_key();
  int get_capture_count();
  byte get_current_key();
  void set_capturing_macro(byte macro);
  void set_long_press_macro(byte macro);
  
  private:

  void set_capture_mode(bool capturing, byte key = 0);
  void set_address_pin(byte position, byte bit, byte pin);
  void set_keyboard_row(byte row);
  void set_keyboard_col(byte col);
  bool get_keyboard_data();
  byte scan_keyboard();

  static Macros * macros;
  byte capturing_macro = -1;
  byte long_press_macro = -1;
  byte sample_count;
  bool paused = false;
  bool capture_mode;
  int capture_count;
  byte current_key;
  byte captured_key;
};

Macros * Keyboard::macros;

void Keyboard::begin(Macros * macros)
{
  this->macros = macros;
  this->reset();
}

void Keyboard::pause(bool paused)
{
  this->paused = paused;
}

bool Keyboard::is_paused()
{
  return this->paused;
}

void Keyboard::reset(bool reset_macros)
{
  this->capture_mode = false;
  this->capture_count = 0;
  this->current_key = 0;
  this->captured_key = 0; 
  this->sample_count = 0;

  if(reset_macros)
  {
    this->capturing_macro = -1;
    this->long_press_macro = -1;
  }
}

byte Keyboard::get_captured_key()
{
  return this->captured_key;
}

void Keyboard::clear_captured_key()
{
  this->captured_key = 0;
}

int Keyboard::get_capture_count()
{
  return this->capture_count;
}

byte Keyboard::get_current_key()
{
  return this->current_key;
}

void Keyboard::set_capturing_macro(byte macro)
{
  this->capturing_macro = macro;
}

void Keyboard::set_long_press_macro(byte macro)
{
  this->long_press_macro = macro;
}

void Keyboard::process()
{
  if(this->sample_count == 0)
  {
    byte key = scan_keyboard();
    if(key)
    {
      if(!this->capture_mode)
      {
        set_capture_mode(true, key);  
      }
      else
      {
        this->capture_count += 1;

        if(this->capture_count >= KEYBOARD_LONG_PRESS && this->long_press_macro >= 0)
        {
          this->macros->run_macro(this->long_press_macro);
          this->set_capture_mode(false);      
          while(scan_keyboard() != 0)
            ;
        }
      }
    }
    else
    {
      if(this->capture_mode)
      {
        this->set_capture_mode(false);      
      }
    }
  }

  this->sample_count = (this->sample_count + 1) % KEYBOARD_PERIOD;
}

void Keyboard::set_address_pin(byte position, byte bit, byte pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, bitRead(position, bit));
}

void Keyboard::set_keyboard_row(byte row)
{
  set_address_pin(row, 0, KEYBOARD_ROWL);
  set_address_pin(row, 1, KEYBOARD_ROWM);
  set_address_pin(row, 2, KEYBOARD_ROWH);
}

void Keyboard::set_keyboard_col(byte col)
{
  set_address_pin(col, 0, KEYBOARD_COLL);
  set_address_pin(col, 1, KEYBOARD_COLH);
}

bool Keyboard::get_keyboard_data()
{
  pinMode(KEYBOARD_DATA, INPUT);
  return digitalRead(KEYBOARD_DATA) == HIGH ? false : true;
}

byte Keyboard::scan_keyboard()
{
  byte result = 0;  
  for(int row = 0; row < KEYBOARD_ROWS; row++)
  {
    set_keyboard_row(row);
    for(int col = 0; col < KEYBOARD_COLS; col++)
    {
      set_keyboard_col(col);
      if(get_keyboard_data())
      {
        result = (row * KEYBOARD_COLS) + col + 1;
      }
    }
  }
  return result;
}

void Keyboard::set_capture_mode(bool capturing, byte key)
{
  this->capture_mode = capturing;
  if(capturing)
  {
    this->capture_count = 0;
    this->current_key = key;        

    if(this->capturing_macro >= 0)
    {
      this->macros->run_macro(this->capturing_macro);
    }
  }
  else
  {
    this->captured_key = this->current_key;
    this->current_key = 0;    
  }
}

#endif

#endif
