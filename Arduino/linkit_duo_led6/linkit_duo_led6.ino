#include <PololuLedStrip.h>
PololuLedStrip<12> ledStrip;

#define RANDOM_SEED_PIN A1
#include "random.h"

#define DEFAULT_BRIGHTNESS_PERCENT 25
#define DIM_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT * 2)
#define MAX_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT * 4)

#include "colors.h"
#include "ease.h"

#define ANIM_LED_COUNT 72
#define LED_COUNT (ANIM_LED_COUNT + EASE_ANIM_MARGIN)
#define MAX_LED (LED_COUNT)

#define DEFAULT_BRIGHTNESS_SCALE (DEFAULT_BRIGHTNESS_PERCENT / 100.0)
#define MINIMUM_BRIGHTNESS_SCALE 0.02

#include "effects.h"
#include "buffer.h"

//float elastic_ease[EASE_COUNT];
//float power_ease[EASE_COUNT];

#include "fade.h"
#include "commands.h"
#include "render.h"

bool pausing = false;
bool paused = false;

void flush(){
  if(!paused){
    render_buffer();
    display_buffer();
  }
}

#include "demo.h"

void reset(){
  pausing = false;
  paused = false;
  window = 0;
  blink_counter = 0;
  blink_state = true;
  blink_state_1 = true; 
  blink_state_2 = true; 
  blink_state_3 = true; 
  blink_state_4 = true; 
  blink_state_5 = true; 
  blink_state_6 = true; 
  blink_state_a = true; 
  blink_state_b = true; 
  breathe_counter = 0;
  breathe_step = 0;
  breathe_direction = 1;
}

void setup() { 
  //Serial.begin(115200);  // open serial connection to USB Serial 
                         //port(connected to your computer)
  Serial1.begin(115200);  // open internal serial connection to 
                         //MT7688
  //pinMode(13, OUTPUT); // in MT7688, this maps to device 

  random_seed();
//  reset();
  setup_colors(false);
  set_brightness(DEFAULT_BRIGHTNESS_PERCENT);
  erase(true);
  generate_elastic_ease(EASE_COUNT, EASE_EXPONENT);
  generate_power_ease(POWER_EASE_COUNT, EASE_EXPONENT);
  do_demo();
}

#define MAX_STRING_LENGTH 100

bool is_command(char *str, char *command){
  return strcmp(str, command) == 0;  
}

#define NUM_SUB_ARGS 3
int sub_args[NUM_SUB_ARGS] = { 0, 0, 0 };

int get_sub_args(char * args){
  char *arg = strtok(args, ",");
  sub_args[0] = atoi(arg);
  arg = strtok(NULL, ",");
  sub_args[1] = atoi(arg);
  arg = strtok(NULL, ",");
  sub_args[2] = atoi(arg);
}

void send_ack(){
  Serial1.write("k");
}

#define DELIMITER_CHAR ':'

void loop(){ 
  char str[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  //char last[MAX_STRING_LENGTH];
  rgb_color color;

  if(Serial1.available() > 0){
    int c = Serial1.readBytesUntil(DELIMITER_CHAR, str, MAX_STRING_LENGTH);
    str[c] = 0;

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

    if(is_command(str, "repeat")){
      int times = sub_args[0];
      if(times < 1) times = 1;
      do_repeat(times);
      strcpy(arg, "");
    }

    if     (is_command(str, "pause"))    paused = true;  
    else if(is_command(str, "pausew"))   pausing = true;  
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
    else if(is_command(str, "effectr"))  start_effect_r();
    else if(is_command(str, "flush"))    flush();
    else if(is_command(str, "blend"))    do_blend();
    else if(is_command(str, "max"))      do_max();
    else if(is_command(str, "dim"))      do_dim();
    else if(is_command(str, "bright"))   do_bright();
    else if(is_command(str, "fade"))     do_fade();
    else if(is_command(str, "efade"))    do_exhale_fade();
    else if(is_command(str, "flood"))    do_flood();
    else if(is_command(str, "random"))   do_random();
    else if(is_command(str, "mirror"))   do_mirror();
    else if(is_command(str, "eshift"))   { do_elastic_shift(sub_args[0]); strcpy(arg, ""); }
    else if(is_command(str, "pshift"))   { do_power_shift(sub_args[0]); strcpy(arg, ""); }
    else if(is_command(str, "pshifto"))  { power_shift_object(sub_args[0], sub_args[1]); strcpy(arg, ""); }
    else if(is_command(str, "window"))   { set_window(sub_args[0]); strcpy(arg, ""); }
    else if(is_command(str, "reset"))    reset();
    else if(is_command(str, "demo"))     do_demo();
    else if(is_command(str, "static"))   start_effect(STATIC_ON); //start_static();
    else if(is_command(str, "rgbcolor")) { push_rgb_color(sub_args[0], sub_args[1], sub_args[2]); strcpy(arg, ""); }
    else if(is_command(str, "hslcolor")) { push_hsl_color(sub_args[0], sub_args[1], sub_args[2]); strcpy(arg, ""); }
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
    else strcpy(arg, str); 
    
    get_sub_args(arg);

    if(Serial1.available() == 0 && !pausing)
      send_ack();
  }
  else 
  {
    bool should_flush = false;
    
    blink_counter = (blink_counter + 1) % MAX_BLINK;
    if(blink_counter == BLINK_1){
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
//        if(next_breathe_step >= BREATHE_MAX_STEP){
        if(next_breathe_step < 0){
          pausing = false;
          send_ack();  
        }
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

