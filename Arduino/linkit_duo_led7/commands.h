#ifndef COMMANDS_H
#define COMMANDS_H

#include "config.h"

#define MAX_BRIGHTNESS_PERCENT (default_brightness * 4)
#define DIM_BRIGHTNESS_PERCENT (default_brightness / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (default_brightness * 2)

#define FADE_TIMES 40
#define FADE_DELAY 10

#define LOW_POWER_TIME 50

class Commands
{
  public:
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, int default_brightness, int visible_led_count, rgb_color *colors, rgb_color *render, char *effects, AutoBrightnessBase *auto_brightness);
  void pause();
  void resume();
  void do_blend();
  void do_max();
  void do_dim();
  void do_bright();
  void do_fade();
  void do_wipe();
  void do_exhale_fade();
  void do_flood();
  void do_random();
  void do_mirror();
  void do_repeat(int times);
  void do_elastic_shift(int count, int max);
  void do_power_shift(int count, int max, bool fast_render);
  void do_power_shift_object(int width, int shift, bool fast_render);
  void do_demo();
  void flush(bool force_display);
  void reset();
  void low_power();
  void high_power();
  void set_display(int display);
  void set_pin(int pin, bool on);
  void set_brightness_level(int level = 0);

  private:
  Buffer *buffer;
  Render *renderer;  
  EffectsProcessor *effects_processor;

  rgb_color *colors;
  rgb_color *render;
  char *effects;
  bool paused = false;
  int default_brightness;
  int visible_led_count;
  bool low_power_mode = false;
  int low_power_position = 0;
  int low_power_timer = 0;
  AutoBrightnessBase *auto_brightness;

  void advance_low_power_position();
};

void Commands::begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, int default_brightness, int visible_led_count, rgb_color *colors, rgb_color *render, char *effects, AutoBrightnessBase *auto_brightness){
  this->buffer = buffer;
  this->renderer = renderer;
  this->effects_processor = effects_processor;
  this->colors = colors;
  this->render = render;
  this->effects = effects;
  this->default_brightness = default_brightness;
  this->visible_led_count = visible_led_count;
  this->auto_brightness = auto_brightness;
}

void Commands::pause(){
  paused = true;
}

void Commands::resume(){
  paused = false;
}

void Commands::low_power(){
  low_power_mode = true;

  // it's too jarring to reset these
  // low_power_position = 0;
  // low_power_timer = 0;
}

void Commands::high_power(){
  low_power_mode = false;
}

void Commands::set_display(int display){
  buffer->set_display(display);
}

void Commands::set_pin(int pin, bool on){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, on ? HIGH : LOW);  
}

void Commands::set_brightness_level(int level){
  if(level == 0){
#ifdef WEARABLE
    level = default_brightness;
#else
    level = auto_brightness->get_auto_brightness_level();
#endif
  }
  ColorMath::set_brightness(level);    
  renderer->set_default_brightness(level);
}

void Commands::do_blend(){
  colors[0] = ColorMath::blend_colors(colors[0], colors[1]);
  colors[1] = colors[0];
}

// only works properly when used immediately after placing a standard color
void Commands::do_max(){
  colors[0] = ColorMath::scale_color(colors[0], MAX_BRIGHTNESS_PERCENT / 100.0);
}

void Commands::do_dim(){
  colors[0].red = colors[0].red >> 1;
  colors[0].green = colors[0].green >> 1;
  colors[0].blue = colors[0].blue >> 1;
}

void Commands::do_bright(){
  colors[0].red = colors[0].red << 1;
  colors[0].green = colors[0].green << 1;
  colors[0].blue = colors[0].blue << 1;
}

void Commands::do_fade(){
  for(int i = 0; i < FADE_TIMES; i++){
    buffer->fade();
    buffer->display_buffer(colors);
    delay(FADE_DELAY);
  }
  buffer->erase();
}

void Commands::do_exhale_fade(){
//  for(int i = 0; i < BREATHE_MAX_STEP; i++){
//    exhale_fade(i);
//    buffer->display_buffer(colors);
//    delay(FADE_DELAY);
//  }
//  buffer->erase();
}

void Commands::do_flood(){
  int max = min(visible_led_count, buffer->get_window());
  for(int i = 1; i < max; i++){
    if(effects[0] == RANDOM){
      colors[i] = ColorMath::random_color();
    } else {
      colors[i] = colors[0];
    }
    effects[i] = effects[0];    
  }
}

void Commands::do_random(){
  buffer->push_color(ColorMath::random_color());
  effects[0] = RANDOM;
}

void Commands::do_mirror(){
  for(int i = 0; i < visible_led_count / 2; i++){
    colors[(visible_led_count - 1) - i] = colors[i];
    effects[(visible_led_count - 1) - i] = effects[i];
  }
}

void Commands::do_repeat(int times = 1){
  times = (times < 1) ? 1 : times;
  for(int i = 0; i < times; i++){
    if(effects[0] == RANDOM){
      buffer->push_color(ColorMath::random_color());
    } else {
      buffer->push_color(colors[0]);
    }
    effects[0] = effects[1];
  }
}

void Commands::do_elastic_shift(int count, int max = 0){
  max = (max == 0) ? visible_led_count : max;
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < ElasticEase::ease_count(); i++){
      int pos = ElasticEase::ease[i] * count;
      delay(ElasticEase::ease_delay());
      buffer->shift(pos+1, max);
    }
    buffer->finalize_shift(count, max);
  }
}

void Commands::do_power_shift(int count, int max = 0, bool fast_render = true){
  max = (max == 0) ? visible_led_count : max;
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < PowerEase::ease_count(); i++){
      int pos = PowerEase::ease[i] * count;
      delay(PowerEase::ease_delay());
      buffer->shift(pos+1, max, fast_render);
    }
    buffer->finalize_shift(count, max);
  }
}

// width = width from the origin the shift will occur within, 
//         including the new space shifted into
// shift = how far to move
// the two should add up to no more than the visible led count
void Commands::do_power_shift_object(int width, int shift, bool fast_render = true){
  do_power_shift(shift, shift + width, fast_render);
}

void Commands::do_wipe(){
  do_power_shift_object(0, visible_led_count, false);
}

void Commands::advance_low_power_position(){
  if(low_power_timer++ % LOW_POWER_TIME == 0){
    low_power_position = ++low_power_position % visible_led_count;    
  }
}

void Commands::flush(bool force_display = false){
  if(force_display || !paused){
    if(low_power_mode){
      renderer->render_buffer_low_power(render, colors, visible_led_count, effects, low_power_position);
      advance_low_power_position();
    } else {
      renderer->render_buffer(render, colors, visible_led_count, effects);
    }
    buffer->display_buffer();
  }
}

void Commands::reset(){
  paused = false;
  low_power_mode = false;
  buffer->set_window(visible_led_count);
  buffer->set_display(0);
  effects_processor->reset_effects();
  set_brightness_level(default_brightness);
}

void Commands::do_demo(){
  int count = visible_led_count / DEMO_TOTAL_SIZE;  
  int window = visible_led_count;
  
  for(int i = 0; i < count; i++){
    rgb_color color = ColorMath::random_color();
    do_power_shift_object(DEMO_TOTAL_SIZE, window);
    window -= DEMO_TOTAL_SIZE;
    int effect = EffectsProcessor::random_effect();
    for(int j = DEMO_GAP_SIZE; j < DEMO_TOTAL_SIZE; j++){
      buffer->set_color(j, color, false, effect);
    }
    delay(DEMO_DELAY);
  }
}

#endif

