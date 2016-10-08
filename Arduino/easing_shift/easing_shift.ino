// try: dim at ends per cosine, bump into each other and transfer momentum

#include <PololuLedStrip.h>
#include <math.h>
 
// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

#define EASE_EXPONENT 3 // cubic
#define EASE_COUNT 64
#define EASE_DELAY 2
#define EASE_ANIM_MARGIN 15

// Create a buffer for holding the colors (3 bytes per color).
#define ANIM_LED_COUNT 64
#define LED_COUNT (ANIM_LED_COUNT + EASE_ANIM_MARGIN)
#define VLED_COUNT (LED_COUNT)
#define MAX_LED (LED_COUNT)
#define MAX_VLED (VLED_COUNT)
#define LED_COUNT_MIDPOINT (LED_COUNT / 2)

rgb_color colors[LED_COUNT];
rgb_color colors2[LED_COUNT];
unsigned char existence[VLED_COUNT];

float ease[LED_COUNT];

#define DELAY 2000
#define FADE_RATE 0.75
#define FADE_COUNT 20

#define BRIGHTNESS_PERCENT 15
#define BRIGHTNESS_DIVISOR 20.0
#define MIN_BRIGHTNESS 1.0

#define RESOLUTION 1000000.0

#define MIN_SPEED 0.50
#define MAX_SPEED 0.75
#define SPEED MIN_SPEED

#define NUM_ROWS 10
#define NUM_COLS 17
#define MAX_ROW (NUM_ROWS - 1)
#define MAX_COL (NUM_COLS - 1)

rgb_color black = {0, 0, 0};
rgb_color gray = {10, 10, 10};
rgb_color white = {20, 20, 20};
rgb_color red = {20, 0, 0};
rgb_color orange = {20, 10, 0};
rgb_color yellow = {20, 20, 0};
rgb_color ltgreen = {10, 20, 0};
rgb_color green = {0, 20, 0};
rgb_color seafoam = {0, 20, 10};
rgb_color cyan = {0, 20, 20};
rgb_color ltblue = {0, 10, 20};
rgb_color blue = {0, 0, 20};
rgb_color purple = {10, 0, 20};
rgb_color magenta = {20, 0, 20};
rgb_color pink = {20, 0, 10};

#define NPALETTE 15
#define NPRETTY_COLORS 12
rgb_color palette[NPALETTE] = { red, orange, pink, magenta,      yellow, green, seafoam, ltgreen,       ltblue, cyan, blue, purple,        white, gray, black }; 

#define NGROUPS 3
#define GROUP_RED_START 0
#define GROUP_RED_MAX 4
#define GROUP_GREEN_START 4
#define GROUP_GREEN_MAX 8
#define GROUP_BLUE_START 8
#define GROUP_BLUE_MAX 12
#define GROUP_WHITE_START 12
#define GROUP_WHITE_MAX 13

int group_start[4] = {GROUP_RED_START,GROUP_GREEN_START,GROUP_BLUE_START,GROUP_WHITE_START};
int group_max[4] = {GROUP_RED_MAX,GROUP_GREEN_MAX,GROUP_BLUE_MAX,GROUP_WHITE_MAX};

rgb_color adjusted_palette[NPALETTE];

// 72 @ 2.0
// int sin223_translation[223] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 69, 69, 69, 69, 69, 69, 70, 70, 70, 70, 70, 70, 70, 70, 70, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71};

// 72 @ 5.0
// int sin557_translation[557] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 57, 57, 57, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 64, 64, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71};

// 64 @ 2.0
// int sin198_translation[198] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 60, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};

// #define WARPED sin223_translation

#define NUM_OBJECTS 6

//#define MIN_PERIOD 50
//#define MAX_PERIOD 100

#define WIDTH 3

#define DECAY 0.0

unsigned char bitmask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

rgb_color scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
}

void set_brightness(int brightness_percent = BRIGHTNESS_PERCENT){
  float percent = brightness_percent / 100.0;
  for(int i = 0; i < NPALETTE; i++){
    adjusted_palette[i] = scale_color(palette[i], percent);
  }
}

