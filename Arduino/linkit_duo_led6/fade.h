
extern rgb_color colors[LED_COUNT];

#define FADE_RATE 0.9999
#define FADE_DELAY 10
#define FADE_TIMES 40

void fade(float rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < ANIM_LED_COUNT * 3; i++){
    *(p + i) *= rate;
  }
}

void exhale_fade(int level){
//  unsigned char *p;
//  p = (unsigned char *)colors; 
//  for(int i = 0; i < ANIM_LED_COUNT * 3; i++){
//    *(p + i) *= breathe_steps[i];
//  }
}

