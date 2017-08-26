#ifndef COMMANDS_H
#define COMMANDS_H

#include <common.h>
#include <macros.h>
#include <scheduler.h>

#include "config.h"
#include "zone_defs.h"
#include "sequencer.h"

#define MAX_BRIGHTNESS_PERCENT (default_brightness * 4)
#define DIM_BRIGHTNESS_PERCENT (default_brightness / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (default_brightness * 2)

#define DEFAULT_MACRO 10

#if defined(MINI_DISC_19)
#define CROSSFADE_DELAY 50
#else
#define CROSSFADE_DELAY 1
#endif


class Commands
{
  public:
  
#ifdef USE_AUTO_BRIGHTNESS
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, AutoBrightnessBase *auto_brightness, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects *fade_effects, Sequencer *sequencer);
#else
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects *fade_effects, Sequencer *sequencer);
#endif

  void process_events();
  void process_commands(char * buffer);
  void process_commands(const __FlashStringHelper * commands);
  void reset();
  void set_brightness_level(byte level = 0);
  bool dispatch_command(int cmd, byte *dispatch_data = NULL);
  void dispatch_color(int cmd);
  void dispatch_effect(int cmd);
  void dispatch_sequence(int cmd);
  void dispatch_color_sequence(int cmd);
  void flush_all(bool force_display = false);
  void run_default_macro();
  static bool dispatch_function(int cmd, byte *dispatch_data = NULL);
  static Scheduler scheduler;

  private:
  void pause(byte type);
  void resume(byte type);
  void pause_effects();
  void pause_schedules();
  void resume_effects();
  void resume_schedules();
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
  void do_copy(byte size, int times, byte zoom);
  void do_repeat(byte times);
#ifdef USE_ELASTIC_SHIFT
  void do_elastic_shift(byte count, byte max);
#endif
#ifdef USE_POWER_SHIFT
  void do_power_shift(byte count, byte max, bool fast_render);
  void do_power_shift_object(byte width, byte shift, bool fast_render);
#endif
  void do_demo();
  void flush(bool force_display);
  void set_display(byte display);
  void set_pin(byte pin, bool on);
  void clear();
  void do_rotate(byte times, byte steps, bool flush);
  void do_delay(int milliseconds);
  int random_num(int max, int min = 0);
  void set_position(int position, int width = 0);
  void random_position(int type);
  void set_fade_rate(int arg0);
  int do_sequence(byte type, int arg0, int arg1, int arg2);
  int do_set_sequence(byte type, int arg0, int arg1, int arg2);
  int do_next_sequence(int arg0, int arg1, int arg2);
  void do_next_window(int arg0, int arg1, int arg2);
//  int do_next_macro(int arg1, int arg2);
  void do_configure(int arg0, int arg1, int arg2);
  bool do_set_macro(byte macro, byte * dispatch_data);
  void do_color_sequence(byte type, int arg0, int arg1, int arg2);
  void do_random_number(int arg0, int arg1, int arg2);
  void do_stop();
  void do_palette(int arg0, int arg1);
  void do_shuffle(int arg0, int arg1, int arg2);
  void set_black_level(int arg0, int arg1, int arg2);

#ifdef TEST_FRAMEWORK
  void do_test(int type, int arg1, int arg2);
  void do_test_inquiry(byte type, int arg2);
  void do_test_macro(byte macro_number);
  void do_test_buffer(byte start, byte count);
  void do_test_effects(byte start, byte count);
  void do_test_render(byte start, byte count);
  void do_test_palette(byte start, byte count);
  void do_test_function(byte type, int arg2);
  void dump_buffer_colors(rgb_color * buffer, byte start, byte count, bool correct_color = true);
#endif

  Buffer *buffer;
  Render *renderer;  
  EffectsProcessor *effects_processor;
  CommandProcessor *command_processor;
  BlinkEffects *blink_effects;
  BreatheEffects *breathe_effects;
  bool effects_paused = false;
  bool schedules_paused = false;
  byte default_brightness;
  byte visible_led_count;
  static Macros macros;
  static Commands * me;
  Sequencer *sequencer;
  FadeEffects *fade_effects;
  
#ifdef USE_AUTO_BRIGHTNESS
  AutoBrightnessBase *auto_brightness;
#endif
};

Macros Commands::macros;
Scheduler Commands::scheduler;
Commands * Commands::me;

#ifdef USE_AUTO_BRIGHTNESS
void Commands::begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, AutoBrightnessBase *auto_brightness, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects *fade_effects, Sequencer *sequencer){
#else
void Commands::begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects *fade_effects, Sequencer *sequencer){
#endif
  this->me = this;
  this->buffer = buffer;
  this->renderer = renderer;
  this->effects_processor = effects_processor;
  this->default_brightness = default_brightness;
  this->visible_led_count = visible_led_count;
  this->command_processor = command_processor;
  this->blink_effects = blink_effects;
  this->breathe_effects = breathe_effects;
  this->sequencer = sequencer;
  this->fade_effects = fade_effects;
#ifdef USE_AUTO_BRIGHTNESS
  this->auto_brightness = auto_brightness;
#endif

  macros.begin(command_processor, &Commands::dispatch_function);
  scheduler.begin(&macros);
}