void flood(rgb_color color){
  for(int i = 0; i < LED_COUNT; i++){
    colors[i] = color;
  }
}

void erase(){
  flood(black);
}

void fade(float rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) *= rate;
  }
}

void fade_fast(){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

void fade_fast2(int rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) = max(0, *(p + i) - rate);
  }
}

float random_dir(){
  float result = ((random(RESOLUTION) / RESOLUTION) * (MAX_SPEED - MIN_SPEED)) + MIN_SPEED;

  result = random(2) == 1 ? result : result * -1.0;

  return result;
  
  //int range = random(RESOLUTION - MIN_SPEED) + MIN_SPEED;
  //return (range / (RESOLUTION * 1.0)) - 0.5;
}

int random_color(){
  return random(NPRETTY_COLORS);
}

int reflect_i(int *i, int *d, int mx, int mn = 0){
  int ld = *d;
  int next = *i + *d;
  if(next > mx || next < mn){
    *d *= -1;
  }
  *i += *d;

  int dir = 0;
  if(*d > ld){
    dir = 1;
  } else if(*d < ld){
    dir = -1;
  }
  return dir;    
}

bool position_occupied(int pos, int id){
  return (existence[pos] & ~bitmask[id]) == 0 ? false : true;
}

float reflect_f(float *f, float *d, int id, float mx = 1.0, float mn = 0.0){
  float ld = *d;
  float last = *f;
  float next = *f + *d;

//  int lasti = WARPAGE[int(last)];
//  int nexti = WARPAGE[int(next)];
  int lasti = int(last);
  int nexti = int(next);
  
//  bool clash = (nexti != lasti) && position_occupied(nexti, id);
  bool clash =  position_occupied(nexti, id);
  
  if(next > mx || next < mn || clash){
    *d *= -1.0;
  }
  *f += *d;

  int dir = 0;
  if(*d > ld){
    dir = 1;
  } else if(*d < ld){
    dir = -1;
  }
  return dir;
}

int wrap_i(int i, int d, int mx){
  int next = i + d;
  if(next >= mx){
    return 0;
  } else {
    return next;
  }
}

float wrap_f(float f, float d, float mx = 1.0){
  float next = f + d;
  if(next >= mx){
    return 0.0;
  } else {
    return next;
  }
}

rgb_color add_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = min(255, color1.red + color2.red);
  new_color.green = min(255, color1.green + color2.green);
  new_color.blue = min(255, color1.blue + color2.blue);
  return new_color;
}

rgb_color subtract_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = max(0, color1.red - color2.red);
  new_color.green = max(0, color1.green - color2.green);
  new_color.blue = max(0, color1.blue - color2.blue);
  return new_color;
}

//swaps red & green, needed on led strands, not on strips
void setup_colors(bool swap = true){
  if(swap == true){
    for(int i = 0; i < NPALETTE; i++){
      unsigned char value = palette[i].red;
      palette[i].red = palette[i].green;  
      palette[i].green = value;
    }
  }
  memcpy(adjusted_palette, palette, sizeof(palette));
}

void poof(int color, int times=10, int brightness=50){
  set_brightness(brightness);
  for(int i = 0; i < times; i++){
    flood(black);
    ledStrip.write(colors, LED_COUNT);
    delay(33);
    flood(adjusted_palette[color]);
    ledStrip.write(colors, LED_COUNT);
    delay(33);
  }

  flood(black);
  ledStrip.write(colors, LED_COUNT);
  delay(33);
}

void draw(rgb_color color, int pos, int id = 0){
  if(pos < 0 || pos >= LED_COUNT) return;
  //int warped = WARPED[pos];
  colors[pos] = add_color(colors[pos], color);
  existence[pos] |= bitmask[id];
}

void undraw(rgb_color color, int pos, int id = 0){
  if(pos < 0 || pos >= LED_COUNT) return;
  //int warped = WARPED[pos];
  //colors[warped] = subtract_color(colors[warped], color);
  existence[pos] &= ~bitmask[id];
}

