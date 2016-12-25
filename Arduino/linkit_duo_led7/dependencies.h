#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#include <PololuLedStrip.h>
#include <random_seed.h>
#include <auto_brightness.h>
#include <color_math.h>
#include <colors.h>
#include <command_processor.h>
#include <power_ease.h>
#ifdef USE_ELASTIC_EASE
#include <elastic_ease.h>
#endif
#include <blink_effects.h>
#include <breathe_effects.h>
#include <effects_processor.h>
#include <buffer.h>
#include <render.h>
#include "config.h"
#include "command_defs.h"
#include "commands.h"

class Dependencies
{
  public:

  // global settings
  static Config config;

  // input display buffer
  static rgb_color colors[config.led_count];
#if defined(STRIP2) || defined(STRIP3)  
  static rgb_color colors2[config.led_count];
#endif
#if defined(STRIP3)  
  static rgb_color colors3[config.led_count];
#endif

  static rgb_color *color_buffers[];

  // rendered output buffer
  static rgb_color render[config.led_count];

  // effect styling per LED position
  static byte effects[config.led_count];
#if defined(STRIP2) || defined(STRIP3)  
  static byte effects2[config.led_count];
#endif
#if defined(STRIP3)  
  static byte effects3[config.led_count];
#endif

  static byte *effects_buffers[];

  // pin-specific LED hardware drivers
  static PololuLedStrip<config.display_pins[0]> ledStrip1;
#if defined(STRIP2) || defined(STRIP3)  
  static PololuLedStrip<config.display_pins[1]> ledStrip2;
#endif
#if defined(STRIP3)  
  static PololuLedStrip<config.display_pins[2]> ledStrip3;
#endif

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

  // show a display to indicate health of the rendering routines on startup
  void self_test();
};

// global settings
Config Dependencies::config;

// input display buffer
rgb_color Dependencies::colors[config.led_count];
#if defined(STRIP2) || defined(STRIP3)  
rgb_color Dependencies::colors2[config.led_count];
#endif
#if defined(STRIP3)  
rgb_color Dependencies::colors3[config.led_count];
#endif

#if defined(STRIP3)
rgb_color *Dependencies::color_buffers[] = { colors, colors2, colors3 };
#elif defined(STRIP2)
rgb_color *Dependencies::color_buffers[] = { colors, colors2 };
#else
rgb_color *Dependencies::color_buffers[] = { colors };
#endif

// rendered output buffer
rgb_color Dependencies::render[config.led_count];

// effect styling per LED position
byte Dependencies::effects[config.led_count];
#if defined(STRIP2) || defined(STRIP3)  
byte Dependencies::effects2[config.led_count];
#endif
#if defined(STRIP3)  
byte Dependencies::effects3[config.led_count];
#endif

#if defined(STRIP3)
byte *Dependencies::effects_buffers[] = { effects, effects2, effects3 };
#elif defined(STRIP2)
byte *Dependencies::effects_buffers[] = { effects, effects2 };
#else
byte *Dependencies::effects_buffers[] = { effects };
#endif

// pin-specific LED hardware drivers
PololuLedStrip<DISPLAY_PIN1> Dependencies::ledStrip1;
#if defined(STRIP2) || defined(STRIP3)  
PololuLedStrip<DISPLAY_PIN2> Dependencies::ledStrip2;
#endif
#if defined(STRIP3)  
PololuLedStrip<DISPLAY_PIN3> Dependencies::ledStrip3;
#endif

// array of drivers for selecting among multiple displays
#if defined(STRIP3)
PololuLedStripBase* Dependencies::ledStrips[config.num_displays] = {&Dependencies::ledStrip1, &Dependencies::ledStrip2, &Dependencies::ledStrip3};
#elif defined(STRIP2)
PololuLedStripBase* Dependencies::ledStrips[config.num_displays] = {&Dependencies::ledStrip1, &Dependencies::ledStrip2};
#else
PololuLedStripBase* Dependencies::ledStrips[config.num_displays] = {&Dependencies::ledStrip1};
#endif

// for generating higher-quality random number seeds
RandomSeed<RANDOM_SEED_PIN> Dependencies::randomizer;

// auto-brightness setting hardware driver
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

//  // establish the default brightness for color scaling
//  ColorMath::set_brightness(DEFAULT_BRIGHTNESS_PERCENT);

  // start up the interface between display buffers and LED strips, passing in config values necessary for rendering, the renderer, the display and render buffers, and effects
  buffer.begin(this->ledStrips, DEFAULT_BRIGHTNESS_PERCENT, FADE_RATE, config.led_count, config.visible_led_count, &this->renderer, color_buffers, render, effects_buffers); //, existence);

  // start up the commands class, passing in dependencies for the buffer interface, renderer and effects processor, values needed for rendering, display and render buffers, and effecrts
  commands.begin(&this->buffer, &this->renderer, &this->effects_processor, config.default_brightness_percent, config.visible_led_count, &this->auto_brightness);

  // set up the blink effects counter and states
  blink_effects.begin();

  // set up the breathe effect counter and state
  breathe_effects.begin();

  // start up the effects processor, passing in the blink and breathe effects instances
  effects_processor.begin(&this->buffer, &this->blink_effects, &this->breathe_effects);

  // start up the renderer, passing in the blink and breathe effects instances, and brightness values needed for rendering
  renderer.begin(&this->blink_effects, &this->breathe_effects, config.default_brightness_percent, config.minimum_brightness_percent);

  // set a higher-quality random seed by reading values from an unconnected analog input
  randomizer.randomize();

#ifdef USE_ELASTIC_EASE
  // generate the cubic ease in/elastic out animation 
  ElasticEase::generate_elastic_ease();
#endif
}

void Dependencies::self_test(){
  for(int i = 0; i < NUM_BUFFERS; i++){
    commands.set_display((NUM_BUFFERS - 1) - i);
    commands.do_demo();
  }
  commands.set_display(0);
}
#endif

