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

  // global settings
  static Config config;

  // input display buffer
  static rgb_color colors[config.led_count];

  // rendered output buffer
  static rgb_color render[config.led_count];

  // effect styling per LED position
  static byte effects[config.led_count];

  // pin-specific LED hardware drivers
  static PololuLedStrip<config.display_pins[0]> ledStrip1;
  static PololuLedStrip<config.display_pins[1]> ledStrip2;
  static PololuLedStrip<config.display_pins[2]> ledStrip3;

  // array of drivers for selecting among multiple displays
  static PololuLedStripBase* ledStrips[config.num_displays];

  // for generating higher-quality random number seeds
  static RandomSeed<RANDOM_SEED_PIN> randomizer;

  // auto-brightness setting hardware driver
  static AutoBrightness<LIGHT_SENSOR_PIN> auto_brightness;

  // processes incoming commands
  static CommandProcessor command_processor;

  // live visual effects
  static BlinkEffects blink_effects;
  static BreatheEffects breathe_effects;

  // manages live visual effects
  static EffectsProcessor effects_processor;

  // interface between in-memory LED buffers and LED strips
  static Buffer buffer;
  
  // renders from the display buffer to the render buffer according to colors and effects
  static Render renderer;
  
  // all the work is done in the commands class
  static Commands commands;

  // initialize all dependencies
  void begin();
};

// global settings
Config Dependencies::config;

// input display buffer
rgb_color Dependencies::colors[config.led_count];

// rendered output buffer
rgb_color Dependencies::render[config.led_count];

// effect styling per LED position
byte Dependencies::effects[config.led_count];

// pin-specific LED hardware drivers
PololuLedStrip<DISPLAY_PIN1> Dependencies::ledStrip1;
PololuLedStrip<DISPLAY_PIN2> Dependencies::ledStrip2;
PololuLedStrip<DISPLAY_PIN3> Dependencies::ledStrip3;

// array of drivers for selecting among multiple displays
PololuLedStripBase* Dependencies::ledStrips[config.num_displays] = {&Dependencies::ledStrip1, &Dependencies::ledStrip2, &Dependencies::ledStrip3};

// for generating higher-quality random number seeds
RandomSeed<RANDOM_SEED_PIN> Dependencies::randomizer;

// auto-brightness setting hardware driver
// (this is here for incubation, but auto-brightness is not in use)
AutoBrightness<LIGHT_SENSOR_PIN> Dependencies::auto_brightness;

// processes incoming commands
CommandProcessor Dependencies::command_processor;

// live visual effects
BlinkEffects Dependencies::blink_effects;
BreatheEffects Dependencies::breathe_effects;

// manages live visual effects
EffectsProcessor Dependencies::effects_processor;

// interface between in-memory LED buffers and LED strips
Buffer Dependencies::buffer;

// renders from the display buffer to the render buffer according to colors and effects
Render Dependencies::renderer;

// all the work is done in the commands class
Commands Dependencies::commands;

void Dependencies::begin(){

  // open internal serial connection to MT7688 for receiving commands
  Serial1.begin(BAUD_RATE); 

  // start up auto-brightness hardware driver
  auto_brightness.begin(AUTO_BRIGHTNESS_MIN, AUTO_BRIGHTNESS_MAX);

  // start up command processor, listening on the serial port and looking for the passed commands
  command_processor.begin(&Serial1, command_strings, NUM_COMMANDS);

  // start up the color math class
  // false = don't swap red & green
#if defined(STRAND1) || defined(STRAND2)
  ColorMath::begin(true);
#else  
  ColorMath::begin(false);
#endif

  // establish the default brightness for color scaling
  ColorMath::set_brightness(DEFAULT_BRIGHTNESS_PERCENT);

  // start up the interface between display buffers and LED strips, passing in config values necessary for rendering, the renderer, the display and render buffers, and effects
  buffer.begin(this->ledStrips, DEFAULT_BRIGHTNESS_PERCENT, FADE_RATE, config.led_count, config.visible_led_count, &this->renderer, colors, render, effects); //, existence);

  // start up the commands class, passing in dependencies for the buffer interface, renderer and effects processor, values needed for rendering, display and render buffers, and effecrts
  commands.begin(&this->buffer, &this->renderer, &this->effects_processor, config.default_brightness_percent, config.visible_led_count, colors, render, effects, &this->auto_brightness);

  // set up the blink effects counter and states
  blink_effects.begin();

  // set up the breathe effect counter and state
  breathe_effects.begin();

  // start up the effects processor, passing in the blink and breathe effects instances
  effects_processor.begin(effects, &this->blink_effects, &this->breathe_effects);

  // start up the renderer, passing in the blink and breathe effects instances, and brightness values needed for rendering
  renderer.begin(&this->blink_effects, &this->breathe_effects, config.default_brightness_percent, config.minimum_brightness_percent);

  // set a higher-quality random seed by reading values from an unconnected analog input
  randomizer.randomize();

  // generate the cubic each in/ease out animation
  PowerEase::generate_power_ease();

  // generate the cubic ease in/elastic out animation 
  ElasticEase::generate_elastic_ease();
}
#endif