/////////////////////////////////
// THIS IS THE MAIN EVENT LOOP
/////////////////////////////////
void Commands::process_events(){
  if(command_processor->received_command())
  {
    dispatch_command(command_processor->get_command());
    command_processor->acknowledge_command();

    // resync the effects to a blank state to minimize visual artifacts 
    // of pausing and restarting if there are display changes
    effects_processor->reset_effects();
  }
  else 
  {
    // do schedule processing
    if(!schedules_paused){
      scheduler.process_schedules();
    }

    // process the effects and update the display if needed
    if(!effects_paused){
      if(effects_processor->process_effects())
        flush_all();
    }
  }
}
/////////////////////////////////
/////////////////////////////////

bool Commands::dispatch_function(int cmd, byte *dispatch_data){
  return me->dispatch_command(cmd, dispatch_data);  
}

#define PAUSE_ALL        0
#define PAUSE_EFFECTS    1
#define PAUSE_SCHEDULES  2
#define RESUME_ALL       0
#define RESUME_EFFECTS   1
#define RESUME_SCHEDULES 2

void Commands::pause(byte type = PAUSE_ALL){
  switch(type){
    case PAUSE_ALL:
      pause_effects();
      pause_schedules();
      break;
    case PAUSE_EFFECTS:
      pause_effects();
      break;
    case PAUSE_SCHEDULES:
      pause_schedules();
      break;
  }
}

void Commands::pause_effects(){
  effects_paused = true;
}

void Commands::pause_schedules(){
  schedules_paused = true;
}

void Commands::resume(byte type = RESUME_ALL){
  switch(type){
    case RESUME_ALL:
      resume_effects();
      resume_schedules();
      break;
    case RESUME_EFFECTS:
      resume_effects();
      break;
    case RESUME_SCHEDULES:
      resume_schedules();
      break;
  }
}

void Commands::resume_effects(){
  effects_paused = false;
}

void Commands::resume_schedules(){
  schedules_paused = false;
}

void Commands::set_display(byte display){
#ifndef NO_SETTERS_GETTERS
  buffer->set_display(display);
#else
  buffer->current_display = display;
#endif

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
  rgb_color * buf = buffer->get_buffer();
  byte offset = buffer->get_offset();
  buf[offset] = ColorMath::blend_colors(buf[offset], buf[offset + 1], strength / 100.0);
  buf[offset + 1] = buf[offset];
}

// todo: change these to a single "adj" command
// 0:adj - adjusts to maximum brightness
// 1:adj - doubles brightness
// 2:adj - quadruples brightness
// -1:adj - halves the brightness
// -2:adj - quarters the brightness
// only works properly when used immediately after placing a standard color
void Commands::do_max(){
  byte offset = buffer->get_offset();
  rgb_color * buf = buffer->get_buffer();
  buf[offset] = ColorMath::scale_color(buf[offset], MAX_BRIGHTNESS_PERCENT / 100.0);
}

void Commands::do_dim(){
  byte offset = buffer->get_offset();
  rgb_color * buf = buffer->get_buffer();
  buf[offset].red = buf[offset].red >> 1;
  buf[offset].green = buf[offset].green >> 1;
  buf[offset].blue = buf[offset].blue >> 1;
}

void Commands::do_bright(){
  byte offset = buffer->get_offset();
  rgb_color * buf = buffer->get_buffer();
  buf[offset].red = buf[offset].red << 1;
  buf[offset].green = buf[offset].green << 1;
  buf[offset].blue = buf[offset].blue << 1;
}

void Commands::do_fade(){
  buffer->erase();
  do_crossfade();
}

void Commands::do_crossfade(){
#ifndef NO_SETTERS_GETTERS
  rgb_color * render_buffer = buffer->get_render_buffer();
#else
  rgb_color * render_buffer = buffer->render;
#endif
  byte steps = ColorMath::crossfade_steps();
  for(byte i = 0; i <= steps; i++){
    buffer->cross_fade(i);
    buffer->display_buffer(render_buffer);
    delay(CROSSFADE_DELAY);
  }
}

