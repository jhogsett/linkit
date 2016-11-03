#ifndef COMMANDS_H
#define COMMANDS_H

#include <PololuLedStrip.h>
#include <buffer.h>
#include <render.h>
#include <effects_processor.h>
#include <color_math.h>

#define MAX_BRIGHTNESS_PERCENT (default_brightness * 4)
#define DIM_BRIGHTNESS_PERCENT (default_brightness / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (default_brightness * 2)

#define FADE_TIMES 40
#define FADE_DELAY 10

#define DEMO_OBJECT_SIZE 3
#define DEMO_GAP_SIZE 1
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)

class Commands
{
  public:
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, int default_brightness, int visible_led_count, rgb_color *colors, rgb_color *render, int *effects);
  void pause();
  void resume();
  void do_blend();
  void do_max();
  void do_dim();
  void do_bright();
  void do_fade();
  void do_exhale_fade();
  void do_flood();
  void do_random();
  void do_mirror();
  void do_repeat(int times);
  void do_elastic_shift(int count, int max);
  void do_power_shift(int count, int max);
  void do_power_shift_object(int width, int shift);
  void do_demo();
  void flush();
  void reset();

  private:
  Buffer *buffer;
  Render *renderer;  
  EffectsProcessor *effects_processor;

  rgb_color *colors;
  rgb_color *render;
  int *effects;
  bool paused = false;
  int default_brightness;
  int visible_led_count;
};

void Commands::begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, int default_brightness, int visible_led_count, rgb_color *colors, rgb_color *render, int *effects){
  this->buffer = buffer;
  this->renderer = renderer;
  this->effects_processor = effects_processor;
  this->colors = colors;
  this->render = render;
  this->effects = effects;
  this->default_brightness = default_brightness;
  this->visible_led_count = visible_led_count;
}

void Commands::pause(){
  paused = true;
}

void Commands::resume(){
  paused = false;
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
  for(int i = 1; i < visible_led_count; i++){
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

void Commands::do_power_shift(int count, int max = 0){
  max = (max == 0) ? visible_led_count : max;
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < PowerEase::ease_count(); i++){
      int pos = PowerEase::ease[i] * count;
      delay(PowerEase::ease_delay());
      buffer->shift(pos+1, max);
    }
    buffer->finalize_shift(count, max);
  }
}

void Commands::do_power_shift_object(int width, int shift){
  do_power_shift(shift, shift + width);
}

void Commands::flush(){
  if(!paused){
    renderer->render_buffer(render, colors, visible_led_count, effects);
    buffer->display_buffer();
  }
}

void Commands::reset(){
  paused = false;
  buffer->set_window(0);
  effects_processor->reset_effects();
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
  }
}

#endif

