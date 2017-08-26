#ifndef POWER_EASE_H
#define POWER_EASE_H

#include <common.h>

#define EASE_COUNT 24
#define EASE_DELAY 1
#define EASE_RANGE 24.0

class PowerEase
{
  public:
  static const float PROGMEM ease[];

  static int ease_count();
  static int ease_delay();
  static float ease_range();
  static float get_ease(int step);
};

/* ruby:
@power=3 # CUBIC
maxsteps=24
midpoint = maxsteps / 2
ease = []
def ease_in(percent) percent ** @power; end
def ease_out(percent) 1 - ((1 - percent) ** @power); end
(0..midpoint).each do |i|
  percent = i / (midpoint * 1.0)
  ease[i] = (0.5 * ease_in(percent)).round(15)
  ease[i + midpoint] = ((0.5 * (ease_out(percent))) + 0.5).round(15)
end
ease.each do |e|
  puts e
end
*/
const float PROGMEM PowerEase::ease[]
= {
  0.0,
  0.000289351851852,
  0.002314814814815,
  0.0078125,
  0.018518518518519,
  0.036168981481481,
  0.0625,
  0.099247685185185,
  0.148148148148148,
  0.2109375,
  0.289351851851852,
  0.385127314814815,
  0.5,
  0.614872685185185,
  0.710648148148148,
  0.7890625,
  0.851851851851852,
  0.900752314814815,
  0.9375,
  0.963831018518519,
  0.981481481481481,
  0.9921875,
  0.997685185185185,
  0.999710648148148,
  1.0
};

int PowerEase::ease_count(){
  return EASE_COUNT;
}

int PowerEase::ease_delay(){
  return EASE_DELAY;
}

float PowerEase::ease_range(){
  return EASE_RANGE;
}

float PowerEase::get_ease(int step){
  return pgm_read_float(&ease[step]);
}

#endif
