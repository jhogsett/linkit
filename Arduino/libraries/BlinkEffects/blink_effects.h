#ifndef BLINK_EFFECTS_H
#define BLINK_EFFECTS_H

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

#define MAX_BLINK 6000
#define BLINK_0 0
#define BLINK_1 999
#define BLINK_2 1999
#define BLINK_3 2999
#define BLINK_4 3999
#define BLINK_5 4999
#define BLINK_6 5999
#define BLINK_A BLINK_1
#define BLINK_B BLINK_4

#define BLINK_PULSE_TIME 200
#define BLINK_P_1 BLINK_1
#define BLINK_P_0 (BLINK_1 + BLINK_PULSE_TIME)

class BlinkEffects
{
  public:
  bool blink_state = true;
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
  bool blink_on(int effect);
  static bool is_blink_effect(int effect);

  private:
  int max_blink = MAX_BLINK;
  int blink_counter = 0;
};

void BlinkEffects::begin(int max_blink = MAX_BLINK){
  this->max_blink = max_blink;
  // to do: spread out the blink state change points per the max_blink
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
  if(blink_counter == BLINK_1){
    if(blink_state){
      blink_state = false;
    } else {
      blink_state = true;
    }
    should_flush = true;
  }

  if(blink_counter == BLINK_1){
    blink_state_1 = true;
    blink_state_a = true;
    blink_state_p = true;
    blink_state_6 = false;
    blink_state_b = false;
    should_flush = true;
  }

  if(blink_counter == BLINK_P_0){
    blink_state_p = false;
    should_flush = true;
  }

  if(blink_counter == BLINK_2){
    blink_state_2 = true;
    blink_state_1 = false;
    should_flush = true;
  }

  if(blink_counter == BLINK_3){
    blink_state_3 = true;
    blink_state_2 = false;
    should_flush = true;
  }

  if(blink_counter == BLINK_4){
    blink_state_4 = true;
    blink_state_b = true;
    blink_state_3 = false;
    blink_state_a = false;
    should_flush = true;
  }

  if(blink_counter == BLINK_5){
    blink_state_5 = true;
    blink_state_4 = false;
    should_flush = true;
  }

  if(blink_counter == BLINK_6){
    blink_state_6 = true;
    blink_state_5 = false;
    should_flush = true;
  }

  return should_flush;
}

bool BlinkEffects::is_blink_effect(int effect){
  return effect >= BLINK_MIN && effect <= BLINK_MAX;
}

bool BlinkEffects::blink_on(int effect){
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
