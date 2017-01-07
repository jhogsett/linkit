#ifndef BLINK_EFFECTS_H
#define BLINK_EFFECTS_H

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

// pulse blink
#define BLINK_ON_P 23

#define BLINK_MIN BLINK_ON 
#define BLINK_MAX BLINK_ON_P

#define DEFAULT_BLINK_TIME 6000
#define BLINK_PULSE_TIME 200

class BlinkEffects
{
  public:
  bool blink_state = true;   // to do: some may be redundant, only need 1-6 and p
  bool blink_state_1 = true;
  bool blink_state_2 = true;
  bool blink_state_3 = true;
  bool blink_state_4 = true;
  bool blink_state_5 = true;
  bool blink_state_6 = true;
  bool blink_state_a = true;
  bool blink_state_b = true;
  bool blink_state_p = true;

  void begin(int max_blink);
  void reset();
  bool process();
  bool blink_on(byte effect);
  static bool is_handled_effect(byte effect);
  void set_blink_period(int period);

  private:
  int max_blink = DEFAULT_BLINK_TIME;
  int blink_counter = 0;

  int blink1_time; // i, a, p on
  int blink2_time;
  int blink3_time;
  int blink4_time; // b
  int blink5_time;
  int blink6_time;
  int blinkp_time; // p off
};

void BlinkEffects::begin(int max_blink = DEFAULT_BLINK_TIME){
  this->max_blink = max_blink;
  int interval = this->max_blink / 6;

  blink1_time = 0 * interval;     // i, a, p on
  blink2_time = 1 * interval;
  blink3_time = 2 * interval;
  blink4_time = 3 * interval;     // b
  blink5_time = 4 * interval;
  blink6_time = 5 * interval;
  blinkp_time = BLINK_PULSE_TIME; // p off

  reset();
}

void BlinkEffects::reset(){
  blink_state = true;
  blink_state_1 = false;
  blink_state_2 = false;
  blink_state_3 = false;
  blink_state_4 = false;
  blink_state_5 = false;
  blink_state_6 = false;
  blink_state_a = false;
  blink_state_b = false;
  blink_state_p = false;
  blink_counter = 0;
}

bool BlinkEffects::process(){
  bool should_flush = false;
  
  blink_counter = (blink_counter + 1) % max_blink;

  if(blink_counter == blink1_time)
  {
    if(blink_state){
      blink_state = false;
    } else {
      blink_state = true;
    }
    should_flush = true;
  }

  if(blink_counter == blink1_time)
  {
    blink_state_1 = true;
    blink_state_6 = false;

    blink_state_a = true;
    blink_state_b = false;

    blink_state_p = true;
    should_flush = true;
  }

  if(blink_counter == blinkp_time)
  {
    blink_state_p = false;
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

  return should_flush;
}

bool BlinkEffects::is_handled_effect(byte effect){
  return effect >= BLINK_MIN && effect <= BLINK_MAX;
}

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
      (effect == BLINK_ON_P && blink_state_p)) {
    return true;
  }
  return false;
}

#endif
