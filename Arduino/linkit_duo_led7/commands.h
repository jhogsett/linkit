#ifndef COMMANDS_H
#define COMMANDS_H

#include "config.h"
#include "zone_defs.h"

#define MAX_BRIGHTNESS_PERCENT (default_brightness * 4)
#define DIM_BRIGHTNESS_PERCENT (default_brightness / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (default_brightness * 2)

#if defined(MINI_DISC_19)
#define CROSSFADE_DELAY 50
#else
#define CROSSFADE_DELAY 1
#endif

#define LOW_POWER_TIME 50

class Commands
{
  public:
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, AutoBrightnessBase *auto_brightness);
  void pause();
  void resume();
  bool is_paused();
  void do_blend(byte strength);
  void do_max();
  void do_dim();
  void do_bright();
  void do_fade();
  void do_crossfade();
  void do_wipe();
  void do_flood();
  void do_random(byte type);
  void do_mirror();
  void do_copy(byte size, byte times, byte zoom);
  void do_repeat(byte times);
  void do_elastic_shift(byte count, byte max);
  void do_power_shift(byte count, byte max, bool fast_render);
  void do_power_shift_object(byte width, byte shift, bool fast_render);
  void do_demo();
  void flush(bool force_display);
  void flush_all(bool force_display = false);
  void reset();
  void low_power();
  void high_power();
  void set_display(byte display);
  void set_buffer(byte nbuffer);
  void set_pin(byte pin, bool on);
  void set_brightness_level(byte level = 0);
  void clear();
  void do_rotate(byte times, byte steps, byte flush);
  void delay(int milliseconds);
  int random_num(int max, int min);
  
  private:
  Buffer *buffer;
  Render *renderer;  
  EffectsProcessor *effects_processor;
  bool paused = false;
  byte default_brightness;
  byte visible_led_count;
  bool low_power_mode = false;
  byte low_power_position = 0;
  int low_power_timer = 0;
  AutoBrightnessBase *auto_brightness;

  void advance_low_power_position();
};

void Commands::begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, AutoBrightnessBase *auto_brightness){
  this->buffer = buffer;
  this->renderer = renderer;
  this->effects_processor = effects_processor;
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

bool Commands::is_paused(){
  return paused;
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

  // reset to the default zone on a display change (? this confused me when working on glasses, maybe not needed)
  buffer->set_zone(0);
}

void Commands::set_pin(byte pin, bool on){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, on ? HIGH : LOW);  
}

void Commands::set_brightness_level(byte level){
  if(level == 0){
#ifdef USE_AUTO_BRIGHTNESS
    level = auto_brightness->get_auto_brightness_level();
#else
    level = default_brightness;
#endif
  }
  renderer->set_default_brightness(level);
  flush_all(true);
}

// strength 1-100, 100 = all color @ offset
// 0 == 50
void Commands::do_blend(byte strength){
  if(strength < 1){
    strength = 50;
  }
  byte offset = buffer->get_offset();
  buffer->get_buffer()[offset] = ColorMath::blend_colors(buffer->get_buffer()[offset], buffer->get_buffer()[offset + 1], strength / 100.0);
  buffer->get_buffer()[offset + 1] = buffer->get_buffer()[offset];
}

// only works properly when used immediately after placing a standard color
void Commands::do_max(){
  byte offset = buffer->get_offset();
  buffer->get_buffer()[offset] = ColorMath::scale_color(buffer->get_buffer()[offset], MAX_BRIGHTNESS_PERCENT / 100.0);
}

void Commands::do_dim(){
  byte offset = buffer->get_offset();
  buffer->get_buffer()[offset].red = buffer->get_buffer()[offset].red >> 1;
  buffer->get_buffer()[offset].green = buffer->get_buffer()[offset].green >> 1;
  buffer->get_buffer()[offset].blue = buffer->get_buffer()[offset].blue >> 1;
}

void Commands::do_bright(){
  byte offset = buffer->get_offset();
  buffer->get_buffer()[offset].red = buffer->get_buffer()[offset].red << 1;
  buffer->get_buffer()[offset].green = buffer->get_buffer()[offset].green << 1;
  buffer->get_buffer()[offset].blue = buffer->get_buffer()[offset].blue << 1;
}

void Commands::do_fade(){
  for(int i = buffer->get_offset(); i < buffer->get_window(); i++){
    buffer->get_buffer()[i] = Buffer::black;
    buffer->get_effects_buffer()[i] = NO_EFFECT;
  }
  do_crossfade();
}

void Commands::do_crossfade(){
  for(int i = 0; i <= ColorMath::crossfade_steps(); i++){
    buffer->cross_fade(i);
    buffer->display_buffer(buffer->get_render_buffer());
    delay(CROSSFADE_DELAY);
  }
}

