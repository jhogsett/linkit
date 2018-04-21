#ifndef SEQUENCER_H
#define SEQUENCER_H

//#include <power_ease.h>
#include "config.h"

// #define NUM_SEQUENCERS 6

#define SEQUENCE_WHEEL        0
#define SEQUENCE_SWING        1
#define SEQUENCE_WHEEL_COSINE 2
#define SEQUENCE_SWING_COSINE 3
#define SEQUENCE_WHEEL_SINE   4 
#define SEQUENCE_SWING_SINE   5
#define SEQUENCE_WHEEL_POWER  6 
#define SEQUENCE_SWING_POWER  7

#define STATE_NORMAL   0
#define STATE_REVERSE  1
#define STATE_BOUND    2
#define STATE_STOP     3

#define DEFAULT_TYPE SEQUENCE_SWING
#define DEFAULT_LOW    0
#define DEFAULT_HIGH  10

#include <macros.h>

class Sequence
{
  public:

  void begin();
  void set(byte type, int low, int high);
  void set_limit(int low, int high);
  void fix_current();
  void reset();
  int next(int advancement, int step);

//  int next_position(int advancement, int step);
//  int next_window(int *advancement, int step);  // sets arg1 to the window distance to fill without gaps
//  int next_macro(int macro, int step);          // runs the macro for each position (needs the ability to pass args into macros)

  int current_position();
  int previous_position();
  int previous_computed();
  void set_previous_computed(int position);

  private:

  int increment(int step);
  int increment_wheel(int step);
  int increment_swing(int step);
  int increment_swing_normal(int step);
  int increment_swing_reverse(int step);
  int increment_wheel_cosine(int step);
  int increment_swing_cosine(int step);
  int increment_wheel_sine(int step);
  int increment_swing_sine(int step);
//  int increment_wheel_power(int step);
//  int increment_swing_power(int step);

  // can type and state be consolidated

  byte type;
  int low;
  int max;
  int current;
  int previous;
  byte state;
  float factor;

// how different is this from previous?
  int prev_computed;

  int width();
 };

int Sequence::width()
{
  return this->max - this->low;    
}
  
void Sequence::begin()
{
  set(DEFAULT_TYPE, DEFAULT_LOW, DEFAULT_HIGH);
  this->reset();
}

void Sequence::set(byte type, int low, int high)
{
  this->type = type;
  this->set_limit(low, high);
  this->reset();
}

void Sequence::set_limit(int low, int high){
  this->low = low;
  this->max = high - 1;

  switch(this->type)
  {
    case SEQUENCE_WHEEL_COSINE:
    case SEQUENCE_SWING_COSINE:
    case SEQUENCE_WHEEL_SINE:
    case SEQUENCE_SWING_SINE:
      this->factor = COSINE_RANGE / this->width();
      break;
  
    case SEQUENCE_WHEEL_POWER:
    case SEQUENCE_SWING_POWER:
//      this->factor = PowerEase::ease_range() / this->width();
      break;
  }
}

void Sequence::fix_current()
{
  this->current = max(this->low, this->current);
  this->current = min(this->max, this->current);

  this->previous = this->current;  
  this->prev_computed = this->current;
}

void Sequence::reset()
{
  this->current = this->low;
  this->previous = this->low;
  this->prev_computed = this->low;
  this->state = STATE_NORMAL;
}

#define ADVANCE_NEXT      0
#define ADVANCE_CURRENT  -1
#define ADVANCE_OPPOSITE -2
#define ADVANCE_RESET    -3
#define ADVANCE_MACRO    -4
#define ADVANCE_NEW_HIGH -5
#define ADVANCE_NEW_LOW  -6

// can advancement be a byte?
int Sequence::next(int advancement, int step) // step or macro
{
   if(step == 0)
    step = 1;

  switch(advancement)
  {
    case ADVANCE_RESET:
      this->reset();
      return this->current;

    case ADVANCE_OPPOSITE:
      return (this->max - this->current) + this->low;
 
    case ADVANCE_CURRENT:
      return this->current;
 
    case ADVANCE_NEXT:
      return this->increment(step);
 
    case ADVANCE_NEW_HIGH:
    {
      int new_max = step - 1;
      if(new_max - 2 > this->low){
        this->set_limit(this->low, new_max + 1);
        this->fix_current();
      }
      return this->current;
    }
 
    case ADVANCE_NEW_LOW:
    {
      int new_low = step;
      if(new_low + 2 < this->max){
        this->set_limit(new_low, this->max + 1);
        this->fix_current();
      }
      return this->current;
    }
  }
}

int Sequence::increment(int step)
{
  this->previous = this->current;

  switch(this->type)
  {
    case SEQUENCE_WHEEL:        
       return this->increment_wheel(step);
       
    case SEQUENCE_SWING:        
      return this->increment_swing(step);
 
    case SEQUENCE_WHEEL_COSINE: 
      return this->increment_wheel_cosine(step);
      
    case SEQUENCE_SWING_COSINE: 
      return this->increment_swing_cosine(step);
    
    case SEQUENCE_WHEEL_SINE:   
      return this->increment_wheel_sine(step);
    
    case SEQUENCE_SWING_SINE:   
      return this->increment_swing_sine(step);
    
    case SEQUENCE_WHEEL_POWER:  
      // return this->increment_wheel_power(step);
    
    case SEQUENCE_SWING_POWER:  
      // return this->increment_swing_power(step);
      break;
  }
}

