#ifndef SEQUENCER_H
#define SEQUENCER_H

//#include <power_ease.h>
#include "config.h"

// #define NUM_SEQUENCERS 6

#define SEQUENCE_WHEEL        0
#define SEQUENCE_SWING        1
#define SEQUENCE_WHEEL_COSINE 2
#define SEQUENCE_WHEEL_SINE   3 

#define STATE_NORMAL   0
#define STATE_REVERSE  1
#define STATE_BOUND    2
#define STATE_STOP     3

#define DEFAULT_TYPE SEQUENCE_SWING
#define DEFAULT_LOW    0
#define DEFAULT_HIGH  10

#include <macros.h>
#include <command_processor.h>

class Sequence
{
  public:

  void begin(Macros * macros, CommandProcessor * command_processor, Sequence * sequences);
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

  // needed for brush strokes with next window
  int previous_computed();
  
  void set_previous_computed(int position);
  int evaluate_macro(byte macro);

  private:

  int increment(int step);
  int increment_wheel(int step);
  int increment_swing(int step);
  int increment_swing_normal(int step);
  int increment_swing_reverse(int step);
  int increment_wheel_cosine(int step);
  int increment_wheel_sine(int step);

  // can type and state be consolidated

  byte type;
  int low;
  int max;
  int current;
  int previous;
  byte state;
  float factor;

  // for getting current value of non-linear sequencers
  int computed;

  // for smooth brush strokes
  int prev_computed;

  int width();

  static Macros * macros;
  static CommandProcessor * command_processor;
  static Sequence * sequences;
 };

Macros * Sequence::macros;
CommandProcessor * Sequence::command_processor;
Sequence * Sequence::sequences;

int Sequence::width()
{
  return (this->max + 1) - this->low;    
}
  
void Sequence::begin(Macros * macros, CommandProcessor * command_processor, Sequence * sequences)
{
  set(DEFAULT_TYPE, DEFAULT_LOW, DEFAULT_HIGH);
  this->macros = macros;
  this->command_processor = command_processor;
  this->sequences = sequences;
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
    case SEQUENCE_WHEEL_SINE:
      this->factor = SINE_RANGE / this->width();
      break;
  }
}

void Sequence::fix_current()
{
  this->current = max(this->low, this->current);
  this->current = min(this->max, this->current);
  this->previous = this->current;  
  this->computed = this->current;
  this->prev_computed = this->current;
}

void Sequence::reset()
{
  this->current = this->low;
  this->previous = this->low;
  this->computed = this->low;
  this->prev_computed = this->low;
  this->state = STATE_NORMAL;
}

#define ADVANCE_NEXT      0
#define ADVANCE_CURRENT  -1
#define ADVANCE_COMPUTED -2
#define ADVANCE_OPPOSITE -3
#define ADVANCE_MACRO    -4
#define ADVANCE_SEQADD   -5
#define ADVANCE_SEQSUB   -6
#define ADVANCE_NEW_HIGH -7
#define ADVANCE_NEW_LOW  -8 
#define ADVANCE_RESET    -9
#define SAFETY_MARGIN 1

int Sequence::next(int advancement, int step) // step or macro
{
  switch(advancement)
  {
    case ADVANCE_RESET:
      this->reset();
      return this->current;

    case ADVANCE_NEW_LOW:
    {
      int new_low = step;
      if(new_low + SAFETY_MARGIN < this->max){
        this->set_limit(new_low, this->max + 1);
        this->fix_current();
      }
      return this->current;
    }

    case ADVANCE_NEW_HIGH:
    {
      int new_max = step - 1;
      if(new_max - SAFETY_MARGIN > this->low){
        this->set_limit(this->low, new_max + 1);
        this->fix_current();
      }
      return this->current;
    }

    case ADVANCE_SEQADD:
    {
      Sequence * other = &this->sequences[step];
      return this->computed + other->computed;
    }
    
    case ADVANCE_SEQSUB:
    {
      Sequence * other = &this->sequences[step];
      return this->computed - other->computed;
    }

    case ADVANCE_MACRO:
      return this->current = this->evaluate_macro(step);
    
    case ADVANCE_OPPOSITE:
    { 
      // computing the value first allows for a natural sequence if starting out sequencing opposites
      int value = (this->max - this->current) + this->low;
      if(step == 0) step = 1;
      this->increment(step);
      return value; 
    }
     
    case ADVANCE_COMPUTED:
      return this->computed;
 
    case ADVANCE_CURRENT:
      return this->current;
 
    case ADVANCE_NEXT:
      if(step == 0) step = 1;
      return this->increment(step);
  }
}

