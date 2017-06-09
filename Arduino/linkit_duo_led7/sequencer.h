#ifndef SEQUENCER_H
#define SEQUENCER_H

#define NUM_SEQUENCES 5

#define SEQUENCE_WHEEL 0
#define SEQUENCE_SWING 1
#define SEQUENCE_DRIVE 2
#define SEQUENCE_BOUND 3

#define STATE_NORMAL   0
#define STATE_REVERSE  1
#define STATE_BOUND    2
#define STATE_STOP     3

#define DEFAULT_TYPE SEQUENCE_SWING
#define DEFAULT_LOW    0
#define DEFAULT_HIGH  10

class Sequence
{
  public:

  void begin();
  void set(byte type, int low, int high);
  void reset();
  int next(int advancement, int step);

  private:

  int increment(int step);
  int increment_wheel(int step);
  int increment_swing(int step);
  int increment_swing_normal(int step);
  int increment_swing_reverse(int step);
  int increment_drive(int step);
  int increment_bound(int step);
  int increment_bound_normal(int step);
  int increment_bound_reverse(int step);

  byte type;
  int low;
  int max;
  
  int current;
  byte state;
};

void Sequence::begin(){
  set(DEFAULT_TYPE, DEFAULT_LOW, DEFAULT_HIGH);
  this->reset();
}

void Sequence::set(byte type, int low, int high){
  this->type = type;
  this->low = low;
  this->max = high - 1;
  this->reset();
}

void Sequence::reset(){
  this->current = this->low;
  this->state = STATE_NORMAL;
}

#define ADVANCE_NEXT      0
#define ADVANCE_CURRENT  -1
#define ADVANCE_OPPOSITE -2
#define ADVANCE_RESET    -3

int Sequence::next(int advancement, int step){
  if(step == 0)
    step = 1;

  switch(advancement){
    case ADVANCE_RESET:
      this->reset();
      return this->current;
    case ADVANCE_OPPOSITE:
      return this->max - this->current;
    case ADVANCE_CURRENT:
      return this->current;
    case ADVANCE_NEXT:
      return this->increment(step);
  }
}

int Sequence::increment(int step){
  switch(this->type){
    case SEQUENCE_WHEEL:
      return this->increment_wheel(step);
    case SEQUENCE_SWING:
      return this->increment_swing(step);
    case SEQUENCE_DRIVE:
      return this->increment_drive(step);
    case SEQUENCE_BOUND:
      return this->increment_bound(step);
  }
}

int Sequence::increment_wheel(int step){
  this->current = this->current + step;

  if(this->current > this->max){
    int step_remaining = step - (this->current - this->max);
    this->current = this->low + step_remaining;    
  }

  return this->current;
}

int Sequence::increment_swing(int step){
  switch(this->state){
    case STATE_NORMAL:
      return increment_swing_normal(step);
    case STATE_REVERSE:
      return increment_swing_reverse(step);
  }
}

int Sequence::increment_swing_normal(int step){
  this->current += step;
  
  if(this->current > this->max){
    int step_remaining = step - (this->current - this->max);
    this->current -= step_remaining;
    this->state = STATE_REVERSE;
  }
  return this->current;
}

int Sequence::increment_swing_reverse(int step){
  this->current -= step;

  if(this->current < this->low){
    int step_remaining = step - (this->low - this->current);
    this->current += step_remaining;
    this->state = STATE_NORMAL;
  }
  return this->current;
}

int Sequence::increment_drive(int step){
  if(this->state == STATE_STOP)
    return this->max;
    
  this->current += step;

  if(this->current > this->max){
    this->state = STATE_STOP;
    this->current = this->max;
  }

  return this->current;
}

int Sequence::increment_bound(int step){
  switch(this->state){
    case STATE_BOUND:
      return increment_bound_normal(step);
    case STATE_STOP:
      return increment_bound_reverse(step);
  }
}

int Sequence::increment_bound_normal(int step){
  this->current += step;

  if(this->current > this->max){
    int step_remaining = step - (this->current - this->max);
    this->current -= step_remaining;
    this->state = STATE_STOP;
  }
  return this->current;
}

int Sequence::increment_bound_reverse(int step){
  if(this->state == STATE_STOP)
    return this->low;

  this->current -= step;
  if(this->current < this->low){
    int step_remaining = step - (this->low - this->current);
    this->current += step_remaining;
    this->state = STATE_NORMAL;
  }

  return this->current;
}


class Sequencer
{
  public:
  void begin();
    
  void set(int sequencer, byte type, int low, int high);
  void reset(int sequencer);
  int next(int sequencer, int advancement, int step);
  
  private:
  
  static Sequence sequences[NUM_SEQUENCES];  
};

Sequence Sequencer::sequences[NUM_SEQUENCES];  

void Sequencer::begin(){
  // set default sequences
  for(int i = 0; i < NUM_SEQUENCES; i++){
    sequences[i].begin();
  }
}

void Sequencer::set(int sequencer, byte type, int low, int high){
  sequences[sequencer].set(type, low, high);
}

void Sequencer::reset(int sequencer){
  sequences[sequencer].reset();
}

int Sequencer::next(int sequencer, int advancement, int step){
  return sequences[sequencer].next(advancement, step);  
}

#endif
