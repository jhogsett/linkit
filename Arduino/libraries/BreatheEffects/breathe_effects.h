#ifndef BREATHE_EFFECTS_H
#define BREATHE_EFFECTS_H

#define BREATHE_ON 30

#define BREATHE_TIME 500
#define BREATHE_BRIGHTNESS_MAX DEFAULT_BRIGHTNESS_PERCENT
#define BREATHE_MAX_STEP 22 
#define BREATHE_BRIGHTNESS_MIN 0

class BreatheEffects
{
  public:
  byte breathe_step = 0;
  static float breathe_steps[];

  void begin(int breathe_time);
  void reset();
  bool process();
  bool is_handled_effect(byte effect);
  float breathe_ratio();
 
  private:
  int breathe_time = BREATHE_TIME;
  int breathe_counter = 0;
  char breathe_direction = 1;
};

// ruby: (0..90).step(5).each { |i| puts (Math.cos(i * Math::PI / 180)).round(4) }
float BreatheEffects::breathe_steps[] = {
  1.0,
  0.9962,
  0.9848,
  0.9659,
  0.9397,
  0.9063,
  0.866,
  0.8192,
  0.766,
  0.7071,
  0.6428,
  0.5736,
  0.5,
  0.4226,
  0.342,
  0.2588,
  0.1736,
  0.0872,
  0.0,
  0.0,  // a few extra make it feel more natural
  0.0,
  0,0
};

void BreatheEffects::begin(int breathe_time = BREATHE_TIME){
  this->breathe_time = breathe_time;
}

void BreatheEffects::reset(){
  breathe_step = 0;
  breathe_counter = 0;
  breathe_direction = 1;
}

bool BreatheEffects::process(){
  bool should_flush = false;

  breathe_counter = (breathe_counter + 1) % breathe_time;
  if(breathe_counter == 0){
    int next_breathe_step = breathe_step + breathe_direction;
    if(next_breathe_step < 0 || next_breathe_step >= BREATHE_MAX_STEP){
      breathe_direction *= -1;
    }
    breathe_step = breathe_step + breathe_direction;
    should_flush = true;
  }
  return should_flush;
}

bool BreatheEffects::is_handled_effect(byte effect){
  return effect == BREATHE_ON;
}

float BreatheEffects::breathe_ratio(){
  return breathe_steps[BREATHE_MAX_STEP - breathe_step];
}

#endif

