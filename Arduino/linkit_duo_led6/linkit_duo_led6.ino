#define DEFAULT_BRIGHTNESS_PERCENT 25 // standard brightness
#define MINIMUM_BRIGHTNESS_PERCENT 2  // brightness scale for blinking leds in the off state
#define ANIM_LED_COUNT 64             // visible led count
#define EASE_ANIM_MARGIN 10           // safety margin for visual effects that go past the end of the LEDs
#define DATA_OUT_PIN 12               // data out pin for sending color data to the LEDs
#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG
#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting

#include <PololuLedStrip.h>
#include <random_seed.h>
#include <auto_brightness.h>
#include <color_math.h>
#include <colors.h>
#include <command_processor.h>
#include <power_ease.h>
#include <elastic_ease.h>
#include "effects.h"                  /* needs ANIM_LED_COUNT, EASE_ANIM_MARGIN */
#include "buffer.h"                   /* needs ledStrip, DEFAULT_BRIGHTNESS_PERCENT, LED_COUNT, ANIM_LED_COUNT, effects[], ... */
#include "fade.h"                     /* needs ANIM_LED_COUNT */
#include "render.h"
#include "command_defs.h"
#include "commands.h"
#include "demo.h" /* needs ANIM_LED_COUNT and a bunch of other stuff */
#include "dispatch_command.h"

PololuLedStrip<DATA_OUT_PIN> ledStrip;
RandomSeed<RANDOM_SEED_PIN> randomizer;
AutoBrightness<LIGHT_SENSOR_PIN> auto_brightness;
CommandProcessor command_processor;

void setup() { 
  randomizer.randomize();
  ColorMath::setup_colors(false);
  ColorMath::set_brightness(DEFAULT_BRIGHTNESS_PERCENT);
  Serial1.begin(115200); // open internal serial connection to MT7688
  command_processor.begin(&Serial1, commands, NUM_COMMANDS);
  PowerEase::generate_power_ease();
  ElasticEase::generate_elastic_ease();
  // auto_brightnesss.begin(pass in the constant)
  reset();
  erase(true);
  do_demo();
}

void loop(){ 
  if(command_processor.received_command())
  {
    // resync the effects to a blank state to minimize visual artifacts of pausing and restarting
    reset_effects();
    dispatch_command(command_processor.get_command());
    command_processor.acknowledge_command();
  }
  else 
  {
    // process the effects and update the display if needed
    if(process_effects())
      flush();
  }
}

