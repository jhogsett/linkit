#ifndef ELASTIC_EASE_H
#define ELASTIC_EASE_H

#include <common.h>

#define EASE_COUNT 32
#define EASE_DELAY 2
#define EASE_EXPONENT 3 // cubic

class ElasticEase
{
  public:
  static float ease[EASE_COUNT];
  static int ease_count();
  static int ease_delay();
  static void generate_elastic_ease(int count, int power);

  private:
  static float ease_power_in(float percent, int power);
  static float ease_elastic_out(float percent);
};

float ElasticEase::ease[EASE_COUNT];

int ElasticEase::ease_count(){
  return EASE_COUNT;
}

int ElasticEase::ease_delay(){
  return EASE_DELAY;
}

float ElasticEase::ease_power_in(float percent, int power){
  return pow(percent, power);
}

float ElasticEase::ease_elastic_out(float percent){
  return sin(-13 * M_PI_2 * (percent + 1)) * pow(2, -10 * percent) + 1;
}
// elastic in: return sin(13 * M_PI_2 * p) * pow(2, 10 * (p - 1));

// power ease in then elastic ease out
// creates a great projectile-hitting-rubber-sheet animation
void ElasticEase::generate_elastic_ease(int count = EASE_COUNT, int power = EASE_EXPONENT){
  float midpoint = count / 2;
  for(int i = 0; i < midpoint; i++){
    float percent = i / midpoint;
    ease[i] = 0.5 * (ease_power_in(percent, power));
    ease[i + int(midpoint)] = (0.5 * (ease_elastic_out(percent))) + 0.5;
  }
}

#endif
