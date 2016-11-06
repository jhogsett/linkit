#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

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
#include "command_defs.h"
#include "commands.h"
#include "config.h"

class Dependencies
{
  public:
  static Config config;
  static rgb_color colors[config.led_count];
  static rgb_color render[config.led_count];
  static int effects[config.led_count];
  static PololuLedStrip<DATA_OUT_PIN> ledStrip;
  static RandomSeed<RANDOM_SEED_PIN> randomizer;
  static AutoBrightness<LIGHT_SENSOR_PIN> auto_brightness;
  static CommandProcessor command_processor;
  static BlinkEffects blink_effects;
  static BreatheEffects breathe_effects;
  static EffectsProcessor effects_processor;
  static Buffer buffer;
  static Render renderer;
  static Commands commands;
  void begin();
};

rgb_color Dependencies::colors[config.led_count];
rgb_color Dependencies::render[config.led_count];
int Dependencies::effects[config.led_count];
Config Dependencies::config;
PololuLedStrip<DATA_OUT_PIN> Dependencies::ledStrip;
RandomSeed<RANDOM_SEED_PIN> Dependencies::randomizer;
AutoBrightness<LIGHT_SENSOR_PIN> Dependencies::auto_brightness;
CommandProcessor Dependencies::command_processor;
BlinkEffects Dependencies::blink_effects;
BreatheEffects Dependencies::breathe_effects;
EffectsProcessor Dependencies::effects_processor;
Buffer Dependencies::buffer;
Render Dependencies::renderer;
Commands Dependencies::commands;

void Dependencies::begin(){
  Serial1.begin(BAUD_RATE); // open internal serial connection to MT7688
  auto_brightness.begin();
  command_processor.begin(&Serial1, command_strings, NUM_COMMANDS);
  ColorMath::begin(false);
  buffer.begin(&this->ledStrip, DEFAULT_BRIGHTNESS_PERCENT, FADE_RATE, config.led_count, config.visible_led_count, &this->renderer, colors, render, effects); //, existence);
  commands.begin(&this->buffer, &this->renderer, &this->effects_processor, config.default_brightness_percent, config.visible_led_count, colors, render, effects);
  blink_effects.begin();
  breathe_effects.begin();
  effects_processor.begin(effects, &this->blink_effects, &this->breathe_effects);
  renderer.begin(&this->blink_effects, &this->breathe_effects, config.default_brightness_percent, config.minimum_brightness_percent);
  randomizer.randomize();
  ColorMath::set_brightness(DEFAULT_BRIGHTNESS_PERCENT);
  PowerEase::generate_power_ease();
  ElasticEase::generate_elastic_ease();
}
#endif