float decay(float value){
  float orig = value;
  if(value >= 0){
    value -= DECAY;
    if(value < 0){
      value = random_dir(); 
    }
  } else {
    value += DECAY;
    if(value > 0){
      value = random_dir(); 
    }
  }
  return value;
}

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

float ease_bounce_out(float percent){
  if(percent < 4/11.0) return (121 * percent * percent)/16.0;
  else if(percent < 8/11.0) return (363/40.0 * percent * percent) - (99/10.0 * percent) + 17/5.0;
  else if(percent < 9/10.0) return (4356/361.0 * percent * percent) - (35442/1805.0 * percent) + 16061/1805.0;
  else return (54/5.0 * percent * percent) - (513/25.0 * percent) + 268/25.0;
}

void generate_power_ease(int count, int power){
  float midpoint = count / 2;
  for(int i = 0; i < midpoint; i++){
    float percent = i / midpoint;
    ease[i] = 0.5 * (ease_power_in(percent, power));
    ease[i + int(midpoint)] = (0.5 * (ease_power_out(percent, power))) + 0.5;
  }
}

// power ease in then elastic ease out
void generate_elastic_ease(int count, int power){
  float midpoint = count / 2;
  for(int i = 0; i < midpoint; i++){
    float percent = i / midpoint;
    ease[i] = 0.5 * (ease_power_in(percent, power));
    ease[i + int(midpoint)] = (0.5 * (ease_elastic_out(percent))) + 0.5;
  }
}

// power ease in then bounce ease out
void generate_bounce_ease(int count, int power){
  float midpoint = count / 2;
  for(int i = 0; i < midpoint; i++){
    float percent = i / midpoint;
    ease[i] = 0.5 * (ease_power_in(percent, power));
    ease[i + int(midpoint)] = (0.5 * (ease_bounce_out(percent))) + 0.5;
  }
}

void flush(){
  ledStrip.write(colors, ANIM_LED_COUNT);
}

void setup()
{
  randomSeed(analogRead(0));
  setup_colors(false);
  set_brightness();

  for(int i = 0; i < VLED_COUNT; i++){
     existence[i] = 0;
  }

  set_brightness(BRIGHTNESS_PERCENT);

  generate_elastic_ease(EASE_COUNT, EASE_EXPONENT);
}

void shift(int count){
  for(int i = 0; i < count; i++){
    colors2[i] = black;
  }
  for(int i = count; i < ANIM_LED_COUNT; i++){
    colors2[i] = colors[i - count];
  }

  ledStrip.write(colors2, ANIM_LED_COUNT);
}

