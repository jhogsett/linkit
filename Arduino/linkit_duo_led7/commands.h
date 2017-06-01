#ifndef COMMANDS_H
#define COMMANDS_H

#include <avr/eeprom.h>

#include "config.h"
#include "zone_defs.h"
#include "macros.h"

#define MAX_BRIGHTNESS_PERCENT (default_brightness * 4)
#define DIM_BRIGHTNESS_PERCENT (default_brightness / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (default_brightness * 2)

//#define MACRO_END_MARKER 0xff
//
//#define MACRO_ARG1_MARKER 0xf9 
//#define MACRO_ARG2_MARKER 0xfa 
//#define MACRO_ARG3_MARKER 0xfb 
//#define MACRO_ARG4_MARKER 0xfc 
//#define MACRO_ARG5_MARKER 0xfd 
//#define MACRO_ARG6_MARKER 0xfe 
//#define ARG_MARKER_FIRST  MACRO_ARG1_MARKER
//#define ARG_MARKER_LAST   MACRO_ARG6_MARKER
//
//// this one doesn't need to be stored in macros, but it could be useful if supporting receiving binary commands
//#define MACRO_CMD_MARKER 0xf0
//
//#define MAX_MEMORY_MACRO (NUM_MEMORY_MACROS - 1)
//#define MAX_EEPROM_MACRO (EEPROM_STARTING_MACRO + NUM_EEPROM_MACROS - 1)
//#define MAX_MACRO (NUM_MACROS - 1)
//
//#define DEFAULT_ERASE_BYTE 0xff

#define DEFAULT_MACRO 10

#define NUM_SCHEDULES NUM_MACROS

#if defined(MINI_DISC_19)
#define CROSSFADE_DELAY 50
#else
#define CROSSFADE_DELAY 1
#endif


class Commands
{
  public:
//  typedef bool (Commands::*DispatchFunction)(int cmd, byte *dispatch_data);
  
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, AutoBrightnessBase *auto_brightness, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects);
//  bool is_macro_programmed(byte macro);
  void process_commands(char * buffer);
  void process_commands(const __FlashStringHelper * commands);
  void reset();
  void set_brightness_level(byte level = 0);
  void reset_all_schedules();
  bool dispatch_command(int cmd, byte *dispatch_data = NULL);
  bool is_paused();
  void process_schedules();
  void flush_all(bool force_display = false);
  void run_default_macro();

  static bool dispatch_function(int cmd, byte *dispatch_data = NULL);
  static Commands * me;
  
  private:
  void pause();
  void resume();
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
  void do_rotate(byte times, byte steps, byte flush);
  void do_delay(int milliseconds);
  int random_num(int max, int min = 0);
  void set_position(int position);
  void random_position(int type);

//  byte set_macro(byte macro, char * commands);
//  byte set_macro_from_serial(byte macro);
//  void run_macro(byte macro, int times = 1, int delay_ = 0);
//  bool is_memory_macro(byte macro);
//  bool is_eeprom_macro(byte macro);
//  byte * get_memory_macro(byte macro);
//  byte * get_eeprom_macro(byte macro);
//  byte num_bytes_from_arg_marker(byte arg_marker);
//  byte num_words_from_arg_marker(byte arg_marker);
//  void set_memory_macro_from_memory(byte macro, byte * buffer);
//  void set_eeprom_macro_from_memory(byte macro, byte * buffer);
//  void set_memory_macro_from_eeprom(byte macro, byte * buffer);
//  void set_eeprom_macro_from_eeprom(byte macro, byte * buffer);
//  void reset_macro(byte macro);
//  void reset_all_macros();
//  void determine_arg_marker(byte &arg_marker, byte &num_args);
//  byte set_memory_macro(byte macro, char * commands);
//  byte set_eeprom_macro(byte macro, char * commands);
//  void run_memory_macro(byte macro, int times);
//  void run_eeprom_macro(byte macro, int times);
  
  void reset_schedule(byte schedule_number);
  void set_schedule(unsigned int schedule_period_, byte schedule_number, byte macro_number_);

  Buffer *buffer;
  Render *renderer;  
  EffectsProcessor *effects_processor;
  CommandProcessor *command_processor;
  BlinkEffects *blink_effects;
  BreatheEffects *breathe_effects;
  bool paused = false;
  byte default_brightness;
  byte visible_led_count;
  AutoBrightnessBase *auto_brightness;
//  static byte macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];
  static unsigned int schedule_period[NUM_SCHEDULES];  // zero means the schedule is turned off
  static byte macro_number[NUM_SCHEDULES];             // could leave this off and assume schedule is same as macro to run, 
  static unsigned int schedule_counter[NUM_SCHEDULES]; // but the ability to switch schedules to run different macros is the basis for toggling
//  DispatchFunction dispatch_function;

  static Macros macros;
};

void Commands::begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, AutoBrightnessBase *auto_brightness, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects){
  this->me = this;
  this->buffer = buffer;
  this->renderer = renderer;
  this->effects_processor = effects_processor;
  this->default_brightness = default_brightness;
  this->visible_led_count = visible_led_count;
  this->auto_brightness = auto_brightness;
  this->command_processor = command_processor;
  this->blink_effects = blink_effects;
  this->breathe_effects = breathe_effects;

  //DispatchFunction dispatch_func = &Commands::dispatch_function;

  macros.begin(command_processor, &Commands::dispatch_function);
}

Commands * Commands::me;