int Sequence::increment(int step)
{
  this->previous = this->current;

  switch(this->type)
  {
    default:
    case SEQUENCE_WHEEL:        
      return this->increment_wheel(step);
       
    case SEQUENCE_SWING:        
      return this->increment_swing(step);
 
    case SEQUENCE_WHEEL_COSINE: 
      return this->increment_wheel_cosine(step);
          
    case SEQUENCE_WHEEL_SINE:   
      return this->increment_wheel_sine(step);
  }
}

// this only works with positive step
int Sequence::increment_wheel(int step)
{
// if step if < 0, need to check the low end instead
  if(step < 0) {
    if(this->current + step < this->low)
    {
      int step_carry = (this->current + step) - this->low;
      this->current = (this->max + 1) + step_carry;    
    } 
    else 
    {
      this->current += step;
    }
  } 
  else 
  {
    if(this->current + step > this->max)
    {
      int step_carry = (this->current + step) - (this->max + 1);
      this->current = this->low + step_carry;    
    } 
    else 
    {
      this->current += step;
    }
  }

  return this->computed = this->current;
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
    int step_reflect = this->current - (this->max + 1);
    this->current = this->max - step_reflect;
    this->state = STATE_REVERSE;
  }

  return this->computed = this->current;
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
  
  return this->computed = this->current;
}

int Sequence::increment_wheel_cosine(int step)
{
  increment_wheel(step);
  byte spread_position = (byte)((this->current - this->low) * this->factor);
  return this->computed = 0.5 + this->low + (this->width() * ColorMath::get_cosine(spread_position));
}

int Sequence::increment_wheel_sine(int step)
{
  increment_wheel(step);
  byte spread_position = (byte)((this->current - this->low) * this->factor);
  return this->computed = 0.5 + this->low + (this->width() * ColorMath::get_sine(spread_position));
}

int Sequence::evaluate_macro(byte macro)
{
  // save stack so macro can do isolated math
  int *accumulators = command_processor->accumulators;
  int acc0 = accumulators[0];
  int acc1 = accumulators[1];
  int acc2 = accumulators[2];
  accumulators[0] = 0;
  accumulators[1] = 0;
  accumulators[2] = 0;

  this->macros->run_macro(macro);

  // restore stack
  accumulators[0] = acc0;
  accumulators[1] = acc1;
  accumulators[2] = acc2;

  return this->command_processor->sub_args[0];
}

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
  
  void begin(Macros * macros, CommandProcessor * command_processor);
  void set(int sequencer, byte type, int low, int high);
  void reset(int sequencer);
  int next(int sequencer, int advancement, int step);
  int current(int sequencer);
  int previous(int sequencer);
  int previous_computed(int sequencer);
  void set_previous_computed(int sequencer, int position);

  static Sequence get_sequence(int sequencer) { return sequences[sequencer]; } 

  private:

  static Sequence sequences[NUM_SEQUENCERS];
};

Sequence Sequencer::sequences[NUM_SEQUENCERS];  

void Sequencer::begin(Macros * macros, CommandProcessor * command_processor)
{
  // set default sequences
  for(int i = 0; i < NUM_SEQUENCERS; i++)
    sequences[i].begin(macros, command_processor, sequences);
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
