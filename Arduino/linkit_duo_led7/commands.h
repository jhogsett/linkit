#ifndef COMMANDS_H
#define COMMANDS_H

#include <macros.h>
#include <scheduler.h>

#include "config.h"
#include "zone_defs.h"

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
  
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, AutoBrightnessBase *auto_brightness, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects);
  void process_commands(char * buffer);
  void process_commands(const __FlashStringHelper * commands);
  void reset();
  void set_brightness_level(byte level = 0);
  bool dispatch_command(int cmd, byte *dispatch_data = NULL);
  bool is_paused();
  void flush_all(bool force_display = false);
  void run_default_macro();
  static bool dispatch_function(int cmd, byte *dispatch_data = NULL);
  static Scheduler scheduler;

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
  static Macros macros;
  static Commands * me;
};

Macros Commands::macros;
Scheduler Commands::scheduler;
Commands * Commands::me;

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

  macros.begin(command_processor, &Commands::dispatch_function);
  scheduler.begin(&macros);
}

bool Commands::dispatch_function(int cmd, byte *dispatch_data){
  return me->dispatch_command(cmd, dispatch_data);  
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

#include "dispatch_command.h"

#include "default_macros.h"

#endif