bool Commands::dispatch_function(int cmd, byte *dispatch_data){
  return me->dispatch_command(cmd, dispatch_data);  
}

Macros Commands::macros;

void Commands::pause(){
  paused = true;
}

void Commands::resume(){
  paused = false;
}

bool Commands::is_paused(){
  return paused;
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
//      buffer->get_effects_buffer()[offset] = NO_EFFECT;
    } else if(effect == RANDOM1){
      // changing random color only, no random effect
      buffer->push_color(ColorMath::random_color(), 1, effect);
//      buffer->get_effects_buffer()[offset] = effect;
    } else if(effect == RANDOM2){
      // changing random color and random effect
      buffer->push_color(ColorMath::random_color(), 1, EffectsProcessor::random_effect());
//      buffer->get_effects_buffer()[offset] = EffectsProcessor::random_effect();
    } else {
      buffer->push_color(color, 1, effect);
//      buffer->get_effects_buffer()[offset] = effect;
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

// default to animated rotation to simplify macros
void Commands::do_rotate(byte times, byte steps, byte flush){
  times = max(1, times);
  steps = max(1, steps);
  flush = max(0, min(1, flush));
  for(byte i = 0; i < times; i++){
    for(byte j = 0; j < steps; j++){
      buffer->rotate(); 
    }
    if(flush == 0){
      this->flush(true);  
    }    
  }
}

//#ifdef USE_LOWER_POWER_MODE
//void Commands::advance_low_power_position(){
//  if(low_power_timer++ % LOW_POWER_TIME == 0){
//    low_power_position = ++low_power_position % visible_led_count; 
//  }
//}
//#endif

void Commands::flush(bool force_display = false){
  if(force_display || !paused){
//#ifdef USE_LOWER_POWER_MODE
//    if(low_power_mode){
//      renderer->render_buffer_low_power(buffer->get_render_buffer(), buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer(), low_power_position);
//      advance_low_power_position();
//    } else {
//#endif
      renderer->render_buffer(buffer->get_render_buffer(), buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer());
//#ifdef USE_LOWER_POWER_MODE
//    }
//#endif
    buffer->display_buffer(buffer->get_render_buffer());
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
  
//#ifdef USE_LOWER_POWER_MODE
//  low_power_mode = false;
//#endif

  buffer->reset();

  // the effects shouldn't be reset
  // otherwise when reset is used in macros
  // if can interfere with the effects
  // effects_processor->reset_effects();

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

#define SET_POSITION_ZONE_START  -1
#define SET_POSITION_ZONE_END    -2
#define SET_POSITION_ZONE_CENTER -3
#define SET_POSITION_NEXT        -4
#define SET_POSITION_PREV        -5

void Commands::set_position(int position){
  byte current_offset = buffer->get_offset();
  byte current_window = buffer->get_window();

  switch(position){
    case SET_POSITION_ZONE_CENTER:
      position = (current_offset + current_window) / 2;
      break;

    case SET_POSITION_ZONE_END:
      position = current_window;
      break;

    case SET_POSITION_ZONE_START:
      position = current_offset;
      break;

    default:
      // offset into zone space
      position = position + current_offset;
  
      // don't go outside of zone
      position = min(current_window, max(current_offset, position));
      break;
  }
    
  buffer->set_offset_override(position); 
  buffer->set_window_override(position); 
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

bool Commands::dispatch_command(int cmd, byte *dispatch_data){
  bool continue_dispatching = true;
  bool reset_args = false;
  
  switch(cmd){
    case CMD_NONE:      
      command_processor->save_args();                                        
      break;
    case CMD_FLUSH:     
      flush(true);                                                              
      break;
    case CMD_ERASE:     
      buffer->erase(false);                                                          
      break;
    case CMD_ROTATE:
      // arg[0] # times to rotate, default = 1
      // arg[1] # rotation steps each time, default = 1
      // arg[2] 0=flush each time, 1=don't flush, default = 0
      do_rotate(command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]); 
      reset_args = true;
      break;
    case CMD_REPEAT:    
      do_repeat(command_processor->sub_args[0]); 
      reset_args = true;
      break;
    case CMD_COPY:   
      do_copy(command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]); 
      reset_args = true;
      break;
    case CMD_FLOOD:     
      do_flood();                                                           
      break;
    case CMD_MIRROR:    
      do_mirror();                                                          
      break;
    case CMD_DISPLAY:    
      set_display(command_processor->sub_args[0]); 
      reset_args = true;
      break;
    case CMD_ZONE:
      buffer->set_zone(command_processor->sub_args[0]); 
      reset_args = true;
      break;
    case CMD_WINDOW:    
      buffer->set_window_override(command_processor->sub_args[0]); 
      reset_args = true;
      break;
    case CMD_OFFSET:
      buffer->set_offset_override(command_processor->sub_args[0]); 
      reset_args = true;
      break;
    case CMD_REVERSE:
      buffer->set_reverse(command_processor->sub_args[0] == 1 ? true : false); 
      reset_args = true;
      break;
    case CMD_RGBCOLOR:  
      buffer->push_rgb_color(command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]); 
      reset_args = true;
      break;
    case CMD_HSLCOLOR:  
      buffer->push_hsl_color(command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]); 
      reset_args = true;
      break;
    case CMD_RED:       
      buffer->push_color(RED, command_processor->sub_args[0], command_processor->sub_args[1]);                                                      
      reset_args = true;
      break;
    case CMD_GREEN:     
      buffer->push_color(GREEN, command_processor->sub_args[0], command_processor->sub_args[1]);                                                    
      reset_args = true;
      break;
    case CMD_BLUE:      
      buffer->push_color(BLUE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                     
      reset_args = true;
      break;
    case CMD_BLACK:     
      buffer->push_color(buffer->black, command_processor->sub_args[0], command_processor->sub_args[1]);                                                    
      reset_args = true;
      break;
    case CMD_YELLOW:    
      buffer->push_color(YELLOW, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_ORANGE:    
      buffer->push_color(ORANGE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_PURPLE:    
      buffer->push_color(PURPLE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_CYAN:      
      buffer->push_color(CYAN, command_processor->sub_args[0], command_processor->sub_args[1]);                                                     
      reset_args = true;
      break;
    case CMD_MAGENTA:   
      buffer->push_color(MAGENTA, command_processor->sub_args[0], command_processor->sub_args[1]);                                                  
      reset_args = true;
      break; 
    case CMD_PINK:      
      buffer->push_color(PINK, command_processor->sub_args[0], command_processor->sub_args[1]);                                                     
      reset_args = true;
      break; 
    case CMD_WHITE:     
      buffer->push_color(WHITE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                    
      reset_args = true;
      break; 
    case CMD_GRAY:      
      buffer->push_color(GRAY, command_processor->sub_args[0], command_processor->sub_args[1]);                                                     
      reset_args = true;
      break;
    case CMD_LTGREEN:   
      buffer->push_color(LTGREEN, command_processor->sub_args[0], command_processor->sub_args[1]);                                                  
      reset_args = true;
      break;
    case CMD_SEAFOAM:   
      buffer->push_color(SEAFOAM, command_processor->sub_args[0], command_processor->sub_args[1]);                                                  
      reset_args = true;
      break;
    case CMD_LTBLUE:    
      buffer->push_color(LTBLUE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_DKGRAY:    
      buffer->push_color(DKGRAY, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_TUNGSTEN:    
      buffer->push_color(TUNGSTEN, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_AMBER:
      buffer->push_color(AMBER, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_OLIVE:
      buffer->push_color(OLIVE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_SKYBLUE:
      buffer->push_color(SKYBLUE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_TURQUOISE:
      buffer->push_color(TURQUOISE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_LAVENDER:
      buffer->push_color(LAVENDER, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_ROSE:
      buffer->push_color(ROSE, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
      reset_args = true;
      break;
    case CMD_RANDOM:    
      do_random(command_processor->sub_args[0]);                                                          
      reset_args = true;
      break; 
    case CMD_BLEND:     
      do_blend(command_processor->sub_args[0]);                                                           
      reset_args = true;
      break;
    case CMD_MAX:         
      do_max();                                                             
      break;
    case CMD_DIM:       
      do_dim();                                                             
      break;
    case CMD_BRIGHT:    
      do_bright();                                                          
      break;
    case CMD_BLINK:     
      effects_processor->start_effect(BLINK_ON);                                               
      break;
    case CMD_BLINK1:    
      effects_processor->start_effect(BLINK_ON_1);                                             
      break;
    case CMD_BLINK2:    
      effects_processor->start_effect(BLINK_ON_2);                                             
      break;
    case CMD_BLINK3:    
      effects_processor->start_effect(BLINK_ON_3);                                             
      break;
    case CMD_BLINK4:    
      effects_processor->start_effect(BLINK_ON_4);                                             
      break;
    case CMD_BLINK5:    
      effects_processor->start_effect(BLINK_ON_5);                                            
      break;
    case CMD_BLINK6:    
      effects_processor->start_effect(BLINK_ON_6);                                             
      break;
    case CMD_BLINKR:    
      effects_processor->start_blinking_r();                                                   
      break;
    case CMD_BLINKA:    
      effects_processor->start_effect(BLINK_ON_A);                                             
      break;
    case CMD_BLINKB:    
      effects_processor->start_effect(BLINK_ON_B);                                             
      break;
    case CMD_BLINKC:
      effects_processor->start_effect(BLINK_ON_C);                                               
      break;
    case CMD_BREATHE:   
      effects_processor->start_effect(BREATHE_ON);                                             
      break;
    case CMD_SLOW_FADE:
      effects_processor->start_effect(SLOW_FADE);                                             
      break;
    case CMD_FAST_FADE:
      effects_processor->start_effect(FAST_FADE);                                             
      break;
    case CMD_TWINKLE:
      effects_processor->start_effect(TWINKLE_ON);                                             
      break;
    case CMD_RAW:
      effects_processor->start_effect(RAW_ON);                                             
      break;
    case CMD_STATIC:    
      effects_processor->start_effect(STATIC_ON);                                                                                                                      
      break;
    case CMD_EFFECTR:   
      effects_processor->start_effect_r();                                                     
      break;
    case CMD_PAUSE:     
      pause();
      break;
    case CMD_CONTINUE:  
      resume();
      break;
    case CMD_RESET:     
      reset();                                                              
      break;
    case CMD_CLEAR:      
      clear();                                                            
      break;
    case CMD_LEVEL:
      set_brightness_level(command_processor->sub_args[0]); 
      reset_args = true;
      break;
    case CMD_FADE:      
      do_fade();                                                            
      break;
    case CMD_WIPE:     
      do_wipe();                                                     
      break;
    case CMD_ESHIFT_OPEN:    
      //do_elastic_shift(command_processor->sub_args[0]); 
      reset_args = true;
      break;
    case CMD_PSHIFT:    
      do_power_shift(command_processor->sub_args[0]);  
      reset_args = true;
      break;
    case CMD_PSHIFTO:   
      do_power_shift_object(command_processor->sub_args[0], command_processor->sub_args[1]); 
      reset_args = true;
      break;
    case CMD_CFADE:      
      do_crossfade();                                                            
      break;
    case CMD_LOPOWER_OPEN:
      //low_power();                                               
      break;
    case CMD_HIPOWER_OPEN:
      //high_power();                                               
      break;
    case CMD_PINON:    
      set_pin(command_processor->sub_args[0], true); 
      reset_args = true;
      break;
    case CMD_PINOFF:    
      set_pin(command_processor->sub_args[0], false); 
      reset_args = true;
      break;
    case CMD_DEMO_OPEN:      
      //do_demo();                                                                                                                                    
      break;
    case CMD_SETBLINKC:
      blink_effects->set_custom_blink(command_processor->sub_args[0]);
      reset_args = true;
      break;
    case CMD_SETBLINKP:
      blink_effects->set_blink_period(command_processor->sub_args[0]);
      reset_args = true;
      break;
    case CMD_SCHEDULE:
      // arg[0] schedule period 0-65534, -1 clears all schedules
      // arg[1] schedule number, default schedule #0 
      // arg[2] macro number, default same as schedule #
      set_schedule(command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      reset_args = true;
      break;
    case CMD_CARRY:
      buffer->push_carry_color();
      break;
    case CMD_SETBREATHET:
      breathe_effects->set_breathe_time(command_processor->sub_args[0]);
      reset_args = true;
      break;
    case CMD_SET_MACRO:
      {
        if(dispatch_data != NULL){
          // being used internally
          macros.set_macro(command_processor->sub_args[0], (char*)dispatch_data);          

          // signal that no more commands should be processed (rest of buffer copied to macro)
          continue_dispatching = false;
        } else {
          // being used over serial
          byte num_bytes = macros.set_macro_from_serial(command_processor->sub_args[0]);
          command_processor->send_int(num_bytes);
        }
      }
      reset_args = true;
      break;
    case CMD_RUN_MACRO:
      // arg[0] macro number to run, default = 0
      // arg[1] number of times to run, default = 1
      // arg[3] milliseconds delay between runs, default = no delay
      macros.run_macro(command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      reset_args = true;
      break;
    case CMD_DELAY:
      do_delay(command_processor->sub_args[0]);
      reset_args = true;
      break;

    case CMD_STOP:
      reset_all_schedules();
      clear();                                                            
      pause();                                                            
      break;      

    case CMD_RANDOM_NUM:
      {
        // arg[0] maximum random number (see Commands::random_num() for special constant values)
        // arg[1] minimum random number (default=0)
        // arg[2] copied into arg[1] to allow passing another argument
        int ran = random_num(command_processor->sub_args[0], command_processor->sub_args[1]);
        command_processor->sub_args[0] = ran;
        command_processor->sub_args[1] = command_processor->sub_args[2];
        command_processor->sub_args[2] = 0;
      }
      break;
    case CMD_POSITION:
      // arg[0] index of insertion pointer
      //        if -1, start of current zone
      //        if -2, end of current zone
      //        if -3, center of current zone
      set_position(command_processor->sub_args[0]);
      reset_args = true;
      break;

    case CMD_RPOSITION:
      // arg[0] if -1, only 
      random_position(command_processor->sub_args[0]);
      reset_args = true;
      break;

    case CMD_PALETTE:
      {
        // arg[0] the index into the palette of the color to insert, or where to stop rubberstamp insert
        // arg[1] if > 0, colors are inserted counting down from this position
        //                the counting down is done so the palette achieves a left-to-right order when inserted  
        //                in this case arg[0] is the stopping point when counting down
        // for example: a rainbow is 0,5:pal, whole palette: 0,17:pal
        
        int arg0 = command_processor->sub_args[0];
        int arg1 = command_processor->sub_args[1];
        if(arg1 > 0){
          arg0 = max(0, arg0);
          rgb_color * palette = Colors::get_palette();
          for(int i = arg1; i >= arg0; i--){
            buffer->push_color(palette[i]);                      
          }
        } else {
          buffer->push_color(Colors::get_palette()[arg0]);                                                      
        }
        
        reset_args = true;
        break;
      }
            
    case CMD_SHUFFLE:
      {
        int arg0 = command_processor->sub_args[0];
        switch(arg0)
        {
          case 0:
            // create a palette of random colors
            Colors::shuffle_palette();
            break;

          case 1:
            // reset palette to original built-in colors
            Colors::reset_palette();  
            break;

          case 2:
            // make every odd color the complimentary color of the previous even color
            Colors::compliment_palette();
            break;

          case 3:
            // create a palette of random complimentary color pairs
            Colors::complimentary_palette();        
             break;
        }            

        reset_args = true;
        break;
      }
    case CMD_SETBLACK:
    {
      rgb_color black_level = {command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]};
      buffer->set_black_level(black_level);
    }
  }

  if(reset_args)
    command_processor->reset_args();   

  return continue_dispatching;
}

// process the series of commands and arguments in the passed memory buffer
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













// setting macros requires command processor

// running macros requires dispatcher

//// in-memory macros
//byte Commands::macros[NUM_MEMORY_MACROS][NUM_MACRO_CHARS];
//
//bool Commands::is_memory_macro(byte macro){
//  return macro >= 0 && macro <= MAX_MEMORY_MACRO;
//}
//
//bool Commands::is_eeprom_macro(byte macro){
//  return macro >= EEPROM_STARTING_MACRO && macro <= MAX_EEPROM_MACRO;
//}
//
//// get a read/write pointer to a memory macro slot
//byte * Commands::get_memory_macro(byte macro){
//  if(macro < 0 || macro > MAX_MEMORY_MACRO){
//    return NULL;
//  }
//  return macros[macro];
//}
//
//// get an eeprom accessible pointer to an eeprom macro slot
//byte * Commands::get_eeprom_macro(byte macro){
//  byte effective_macro = macro - EEPROM_STARTING_MACRO;
//  return (byte*)(effective_macro * NUM_MACRO_CHARS);
//}
//
//byte Commands::num_bytes_from_arg_marker(byte arg_marker){
//  return (arg_marker - ARG_MARKER_FIRST) + 1;
//}
//
//byte Commands::num_words_from_arg_marker(byte arg_marker){
//  return num_bytes_from_arg_marker(arg_marker) / 2;
//}
//
//// copy bytes until the end of macro marker
//void Commands::set_memory_macro_from_memory(byte macro, byte * buffer){
//  byte * str = get_memory_macro(macro);
//
//  byte b;
//  while((b = *buffer++) != MACRO_END_MARKER){
//    *str++ = b;
//
//    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
//      // copy the packed arguments, which can be any value 0-255
//      // including the end of macro marker
//      byte num_bytes = num_bytes_from_arg_marker(b);
//      
//      for(byte i = 0; i < num_bytes; i++){
//        *str++ = *buffer++;  
//      }
//    }
//  }
//  *str = MACRO_END_MARKER;
//}
//
//// copy bytes until the end of macro marker
//void Commands::set_eeprom_macro_from_memory(byte macro, byte * buffer){
//  byte * str = get_eeprom_macro(macro);
//
//  byte b;
//  while((b = *buffer++) != MACRO_END_MARKER){
//    eeprom_write_byte(str++, b);
//
//    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
//      // copy the packed arguments, which can be any value 0-255
//      // including the end of macro marker
//      byte num_bytes = num_bytes_from_arg_marker(b);
//
//      for(byte i = 0; i < num_bytes; i++){
//        eeprom_write_byte(str++, *buffer++);  
//      }
//    }
//  }
//
//  eeprom_write_byte(str, MACRO_END_MARKER);
//}
//
//// copy bytes until the end of macro marker
//void Commands::set_memory_macro_from_eeprom(byte macro, byte * buffer){
//  byte * str = get_memory_macro(macro);
//
//  byte b;
//  while((b = eeprom_read_byte(buffer++)) != MACRO_END_MARKER){
//    *str++ = b;
//
//    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
//      // copy the packed arguments, which can be any value 0-255
//      // including the end of macro marker
//      byte num_bytes = num_bytes_from_arg_marker(b);
//
//      for(byte i = 0; i < num_bytes; i++){
//        *str++ = eeprom_read_byte(buffer++);  
//      }
//    }
//  }
//  *str = MACRO_END_MARKER;
//}
//
//// copy bytes until the end of macro marker
//void Commands::set_eeprom_macro_from_eeprom(byte macro, byte * buffer){
//  byte * str = get_eeprom_macro(macro);
//
//  byte b;
//  while((b = eeprom_read_byte(buffer++)) != MACRO_END_MARKER){
//    eeprom_write_byte(str++, b);
//
//    if(b >= ARG_MARKER_FIRST && b <= ARG_MARKER_LAST){
//      // copy the packed arguments, which can be any value 0-255
//      // including the end of macro marker
//      byte num_bytes = num_bytes_from_arg_marker(b);
//
//      for(byte i = 0; i < num_bytes; i++){
//        eeprom_write_byte(str++, eeprom_read_byte(buffer++));  
//      }
//    }
//  }
//  
//  eeprom_write_byte(str, MACRO_END_MARKER);
//}
//
//void Commands::reset_macro(byte macro){
//  if(is_memory_macro(macro))
//    get_memory_macro(macro)[0] = '\0';
//  else if(is_eeprom_macro(macro))
//    eeprom_write_byte(get_eeprom_macro(macro), MACRO_END_MARKER);
//}
//
//void Commands::reset_all_macros(){
//  for(byte i = 0; i < NUM_MACROS; i++)
//    reset_macro(i);
//}
//
//bool Commands::is_macro_programmed(byte macro){
//  return eeprom_read_byte(get_eeprom_macro(macro)) != DEFAULT_ERASE_BYTE;
//}
//
//void Commands::determine_arg_marker(byte &arg_marker, byte &num_args){
//  int * sub_args = command_processor->sub_args;
//
//  if(sub_args[2] != 0){
//    arg_marker = MACRO_ARG6_MARKER;
//    num_args = 3;
//  } else if(sub_args[1] != 0){
//    arg_marker = MACRO_ARG4_MARKER;
//    num_args = 2;
//  } else {
//    int arg0 = sub_args[0];
//    if(arg0 >= 1 && arg0 <= 255 ) {
//      arg_marker = MACRO_ARG1_MARKER;
//    } else if(arg0 != 0){
//      arg_marker = MACRO_ARG2_MARKER;
//      num_args = 1;
//    }
//  }
//}
//
//byte Commands::set_memory_macro(byte macro, char * commands){
//  byte * macro_buffer = get_memory_macro(macro);
//  if(macro_buffer == NULL)
//    // not a valid memory macro location
//    return 0;
//
//  if(commands == NULL || *commands == '\0'){
//    // no commands; empty the macro
//    *macro_buffer = MACRO_END_MARKER;
//    return 0;
//  }
//
//  // begin_get_commands() and get_next_command() need an external pointer
//  // to hold onto the strtok_r state
//  char *saveptr;
//
//  // get the first command or set of arguments
//  char *command = command_processor->begin_get_commands(commands, &saveptr);
//  int cmd = command_processor->lookup_command(command);
//
//  if(cmd == CMD_NULL){
//    // no commands; empty buffer
//    *macro_buffer = MACRO_END_MARKER;
//    return 0;
//  }
//    
//  byte byte_count = 0;
//  do{
//    if(cmd == CMD_NONE){
//      // this is a set of arguments
//      command_processor->get_sub_args(command);
//
//      // pack the arguments 
//      byte arg_marker;
//      byte num_args = 0;
//      determine_arg_marker(arg_marker, num_args);
//
//      // write arguments marker
//      *macro_buffer++ = arg_marker;
//      byte_count++;
//      
//      if(arg_marker == MACRO_ARG1_MARKER){
//        *macro_buffer++ = (byte)command_processor->sub_args[0];
//        byte_count++;
//      } else {
//        for(byte i = 0; i < num_args; i++){
//          *((int *)macro_buffer) = command_processor->sub_args[i];
//          macro_buffer += 2;
//          byte_count += 2;
//        }
//      }
//    } else {
//      // write the command byte to the macro buffer
//      *macro_buffer++ = (byte)cmd;
//      byte_count++;
//    }
//
//    // get the next command or argumemts
//    command = command_processor->get_next_command(&saveptr);
//    cmd = command_processor->lookup_command(command);
//    
//  }while(cmd != CMD_NULL);
//
//  // write end of macro marker
//  *macro_buffer = MACRO_END_MARKER;
//
//  return byte_count;
//}
//
//byte Commands::set_eeprom_macro(byte macro, char * commands){
//  byte * macro_buffer = get_eeprom_macro(macro);
//
//  if(commands == NULL || *commands == '\0'){
//    // no commands; empty the macro
//    eeprom_write_byte(macro_buffer, MACRO_END_MARKER);
//    return 0;
//  }
//
//  // begin_get_commands() and get_next_command() need an external pointer
//  // to hold onto the strtok_r state
//  char *saveptr;
//
//  // get the first command or set of arguments
//  char *command = command_processor->begin_get_commands(commands, &saveptr);
//  int cmd = command_processor->lookup_command(command);
//
//  if(cmd == CMD_NULL){
//    // no commands; empty buffer
//    eeprom_write_byte(macro_buffer, MACRO_END_MARKER);
//    return 0;
//  }
//
//  byte byte_count = 0;
//  do{
//    if(cmd == CMD_NONE){
//      // this is a set of arguments
//      command_processor->get_sub_args(command);
//
//      // pack the arguments 
//      byte arg_marker;
//      byte num_args = 0;
//      determine_arg_marker(arg_marker, num_args);
//
//      // write arguments marker
//      eeprom_write_byte(macro_buffer++, arg_marker);
//      byte_count++;
//      
//      // pack the arguments 
//      int * sub_args = command_processor->sub_args;
//      if(arg_marker == MACRO_ARG1_MARKER){
//        eeprom_write_byte(macro_buffer++, sub_args[0] & 0xff);
//        byte_count++;
//      } else {
//        for(byte i = 0; i < num_args; i++){
//          eeprom_write_word((word*)macro_buffer, (word)sub_args[i]);
//          macro_buffer += 2;
//          byte_count += 2;
//        }
//      }
//    } else {
//      // write the command byte to the macro buffer
//      eeprom_write_byte(macro_buffer++, cmd);
//      byte_count++;
//    }
//
//    // get the next command or argumemts
//    command = command_processor->get_next_command(&saveptr);
//    cmd = command_processor->lookup_command(command);
//    
//  }while(cmd != CMD_NULL);
//
//  // write end of macro marker
//  eeprom_write_byte(macro_buffer, MACRO_END_MARKER);
//
//  return byte_count;
//}
//
//byte Commands::set_macro(byte macro, char * commands){
//  if(is_memory_macro(macro))
//    return set_memory_macro(macro, commands);
//  else if(is_eeprom_macro(macro))
//    return set_eeprom_macro(macro, commands);
//}
//
//// used with the "set" command to set a macro from the serial input buffer
//byte Commands::set_macro_from_serial(byte macro){
//  return set_macro(macro, command_processor->get_input_buffer());
//}
//
//void Commands::run_memory_macro(byte macro, int times){
//  // don't pass in this macro running's arguments
//  command_processor->reset_args();
//
//  times = max(1, times);
//  byte * cached_macro_buffer = get_memory_macro(macro);
//
//  byte * macro_buffer = cached_macro_buffer;
//  if(macro_buffer == NULL || *macro_buffer == '\0')
//    // not a valid macro location or macro is empty
//    return;
//
//  int * sub_args = command_processor->sub_args;
//  for(int i = 0; i < times; i++){     
//    macro_buffer = cached_macro_buffer;
//
//    byte cmd;
//    while((cmd = *macro_buffer++) != MACRO_END_MARKER){
//      if(cmd >= ARG_MARKER_FIRST && cmd <= ARG_MARKER_LAST){
//        // unpack the arguments
//        if(cmd == MACRO_ARG1_MARKER){
//          // the most common case a value 1-255
//          sub_args[0] = *macro_buffer++;
//        } else {
//          byte num_args = num_words_from_arg_marker(cmd);
//          for(byte i = 0; i < num_args; i++){
//            sub_args[i] = *((int *)macro_buffer);
//            macro_buffer += 2;
//          }
//        }
//      } else {
//        if(cmd == CMD_SET_MACRO){
//          byte new_macro = sub_args[0];
//          if(is_memory_macro(new_macro))
//            set_memory_macro_from_memory(new_macro, macro_buffer);
//          else if(is_eeprom_macro(new_macro))
//            set_eeprom_macro_from_memory(new_macro, macro_buffer);
//
//          // remaining macro has been consumed
//          return;
//        } else {
//          if(!dispatch_command(cmd, macro_buffer)){
//            // the remaining macro buffer has been copied to set a macro
//            // so there are no more commands to dispatch
//            return;
//          }
//        }
//      }
//    }
//  }
//}
//
//void Commands::run_eeprom_macro(byte macro, int times){
//  // don't pass in this macro running's arguments
//  command_processor->reset_args();
//  
//  times = max(1, times);
//  byte * cached_macro_buffer = get_eeprom_macro(macro);
//
//  int * sub_args = command_processor->sub_args;
//  for(int i = 0; i < times; i++){     
//    byte * macro_buffer = cached_macro_buffer;
//    
//    byte cmd;
//    while((cmd = eeprom_read_byte(macro_buffer)) != MACRO_END_MARKER){
//      macro_buffer++;
//
//      if(cmd >= ARG_MARKER_FIRST && cmd <= ARG_MARKER_LAST){
//        // unpack the arguments
//        if(cmd == MACRO_ARG1_MARKER){
//          // the most common case a value 1-255
//          sub_args[0] = eeprom_read_byte(macro_buffer++);
//        } else {
//          byte num_args = num_words_from_arg_marker(cmd);
//          for(byte i = 0; i < num_args; i++){
//            sub_args[i] = (int)eeprom_read_word((word*)macro_buffer);
//            macro_buffer += 2;
//          }
//        }
//      } else {
//        if(cmd == CMD_SET_MACRO){
//          byte new_macro = sub_args[0];
//          if(is_memory_macro(new_macro)){
//            set_memory_macro_from_eeprom(new_macro, macro_buffer);
//          } else if(is_eeprom_macro(macro)){
//            set_eeprom_macro_from_eeprom(new_macro, macro_buffer);
//          }
//        
//        // remaining macro has been consumed
//        return;
//        } else {
//        if(!dispatch_command(cmd, macro_buffer)){
//          // the remaining macro buffer has been copied to set a macro
//          // so there are no more commands to dispatch
//          return;
//          }
//        }
//      }
//    }
//  }
//}
//
//void Commands::run_macro(byte macro, int times, int delay_)
//{
//  if(is_memory_macro(macro))
//    run_memory_macro(macro, times);
//  else if(is_eeprom_macro(macro))
//    run_eeprom_macro(macro, times);
//
//  if(delay_ > 0)
//    delay(delay_);
//}
//












unsigned int Commands::schedule_period[NUM_SCHEDULES];  // zero means the schedule is turned off
byte Commands::macro_number[NUM_SCHEDULES];             // could leave this off and assume schedule is same as macro to run, 
unsigned int Commands::schedule_counter[NUM_SCHEDULES]; // but the ability to switch schedules to run different macros is the basis for toggling

void Commands::process_schedules(){
  for(byte i = 0; i < NUM_SCHEDULES; i++){

    if(schedule_period[i] == 0)
      continue;
    
    schedule_counter[i] = (schedule_counter[i] + 1) % schedule_period[i];

    if(schedule_counter[i] == 0){
      macros.run_macro(macro_number[i]);
    }
  }
}

void Commands::reset_schedule(byte schedule_number){
  macro_number[schedule_number]     = 0;
  schedule_period[schedule_number]  = 0;
  schedule_counter[schedule_number] = 0;
}

void Commands::reset_all_schedules(){
  for(byte i = 0; i < NUM_SCHEDULES; i++){
    reset_schedule(i); 
  }
}

// if schedule period is -1, it clears all schedules
void Commands::set_schedule(unsigned int schedule_period_, byte schedule_number, byte macro_number_){
  if((int)schedule_period_ == -1){
    reset_all_schedules();  
    return;
  }

  if(macro_number_ == 0){
    // default the macro to be the same as the schedule
    macro_number_ = schedule_number;
  }
  
  reset_schedule(schedule_number);
  schedule_period[schedule_number]  = schedule_period_;
  macro_number[schedule_number]     = macro_number_;

  // set to zero for a complete schedule period to pass before it runs the macro (probably best)
  // could set to schedule_period - 1 to have the macro run immediately upon being set
  schedule_counter[schedule_number] = 0;
}














void Commands::run_default_macro(){

// #define FORCE_PROGRAM_MACROS

#if !defined(FORCE_PROGRAM_MACROS)
  if(!macros.is_macro_programmed(DEFAULT_MACRO)){
#endif

#if defined(SPHERE) || defined(DUAL_STRIP) || defined(WEARABLE_AND_HARDHAT) || defined(RADIUS8) // || defined(APOLLO_LIGHTS2) //|| defined(WEARABLE_AND_DISC93)
    process_commands(F("10:set:19:run"));

    process_commands(F("11:set:wht:brt:brt:flo:flu:30:del"));
    process_commands(F("12:set:rnd:brt:brt:sfd:flo:flu"));
    process_commands(F("13:set:11:run:12:run"));
    process_commands(F("14:set:13:run:20000,10000,14:rng:sch"));
    
    process_commands(F("15:set:rps:wht:brt:brt:sfd:rst"));
    process_commands(F("16:set:rps:1:rnd:sfd:flu:rst"));
    process_commands(F("17:set:fad:50,15:sch:20000,14:sch"));
    process_commands(F("18:set:fad:50,16:sch"));
    
    process_commands(F("19:set:-1:sch:60000,19,20:sch:17:run"));
    process_commands(F("20:set:-1:sch:60000,19,21:sch:18:run"));
    process_commands(F("21:set:-1:sch:60000,19,19:sch:23:run"));

    process_commands(F("22:set:-3:rng:zon:rot:flu:rst"));
    process_commands(F("23:set:fad:2:rnd:flo:flu:50,22:sch"));

#elif defined(MINI_DISC_19)
    // monument animation
    process_commands(F("10:set:100:lev:800,14:sch:1600,15:sch:60000,13:sch"));
    process_commands(F("11:set:3:zon:0,2:pal:2,0,6:cpy"));
    process_commands(F("12:set:2:zon:0,2:pal:2,0,3:cpy"));
    process_commands(F("13:set:3:shf:11:run:12:run:rst"));
    process_commands(F("14:set:3:zon:rot:flu:rst"));
    process_commands(F("15:set:2:zon:rot:flu:rst"));

#elif defined(APOLLO_LIGHTS2) //|| defined(APOLLO_LIGHTS2_DEV)
    // random colors into warm white
    process_commands(F("10:set:clr:era:70:lev:13,120:run:tun:flo:cfa:100:lev"));

    // sparking colors with drain
    process_commands(F("11:set:clr:100,13:sch:10000,14:sch"));

    // sparkling white
    process_commands(F("12:set:clr:200,19:sch"));

    process_commands(F("13:set:-1:rps:1:rnd:sfd:flu:rst"));
    process_commands(F("14:set:15,15:run:500:del"));
    process_commands(F("15:set:16:run:17:run:18:run:flu"));
    process_commands(F("16:set:1:zon:1:blk:2:zon:1:rev:1:blk:rst"));
    process_commands(F("17:set:3:zon:1:blk:4:zon:1:rev:1:blk:rst"));
    process_commands(F("18:set:5:zon:1:blk:6:zon:1:rev:1:blk:rst"));

    process_commands(F("19:set:-1:rps:wht:sfd:flu:rst"));

    // fade into tungsten lamps
    process_commands(F("10:set:70:lev:tun:flo:cfa:100:lev"));

#elif defined(APOLLO_LIGHTS2_DEV) 
    // start up
    process_commands(F("10:set:25:run"));

    // random colors with drains
    process_commands(F("11:set:60,12:sch:20,13:sch:5000,14:sch"));
    process_commands(F("12:set:-1:rps:rnd:twi:flu:rst"));
    process_commands(F("13:set:-2:rps:sfd:flu:rst"));
    process_commands(F("14:set:15,8,5:run:200:del"));
    process_commands(F("15:set:16:run:17:run:18:run:flu"));
    process_commands(F("16:set:1:zon:2:blk:2:zon:1:rev:2:blk:rst"));
    process_commands(F("17:set:3:zon:2:blk:4:zon:1:rev:2:blk:rst"));
    process_commands(F("18:set:5:zon:2:blk:6:zon:1:rev:2:blk:rst"));

    // bubbling up dots
    process_commands(F("20:set:60,21:sch:30,22:sch:15,23:sch"));
    process_commands(F("21:set:9:zon:rnd:flu:3,1,1:rng:blk:flu")); 
    process_commands(F("22:set:10:zon:car:flu"));
    process_commands(F("23:set:11:zon:car:flu"));

    // switch betwen the two patterns
    process_commands(F("25:set:-1:sch:11:run:10000,25,26:sch"));
    process_commands(F("26:set:-1:sch:20:run:10000,25,25:sch"));

#elif defined(WEARABLE_AND_GLASSES) || defined(PROJECTOR) || defined(WEARABLE_AND_DISC93) || defined(DISC93) // || defined(RADIUS8) 
    // random colors and random zone rotations
#ifdef DISC93
    process_commands(F("10:set:100:lev:era:50,11:sch:15,12:sch:200,16:sch"));
#else    
    process_commands(F("10:set:era:50,11:sch:15,12:sch:200,16:sch"));
#endif
    process_commands(F("11:set:rng:pos:rnd:twi:flu:rst"));
    process_commands(F("12:set:rng:pos:sfd:flu:rst"));
    process_commands(F("13:set:-3:rng:zon"));
    process_commands(F("14:set:2:rng:rev"));
    process_commands(F("15:set:0,3:rng:rot"));
    process_commands(F("16:set:13:run:14:run:15:run:rst"));
#endif

#if !defined(FORCE_PROGRAM_MACROS)
  }
#endif
  
  // run auto-start macro
  if(macros.is_macro_programmed(DEFAULT_MACRO)){
    process_commands(F("10:run"));
  }

}

#endif

