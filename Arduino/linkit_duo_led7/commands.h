#ifndef COMMANDS_H
#define COMMANDS_H

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
  void do_elastic_shift(byte count, byte max);
  void do_power_shift(byte count, byte max, bool fast_render);
  void do_power_shift_object(byte width, byte shift, bool fast_render);
  void do_demo();
  void flush(bool force_display);
  void low_power();
  void high_power();
  void set_display(byte display);
  void set_buffer(byte nbuffer);
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
  int do_next_window(int arg0, int *arg1, int arg2);
//  int do_next_macro(int arg1, int arg2);
  void do_configure(int arg0, int arg1, int arg2);
  bool do_set_macro(byte macro, byte * dispatch_data);

//      {
//        if(dispatch_data != NULL){
//          // being used internally
//          macros.set_macro(arg0, (char*)dispatch_data);          
//
//          // signal that no more commands should be processed (rest of buffer copied to macro)
//          continue_dispatching = false;
//        } else {
//          // being used over serial
//          byte num_bytes = macros.set_macro_from_serial(arg0);
//          command_processor->send_int(num_bytes);
//        }
//      }

#ifdef TEST_FRAMEWORK
  void do_test(int type, int arg1, int arg2);
  void do_test_inquiry(byte type, int arg2);
  void do_test_macro(byte macro_number);
  void do_test_buffer(byte start, byte count);
  void do_test_effects(byte start, byte count);
  void do_test_render(byte start, byte count);
  void do_test_palette(byte start, byte count);
  void do_test_function(byte type, int arg2);
  void dump_buffer_colors(rgb_color * buffer, byte start, byte count);
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
  rgb_color * buf = buffer->get_buffer();
  byte offset = buffer->get_offset();
  buf[offset] = ColorMath::blend_colors(buf[offset], buf[offset + 1], strength / 100.0);
  buf[offset + 1] = buf[offset];
}

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
  for(byte i = buffer->get_offset(); i < buffer->get_window(); i++){
    buffer->get_buffer()[i] = buffer->black;
    buffer->get_effects_buffer()[i] = NO_EFFECT;
  }
  do_crossfade();
}

