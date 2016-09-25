
#include <PololuLedStrip.h>
PololuLedStrip<12> ledStrip;

#define RANDOM_SEED_PIN A1

#define LED_COUNT 64
#define MAX_LED (LED_COUNT)
rgb_color colors[LED_COUNT];
rgb_color render[LED_COUNT];

int effects[LED_COUNT];

#define BRIGHTNESS_DIVISOR 20.0
#define DEFAULT_BRIGHTNESS_PERCENT 20
#define DIM_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT * 2)
#define MAX_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT * 4)

#define FADE_RATE 0.9999
#define FADE_DELAY 10
#define FADE_TIMES 40

// Standard colors
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
rgb_color dkgray = {5, 5, 5};

#define NPALETTE 16
#define NPRETTY_COLORS 13
rgb_color palette[NPALETTE] = { white, red, orange, yellow, ltgreen, green, seafoam, cyan, ltblue, blue, purple, magenta, pink, black, gray, dkgray };
rgb_color adjusted_palette[NPALETTE];

rgb_color scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
}

void set_brightness(int brightness_percent = DEFAULT_BRIGHTNESS_PERCENT){
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

void erase(bool display = false){
  for(int i = 0; i < LED_COUNT; i++){
    colors[i] = black;
    effects[i] = 0;
  }
  if(display){
    ledStrip.write(colors, LED_COUNT); 
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

#define NO_STATE 0
#define BREATHE_ON 1

#define BLINK_ON 2

// blink group 1-6
#define BLINK_ON_1 11
#define BLINK_ON_2 12
#define BLINK_ON_3 13
#define BLINK_ON_4 14
#define BLINK_ON_5 15
#define BLINK_ON_6 16

// blink group a-b
#define BLINK_ON_A 21
#define BLINK_ON_B 22

#define BLINK_MIN BLINK_ON 
#define BLINK_MAX BLINK_ON_B

#define STATIC_ON 101

void push_color(rgb_color color, bool display = false){
  for(int i = MAX_LED - 1; i >= 1; i--){
    colors[i] = colors[i-1];
    effects[i] = effects[i-1];
  }
  colors[0] = color;
  effects[0] = NO_STATE;

  if(display){
    ledStrip.write(colors, LED_COUNT); 
  }
}

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
int blink_counter = 0;
bool blink_state = true; // false means blank

// blink groups
bool blink_state_1 = true; 
bool blink_state_2 = true; 
bool blink_state_3 = true; 
bool blink_state_4 = true; 
bool blink_state_5 = true; 
bool blink_state_6 = true; 
bool blink_state_a = true; 
bool blink_state_b = true; 

void start_effect(int effect){
  effects[0] = effect;
}

void start_blinking_r(){
  effects[0] = BLINK_ON_1 + random((BLINK_ON_6 - BLINK_ON_1) + 1);
}

#define BREATHE_TIME 500
#define BREATHE_BRIGHTNESS_MAX DEFAULT_BRIGHTNESS_PERCENT
#define BREATHE_MAX_STEP 22 
#define BREATHE_BRIGHTNESS_MIN 0
#define DEFAULT_BRIGHTNESS_SCALE (DEFAULT_BRIGHTNESS_PERCENT / 100.0)
#define MINIMUM_BRIGHTNESS_SCALE 0.02

int breathe_counter = 0;
int breathe_step = 0;
int breathe_direction = 1;

// ruby: (0..90).step(5).each { |i| puts (Math.cos(i * Math::PI / 180)).round(4) }
float breathe_steps[] = {
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

void start_breathing(){
  effects[0] = BREATHE_ON;
}

void start_static(){
  effects[0] = STATIC_ON;
}

rgb_color blend_colors(rgb_color color1, rgb_color color2){
  rgb_color result;
  result.red = (color1.red + color2.red) / 2;
  result.green = (color1.green + color2.green) / 2;
  result.blue = (color1.blue + color2.blue) / 2;
  return result;
}

void do_blend(){
  colors[0] = blend_colors(colors[0], colors[1]);
  colors[1] = colors[0];
}

// only works proper used immediately after placing a standard color
void do_max(){
  colors[0] = scale_color(colors[0], MAX_BRIGHTNESS_PERCENT / 100.0);
}

void do_dim(){
  colors[0].red = colors[0].red >> 1;
  colors[0].green = colors[0].green >> 1;
  colors[0].blue = colors[0].blue >> 1;
}

void do_bright(){
  colors[0].red = colors[0].red << 1;
  colors[0].green = colors[0].green << 1;
  colors[0].blue = colors[0].blue << 1;
}

void fade(float rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) *= rate;
  }
}

void do_fade(float rate = FADE_RATE){
  for(int i = 0; i < FADE_TIMES; i++){
    fade(rate);
    ledStrip.write(colors, LED_COUNT);
    delay(FADE_DELAY);
  }
}

void do_flood(){
  for(int i = 1; i < LED_COUNT; i++){
    colors[i] = colors[0];
    effects[i] = effects[0];    
  }
}

rgb_color random_color(){
  return palette[random(NPRETTY_COLORS)];
}

void do_random(){
  push_color(random_color());
}

void do_mirror(){
  for(int i = 0; i < LED_COUNT / 2; i++){
    colors[LED_COUNT - i - 1] = colors[i];
    effects[LED_COUNT - i - 1] = effects[i];
  }
}

void do_repeat(){
  push_color(colors[0]);
  effects[0] = effects[1];
}

void setup_buffer(){
  for(int i = 0; i < LED_COUNT; i++){
    int effect = effects[i];

    if(effect ==  STATIC_ON) colors[i] = random_color();

    if(effect >= BLINK_MIN && effect <= BLINK_MAX){
      if((effect == BLINK_ON && blink_state) || 
            (effect == BLINK_ON_1 && blink_state_1) || 
            (effect == BLINK_ON_2 && blink_state_2) || 
            (effect == BLINK_ON_3 && blink_state_3) || 
            (effect == BLINK_ON_4 && blink_state_4) || 
            (effect == BLINK_ON_5 && blink_state_5) || 
            (effect == BLINK_ON_6 && blink_state_6) || 
            (effect == BLINK_ON_A && blink_state_a) || 
            (effect == BLINK_ON_B && blink_state_b)) {
          render[i] = scale_color(colors[i], DEFAULT_BRIGHTNESS_SCALE);  
        } else {
          render[i] = scale_color(colors[i], MINIMUM_BRIGHTNESS_SCALE);
        }
    }
    
    else if(effects[i] == BREATHE_ON) render[i] = scale_color(colors[i], DEFAULT_BRIGHTNESS_SCALE * breathe_steps[BREATHE_MAX_STEP - breathe_step]);

    else render[i] = scale_color(colors[i], DEFAULT_BRIGHTNESS_SCALE);
  }
}

void display_buffer(){
  ledStrip.write(render, LED_COUNT);  
}

bool paused = false;

void flush(){
  if(!paused){
    setup_buffer();
    display_buffer();
  }
}

// Compute a random seed by sampling an unconnected analog input pin 
int random_seed(){
  int seed = analogRead(RANDOM_SEED_PIN);
  for(int i = 0; i < 16; i++){
    seed = (seed << 1) ^ analogRead(0);  
  }
  randomSeed(seed);
}

void setup() { 
  //Serial.begin(115200);  // open serial connection to USB Serial 
                         //port(connected to your computer)
  Serial1.begin(57600);  // open internal serial connection to 
                         //MT7688
  //pinMode(13, OUTPUT); // in MT7688, this maps to device 

  random_seed();
  setup_colors(false);
  erase(true);
}

#define MAX_STRING_LENGTH 16

bool is_command(char *str, char *command){
  return strcmp(str, command) == 0;  
}

void loop(){ 
  char str[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  rgb_color color;

  if(Serial1.available() > 0){
    int c = Serial1.readBytesUntil('|', str, MAX_STRING_LENGTH);
    str[c] = 0;
    strcpy(arg, "");

    // reset the effects so the automatic render won't interfere
    blink_state = true;
    blink_state_1 = false;
    blink_state_2 = false;
    blink_state_3 = false;
    blink_state_4 = false;
    blink_state_5 = false;
    blink_state_6 = false;
    blink_state_a = false;
    blink_state_b = false;
    blink_counter = 0;
    breathe_step = 0;
    breathe_counter = 0;
  
    //Serial.println(str);

// fast blink, breathe
// static, flame
// invert hue, swing hue, back and forth between two arbitrary colors, rgb cube
// rainbow colors incremental, or continuously changing
// shooting up
// push-down animation when a new build is seen (to emphasize its adding)
// fade based on build time
// change refresh rate based on time of day (afterhours it doesn't need to check that often)
// show something if the data hasn't updated in a long time
// automatically stop and restart the script at night
// blink a/b
// show something if the data has been unchanged for some time, like after hours
// detect if the group of builds has been added to, then do an insertion animation
// after pause, allow breathing and blinking to come to a halt first
// rotation
// transitions
// if something is not recognized assume it's an argument for the next command

    if     (is_command(str, "pause"))    paused = true;  
    else if(is_command(str, "continue")) paused = false;
    else if(is_command(str, "erase"))    erase(true);
    else if(is_command(str, "blink"))    start_effect(BLINK_ON);
    else if(is_command(str, "blink1"))   start_effect(BLINK_ON_1);
    else if(is_command(str, "blink2"))   start_effect(BLINK_ON_2);
    else if(is_command(str, "blink3"))   start_effect(BLINK_ON_3);
    else if(is_command(str, "blink4"))   start_effect(BLINK_ON_4);
    else if(is_command(str, "blink5"))   start_effect(BLINK_ON_5);
    else if(is_command(str, "blink6"))   start_effect(BLINK_ON_6);
    else if(is_command(str, "blinkr"))   start_blinking_r();
    else if(is_command(str, "blinka"))   start_effect(BLINK_ON_A);
    else if(is_command(str, "blinkb"))   start_effect(BLINK_ON_B);
    else if(is_command(str, "breathe"))  start_effect(BREATHE_ON);
    else if(is_command(str, "flush"))    flush();
    else if(is_command(str, "blend"))    do_blend();
    else if(is_command(str, "max"))      do_max();
    else if(is_command(str, "dim"))      do_dim();
    else if(is_command(str, "bright"))   do_bright();
    else if(is_command(str, "fade"))     do_fade();
    else if(is_command(str, "flood"))    do_flood();
    else if(is_command(str, "random"))   do_random();
    else if(is_command(str, "mirror"))   do_mirror();
    else if(is_command(str, "repeat"))   do_repeat();
    else if(is_command(str, "static"))   start_static();
    else if(is_command(str, "red"))      push_color(red);
    else if(is_command(str, "green"))    push_color(green);
    else if(is_command(str, "blue"))     push_color(blue);
    else if(is_command(str, "black"))    push_color(black);
    else if(is_command(str, "yellow"))   push_color(yellow);
    else if(is_command(str, "orange"))   push_color(orange);
    else if(is_command(str, "purple"))   push_color(purple);
    else if(is_command(str, "cyan"))     push_color(cyan);
    else if(is_command(str, "magenta"))  push_color(magenta);
    else if(is_command(str, "pink"))     push_color(pink);
    else if(is_command(str, "white"))    push_color(white);
    else if(is_command(str, "gray"))     push_color(gray);
    else if(is_command(str, "ltgreen"))  push_color(ltgreen);
    else if(is_command(str, "seafoam"))  push_color(seafoam);
    else if(is_command(str, "ltblue"))   push_color(ltblue);
    else if(is_command(str, "dkgray"))   push_color(dkgray);
    else Serial.println("WTF");
    
  } else {
    bool should_flush = false;
    
    blink_counter = (blink_counter + 1) % MAX_BLINK;
    if(blink_counter == BLINK_0){
      if(blink_state){
        blink_state = false;
      } else {
        blink_state = true;
      }
      should_flush = true;
    }

    if(blink_counter == BLINK_1){
      blink_state_6 = false;
      blink_state_1 = true;
      blink_state_a = true;
      blink_state_b = false;
      should_flush = true;
    }

    if(blink_counter == BLINK_2){
      blink_state_1 = false;
      blink_state_2 = true;
      should_flush = true;
    }

    if(blink_counter == BLINK_3){
      blink_state_2 = false;
      blink_state_3 = true;
      should_flush = true;
    }

    if(blink_counter == BLINK_4){
      blink_state_3 = false;
      blink_state_4 = true;
      blink_state_a = false;
      blink_state_b = true;
      should_flush = true;
    }

    if(blink_counter == BLINK_5){
      blink_state_4 = false;
      blink_state_5 = true;
      should_flush = true;
    }

    if(blink_counter == BLINK_6){
      blink_state_5 = false;
      blink_state_6 = true;
      should_flush = true;
    }

    breathe_counter = (breathe_counter + 1) % BREATHE_TIME;
    if(breathe_counter == 0){
      int next_breathe_step = breathe_step + breathe_direction;
      if(next_breathe_step < 0 || next_breathe_step >= BREATHE_MAX_STEP){
        breathe_direction *= -1;
      }
      breathe_step = breathe_step + breathe_direction;
      should_flush = true;
    }
    
    if(should_flush){
      flush();
    }
  }
}