void Commands::do_flood(){
  byte offset = buffer->get_offset();
  byte window = buffer->get_window();

  // no flooding necessary unless there are two or more pixels
  if((window - offset) < 2)
    return;
  
  rgb_color * buf = buffer->get_buffer();
  byte * effects = buffer->get_effects_buffer();

  byte start;
  byte end_;
  byte source;
  
#ifndef NO_SETTERS_GETTERS
  if(buffer->get_reverse()){
#else
  if(buffer->reverse){
#endif
    source = buffer->get_window() - 1;
    start = offset;
    end_ = window - 1;
  } else {
    source = buffer->get_offset();
    start = offset + 1;
    end_ = window;
  }

  rgb_color color = buf[source];
  byte effect = effects[source];

  buf += start;
  effects += start;

  for(byte i = start; i < end_; i++){
    if(effect == RANDOM1){
      *buf = ColorMath::random_color();
      *effects = NO_EFFECT;
    } else if(effect == RANDOM2){
      *buf = ColorMath::random_color();
      *effects = EffectsProcessor::random_effect();
    } else {
      *buf = color;
      *effects = effect;    
    }
    buf++;
    effects++;
  }
}

#define RANDOM_COLOR_TYPE_SAME_COLOR_REPEAT 0
#define RANDOM_COLOR_TYPE_DIFF_COLOR_REPEAT 1
#define RANDOM_COLOR_TYPE_DIFF_PLUS_EFFECTS 2
#define RANDOM_COLOR_TYPE_PALETTE           3

// types 
// 0: random color with no effect
// 1: like #0 but will flood and repeat with random colors 
// 2: like #1 but will also set random effects
// others: b&w palette, all colors, all colors including blk
void Commands::do_random(byte type){
  type = (type < 0) ? 0 : type;

  byte effect;
  switch(type){
    case RANDOM_COLOR_TYPE_SAME_COLOR_REPEAT: effect = RANDOM0; break;
    case RANDOM_COLOR_TYPE_DIFF_COLOR_REPEAT: effect = RANDOM1; break;
    case RANDOM_COLOR_TYPE_DIFF_PLUS_EFFECTS: effect = RANDOM2; break;

    case RANDOM_COLOR_TYPE_PALETTE: buffer->push_color(Colors::random_palette_color()); return;
  }

  buffer->push_color(ColorMath::random_color());
  buffer->get_effects_buffer()[buffer->get_offset()] = effect;
}

void Commands::do_mirror(){
  byte front = buffer->get_offset();
  byte back = buffer->get_window() - 1;
  byte width = buffer->get_width() / 2;
  rgb_color * buf = buffer->get_buffer();
  byte * effects = buffer->get_effects_buffer();
  
#ifndef NO_SETTERS_GETTERS
  bool reverse = buffer->get_reverse();
#else
  bool reverse = buffer->reverse;
#endif

  for(byte i = 0; i < width; i++){
    if(reverse){
      buf[front + i] = buf[back - i];
      effects[front + i] = effects[back - i];
    } else {
      buf[back - i] = buf[front + i];
      effects[back - i] = effects[front + i];
    }
  }
}

// to do: handle reverse direction
// to do: copy only copies from position zero

// copies pixels starting at the current offset (normally zero)
// size - how many pixels to copy
// times - how many times to duplicate it, 
//         0 = repeat and fill the current window
//        -1 = copy the pattern to the palette buffer but don't duplicate
//        -2 = duplicate the pattern in the palette buffer but don't copy first
// zoom - how many pixels to draw per source pixel
void Commands::do_copy(byte size, int times, byte zoom){
  size = max(1, size);
  zoom = max(1, zoom);

  byte effective_size = size * zoom;

  bool use_palette_buffer = false;
  if(size <= NUM_PALETTE_COLORS)
    // use the palette to store the color/effects data if it will fit
    // this allows the render buffer to be used for crossfading
    use_palette_buffer = true;

  bool copy_only = false;
  bool dupe_only = false;
  if(times == -1){
    // copy the pattern but don't paste it
    copy_only = true;  
  } else if(times == -2){
    // duplicate the pattern but don't copy it first
    dupe_only = true;
    // can't pass in a specific number of times
    // 1 is better than fill because it's less restrictive
    times = 1;
  } else if(times < 1){
    times = (buffer->get_width() / effective_size) + 1; // repeat and fill
  }

  byte offset = buffer->get_offset();
  byte window = buffer->get_window();
  byte * effects = buffer->get_effects_buffer();
  rgb_color * buf = buffer->get_buffer();
#ifndef NO_SETTERS_GETTERS
  rgb_color * render_buffer = buffer->get_render_buffer();
#else
  rgb_color * render_buffer = buffer->render;
#endif

  rgb_color * palette;
  if(use_palette_buffer)
    palette = Colors::get_palette();

  if(!copy_only && !dupe_only){
    // copy the effects pattern to the buffer temporarily
    for(byte i = 0; i < size; i++){
      byte source = offset + i;
      if(use_palette_buffer)
        palette[i].red = effects[source];
      else
        render_buffer[i].red = effects[source]; 
    }

    // TODO DRY
  
    // copy the effects
    for(byte i = 0; i < times; i++){
      for(byte j = 0; j < size; j++){
        for(byte k = 0; k < zoom; k++){
          byte source = j;
          byte dest = offset + (i * effective_size) + (j * zoom) + k;
  
          if(dest < window){ // prevent buffer overrun
            if(use_palette_buffer)
              effects[dest] = palette[source].red;
            else
              effects[dest] = render_buffer[source].red;
          }
        }
      }
    }
  }

  // copy the color pattern to the buffer temporarily
  if(!dupe_only){
    for(byte i = 0; i < size; i++){
      byte source = offset + i;
      if(use_palette_buffer)
        // uncorrect corrected color so it works in the palette for push_color()
        palette[i] = ColorMath::correct_color(buf[source]);
      else
        render_buffer[i] = buf[source]; 
    }
  }
  
  if(!copy_only){
    // copy the colors
    for(byte i = 0; i < times; i++){
      for(byte j = 0; j < size; j++){
        for(byte k = 0; k < zoom; k++){
          byte source = j;
          byte dest = offset + (i * effective_size) + (j * zoom) + k;
    
          if(dest < window){ // prevent buffer overrun
            if(use_palette_buffer)
              // correct the uncorrected color in the palette
              buf[dest] = ColorMath::correct_color(palette[source]);
            else
              buf[dest] = render_buffer[source];
          }
        }
      }
    }
  }

  // is this necessary?
  // erase the render buffer
//  if(!use_palette_buffer){
//    for(byte i = 0; i < visible_led_count; i++){
//      render_buffer[i] = buffer->black;  
//    }
//  }
}

// to do fix re: reverse (fixed?)
// consider support a repeat of zero times (doing nothing)
void Commands::do_repeat(byte times = 1){
  times = max(1, times);

  byte offset;
  
#ifndef NO_SETTERS_GETTERS
  if(buffer->get_reverse()){
#else
  if(buffer->reverse){
#endif
    offset = buffer->get_window() - 1;
  } else {
    offset = buffer->get_offset();
  }
  
  byte effect = buffer->get_effects_buffer()[offset];
  switch(effect){
    case RANDOM0:
      {
        // repeat the same color, no effect
        rgb_color color = ColorMath::correct_color(buffer->get_buffer()[offset]);
        buffer->push_color(color, times, false, NO_EFFECT);
      }
      break;
    case RANDOM1:
      {
        // changing random color only, no random effect
        for(byte i = 0; i < times; i++){
          buffer->push_color(ColorMath::random_color(), 1, false, effect);
        }
      }
      break;
    case RANDOM2:
      {
        // changing random color and random effect
        for(byte i = 0; i < times; i++){
          buffer->push_color(ColorMath::random_color(), 1, false, EffectsProcessor::random_effect());
        }
      }
      break;
    default:
      {
        rgb_color color = ColorMath::correct_color(buffer->get_buffer()[offset]);
        buffer->push_color(color, times, false, effect);
      }
      break;
  }
}

// void Buffer::push_color(rgb_color color, byte times = 1, bool display = false, byte effect = NO_EFFECT, byte max = 0, byte start = 0)

//void Commands::do_elastic_shift(byte count, byte max = 0){
//#ifdef USE_ELASTIC_EASE  
//  max = (max == 0) ? visible_led_count : max;
//  count = count == 0 ? 1 : count;
//  if(count >= 1){
//    for(byte i = 0; i < ElasticEase::ease_count(); i++){
//      byte pos = ElasticEase::ease[i] * count;
//      delay(ElasticEase::ease_delay());
//      buffer->shift(pos+1, max);
//    }
//    buffer->finalize_shift(count, max);
//  }
//#endif
//}

#ifdef USE_POWER_EASE
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
#endif

void Commands::do_wipe(){
#ifdef USE_POWER_EASE
  do_power_shift_object(0, visible_led_count, false);
#endif
}

// arg[0] # times to rotate, default = 1
// arg[1] # rotation steps each time, default = 1
// arg[2] 0=flush each time, 1=don't flush, default = 0
void Commands::do_rotate(byte times, byte steps, bool flush){
  times = max(1, times);
  steps = max(1, steps);
  for(byte i = 0; i < times; i++){
    for(byte j = 0; j < steps; j++){
      buffer->rotate(); 
    }
    if(flush){
      this->flush(true);  
    }    
  }
}

void Commands::flush(bool force_display = false){
  if(force_display || !effects_paused){
#ifndef NO_SETTERS_GETTERS
    rgb_color * render_buffer = buffer->get_render_buffer();
#else
    rgb_color * render_buffer = buffer->render;
#endif
    renderer->render_buffer(render_buffer, buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer());
    buffer->display_buffer(render_buffer);
  }
}

// todo: force a particular rendering buffer and display 
// instead of setting and unsetting
void Commands::flush_all(bool force_display){
#ifndef NO_SETTERS_GETTERS
  byte orig_display = buffer->get_current_display();
#else
  byte orig_display = buffer->current_display;
#endif
  
  for(byte i = 0; i < NUM_BUFFERS; i++){
  
#ifndef NO_SETTERS_GETTERS
    buffer->set_display(i);
#else
    buffer->current_display = i;
#endif

    flush(force_display);
  }

#ifndef NO_SETTERS_GETTERS
  buffer->set_display(orig_display);
#else
  buffer->current_display = orig_display;
#endif
}

// reset internal states without pausing
void Commands::reset(){
  // paused = false;
  
  buffer->reset();

  // the effects shouldn't be reset
  // otherwise when reset is used in macros
  // if can interfere with the effects
  // effects_processor->reset_effects();

  // the brightness shouldn't be set on reset, because the device could be set
  // to an appropriate brightness level already
  //set_brightness_level(default_brightness);

  // TODO maybe clear command processor arguments here too, to help with passing args into macros
}

// full reset and clear of displays with un-pausing
void Commands::clear(){
  reset();
  
  // pausing on clear causes a problem for programs starting up,
  // that have to pause again afterwards; moved to reset
  resume();

#ifndef NO_SETTERS_GETTERS
  byte orig_display = buffer->get_current_display();
#else
  byte orig_display = buffer->current_display;
#endif
  
  for(byte i = 0; i < NUM_BUFFERS; i++){
#ifndef NO_SETTERS_GETTERS
    buffer->set_display(i);
#else
    buffer->current_display = i;
#endif
    buffer->reset_black_level();
    buffer->erase(true);                                                          
  }

#ifndef NO_SETTERS_GETTERS
  buffer->set_display(orig_display);
#else
  buffer->current_display = orig_display;
#endif
}

void Commands::do_delay(int milliseconds){
  ::delay(milliseconds);
}

// arg0 - 10000 = 1.0, 9999 = 0.9999
// arg0 <= zero, reset to default
void Commands::set_fade_rate(int arg0){
  if(arg0 < 1){
    fade_effects->set_fade_rate(FADE_RATE);
  } else {
    fade_effects->set_fade_rate(arg0/ 10000.0);
  }
}

//  @ 508 era:1:rev:3:pos:tun:flo:flu -0,0,0,0,0,0,0,0,0,20,11,2,0,0,0 +0,0,0,0,0,0,20,11,2,0,0,0,0,0,0
      
// arg[0] index of insertion pointer, default = 0
// arg[1] width of window, default = 1
//        -- any magic widths?
void Commands::set_position(int position, int width){
  byte current_offset = buffer->get_offset();
  byte current_window = buffer->get_window();

  // offset into zone space
  position = current_offset + position;

  // don't go outside of zone
  position = min(current_window, max(current_offset, position));
    
  buffer->set_offset_override(position); 

  // set the window at the specified width
  byte window = position + width;
  window = min(current_window, window);
  
  buffer->set_window_override(window); 

//  position = position + current_offset;
//  
//  // don't go outside of zone
//  position = min(current_window, max(current_offset, position));
//    
//  buffer->set_window_override(position); 
}

#define RANDOM_NUM_WIDTH_NOT_EMPTY -2
#define RANDOM_NUM_WIDTH_EMPTY     -1
#define RANDOM_NUM_WIDTH            0

// todo: position should be in relation to the current zone
// todo: a position of zero here when a zone is selected means zero in zone #0
void Commands::random_position(int type){
  byte position;
  switch(type){
    case RANDOM_NUM_WIDTH_NOT_EMPTY:
      position = random_num(RANDOM_NUM_WIDTH_NOT_EMPTY);
      break;
    
    case RANDOM_NUM_WIDTH_EMPTY:
      position = random_num(RANDOM_NUM_WIDTH_EMPTY);
      break;
    
    case RANDOM_NUM_WIDTH:
      position = random_num(RANDOM_NUM_WIDTH);
      break; 
  }
  
  set_position(position);
}

//#define RANDOM_NUM_ACCUM           -8
//#define RANDOM_NUM_ZONES           -7
//#define RANDOM_NUM_LEDS            -6
//#define RANDOM_NUM_DISPLAYS        -5

#define RANDOM_NUM_PALCOLORS       -4
#define RANDOM_NUM_FINE_ZONES      -3

int Commands::random_num(int max, int min){
  // handle special cases
  bool non_empty_only = false;
  bool empty_only = false;
  switch(max){ 
//    case RANDOM_NUM_ACCUM:
//      // need to add dependency on command processor
//      break;
//
//    case RANDOM_NUM_ZONES: 
//      max = NUM_ZONES; 
//      break;
//
//    case RANDOM_NUM_LEDS: 
//      max = this->visible_led_count; 
//      break;
//    
//    case RANDOM_NUM_DISPLAYS: 
//      max = NUM_DISPLAYS; 
//      break;
    
    case RANDOM_NUM_PALCOLORS: 
      max = NUM_PALETTE_COLORS; 
      break;
    
    case RANDOM_NUM_FINE_ZONES:
      min = FIRST_FINE_ZONE;
      max = FINE_ZONES + 1; 
      break;
    
    case RANDOM_NUM_WIDTH_NOT_EMPTY: 
      non_empty_only = true;
      max = buffer->get_width(); 
      break;

    case RANDOM_NUM_WIDTH_EMPTY:
      empty_only = true;
      max = buffer->get_width(); 
      break;

    case  RANDOM_NUM_WIDTH: 
      max = buffer->get_width(); 
      break;
  }

  // TODO DRY

  if(empty_only){
    byte protection = 0;
    rgb_color * buf = buffer->get_buffer();
    byte width = buffer->get_width();
    do{
      byte pos = random(min, max); 
      if(ColorMath::equal(buf[pos], buffer->black)){
        return pos;
      }
    }while(protection++ <= width);
    return 0;
  } else if(non_empty_only){
    byte protection = 0;
    rgb_color * buf = buffer->get_buffer();
    byte width = buffer->get_width();
    do{
      byte pos = random(min, max); 
      if(!ColorMath::equal(buf[pos], buffer->black)){
        return pos;
      }
    }while(protection++ <= width);
    return 0;
  } else {
    return random(min, max);  
  }
}

// shortcut: once a special command is set to establish type, 'seq' can be used to read the values regardless of type
//
// need to be able to toggle the direction of a wheel sequencer
// 
// setting
// arg0 - sequence number, 0-9, default = 0
// arg1 - high limit, default = 10 for 0-9, must be >0 for 'setting' mode
// arg2 - low limit, default = 0 for 0-9
// 1,10,0:seq - set sequence #1 to 0 - 9, reset to 0
// 1,10,2:seq - set sequence #1 to 2 - 9, reset to 2
// note: leaves arg0 set to the sequence low value
//       to clear, use an argument of "0"
//
// getting
// arg0 - sequence number, 0-5, default = 0
// arg1 - advancement, must be <= 0 for 'getting' mode 
// arg1 -1 = get current number without advancing
// arg1 -2 = get opposite of current number without advancing (for range 0-9 and current number 4, this would be 5)
// arg1 -3 = reset current number to low limit and return it without advancing
// arg1 -4 = step (arg2) instead is a macro # to run for each position, filling gaps

// arg1 -5 = set new upper limit
// arg1 -6 = set new lower limit

// arg2 - step, default = 1
// 1,0,0:seq - get next number from #1
// 1:seq - get next number from #1
// seq - get next number from #0
// 
// seq - get next number from sequence #0
// 1:-1:seq - get current number from #1
// 1:-2:seq - get opposite of current number from #1 (for range 0-9 and current number 4, this would be 5)
// 1:-3:seq - reset #1 to low and return it
// 1:0:2:seq - get next number from #1, stepping by 2
// 1:0:-1:seq - get next number from #1, stepping by -1 (THIS DOESN'T SEEM TO WORK)

int Commands::do_sequence(byte type, int arg0, int arg1, int arg2){
  
  if(arg1 > 0){
    //                           num   high  low
    return do_set_sequence(type, arg0, arg1, arg2);
  } else {
    //                      num   adv   step
    return do_next_sequence(arg0, arg1, arg2);
  }
}

// returns the low value to begin the sequece
int Commands::do_set_sequence(byte type, int arg0, int arg1, int arg2){
  // values are entered in high,low order, so low can be skipped if zero
  // note the reverse argument order compared to do_next_sequence()
  //                         low   high
  sequencer->set(arg0, type, arg2, arg1);
  return arg2;
}

int Commands::do_next_sequence(int arg0, int arg1, int arg2){
  //                     num   adv   step
  return sequencer->next(arg0, arg1, arg2);
}

// advance the sequencer, then leave arg0 = position, arg1 = width
// for the position command to allow filling gaps
void Commands::do_next_window(int arg0, int arg1, int arg2){
  byte position = sequencer->next(arg0, arg1, arg2);
  byte previous_position = sequencer->previous_computed(arg0);
  sequencer->set_previous_computed(arg0, position);
  command_processor->sub_args[2] = 0;

  if(position == previous_position){      // short-circuit - no change in position
    command_processor->sub_args[0] = position;
    command_processor->sub_args[1] = 0;
    return;
  }

  byte offset, width;
  if(position > previous_position){       // going up by at least one
    width = position - previous_position; // width needing drawing
    if(width > 0){
      offset = previous_position;         // there's a gap, start right after the last position
    } else {
      offset = position;                  // there's no movement
      width = 0;
    }
  } 
  else if(position < previous_position){  // going down
    width = previous_position - position; // width needing drawing
    if(width > 0){                        // there's a gap, new position is beginning of range
      offset = position;                  // and width is already computed, though it might need a - 1
    }
    else {
      offset = position;                  // there's no movement
      width = 0;
    }
  } 

  command_processor->sub_args[0] = offset;
  command_processor->sub_args[1] = width;
  return;
}

//int Commands::do_next_macro(int arg1, int arg2){
//  
//}

#define CONFIG_SETBLINKC   0
#define CONFIG_SETBLINKP   1
#define CONFIG_SETBREATHET 2
#define CONFIG_SETFADERATE 3

// arg0 setting to configure
// arg1 value to set
void Commands::do_configure(int arg0, int arg1, int arg2){
  switch(arg0){
    case CONFIG_SETBLINKC:
      blink_effects->set_custom_blink(arg1);
      break;
    case CONFIG_SETBLINKP:
      blink_effects->set_blink_period(arg1);
      break;
    case CONFIG_SETBREATHET:
      breathe_effects->set_breathe_time(arg1);
      break;
    case CONFIG_SETFADERATE:
      set_fade_rate(arg1);
      break;
  }
}

// process the series of unpacked commands and arguments in the passed memory buffer
// the string must be tokenizable by strtok (get's corrupted)
void Commands::process_commands(char * buffer){
  if(buffer == NULL || *buffer == '\0'){
    return;
  }

  // point to the end of the buffer for overrun protection
  // this points at the terminator
  char *last_char = buffer + strlen(buffer);

  // begin_get_commands() and get_next_command() need an external pointer
  // to hold onto the strtok_r state
  // this allows this function to be reentrant
  char *saveptr;

  // get the first command or set of arguments
  char *command = command_processor->begin_get_commands(buffer, &saveptr);

  // point to the remaining string after this command + terminator
  // this is needed for copying strings when setting macros
  // if this is the last command in the string, the location 
  //   1 past the string + terminator overruns the end of the buffer
  byte * rest_of_buffer = (byte*)min(command + strlen(command) + 1, last_char);

  // process the command or arguments
  int cmd = command_processor->process_command(command);

  if(cmd == CMD_NULL){
    // there was no command or arguments
    return;
  }

  do{
    // CMD_NONE is returned when there are arguments instead of a command
    // arguments are not dispatched they're captured in CommandProcessor::process_command()
    if(cmd != CMD_NONE){
      // send the command to the dispatcher to be run
      // pass the pointer to the rest of the buffer 
      //   in case it's needed to set a macro
      if(!dispatch_command(cmd, rest_of_buffer)){
        // false means the rest of the buffer has been copied 
        // so there are no more commands to process 
        return;
      }
    }

    // get the next set command or argumemts
    command = command_processor->get_next_command(&saveptr);
    rest_of_buffer = (byte*)min(command + strlen(command) + 1, last_char);
    cmd = command_processor->process_command(command);
    
  }while(cmd != CMD_NULL);
}

// process commands stored in PROGMEM 
void Commands::process_commands(const __FlashStringHelper * commands){
  char * buffer = command_processor->borrow_char_buffer();
  strcpy_P(buffer, (const char *)commands);
  process_commands(buffer);
}

bool Commands::do_set_macro(byte macro, byte * dispatch_data){
  bool continue_dispatching = true;
  if(dispatch_data != NULL){
    // being used internally
    macros.set_macro(macro, (char*)dispatch_data);          

    // signal that no more commands should be processed (rest of buffer copied to macro)
    continue_dispatching = false;
  } else {
    // being used over serial
    byte num_bytes = macros.set_macro_from_serial(macro);
    command_processor->send_int(num_bytes);
  }
  return continue_dispatching;
}

#define COLOR_SEQUENCE_HUE 0
#define COLOR_SEQUENCE_LIT 2
#define COLOR_SEQUENCE_SAT 1

// to do need to add some limiting to not go over 255, etc.

void Commands::do_color_sequence(byte type, int arg0, int arg1, int arg2){
  switch(type){
    case COLOR_SEQUENCE_HUE:
      // arg0 - starting hue angle 0-359, default = 0 (red)
      // arg1 - step angle, default = 20
      //   if arg1 < 0, the order is reverse
      // arg2 - lightness, default = 20
      // (saturation = 255)
      {
        if(arg1 == 0) arg1 = 20;
        if(arg2 < 1) arg2 = 20;

        int angle = arg0;
        rgb_color *palette = Colors::get_palette();
        for(byte i = 0; i < NUM_PALETTE_COLORS; i++){
          palette[i] = ColorMath::hsl_to_rgb(angle % 360, 255, arg2);
          angle += arg1;
        }
      }
      break;

    case COLOR_SEQUENCE_SAT:
      // arg0 - hue angle 0-359, default = 0 (red)
      // arg1 - step, default = 256 / 18 = 14.22222222 (magic value, others must be integers)
      // arg2 - lightness 0-255, default = 20
      {
        if(arg1 == 0) arg1 = 1422; else arg1 *= 100;
        if(arg2 < 1) arg2 = 20;

        rgb_color *palette = Colors::get_palette();
        int saturation = 0;
        for(byte i = 0; i < NUM_PALETTE_COLORS; i++){         
          // the saturation goes from richest to whitest
          palette[i] = ColorMath::hsl_to_rgb(arg0, 255 - (saturation / 100), arg2);
          saturation = (saturation + arg1) % 25600;
        }
      }
      break;

    case COLOR_SEQUENCE_LIT:
      // arg0 - hue angle 0-359, default = 0 (red)
      // arg1 - step, default = 256 / 18 = 14.22222222 (magic value, others must be integers)
      //   (future) if arg0 if < 0, the order is reversed
      // arg2 - lightness scaling, default = 20 (brightness divisor)
      // (starting percent = 0)
      {
        if(arg1 == 0) arg1 = 1422; else arg1 *= 100;
        if(arg2 < 1) arg2 = 20;

        rgb_color *palette = Colors::get_palette();

        // skip ahead past the black color
        int lightness = arg1;
        
        for(byte i = 0; i < NUM_PALETTE_COLORS; i++){
          palette[i] = ColorMath::hsl_to_rgb(arg0, 255, lightness / 100);
          palette[i].red = palette[i].red * arg2 / 256;
          palette[i].green = palette[i].green * arg2 / 256;
          palette[i].blue = palette[i].blue * arg2 / 256;
          lightness = (lightness + arg1) % 25600;
        }
      }
      break;
  }
}

// arg[0] maximum random number (see Commands::random_num() for special constant values)
// arg[1] minimum random number (default=0)
// arg[2] copied into arg[1] to allow passing another argument
void Commands::do_random_number(int arg0, int arg1, int arg2){
  int ran = random_num(arg0, arg1);
  command_processor->sub_args[0] = ran;
  command_processor->sub_args[1] = arg2;
  command_processor->sub_args[2] = 0;
}

void Commands::do_stop(){
  scheduler.reset_all_schedules();
  clear();                                                            
  pause();                                                            
 }

// TODO: it's inefficient to insert the colors one at a time; would be better to shift the buffer then overwrite the colors
// arg[0] the index into the palette of the color to insert, or where to stop rubberstamp insert
// arg[1] if > 0, colors are inserted counting down from this position
//                the counting down is done so the palette achieves a left-to-right order when inserted  
//                in this case arg[0] is the stopping point when counting down
// for example: a rainbow is 0,5:pal, whole palette: 0,17:pal
void Commands::do_palette(int arg0, int arg1){
  rgb_color * palette = Colors::get_palette();
  if(arg1 > 0){
    arg0 = max(0, arg0);
    for(int i = arg1; i >= arg0; i--){
      buffer->push_color(palette[i]);                      
    }
  } else {
    buffer->push_color(palette[arg0]);                                                      
  }
}

#define SHUFFLE_RANDOM           0
#define SHUFFLE_RESET            1
#define SHUFFLE_COMPLIMENT_PAIRS 2
#define SHUFFLE_RAND_COMP_PAIRS  3
#define SHUFFLE_COMPLIMENT       4
#define SHUFFLE_ROTATE_DOWN      5
#define SHUFFLE_ROTATE_UP        6
#define SHUFFLE_REVERSE          7

// arg0 - type of shuffle operation
// arg1 - type-specific argument
// arg2 - type-specific argument
void Commands::do_shuffle(int arg0, int arg1, int arg2){
  switch(arg0)
  {
    case SHUFFLE_RANDOM:
      // create a palette of random colors
      Colors::shuffle_palette();
      break;

    case SHUFFLE_RESET:
      // reset palette to original built-in colors
      Colors::reset_palette();  
      break;

    case SHUFFLE_COMPLIMENT_PAIRS:
      // make every odd color the complimentary color of the previous even color
      Colors::compliment_pairs();
      break;

    case SHUFFLE_RAND_COMP_PAIRS:
      // create a palette of random complimentary color pairs
      Colors::random_compliment_pairs();        
       break;

    case SHUFFLE_COMPLIMENT:
      Colors::compliment_palette();        
      break;
      
    case SHUFFLE_ROTATE_DOWN:
      Colors::rotate_palette(arg1, arg2, true);
      break;
      
    case SHUFFLE_ROTATE_UP:
      Colors::rotate_palette(arg1, arg2, false); 
      break;
      
    case SHUFFLE_REVERSE:
      Colors::reverse_palette();
      break;
  }            
}

void Commands::set_black_level(int arg0, int arg1, int arg2){
  rgb_color black_level = {(byte)arg0, (byte)arg1, (byte)arg2};
  buffer->set_black_level(black_level);
}

#include "dispatch_command.h"

#include "testing.h"

#endif