void loop()
{
  rgb_color color = adjusted_palette[random_color()];
  for(int i = 0; i < ANIM_LED_COUNT; i++){
    if(i % 4 == 0){
      colors[i] = black;
      color = adjusted_palette[random_color()];
    } else {
      colors[i] = color;
    }
    flush();
  }

  while(true){
    delay(DELAY);

    int count = random(1,15)  * 4;

    generate_elastic_ease(EASE_COUNT, EASE_EXPONENT);
    for(int i = 0; i < EASE_COUNT; i++){
      int pos = ease[i] * count;
      delay(EASE_DELAY);
      shift(pos);
    }

    delay(DELAY);

    generate_power_ease(EASE_COUNT, EASE_EXPONENT);
    for(int i = EASE_COUNT-1; i >= 0 ; i--){
      int pos = ease[i] * count;
      delay(EASE_DELAY);
      shift(pos);
    }
  }


  
  int lx = 0;
  int x = 0;
  
  while(true){
    lx = x;
    x = random(ANIM_LED_COUNT);
    rgb_color color = adjusted_palette[random_color()];

    int anim_start = lx;
    int anim_width = x - lx;
    int anim_dir = anim_width < 0 ? -1 : 1;
    if(anim_dir == -1){
      anim_width *= -1;
      anim_start = x;
    }

    for(int i = 0; i < EASE_COUNT; i++){
      fade_fast();
      int pos = ease[i] * anim_width;

      if(anim_dir == -1){
        pos = anim_width - pos;
      } 

      draw(gray, x);
      draw(color,  pos + anim_start);
      flush(); 
      delay(EASE_DELAY);
    }

    delay(DELAY);
  }

//      for(int i = 0; i < FADE_COUNT; i++){
//        fade_fast();
//        draw(white, ANIM_LED_COUNT-1);
//        flush(); 
//        delay(EASE_DELAY);
//      }
//
//      erase();
      
//      for(int i = EASE_COUNT - 1; i >= 0; i--){
//        fade_fast();
//        int x = ease[i] * (LED_COUNT - ANIM_MARGIN);
//        if(x >= 0 && x < LED_COUNT){
//          draw(adjusted_palette[e],  x);
//        }
//        flush(); 
//        delay(EASE_DELAY);
//      }

//      delay(DELAY);
//    }

  return;

//  float x[NUM_OBJECTS];
//  float dx[NUM_OBJECTS];
//  int lx[NUM_OBJECTS];
//  int group[NUM_OBJECTS];
//  int period[NUM_OBJECTS];
//  int count[NUM_OBJECTS];
//  bool state[NUM_OBJECTS];
//  // int change[NUM_OBJECTS];
//  rgb_color show_color[NUM_OBJECTS];

//  int choices[NGROUPS];
//  for(int i = 0; i < NGROUPS; i++){
//    choices[i] = -1;
//  }

/*
  for(int i = 0; i < NGROUPS; i++){
    while(true){
      int color = random(NGROUPS);
  
      bool try_again = false;
      for(int j = 0; j < i; j++){
        if(color == choices[j]){
          try_again = true;
          break;
        }
      }
      if(try_again == true){
        continue;
      }

      choices[i] = color;
      break;
    }  
  }
*/
//  for(int i = 0; i < NUM_OBJECTS; i++){
//    x[i] = i * (VLED_COUNT / NUM_OBJECTS);
//    dx[i] = random_dir(); 
////    dx[i] = 1.0;
//    lx[i] = -1;
////    period[i] = random(MIN_PERIOD, MAX_PERIOD);
////    count[i] = period[i];
////    state[i] = false;
//  }
//
//  rgb_color random_colors[NUM_OBJECTS];
//  for(int i = 0; i < NUM_OBJECTS; i++){
////    group[i] = choices[i % NGROUPS];
////    random_colors[i] = adjusted_palette[random(group_start[group[i]], group_max[group[i]])];
//    random_colors[i] = adjusted_palette[i];
////    show_color[i] = random_colors[i];
//  }
//
//  while(true){
//    //fade();
//    //erase();
//    fade_fast();
//    //fade_fast2();
//
//    for(int i = 0; i < NUM_OBJECTS; i++){
//      //change[i] = 
//      reflect_f(&x[i], &dx[i], i, MAX_VLED);
//
//      //dx[i] = decay(dx[i]);
//
////      int tpos = WARPAGE[int(x[i])];
//      int tpos = int(x[i]);
////      if(tpos != lx[i]){
//        undraw(random_colors[i], lx[i], i);
//        draw(random_colors[i], tpos, i);
//        lx[i] = tpos;
////      }
//      
////      if(lx != x[i]){
////        int lpos = sin223_translation[int(lx)];
////      }
//    }

/*
    for(int i = 0; i < NUM_OBJECTS; i++){
      if(change[i] == 0){
        if(position_occupied(int(x[i]), i)){
          dx[i] *= -1.0;
        }
      }
    }
*/

//    ledStrip.write(colors, LED_COUNT); 
//    delay(DELAY);
//  }


}

int low_limit(int value, int limit){
  return value < limit ? limit : value;  
}

int high_limit(int value, int limit){
  return value > limit ? limit : value;  
}

int keep_in(int value, int lower, int upper){
  return low_limit(high_limit(value, upper), lower);
}