void Commands::do_crossfade(){
  rgb_color * render_buffer = buffer->get_render_buffer();
  for(byte i = 0; i <= ColorMath::crossfade_steps(); i++){
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

  rgb_color color = buf[offset];
  byte effect = effects[offset];

  for(byte i = offset + 1; i < window; i++){
    if(effect == RANDOM1){
      buf[i] = ColorMath::random_color();
      effects[i] = NO_EFFECT;
    } else if(effect == RANDOM2){
      buf[i] = ColorMath::random_color();
      effects[i] = EffectsProcessor::random_effect();
    } else {
      buf[i] = color;
      effects[i] = effect;    
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
  byte * effect = &buffer->get_effects_buffer()[buffer->get_offset()];
  if(type == 0){
    *effect = RANDOM0;
  } else if(type == 1){
    *effect = RANDOM1;
  } else if(type == 2){
    *effect = RANDOM2;
  }
}

void Commands::do_mirror(){
  byte front = buffer->get_offset();
  byte back = buffer->get_window() - 1;
  byte width = buffer->get_width() / 2;
  rgb_color * buf = buffer->get_buffer();
  byte * effects = buffer->get_effects_buffer();
  bool reverse = buffer->get_reverse();
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
  rgb_color * render_buffer = buffer->get_render_buffer();

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
  byte offset = buffer->get_offset();
  byte effect = buffer->get_effects_buffer()[offset];

  // the stored color has been red/green corrected, so
  // to repeat it, first uncorrect it by swapping
  rgb_color color = ColorMath::correct_color(buffer->get_buffer()[offset]);
 
  for(byte i = 0; i < times; i++){
    if(effect == RANDOM0){
      // repeat the same color, no effect
      buffer->push_color(color, 1, NO_EFFECT);
    } else if(effect == RANDOM1){
      // changing random color only, no random effect
      buffer->push_color(ColorMath::random_color(), 1, effect);
    } else if(effect == RANDOM2){
      // changing random color and random effect
      buffer->push_color(ColorMath::random_color(), 1, EffectsProcessor::random_effect());
    } else {
      buffer->push_color(color, 1, effect);
    }
  }
}

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
    rgb_color * render_buffer = buffer->get_render_buffer();
    renderer->render_buffer(render_buffer, buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer());
    buffer->display_buffer(render_buffer);
  }
}

// todo: force a particular rendering buffer and display 
// instead of setting and unsetting
void Commands::flush_all(bool force_display){
  byte orig_display = buffer->get_current_display();
  
  for(byte i = 0; i < NUM_BUFFERS; i++){
    buffer->set_display(i);
    flush(force_display);
  }

  buffer->set_display(orig_display);
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

  byte orig_display = buffer->get_current_display();
  for(byte i = 0; i < NUM_BUFFERS; i++){
    buffer->set_display(i);
    buffer->reset_black_level();
    buffer->erase(true);                                                          
  }

  buffer->set_display(orig_display);
}

void Commands::do_delay(int milliseconds){
  ::delay(milliseconds);
}

// arg0 - 10000 = 1.0, 9999 = 0.9999
void Commands::set_fade_rate(int arg0){
  fade_effects->set_fade_rate(arg0/ 10000.0);
}

// arg[0] index of insertion pointer, default = 0
// arg[1] width of window, default = 1
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
//  buffer->set_offset_override(position); 
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
// setting
// arg0 - sequence number, 0-5, default = 0
// arg1 - high limit, default = 10 for 0-9, must be >0 for 'setting' mode
// arg2 - low limit, default = 0 for 0-9
// 1,10,0:seq - set sequence #1 to 0 - 9, reset to 0
// 1,10,2:seq - set sequence #1 to 2 - 9, reset to 2
//
// getting
// arg0 - sequence number, 0-5, default = 0
// arg1 - advancement, must be <= 0 for 'getting' mode 
// arg1 -1 = get current number without advancing
// arg1 -2 = get opposite of current number without advancing (for range 0-9 and current number 4, this would be 5)
// arg1 -3 = reset current number to low limit and return it without advancing
// arg1 -4 = step (arg2) instead is a macro # to run for each position, filling gaps
// 1,0,0:seq - get next number from #1
// 1:seq - get next number from #1
// seq - get next number from #0
// 
// seq - get next number from sequence #0
// 1:-1:seq - get current number from #1
// 1:-2:seq - get opposite of current number from #1 (for range 0-9 and current number 4, this would be 5)
// 1:-3:seq - reset #1 to low and return it
// 1:0:2:seq - get next number from #1, stepping by 2
// 1:0:-1:seq - get next number from #1, stepping by -1

int Commands::do_sequence(byte type, int arg0, int arg1, int arg2){
  
  if(arg1 > 0){
    //                           num   high  low
    return do_set_sequence(type, arg0, arg1, arg2);
  } else {
    //                      num   adv   step
    return do_next_sequence(arg0, arg1, arg2);
  }
}

int Commands::do_set_sequence(byte type, int arg0, int arg1, int arg2){
  // values are entered in high,low order, so low can be skipped if zero
  // note the reverse argument order compared to do_next_sequence()
  //                         low   high
  sequencer->set(arg0, type, arg2, arg1);
  return 0;
}

int Commands::do_next_sequence(int arg0, int arg1, int arg2){
  //                     num   adv   step
  return sequencer->next(arg0, arg1, arg2);
}

// advance the sequencer, then leave arg0 = position, arg1 = width
// for the position command to allow filling gaps
int Commands::do_next_window(int arg0, int *arg1, int arg2){
  byte position = sequencer->next(arg0, *arg1, arg2);
  byte previous_position = sequencer->previous_computed(arg0);
  sequencer->set_previous_computed(arg0, position);

  if(position == previous_position){      // short-circuit - no change in position
    *arg1 = 0;
    return position;
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
//  else {
//    offset = position;                    // no change in position
//    width = 0;
//  }

  *arg1 = width;
  return offset;
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


#include "dispatch_command.h"

#include "testing.h"

#endif

