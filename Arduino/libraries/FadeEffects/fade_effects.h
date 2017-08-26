#ifndef FADE_EFFECTS_H
#define FADE_EFFECTS_H

#include <common.h>

#define SLOW_FADE 40
#define FAST_FADE 41

#define FADE_MIN SLOW_FADE
#define FADE_MAX FAST_FADE

#define DEFAULT_FADE_PERIOD 10
#define DEFAULT_FADE_RATE 0.95

class FadeEffects
{
  public:

  void begin(float slow_fade_rate, int slow_fade_period);
  void reset();
  bool process();
  static bool is_handled_effect(byte effect);
  rgb_color apply_fade(rgb_color color, byte effect);
  void set_fade_rate(float fade_rate);
  float get_fade_rate();

  private:

  bool should_apply();
  rgb_color apply_slow_fade(rgb_color color);
  rgb_color apply_fast_fade(rgb_color color);

  float fade_rate;
  int fade_period;
  int fade_counter;
};

void FadeEffects::begin(float fade_rate = DEFAULT_FADE_RATE, int fade_period = DEFAULT_FADE_PERIOD){
  this->fade_rate = fade_rate;
  this->fade_period = fade_period;
  reset();
}

void FadeEffects::reset(){
  fade_counter = 0;
}

bool FadeEffects::process(){
  bool should_flush = false;
  fade_counter = (fade_counter + 1) % fade_period;

  if(fade_counter == 0){
    should_flush = true;
  }

  return should_flush;
}

bool FadeEffects::should_apply(){
  return fade_counter == 0 ? true : false;
}

bool FadeEffects::is_handled_effect(byte effect){
  return effect >= FADE_MIN && effect <= FADE_MAX;
}

rgb_color FadeEffects::apply_fade(rgb_color color, byte effect){
  if(should_apply()){
    switch(effect){
      case SLOW_FADE:
        color = apply_slow_fade(color);
        break;
      case FAST_FADE:
        color = apply_fast_fade(color);
        break;
    }
  }

  return color;
}

void FadeEffects::set_fade_rate(float fade_rate){
  this->fade_rate = fade_rate;
}

float FadeEffects::get_fade_rate(){
  return this->fade_rate;
}

rgb_color FadeEffects::apply_slow_fade(rgb_color color){
  color.red = byte(color.red * fade_rate);
  color.green = byte(color.green * fade_rate);
  color.blue = byte(color.blue * fade_rate);
  return color;
}

rgb_color FadeEffects::apply_fast_fade(rgb_color color){
  color.red = color.red / 2;
  color.green = color.green / 2;
  color.blue = color.blue / 2;
  return color;
}

#endif
