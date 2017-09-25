#ifndef BLINK_EFFECTS_H
#define BLINK_EFFECTS_H

// main blink
#define BLINK_ON 11

// blink group 1-6 (1/6)
#define BLINK_ON_1 12
#define BLINK_ON_2 13
#define BLINK_ON_3 14
#define BLINK_ON_4 15
#define BLINK_ON_5 16
#define BLINK_ON_6 17

// blink group a-b (1/2)
#define BLINK_ON_A 18
#define BLINK_ON_B 19

#define BLINK_MIN BLINK_ON
#define BLINK_MAX BLINK_ON_B

#define MAX_BLINK_SEGMENTS 6
#define DEFAULT_BLINK_PERIOD 1500

class BlinkEffects
{
  public:

  void begin(int blink_period);
  void reset();
  bool process();
  bool blink_on(byte effect);
  static bool is_handled_effect(byte effect);
  void set_blink_period(int blink_period);

  private:

  bool blink_1_6_test(byte effect);
  bool blink_a_b_test(byte effect);

  int blink_period = DEFAULT_BLINK_PERIOD;
  int blink_counter = 0;
  int interval;
  int half_period;
  int half_counter;
};

void BlinkEffects::begin(int blink_period = DEFAULT_BLINK_PERIOD)
{
  set_blink_period(blink_period);
  reset();
}

void BlinkEffects::reset()
{
  this->blink_counter = 0;
  this->half_counter = 0;
}

void BlinkEffects::set_blink_period(int blink_period)
{
  if(blink_period < MAX_BLINK_SEGMENTS)
    this->half_period = DEFAULT_BLINK_PERIOD;
  else
    this->half_period = blink_period;

  // for testing 1-6 blink
  this->interval = this->half_period / MAX_BLINK_SEGMENTS;

  // double the blink period internally for toggling the main blink on and off
  this->blink_period = this->half_period * 2;
}

bool BlinkEffects::process()
{
  this->blink_counter = (this->blink_counter + 1) % this->blink_period;
  this->half_counter = this->blink_counter % this->half_period;
  return this->blink_counter % this->interval == 0;
}

bool BlinkEffects::is_handled_effect(byte effect)
{
  return effect >= BLINK_MIN && effect <= BLINK_MAX;
}

bool BlinkEffects::blink_1_6_test(byte effect)
{
  int start = (effect - BLINK_ON_1) * this->interval;
  int end = start + this->interval;
  return this->half_counter >= start && half_counter < end;
}

bool BlinkEffects::blink_a_b_test(byte effect)
{
  int quarter_period = this->half_period / 2;
  switch(effect)
  {
    case BLINK_ON_A:
      return this->half_counter < quarter_period;

    case BLINK_ON_B:
      return this->half_counter >= quarter_period;
  }
}

bool BlinkEffects::blink_on(byte effect)
{
  switch(effect){
    case BLINK_ON:
       return this->blink_counter < this->half_period;

    case BLINK_ON_1:
    case BLINK_ON_2:
    case BLINK_ON_3:
    case BLINK_ON_4:
    case BLINK_ON_5:
    case BLINK_ON_6:
      return blink_1_6_test(effect);

    case BLINK_ON_A:
    case BLINK_ON_B:
      return blink_a_b_test(effect);
  }
}
#endif
