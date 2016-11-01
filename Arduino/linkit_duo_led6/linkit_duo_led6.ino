#include <PololuLedStrip.h>
#include <random_seed.h>
#include <auto_brightness.h>
#include <color_math.h>
#include <colors.h>
#include <command_processor.h>
#include <power_ease.h>
#include <elastic_ease.h>
#include <blink_effects.h>
#include <breathe_effects.h>
#include <effects_processor.h>
#include <buffer.h>
#include <render.h>
#include "config.h"
#include "fade.h"
#include "command_defs.h"
#include "commands.h"
#include "demo.h"
#include "dispatch_command.h"

rgb_color colors[LED_COUNT];
rgb_color render[LED_COUNT];
int effects[LED_COUNT];
//int existence[LED_COUNT];

PololuLedStrip<DATA_OUT_PIN> ledStrip;
RandomSeed<RANDOM_SEED_PIN> randomizer;
AutoBrightness<LIGHT_SENSOR_PIN> auto_brightness;
CommandProcessor command_processor;
BlinkEffects blink_effects;
BreatheEffects breathe_effects;
EffectsProcessor effects_processor;
Buffer buffer;
Render renderer;

void setup() { 
  auto_brightness.begin();
  Serial1.begin(115200); // open internal serial connection to MT7688
  command_processor.begin(&Serial1, commands, NUM_COMMANDS);
  ColorMath::begin(false);
  buffer.begin(&ledStrip, DEFAULT_BRIGHTNESS_PERCENT, LED_COUNT, ANIM_LED_COUNT, &renderer, colors, render, effects); //, existence);
  blink_effects.begin();
  breathe_effects.begin();
  effects_processor.begin(effects, &blink_effects, &breathe_effects);
  renderer.begin(&blink_effects, &breathe_effects, DEFAULT_BRIGHTNESS_PERCENT, MINIMUM_BRIGHTNESS_PERCENT);

  randomizer.randomize();
  ColorMath::set_brightness(DEFAULT_BRIGHTNESS_PERCENT);
  PowerEase::generate_power_ease();
  ElasticEase::generate_elastic_ease();

  reset();
  buffer.erase(true);
  do_demo();
}

void loop(){ 
  if(command_processor.received_command())
  {
    // resync the effects to a blank state to minimize visual artifacts of pausing and restarting
    effects_processor.reset_effects();
    dispatch_command(command_processor.get_command());
    command_processor.acknowledge_command();
  }
  else 
  {
    // process the effects and update the display if needed
    if(effects_processor.process_effects())
      flush();
  }
}