void Commands::do_flood(){
  int offset = buffer->get_offset();
  rgb_color color = buffer->get_buffer()[offset];
  byte effect = buffer->get_effects_buffer()[offset];
  for(byte i = (offset + 1); i < buffer->get_window(); i++){
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

// types 
// 0: random color with no effect
// 1: like #0 but will flood and repeat with random colors 
// 2: like #1 but will also set random effects
// others: b&w palette, all colors, all colors including blk
void Commands::do_random(byte type){
  type = (type < 0) ? 0 : type;
  buffer->push_color(ColorMath::random_color());
  if(type == 0){
    buffer->get_effects_buffer()[buffer->get_offset()] = RANDOM0;
  } else if(type == 1){
    buffer->get_effects_buffer()[buffer->get_offset()] = RANDOM1;
  } else if(type == 2){
    buffer->get_effects_buffer()[buffer->get_offset()] = RANDOM2;
  }
}

// to do: fix re: reverse
void Commands::do_mirror(){
  byte front = buffer->get_offset();
  byte back = buffer->get_window() - 1;
  byte width = buffer->get_width() / 2;

  for(byte i = 0; i < width; i++){
    buffer->get_buffer()[back - i] = buffer->get_buffer()[front + i];
    buffer->get_effects_buffer()[back - i] = buffer->get_effects_buffer()[front + i];
  }
}

// to do: handle reverse direction
void Commands::do_copy(byte size, byte times, byte zoom){
  size = max(1, size);
  zoom = max(1, zoom);

  int effective_size = size * zoom;
  
  if(times < 1){
    times = (buffer->get_width() / effective_size) - 0; // repeat and fill
  }

  // copy the color pattern to the render buffer temporarily
  for(int i = 0; i < size; i++){
    byte dest = i;
    byte source = buffer->get_offset() + i;
    buffer->get_render_buffer()[dest] = buffer->get_buffer()[source]; 
  }

  // copy the colors
  for(int i = 0; i < times; i++){
    for(int j = 0; j < size; j++){
      for(int k = 0; k < zoom; k++){
        byte source = j;
        byte dest = buffer->get_offset() + (i * effective_size) + (j * zoom) + k;
  
        if(dest < buffer->get_window()){ // prevent buffer overrun
          buffer->get_buffer()[dest] = buffer->get_render_buffer()[source];
        }
      }
    }
  }
  
  // copy the effects pattern to the render buffer temporarily
  for(int i = 0; i < size; i++){
    byte source = buffer->get_offset() + i;
    buffer->get_render_buffer()[i].red = buffer->get_effects_buffer()[source]; 
  }

  // copy the effects
  for(int i = 0; i < times; i++){
    for(int j = 0; j < size; j++){
      for(int k = 0; k < zoom; k++){
        byte source = j;
        byte dest = buffer->get_offset() + (i * effective_size) + (j * zoom) + k;
  
        if(dest < buffer->get_window()){ // prevent buffer overrun
          buffer->get_effects_buffer()[dest] = buffer->get_render_buffer()[source].red;
        }
      }
    }
  }

  // erase the render buffer
  for(int i = 0; i < visible_led_count; i++){
    buffer->get_render_buffer()[i] = BLACK;  
  }
}

// to do fix re: reverse
void Commands::do_repeat(byte times = 1){
  times = max(1, times);
  byte offset = buffer->get_offset();
  byte effect = buffer->get_effects_buffer()[offset];

  // the stored color has been red/green corrected, so
  // to repeat it, first uncorrect it by swapping
  rgb_color color = ColorMath::correct_color(buffer->get_buffer()[offset]);
 
  for(byte i = 0; i < times; i++){
    if(effect == RANDOM0){
      // repeat the same color, no effect
      buffer->push_color(color);
      buffer->get_effects_buffer()[offset] = NO_EFFECT;
    } else if(effect == RANDOM1){
      // changing random color only, no random effect
      buffer->push_color(ColorMath::random_color());
      buffer->get_effects_buffer()[offset] = effect;
    } else if(effect == RANDOM2){
      // changing random color and random effect
      buffer->push_color(ColorMath::random_color());
      buffer->get_effects_buffer()[offset] = EffectsProcessor::random_effect();
    } else {
      buffer->push_color(color);
      buffer->get_effects_buffer()[offset] = effect;
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

// width = number of pixels to include in shift
// shift = number of positions to shift
// note: the two should add up to no more than the visible led count!
void Commands::do_power_shift_object(byte width, byte shift, bool fast_render = true){
  do_power_shift(shift, shift + width, fast_render);
}

void Commands::do_wipe(){
  do_power_shift_object(0, visible_led_count, false);
}

// default to animated rotation to simplify macros
void Commands::do_rotate(byte times, byte steps, byte flush){
  times = max(1, times);
  steps = max(1, steps);
  flush = max(0, min(1, flush));
  for(int i = 0; i < times; i++){
    for(int j = 0; j < steps; j++){
      buffer->rotate(); 
    }
    if(flush == 0){
      this->flush(true);  
    }    
  }
}

void Commands::advance_low_power_position(){
  if(low_power_timer++ % LOW_POWER_TIME == 0){
    low_power_position = ++low_power_position % visible_led_count; 
  }
}

void Commands::flush(bool force_display = false){
  if(force_display || !paused){
    if(low_power_mode){
      renderer->render_buffer_low_power(buffer->get_render_buffer(), buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer(), low_power_position);
      advance_low_power_position();
    } else {
      renderer->render_buffer(buffer->get_render_buffer(), buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer());
    }
    buffer->display_buffer(buffer->get_render_buffer());
  }
}

// todo: force a particular rendering buffer and display 
// instead of setting and unsetting
void Commands::flush_all(bool force_display){
  byte orig_display = buffer->get_current_display();
  
  for(int i = 0; i < NUM_BUFFERS; i++){
    buffer->set_display(i);
    flush(force_display);
  }

  buffer->set_display(orig_display);
}

// reset internal states without pausing
void Commands::reset(){
  // paused = false;
  
  low_power_mode = false;
  
  buffer->reset();
  effects_processor->reset_effects();

  // the brightness shouldn't be set on reset, because the device could be set
  // to an appropriate brightness level already
  //set_brightness_level(default_brightness);
}

// full reset and clear of displays with un-pausing
void Commands::clear(){
  reset();
  
  // pausing on clear causes a problem for programs starting up,
  // that have to pause again afterwards; moved to reset
  paused = false;

  byte orig_display = buffer->get_current_display();
  for(int i = 0; i < NUM_BUFFERS; i++){
    buffer->set_display(i);
    buffer->erase(true);                                                          
  }

  buffer->set_display(orig_display);
}

void Commands::delay(int milliseconds){
  ::delay(milliseconds);
}

#define RANDOM_NUM_ACCUM      -6
#define RANDOM_NUM_DISPLAYS   -5
#define RANDOM_NUM_PALETTES   -4
#define RANDOM_NUM_FINE_ZONES -3
#define RANDOM_NUM_ZONES      -2
#define RANDOM_NUM_LEDS       -1
#define RANDOM_NUM_WIDTH       0

int Commands::random_num(int max, int min){
  // handle special cases
  switch(max){ 
    case RANDOM_NUM_ACCUM:
      // need to add dependency on command processor
      break;
    case RANDOM_NUM_DISPLAYS: 
      max = NUM_DISPLAYS; 
      break;
    
    case RANDOM_NUM_PALETTES: 
      max = NUM_PALETTE_COLORS; 
      break;
    
    case RANDOM_NUM_FINE_ZONES:
      min = FIRST_FINE_ZONE;
      max = FINE_ZONES + 1; 
      break;
    
    case RANDOM_NUM_ZONES: 
      max = NUM_ZONES; 
      break;

    case RANDOM_NUM_LEDS: 
      max = this->visible_led_count; 
      break;
    
    case  RANDOM_NUM_WIDTH: 
      max = buffer->get_width(); 
      break;
  }

  return random(min, max);  
}

void Commands::do_demo(){
  int count;  
  int window;
  int size_;
  int gap_;
  int delay_;
  
#if defined(WEARABLE) || defined(WEARABLE_AND_STRIP) || defined(WEARABLE_AND_GLASSES) || defined(WEARABLE_AND_DISC93)
  if(buffer->get_display() == 1){
    count = 8; 
    window = 8;
    size_ = 1;
    gap_ = 0;
    delay_ = 125;
  } else {
#endif
    
  size_ = DEMO_TOTAL_SIZE;
  gap_ = DEMO_GAP_SIZE;
  delay_ = DEMO_DELAY;
  count = visible_led_count / DEMO_TOTAL_SIZE;  
  window = visible_led_count;
    
#if defined(WEARABLE) || defined(WEARABLE_AND_STRIP) || defined(WEARABLE_AND_GLASSES) || defined(WEARABLE_AND_DISC93)
  }
#endif
  
  for(byte i = 0; i < count; i++){
//#ifdef APOLLO_LIGHTS2
//    rgb_color color = WHITE;
//#else
    rgb_color color = ColorMath::random_color();
//#endif
    do_power_shift_object(size_, window);
    window -= size_;
#ifdef APOLLO_LIGHTS2
    byte effect = NO_EFFECT;
#else
    byte effect = EffectsProcessor::random_effect();
  #endif
    for(byte j = gap_; j < size_; j++){
      buffer->set_color(j, color, false, effect);
    }
    delay(delay_);
  }

#ifdef APOLLO_LIGHTS2
  buffer->push_color(TUNGSTEN);
  do_flood();
#endif
}

#endif

