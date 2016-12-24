#ifndef COMMANDS_H
#define COMMANDS_H

#include "config.h"

#define MAX_BRIGHTNESS_PERCENT (default_brightness * 4)
#define DIM_BRIGHTNESS_PERCENT (default_brightness / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (default_brightness * 2)

#define CROSSFADE_DELAY 1

#define LOW_POWER_TIME 50

class Commands
{
  public:
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, rgb_color **color_buffers, rgb_color *render, byte **effect_buffers, AutoBrightnessBase *auto_brightness);
  void pause();
  void resume();
  void do_blend();
  void do_max();
  void do_dim();
  void do_bright();
  void do_fade();
  void do_crossfade();
  void do_wipe();
  void do_exhale_fade();
  void do_flood();
  void do_random(byte type);
  void do_mirror();
  void do_copy(byte width, byte times);
  void do_repeat(byte times);
  void do_elastic_shift(byte count, byte max);
  void do_power_shift(byte count, byte max, bool fast_render);
  void do_power_shift_object(byte width, byte shift, bool fast_render);
  void do_demo();
  void flush(bool force_display);
  void reset();
  void low_power();
  void high_power();
  void set_display(byte display);
  void set_buffer(byte nbuffer);
  void set_pin(byte pin, bool on);
  void set_brightness_level(byte level = 0);

  private:
  Buffer *buffer;
  Render *renderer;  
  EffectsProcessor *effects_processor;

  rgb_color **color_buffers;
  rgb_color *render;
  byte **effects_buffers;
  bool paused = false;
  byte default_brightness;
  byte visible_led_count;
  bool low_power_mode = false;
  byte low_power_position = 0;
  int low_power_timer = 0;
  AutoBrightnessBase *auto_brightness;

  void advance_low_power_position();
};

void Commands::begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, rgb_color **color_buffers, rgb_color *render, byte **effects_buffers, AutoBrightnessBase *auto_brightness){
  this->buffer = buffer;
  this->renderer = renderer;
  this->effects_processor = effects_processor;
  this->color_buffers = color_buffers;
  this->render = render;
  this->effects_buffers = effects_buffers;
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

void Commands::set_display(byte display){
  buffer->set_display(display);
}

void Commands::set_buffer(byte nbuffer){
  buffer->set_buffer(nbuffer);
}

void Commands::set_pin(byte pin, bool on){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, on ? HIGH : LOW);  
}

void Commands::set_brightness_level(byte level){
  if(level == 0){
#if defined(WEARABLE) || defined(DISC93) || defined(STRAND1) || defined(STRAND2)
    level = default_brightness;
#else
    level = auto_brightness->get_auto_brightness_level();
#endif
  }
  ColorMath::set_brightness(level);    
  renderer->set_default_brightness(level);
}

void Commands::do_blend(){
//  colors[0] = ColorMath::blend_colors(colors[0], colors[1]);
//  colors[1] = colors[0];
  buffer->get_buffer()[0] = ColorMath::blend_colors(buffer->get_buffer()[0], buffer->get_buffer()[1]);
  buffer->get_buffer()[1] = buffer->get_buffer()[0];
}

// only works properly when used immediately after placing a standard color
void Commands::do_max(){
  buffer->get_buffer()[0] = ColorMath::scale_color(buffer->get_buffer()[0], MAX_BRIGHTNESS_PERCENT / 100.0);
}

void Commands::do_dim(){
  buffer->get_buffer()[0].red = buffer->get_buffer()[0].red >> 1;
  buffer->get_buffer()[0].green = buffer->get_buffer()[0].green >> 1;
  buffer->get_buffer()[0].blue = buffer->get_buffer()[0].blue >> 1;
}

void Commands::do_bright(){
  buffer->get_buffer()[0].red = buffer->get_buffer()[0].red << 1;
  buffer->get_buffer()[0].green = buffer->get_buffer()[0].green << 1;
  buffer->get_buffer()[0].blue = buffer->get_buffer()[0].blue << 1;
}

void Commands::do_fade(){
  for(int i = 0; i < visible_led_count; i++){
    buffer->get_buffer()[i] = black;
    buffer->get_effects_buffer()[i] = NO_EFFECT;
  }
  do_crossfade();
//  for(int i = 0; i < FADE_TIMES; i++){
//    buffer->fade();
//    buffer->display_buffer(colors);
//    delay(FADE_DELAY);
//  }
//  buffer->erase();
}

