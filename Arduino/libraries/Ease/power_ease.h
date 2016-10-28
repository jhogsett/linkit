#ifndef POWER_EASE_H
#define POWER_EASE_H

#define EASE_EXPONENT 3 // cubic
#define EASE_COUNT 24
#define EASE_DELAY 1

class PowerEase
{
  public:
  static float ease[EASE_COUNT];

  static int ease_count();
  static int ease_delay();
  static void generate_power_ease(int count, int power);

  private:
  static float ease_power_in(float percent, int power);
  static float ease_power_out(float percent, int power);
};

float PowerEase::ease[EASE_COUNT];

int PowerEase::ease_count(){
  return EASE_COUNT;
}

int PowerEase::ease_delay(){
  return EASE_DELAY;
}

void PowerEase::generate_power_ease(int count = EASE_COUNT, int power = EASE_EXPONENT){
  float midpoint = count / 2;
  for(int i = 0; i < midpoint; i++){
    float percent = i / midpoint;
    ease[i] = 0.5 * (ease_power_in(percent, power));
    ease[i + int(midpoint)] = (0.5 * (ease_power_out(percent, power))) + 0.5;
  }
}

float PowerEase::ease_power_in(float percent, int power){
  return pow(percent, power);
}

float PowerEase::ease_power_out(float percent, int power){
  return 1 - pow(1 - percent, power);
}

#endif

