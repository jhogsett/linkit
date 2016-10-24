
#define EASE_EXPONENT 3 // cubic
#define EASE_COUNT 32
#define POWER_EASE_COUNT 24
#define EASE_DELAY 2
#define POWER_EASE_DELAY 1
#define EASE_ANIM_MARGIN 10

float elastic_ease[EASE_COUNT];
float power_ease[EASE_COUNT];

float ease_power_in(float percent, int power){
  return pow(percent, power);
}

float ease_power_out(float percent, int power){
  return 1 - pow(1 - percent, power);
}

float ease_elastic_out(float percent){
  return sin(-13 * M_PI_2 * (percent + 1)) * pow(2, -10 * percent) + 1;  
}
// elastic in: return sin(13 * M_PI_2 * p) * pow(2, 10 * (p - 1));

void generate_power_ease(int count, int power){
  float midpoint = count / 2;
  for(int i = 0; i < midpoint; i++){
    float percent = i / midpoint;
    power_ease[i] = 0.5 * (ease_power_in(percent, power));
    power_ease[i + int(midpoint)] = (0.5 * (ease_power_out(percent, power))) + 0.5;
  }
}

// power ease in then elastic ease out
void generate_elastic_ease(int count, int power){
  float midpoint = count / 2;
  for(int i = 0; i < midpoint; i++){
    float percent = i / midpoint;
    elastic_ease[i] = 0.5 * (ease_power_in(percent, power));
    elastic_ease[i + int(midpoint)] = (0.5 * (ease_elastic_out(percent))) + 0.5;
  }
}

