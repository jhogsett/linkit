#ifndef BLINK_EFFECTS_H
#define BLINK_EFFECTS_H

#include <common.h>

// to do: investigate storing states as bit patterns

// main blink
#define BLINK_ON 10

// blink group 1-6 (1/6)
#define BLINK_ON_1 11
#define BLINK_ON_2 12
#define BLINK_ON_3 13
#define BLINK_ON_4 14
#define BLINK_ON_5 15
#define BLINK_ON_6 16

// blink group a-b (1/2)
#define BLINK_ON_A 21
#define BLINK_ON_B 22

// custom blink
#define BLINK_ON_C 23

#define BLINK_MIN BLINK_ON
#define BLINK_MAX BLINK_ON_C

#define MAX_BLINK_SEGMENTS 6
#define DEFAULT_BLINK_PERIOD 6000
#define DEFAULT_BLINKC_TIME 500
#define MINIMUM_BLINKC_TIME 50

class BlinkEffects
{
  public:
  bool blink_state = true;   // to do: some may be redundant, only need 1-6 and c
  bool blink_state_1 = true;
  bool blink_state_2 = true;
  bool blink_state_3 = true;
  bool blink_state_4 = true;
  bool blink_state_5 = true;
  bool blink_state_6 = true;
  bool blink_state_a = true;
  bool blink_state_b = true;
  bool blink_state_c = true;

  void begin(int blink_period, int custom_blink);
  void reset();
  bool process();
  bool blink_on(byte effect);
  static bool is_handled_effect(byte effect);
  void set_blink_period(int blink_period);
  void set_custom_blink(int blink_period);

  private:
  int blink_period = DEFAULT_BLINK_PERIOD;
  int blink_counter = 0;

  int blink1_time; // i, a, c on
  int blink2_time;
  int blink3_time;
  int blink4_time; // b
  int blink5_time;
  int blink6_time;

  int blinkc_counter; // independent counter for custom blink period
  int blinkc_period;  // state is toggled each one of these
  // this is a 50% duty cycle, would need to add another value to alter
};

void BlinkEffects::begin(int blink_period = DEFAULT_BLINK_PERIOD, int custom_blink = DEFAULT_BLINKC_TIME){
  set_blink_period(blink_period);
  set_custom_blink(custom_blink);
  reset();
}

void BlinkEffects::reset(){
  blink_state = true;         // I wonder why I start the main blink on?
  blink_state_1 = false;
  blink_state_2 = false;
  blink_state_3 = false;
  blink_state_4 = false;
  blink_state_5 = false;
  blink_state_6 = false;
  blink_state_a = false;
  blink_state_b = false;
  blink_state_c = false;

  blink_counter = 0;
  blinkc_counter = 0;
}

void BlinkEffects::set_blink_period(int blink_period){
  if(blink_period < MAX_BLINK_SEGMENTS){
    this->blink_period = DEFAULT_BLINK_PERIOD;
  } else {
    this->blink_period = blink_period;
  }
  int interval = this->blink_period / MAX_BLINK_SEGMENTS;

  this->blink1_time = 0 * interval; // i, a, c on
  this->blink2_time = 1 * interval;
  this->blink3_time = 2 * interval;
  this->blink4_time = 3 * interval;  // b
  this->blink5_time = 4 * interval;
  this->blink6_time = 5 * interval;
}

void BlinkEffects::set_custom_blink(int blink_period){
  if(blink_period < MINIMUM_BLINKC_TIME){
    this->blinkc_period = MINIMUM_BLINKC_TIME;
  } else {
    this->blinkc_period = blink_period;
  }
  blinkc_counter = 0;
  blink_state_c = false;
}

bool BlinkEffects::process(){
  bool should_flush = false;

  blink_counter = (blink_counter + 1) % blink_period;
  blinkc_counter = (blinkc_counter + 1) % blinkc_period;

  if(blink_counter == blink1_time)
  {
    blink_state = blink_state ? false : true;

    blink_state_1 = true;
    blink_state_6 = false;

    blink_state_a = true;
    blink_state_b = false;

    should_flush = true;
  }

  if(blink_counter == blink2_time)
  {
    blink_state_2 = true;
    blink_state_1 = false;
    should_flush = true;
  }

  if(blink_counter == blink3_time)
  {
    blink_state_3 = true;
    blink_state_2 = false;
    should_flush = true;
  }

  if(blink_counter == blink4_time)
  {
    blink_state_4 = true;
    blink_state_3 = false;

    blink_state_b = true;
    blink_state_a = false;
    should_flush = true;
  }

  if(blink_counter == blink5_time)
  {
    blink_state_5 = true;
    blink_state_4 = false;
    should_flush = true;
  }

  if(blink_counter == blink6_time)
  {
    blink_state_6 = true;
    blink_state_5 = false;
    should_flush = true;
  }

  if(blinkc_counter == blink1_time)
  {
    blink_state_c = blink_state_c ? false : true;
    should_flush = true;
  }

  return should_flush;
}

bool BlinkEffects::is_handled_effect(byte effect){
  return effect >= BLINK_MIN && effect <= BLINK_MAX;
}

// TODO: refactor
bool BlinkEffects::blink_on(byte effect){
  if((effect == BLINK_ON && blink_state) ||
      (effect == BLINK_ON_1 && blink_state_1) ||
      (effect == BLINK_ON_2 && blink_state_2) ||
      (effect == BLINK_ON_3 && blink_state_3) ||
      (effect == BLINK_ON_4 && blink_state_4) ||
      (effect == BLINK_ON_5 && blink_state_5) ||
      (effect == BLINK_ON_6 && blink_state_6) ||
      (effect == BLINK_ON_A && blink_state_a) ||
      (effect == BLINK_ON_B && blink_state_b) ||
      (effect == BLINK_ON_C && blink_state_c)) {
    return true;
  }
  return false;
}

#endif
