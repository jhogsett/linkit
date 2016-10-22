/*******************************************************************/

// visible led count
#define ANIM_LED_COUNT 64

// standard brightness
#define DEFAULT_BRIGHTNESS_PERCENT 25

// brightness scale for blinking leds in the off state
#define MINIMUM_BRIGHTNESS_SCALE 0.02

/*******************************************************************/

#include <PololuLedStrip.h>


#include "colors.h"
#include "ease.h"
#include "effects.h"
#include "buffer.h"
#include "fade.h"
#include "render.h"
#include "commands.h"
#include "demo.h"
#include "command_processor.h"

#include "random.h"
RandomSeed<RANDOM_SEED_PIN> randomizer;

void setup() { 
  //Serial.begin(115200);   // open serial connection to USB Serial port(connected to your computer)
  Serial1.begin(115200);  // open internal serial connection to MT7688
  //pinMode(13, OUTPUT);    // in MT7688, this maps to device 

  //random_seed();
  randomizer.randomize();
  reset();
  setup_colors(false);
  set_brightness(DEFAULT_BRIGHTNESS_PERCENT);
  erase(true);
  //generate_elastic_ease(EASE_COUNT, EASE_EXPONENT);
  generate_power_ease(POWER_EASE_COUNT, EASE_EXPONENT);
  do_demo();
}

void loop(){ 
  rgb_color color;

  if(received_command()){
    reset_effects();
    dispatch_command(get_command(str));

    if(Serial1.available() == 0) // only acknowledge when there are no more commands available
      send_ack();
  }
  else 
  {
    if(process_blinking()){
      flush();
    }
  }
}