void Commands::do_crossfade(){
  for(int i = 0; i <= ColorMath::crossfade_steps(); i++){
    buffer->cross_fade(i);
    buffer->display_buffer();
    delay(CROSSFADE_DELAY);
  }
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
  rgb_color color = buffer->get_buffer()[0];
  byte effect = buffer->get_effects_buffer()[0];
  for(byte i = 1; i < max; i++){
    if(effect == RANDOM1){
      buffer->get_buffer()[i] = ColorMath::random_color();
      buffer->get_effects_buffer()[i] = NO_EFFECT;
    } else if(effect == RANDOM2){
      buffer->get_buffer()[i] = ColorMath::random_color();
      buffer->get_effects_buffer()[i] = EffectsProcessor::random_effect();
    } else {
      buffer->get_buffer()[i] = color;
      buffer->get_effects_buffer()[i] = effect;    
    }
  }
}

void Commands::do_random(byte type){
  type = (type < 1) ? 1 : type;
  buffer->push_color(ColorMath::random_color());
  if(type == 1){
    buffer->get_effects_buffer()[0] = RANDOM1;
  } else if(type == 2){
    buffer->get_effects_buffer()[0] = RANDOM2;
  }
}

// to do: operate on a specific colors buffer
void Commands::do_mirror(){
  for(byte i = 0; i < visible_led_count / 2; i++){
    buffer->get_buffer()[(visible_led_count - 1) - i] = buffer->get_buffer()[i];
    buffer->get_effects_buffer()[(visible_led_count - 1) - i] = buffer->get_effects_buffer()[i];
  }
}

void Commands::do_copy(byte width, byte times){
  if(width < 1){
    width = 1;
  }
  if(times < 1){
    times = (visible_led_count / width) - 1;
  }
  for(int i = 0; i < times; i++){
    for(int j = 0; j < width; j++){
      byte offset = ((i + 1) * width) + j;
      buffer->get_buffer()[offset] = buffer->get_buffer()[j];
      buffer->get_effects_buffer()[offset] = buffer->get_effects_buffer()[j];
    }
  }
}

void Commands::do_repeat(byte times = 1){
  times = (times < 1) ? 1 : times;

  // the stored color has been red/green corrected, so
  // to repeat it, first uncorrect it by swapping
  rgb_color color = ColorMath::correct_color(buffer->get_buffer()[0]);
  
  byte effect = buffer->get_effects_buffer()[0];
  for(byte i = 0; i < times; i++){
    if(effect == RANDOM1){
      buffer->push_color(ColorMath::random_color());
      buffer->get_effects_buffer()[0] = NO_EFFECT;
    } else if(effect == RANDOM2){
      buffer->push_color(ColorMath::random_color());
      buffer->get_effects_buffer()[0] = EffectsProcessor::random_effect();
    } else {
      buffer->push_color(color);
      buffer->get_effects_buffer()[0] = effect;
    }
  }
}

void Commands::do_elastic_shift(byte count, byte max = 0){
#ifdef USE_ELASTIC_EASE  
  max = (max == 0) ? visible_led_count : max;
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(byte i = 0; i < ElasticEase::ease_count(); i++){
      byte pos = ElasticEase::ease[i] * count;
      delay(ElasticEase::ease_delay());
      buffer->shift(pos+1, max);
    }
    buffer->finalize_shift(count, max);
  }
#endif
}

void Commands::do_power_shift(byte count, byte max = 0, bool fast_render = true){
  max = (max == 0) ? visible_led_count : max;
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(byte i = 0; i < PowerEase::ease_count(); i++){
      byte pos = PowerEase::get_ease(i) * count;
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
void Commands::do_power_shift_object(byte width, byte shift, bool fast_render = true){
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
//      renderer->render_buffer_low_power(render, buffer->get_buffer(), visible_led_count, effects, low_power_position);
      renderer->render_buffer_low_power(render, buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer(), low_power_position);
      advance_low_power_position();
    } else {
//      renderer->render_buffer(render, buffer->get_buffer(), visible_led_count, effects);
      renderer->render_buffer(render, buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer());
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
  
  for(byte i = 0; i < count; i++){
    rgb_color color = ColorMath::random_color();
    do_power_shift_object(DEMO_TOTAL_SIZE, window);
    window -= DEMO_TOTAL_SIZE;
    byte effect = EffectsProcessor::random_effect();
    for(byte j = DEMO_GAP_SIZE; j < DEMO_TOTAL_SIZE; j++){
      buffer->set_color(j, color, false, effect);
    }
    delay(DEMO_DELAY);
  }
}

#endif