int Sequence::increment_wheel(int step)
{
  this->current += step;

  if(this->current > this->max)
  {
    int step_carry = step - (this->current - this->max);
    this->current = this->low + step_carry;    
  }

  return this->current;
}

int Sequence::increment_swing(int step)
{
  switch(this->state)
  {
    case STATE_NORMAL:
      return increment_swing_normal(step);

    case STATE_REVERSE:
      return increment_swing_reverse(step);
  }
}

int Sequence::increment_swing_normal(int step)
{
  this->current += step;
  
  if(this->current > this->max)
  {
    int step_reflect = this->current - this->max;
    this->current = this->max - step_reflect;
    this->state = STATE_REVERSE;
  }

  return this->current;
}

int Sequence::increment_swing_reverse(int step)
{
  this->current -= step;

  if(this->current < this->low)
  {
    int step_reflect = this->low - this->current;
    this->current = this->low + step_reflect;
    this->state = STATE_NORMAL;
  }
  
  return this->current;
}

int Sequence::increment_wheel_cosine(int step)
{
  increment_wheel(step);
  byte spread_position = (this->current - this->low) * this->factor;
  return this->low + (this->width() * ColorMath::get_cosine(spread_position));
}

int Sequence::increment_wheel_sine(int step)
{
  increment_wheel(step);
  byte spread_position = (this->current - this->low) * this->factor;
  return this->low + (this->width() * ColorMath::get_sine(spread_position));
}

int Sequence::increment_swing_cosine(int step)
{
  increment_swing(step);
  byte spread_position = 0.5 + ((this->current - this->low) * this->factor);
  return this->low + (this->width() * ColorMath::get_cosine(spread_position));
}

int Sequence::increment_swing_sine(int step)
{
  increment_swing(step);
  byte spread_position = 0.5 + ((this->current - this->low) * this->factor);
  return this->low + (this->width() * ColorMath::get_sine(spread_position));
}

//int Sequence::increment_wheel_power(int step)
//{
//  increment_wheel(step);
//  byte spread_position = (this->current - this->low) * this->factor;
//  return this->low + (this->width() * PowerEase::get_ease(spread_position));
//}

// todo: optional
// TODO the sequence is not symmetrical forward and back so when coming back need to treat it oppositely
//int Sequence::increment_swing_power(int step)
//{
//  increment_swing(step);
////  byte spread_position = 0.5 + ((this->current - this->low) * this->factor);
//  byte spread_position;
//  if(state == STATE_NORMAL)
//  {
//    // current-low = progress toward end
//    spread_position = (this->current - this->low) * this->factor;
//  } 
//  else 
//  {
//    // width - (current-low) = progress toward start 
//
//    // this needs rework - power ease data oriented toward forward going animation
//    // just flipping the index when going back makes it appear to go in the same direction as before flipping due to the ordering
//    // to make it work, treat power ease data as progress towards start, not progress toward beginning
//
//    // when this is at max, need to treat as if at low
//    
//    spread_position = (this->max - this->current) * this->factor;
////    spread_position = (this->current - this->low) * this->factor;
////    spread_position = (this->width - (this->current - this->low)) * this->factor;
//
//  // spread position 
//
//
//
//
//  }
//  return this->low + (this->width() * PowerEase::get_ease(spread_position));
//}

int Sequence::current_position()
{
  return this->current;
}

int Sequence::previous_position()
{
  return this->previous;
}

int Sequence::previous_computed()
{
  return this->prev_computed;
}

void Sequence::set_previous_computed(int position)
{
  this->prev_computed = position;  
}






class Sequencer
{
  public:
  
  void begin();
  void set(int sequencer, byte type, int low, int high);
  void reset(int sequencer);
  int next(int sequencer, int advancement, int step);
  int current(int sequencer);
  int previous(int sequencer);
  int previous_computed(int sequencer);
  void set_previous_computed(int sequencer, int position);

  private:

  static Sequence sequences[NUM_SEQUENCERS];  
};

Sequence Sequencer::sequences[NUM_SEQUENCERS];  

void Sequencer::begin()
{
  // set default sequences
  for(int i = 0; i < NUM_SEQUENCERS; i++)
    sequences[i].begin();
}

void Sequencer::set(int sequencer, byte type, int low, int high)
{
  sequences[sequencer].set(type, low, high);
}

void Sequencer::reset(int sequencer)
{
  sequences[sequencer].reset();
}

int Sequencer::next(int sequencer, int advancement, int step)
{
  return sequences[sequencer].next(advancement, step);  
}

int Sequencer::current(int sequencer)
{
  return sequences[sequencer].current_position();  
}

int Sequencer::previous(int sequencer)
{
  return sequences[sequencer].previous_position();  
}

int Sequencer::previous_computed(int sequencer)
{
  return sequences[sequencer].previous_computed();
}

void Sequencer::set_previous_computed(int sequencer, int position)
{
  sequences[sequencer].set_previous_computed(position);  
}

#endif
