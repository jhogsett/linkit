#ifndef COMMANDS_H
#define COMMANDS_H

#include <macros.h>
#include <scheduler.h>
#include "config.h"
#include "zone_defs.h"
#include "sequencer.h"

#ifdef USE_MAPPING
#include "map_defs.h"
#endif

#define MAX_BRIGHTNESS_PERCENT (default_brightness * 4)
#define DIM_BRIGHTNESS_PERCENT (default_brightness / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (default_brightness * 2)

#define DEFAULT_MACRO 10

#if defined(MINI_DISC_19)
#define CROSSFADE_DELAY 50
#else
#define CROSSFADE_DELAY 1
#endif



//Buffer *buffer
//Render *renderer
//EffectsProcessor *effects_processor
//CommandProcessor *command_processor
//BlinkEffects *blink_effects
//BreatheEffects *breathe_effects
//FadeEffects *fade_effects
//Sequencer *sequencer

//, byte default_brightness, byte visible_led_count, , 
//
//
//);


class Commands
{
  public:
  
#ifdef USE_AUTO_BRIGHTNESS
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, AutoBrightnessBase *auto_brightness, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects *fade_effects, Sequencer *sequencer);
#else
  void begin(Buffer *buffer, Render *renderer, EffectsProcessor *effects_processor, byte default_brightness, byte visible_led_count, CommandProcessor *command_processor, BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects *fade_effects, Sequencer *sequencer);
#endif

  void process_events();
  void do_run_macro(byte macro, int times = 1, int delay_ = 0);
  void reset();
  void set_brightness_level(byte level = 0);
  bool dispatch_command(int cmd, byte *dispatch_data = NULL);
  void flush_all(bool force_display = false);
  static bool dispatch_function(int cmd, byte *dispatch_data = NULL);
  static Scheduler scheduler;

  private:
  
  void pause(byte type);
  void resume(int type);
  void pause_effects(bool paused);
  void pause_schedules(bool paused);
  void restore_paused_state();
  void save_paused_state();
  rgb_color * offset_buffer();

  #ifdef USE_BLEND
  void do_blend(byte strength);
  #endif

  void do_max();
  void do_dim();
  void do_bright();
  void do_fade();
  void do_crossfade();
//  void do_wipe();
  void do_flood();
  void do_random(byte type, int times);
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

  void flush(bool force_display);
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
  void do_next_window(int arg0, int arg1, int arg2);
  void do_configure(int arg0, int arg1, int arg2);
  bool do_set_macro(byte macro, byte * dispatch_data);
  void do_color_sequence(byte type, int arg0, int arg1, int arg2);
  void do_random_number(int arg0, int arg1, int arg2);
  void do_stop();
  void do_palette(int arg0, int arg1);
  void do_shuffle(int arg0, int arg1, int arg2);
  void set_black_level(int arg0, int arg1, int arg2);
  void do_store(int arg0, int arg1, int arg2);
  void do_recall(int arg0, int arg1, int arg2);
  void dispatch_effect(byte cmd);
  void displatch_color(byte cmd, int arg0, int arg1);
  void do_xy_position(byte arg0, byte arg1);
  void do_dynamic_color(byte arg0, byte arg1, byte arg2, byte effect=NO_EFFECT);
  void do_fan(bool fan_on, bool auto_set=false);

  void do_test_inquiry(byte type, int arg2);
  void do_test(int type, int arg1, int arg2);
//  void do_test_process(int times, bool schedules);

#ifdef USE_TEST_FRAMEWORK
  void do_test_macro(byte macro_number);
  void do_test_buffer(byte start, byte count);
  void do_test_effects(byte start, byte count);
  void do_test_render(byte start, byte count);
  void do_test_palette(byte start, byte count);
  void do_test_function(byte type, int arg2);
  void do_test_accumulators();
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
  bool effects_were_paused = false;
  bool schedules_were_paused = false;
  byte default_brightness;
  byte visible_led_count;
  static Macros macros;
  static Commands * me;
  Sequencer *sequencer;
  FadeEffects *fade_effects;
  bool user_fan_on;
  
#ifdef USE_MAPPING
  Maps maps;
#endif
  
#ifdef USE_AUTO_BRIGHTNESS
  AutoBrightnessBase *auto_brightness;
#endif
};

Macros Commands::macros;
Scheduler Commands::scheduler;
Commands * Commands::me;

#ifdef USE_AUTO_BRIGHTNESS
void Commands::begin(
  Buffer *buffer, 
  Render *renderer, 
  EffectsProcessor *effects_processor, 
  byte default_brightness, 
  byte visible_led_count, 
  AutoBrightnessBase *auto_brightness, 
  CommandProcessor *command_processor, 
  BlinkEffects *blink_effects, 
  BreatheEffects *breathe_effects, 
  FadeEffects *fade_effects, 
  Sequencer *sequencer)
{
#else
void Commands::begin(
  Buffer *buffer, 
  Render *renderer, 
  EffectsProcessor *effects_processor, 
  byte default_brightness, 
  byte visible_led_count, 
  CommandProcessor *command_processor, 
  BlinkEffects *blink_effects, 
  BreatheEffects *breathe_effects, 
  FadeEffects *fade_effects, 
  Sequencer *sequencer)
{
#endif
  this->me                 = this;
  this->buffer             = buffer;
  this->renderer           = renderer;
  this->effects_processor  = effects_processor;
  this->default_brightness = default_brightness;
  this->visible_led_count  = visible_led_count;
  this->command_processor  = command_processor;
  this->blink_effects      = blink_effects;
  this->breathe_effects    = breathe_effects;
  this->sequencer          = sequencer;
  this->fade_effects       = fade_effects;

#ifdef USE_AUTO_BRIGHTNESS
  this->auto_brightness    = auto_brightness;
#endif

  macros.begin(command_processor, &Commands::dispatch_function);
  scheduler.begin(&macros);

#ifdef USE_MAPPING
  maps.begin(map_rows);
#endif

  //this->default_brightness = maps.get_led(5, 7);

  this->user_fan_on = false;
}

#include "event_loop.h"

bool Commands::dispatch_function(int cmd, byte *dispatch_data)
{
  return me->dispatch_command(cmd, dispatch_data);  
}

#define PAUSE_ALL        0
#define PAUSE_EFFECTS    1
#define PAUSE_SCHEDULES  2
#define PAUSE_PUSH       3

// change to resume as was
#define RESUME_ALL       0 
#define RESUME_EFFECTS   1
#define RESUME_SCHEDULES 2
#define RESUME_POP       3 

void Commands::pause(byte type = PAUSE_ALL)
{
  switch(type)
  {
    case PAUSE_ALL:
      pause_effects(true);
      pause_schedules(true);
      break;

    case PAUSE_EFFECTS:
      pause_effects(true);
      break;
      
    case PAUSE_SCHEDULES:
      pause_schedules(true);
      break;

    case PAUSE_PUSH:
      save_paused_state();
      pause_effects(true);
      pause_schedules(true);
      break;
  }
}

void Commands::resume(int type = RESUME_ALL)
{
  switch(type)
  {
    case RESUME_ALL:
      pause_effects(false);
      pause_schedules(false);
      break;

    case RESUME_EFFECTS:
      pause_effects(false);
      break;

    case RESUME_SCHEDULES:
      pause_schedules(false);
      break;

    case RESUME_POP:
      restore_paused_state();
      break;

  }
}

void Commands::save_paused_state(){
  this->effects_were_paused = this->effects_paused;
  this->schedules_were_paused = this->schedules_paused;  
}

void Commands::restore_paused_state(){
  pause_effects(this->effects_were_paused);
  pause_schedules(this->schedules_were_paused);
}

void Commands::pause_effects(bool paused)
{
  effects_paused = paused;
}

void Commands::pause_schedules(bool paused)
{
  schedules_paused = paused;
}

void Commands::set_display(byte display)
{
  buffer->set_display(display);

  // reset to the default zone on a display change (? this confused me when working on glasses, maybe not needed)
  buffer->set_zone(0);
}

void Commands::set_pin(byte pin, bool on)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, on ? HIGH : LOW);  
}

void Commands::set_brightness_level(byte level)
{
  if(level == 0)
  {
#ifdef USE_AUTO_BRIGHTNESS
    level = auto_brightness->get_auto_brightness_level();
#else
    level = default_brightness;
#endif
  }

  renderer->set_default_brightness(level);
  do_fan(level >= FAN_AUTO_ON_BRIGHTNESS, true);  
  flush_all(true);
}

#ifdef USE_BLEND
// todo: allow blending over a range of leds
// strength 1-100, 100 = all color @ offset
// 0 == 50
// todo: negative # blends in the opposite direction
void Commands::do_blend(byte strength)
{
  if(strength < 1)
    strength = 50;

  byte offset = buffer->get_offset();
  rgb_color * buf = &buffer->get_buffer()[offset];

  *buf = ColorMath::blend_colors(*buf, *(buf+1), strength / 100.0);
  *(buf+1) = *buf;
}
#endif

// todo: change these to a single "adj" command
// 0:adj - adjusts to maximum brightness
// 1:adj - doubles brightness
// 2:adj - quadruples brightness
// -1:adj - halves the brightness
// -2:adj - quarters the brightness

rgb_color * Commands::offset_buffer()
{
  return &buffer->get_buffer()[buffer->get_offset()];
}

// todo: optional
// only works properly when used immediately after placing a standard color
void Commands::do_max()
{
  rgb_color * buf = offset_buffer();
  *buf = ColorMath::scale_color(*buf, MAX_BRIGHTNESS_PERCENT / 100.0);
}

// todo: optional
void Commands::do_dim(){
  rgb_color * buf = offset_buffer();
  buf->red = buf->red >> 1;
  buf->green = buf->green >> 1;
  buf->blue = buf->blue >> 1;
}

// todo: optional
void Commands::do_bright(){
  rgb_color * buf = offset_buffer();
  buf->red = buf->red << 1;
  buf->green = buf->green << 1;
  buf->blue = buf->blue << 1;
}

// todo: optional
void Commands::do_fade()
{
  buffer->erase();
  do_crossfade();
}

// todo: move delay to the color math class
void Commands::do_crossfade()
{
  rgb_color * render_buffer = buffer->get_render_buffer();
  byte steps = ColorMath::crossfade_steps();

  for(byte i = 0; i <= steps; i++)
  {
    buffer->cross_fade(i);
    buffer->display_buffer(render_buffer);
    delay(CROSSFADE_DELAY);
  }
}

void Commands::do_flood()
{
  byte offset = buffer->get_offset();
  byte window = buffer->get_window();

  // no flooding necessary unless there are two or more pixels
  if((window - offset) < 2)
    return;
  
  byte start;
  byte end_;
  byte source;

  if(buffer->get_reverse())
  {
    source = window - 1;
    start = offset;
    end_ = window - 1;
  } 
  else 
  {
    source = offset;
    start = offset + 1;
    end_ = window;
  }

  rgb_color * buf = buffer->get_buffer();
  byte * effects = buffer->get_effects_buffer();
  rgb_color color = buf[source];
  byte effect = effects[source];
  buf += start;
  effects += start;

  for(byte i = start; i < end_; i++)
  {
    *buf = color;
    *effects = effect;    
    buf++;
    effects++;
  }
}

#define RANDOM_COLOR_TYPE_SAME_COLOR_REPEAT     0
#define RANDOM_COLOR_TYPE_DIFF_COLOR_REPEAT     1
#define RANDOM_COLOR_TYPE_DIFF_PLUS_EFFECTS     2
#define RANDOM_COLOR_TYPE_PALETTE               3
#define RANDOM_COLOR_TYPE_DYNAMIC_COLOR_EFFECTS 4

// types 
// 0: random color with no effect
// 1: like #0 but will flood and repeat with random colors 
// 2: like #1 but will also set random effects
// others: b&w palette, all colors, all colors including blk
// if times = -1, it fills the current width
void Commands::do_random(byte type, int times)
{
  type = max(0, type);

  if(times == -1)
    times = buffer->get_width();
  else
    times = max(1, times);

  if(type == RANDOM_COLOR_TYPE_SAME_COLOR_REPEAT)
  {
    buffer->push_color(ColorMath::random_color(), times);
    return;
  }

// todo: optional
  if(type == RANDOM_COLOR_TYPE_DYNAMIC_COLOR_EFFECTS)
  {
    for(int i = 0; i < times; i++)
    {
      byte color1 = random(0, NUM_PALETTE_COLORS);
      byte color2 = random(0, NUM_PALETTE_COLORS);
      byte effect = random(0, 2) == 0 ? BLINK_ON_D : BREATHE_ON_D;
      do_dynamic_color(color1, color2, 0, effect);
    }
    return;
  }

  for(int i = 0; i < times; i++)
  {
    rgb_color color;
    byte effect = NO_EFFECT;
    switch(type)
    {
      case RANDOM_COLOR_TYPE_DIFF_PLUS_EFFECTS:
        effect = EffectsProcessor::random_effect();
      case RANDOM_COLOR_TYPE_DIFF_COLOR_REPEAT: 
        color = ColorMath::random_color();
        break;

// todo: optional
      case RANDOM_COLOR_TYPE_PALETTE:
        color = Colors::random_palette_color();
        break;
    }
    
    buffer->push_color(color, 1, false, effect);  
  }
}

// this won't clobber dynamic effects because this is a copy operation
void Commands::do_mirror()
{
  bool reverse = buffer->get_reverse();
  byte front = buffer->get_offset();
  byte back = buffer->get_window() - 1;
  byte width = buffer->get_width() / 2;

  rgb_color * buf_front = &buffer->get_buffer()[front];
  rgb_color * buf_back = &buffer->get_buffer()[back];
  byte * effects_front = &buffer->get_effects_buffer()[front];
  byte * effects_back = &buffer->get_effects_buffer()[back];

  for(byte i = 0; i < width; i++)
  {
    if(reverse)
    {
      *(buf_front + i) = *(buf_back - i);;
      *(effects_front + i) = *(effects_back - i);
    } 
    else 
    {
      *(buf_back - i) = *(buf_front + i);
      *(effects_back - i) = *(effects_front + i);
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

// this won't clobber dynamic effect marker becauwe this is a copy operation
void Commands::do_copy(byte size, int times, byte zoom)
{
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
  } 
  else if(times == -2)
  {
    // duplicate the pattern but don't copy it first
    dupe_only = true;
    // can't pass in a specific number of times
    // 1 is better than fill because it's less restrictive
    times = 1;
  } 
  else if(times < 1)
  {
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

  if(!copy_only && !dupe_only)
  {
    // copy the effects pattern to the buffer temporarily
    byte * peffects = &effects[offset];
    for(byte i = 0; i < size; i++)
    {
      int effect = *(peffects + i);
      if(use_palette_buffer)
        palette[i].red = effect;
      else
        render_buffer[i].red = effect; 
    }

    // copy the effects
    for(byte i = 0; i < times; i++)
    {
      int size_factor = offset + (i * effective_size);
      for(byte j = 0; j < size; j++)
      {
        int zoom_factor = j * zoom;
        for(byte k = 0; k < zoom; k++)
        {
          byte dest = size_factor + zoom_factor + k;
          if(dest < window) // prevent buffer overrun
          { 
            byte * effect = &effects[dest];
            if(use_palette_buffer)
              *effect = palette[j].red;
            else
              *effect = render_buffer[j].red;
          }
        }
      }
    }
  }

  // copy the color pattern to the buffer temporarily
  if(!dupe_only)
  {
    for(byte i = 0; i < size; i++)
    {
      byte source = offset + i;
      rgb_color * buff = &buf[source];
      if(use_palette_buffer)
        // uncorrect corrected color so it works in the palette for push_color()
        palette[i] = ColorMath::correct_color(*buff);
      else
        render_buffer[i] = *buff; 
    }
  }
  
  if(!copy_only)
  {
    // copy the colors
    for(byte i = 0; i < times; i++)
    {
      int size_factor = offset + (i * effective_size) ;
      for(byte j = 0; j < size; j++)
      {
        int zoom_factor = j * zoom;
        for(byte k = 0; k < zoom; k++)
        {
          byte dest = size_factor + zoom_factor + k;
          if(dest < window)
          { // prevent buffer overrun
            rgb_color * buff = &buf[dest];
            if(use_palette_buffer)
              // correct the uncorrected color in the palette
              *buff = ColorMath::correct_color(palette[j]);
            else
              *buff = render_buffer[j];
          }
        }
      }
    }
  }
}

// to do fix re: reverse (fixed?)
// consider support a repeat of zero times (doing nothing)

// this won't clobber dynamic effects marker because this is a copy operation
void Commands::do_repeat(byte times = 1){
  times = max(1, times);
  byte offset = buffer->get_reverse() ? buffer->get_window() - 1 : buffer->get_offset();
  rgb_color color = ColorMath::correct_color(buffer->get_buffer()[offset]);
  byte effect = buffer->get_effects_buffer()[offset];
  buffer->push_color(color, times, false, effect);
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
void Commands::do_power_shift_object(byte width, byte shift, bool fast_render = true)
{
  do_power_shift(shift, shift + width, fast_render);
}
#endif

// todo: optional
//void Commands::do_wipe()
//{
#ifdef USE_POWER_EASE
  do_power_shift_object(0, visible_led_count, false);
#endif
//}

// arg[0] # times to rotate, default = 1
// arg[1] # rotation steps each time, default = 1
// arg[2] 0=flush each time, 1=don't flush, default = 0
void Commands::do_rotate(byte times, byte steps, bool flush)
{
  times = max(1, times);
  steps = max(1, steps);
  for(byte i = 0; i < times; i++)
  {
    for(byte j = 0; j < steps; j++)
      buffer->rotate(); 
    if(flush){
      this->flush(true);  
    }    
  }
}

void Commands::flush(bool force_display = false)
{
  if(force_display || !effects_paused)
  {
    rgb_color * render_buffer = buffer->get_render_buffer();
    renderer->render_buffer(render_buffer, buffer->get_buffer(), visible_led_count, buffer->get_effects_buffer());
    buffer->display_buffer(render_buffer);
  }
}

// todo: force a particular rendering buffer and display 
// instead of setting and unsetting
void Commands::flush_all(bool force_display)
{
  byte orig_display = buffer->get_current_display();
  
  for(byte i = 0; i < NUM_BUFFERS; i++)
  {
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
  for(byte i = 0; i < NUM_BUFFERS; i++)
  {
    buffer->set_display(i);
    buffer->reset_black_level();
    buffer->erase(true);                                                          
  }

  buffer->set_display(orig_display);

  do_fan(false);
}

void Commands::do_delay(int milliseconds)
{
  ::delay(milliseconds);
}

// todo: move to config
// arg0 - 10000 = 1.0, 9999 = 0.9999
// arg0 <= zero, reset to default
void Commands::set_fade_rate(int arg0)
{
  float rate;
  if(arg0 < 1)
    rate = FADE_RATE;
    //fade_effects->set_fade_rate(FADE_RATE);
  else
    rate = arg0/ 10000.0; 
    //fade_effects->set_fade_rate(arg0/ 10000.0);

    fade_effects->set_fade_rate(rate);
}

// arg[0] index of insertion pointer, default = 0
// arg[1] width of window, default = 1
//        -- any magic widths?
void Commands::set_position(int position, int width)
{
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
void Commands::random_position(int type)
{
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

int Commands::random_num(int max, int min)
{
  // handle special cases
  bool non_empty_only = false;
  bool empty_only = false;
  byte new_max = buffer->get_width(); 

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
      max = new_max; 
      break;

    case RANDOM_NUM_WIDTH_EMPTY:
      empty_only = true;
      max = new_max; 
      break;

    case  RANDOM_NUM_WIDTH: 
      max = new_max; 
      break;
  }

  if(empty_only || non_empty_only)
  {
    byte protection = 0;
    rgb_color * buf = buffer->get_buffer();
    do
    {
      byte pos = random(min, max); 
      rgb_color test_color = buf[pos];
      bool empty = ColorMath::equal(test_color, buffer->black);

      // eo  e   ne  r
      // 1   1   0   1
      // 1   0   1   0
      // 0   1   0   0
      // 0   0   1   1
      if(empty_only == empty)
        return pos;
    }
    while(protection++ <= max);
    
    return random(min, max);
  } 
  else 
    return random(min, max);  
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

int Commands::do_sequence(byte type, int arg0, int arg1, int arg2)
{
  
  if(arg1 > 0)
  {
    //                           num   high  low
    return do_set_sequence(type, arg0, arg1, arg2);
  } 
  else 
  {
    //                      num   adv   step
    return do_next_sequence(arg0, arg1, arg2);
  }
}

// returns the low value to begin the sequece
int Commands::do_set_sequence(byte type, int arg0, int arg1, int arg2)
{
  // values are entered in high,low order, so low can be skipped if zero
  // note the reverse argument order compared to do_next_sequence()
  //                         low   high
  sequencer->set(arg0, type, arg2, arg1);
  return arg2;
}

int Commands::do_next_sequence(int arg0, int arg1, int arg2)
{
  //                     num   adv   step
  return sequencer->next(arg0, arg1, arg2);
}

// advance the sequencer, then leave arg0 = position, arg1 = width
// for the position command to allow filling gaps
void Commands::do_next_window(int arg0, int arg1, int arg2)
{
  byte position = sequencer->next(arg0, arg1, arg2);
  byte previous_position = sequencer->previous_computed(arg0);
  sequencer->set_previous_computed(arg0, position);
  command_processor->sub_args[2] = 0;

  if(position == previous_position)
  {      
    // short-circuit - no change in position
    command_processor->sub_args[0] = position;
    command_processor->sub_args[1] = 0;
    return;
  }

  byte offset = position;
  byte width = 0;
  byte width_;

  if(position > previous_position)         // going up by at least one
  { 
    width_ = position - previous_position; // width needing drawing
    if(width_ > 0)
    {
      offset = previous_position;          // there's a gap, start right after the last position
      width = width_;
    }
  } 
  else                                     // going down
  {
    width_ = previous_position - position; // width needing drawing
    if(width_ > 0)                         // there's a gap, new position is beginning of range
      width = width_;                      // and width is already computed, though it might need a - 1
  } 

  command_processor->sub_args[0] = offset;
  command_processor->sub_args[1] = width;
}

#define CONFIG_SET_BLINK_PERIOD   0
#define CONFIG_SET_BREATHE_TIME   1
#define CONFIG_SET_FADE_RATE      2
#define CONFIG_SET_DEFAULT_EFFECT 3

// arg0 setting to configure
// arg1 value to set (0 = default)
void Commands::do_configure(int arg0, int arg1, int arg2)
{
  switch(arg0){
    case CONFIG_SET_BLINK_PERIOD:
      blink_effects->set_blink_period(arg1 == 0 ? BLINK_PERIOD : arg1);
      break;

    case CONFIG_SET_BREATHE_TIME:
      breathe_effects->set_breathe_time(arg1 == 0 ? BREATHE_PERIOD : arg1);
      break;

    case CONFIG_SET_FADE_RATE:
      set_fade_rate(arg1);
      break;

    case CONFIG_SET_DEFAULT_EFFECT:
      buffer->set_default_effect(arg1);
      break;
  }
}

bool Commands::do_set_macro(byte macro, byte * dispatch_data)
{
  //bool continue_dispatching = true;

  if(dispatch_data != NULL)
  {
    // being used internally
    macros.set_macro(macro, (char*)dispatch_data);          

    // signal that no more commands should be processed (rest of buffer copied to macro)
    return false;
  } 

  // being used over serial
  byte num_bytes = macros.set_macro_from_serial(macro);
  command_processor->send_int(num_bytes);
  return true;
}

void Commands::do_run_macro(byte macro, int times, int delay_)
{
  macros.run_macro(macro, times, delay_); 
}

#define COLOR_SEQUENCE_HUE 0
#define COLOR_SEQUENCE_SAT 1
#define COLOR_SEQUENCE_LIT 2

// to do need to add some limiting to not go over 255, etc.

void Commands::do_color_sequence(byte type, int arg0, int arg1, int arg2)
{
   rgb_color *palette = Colors::get_palette();

   switch(type)
  {
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
        for(byte i = 0; i < NUM_PALETTE_COLORS; i++)
        {
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

        int saturation = 0;
        for(byte i = 0; i < NUM_PALETTE_COLORS; i++)
        {         
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

        // skip ahead past the black color
        int lightness = arg1;
        
        for(byte i = 0; i < NUM_PALETTE_COLORS; i++)
        {
          rgb_color color = ColorMath::hsl_to_rgb(arg0, 255, lightness / 100);
          color.red = color.red * arg2 / 256;
          color.green = color.green * arg2 / 256;
          color.blue = color.blue * arg2 / 256;
          palette[i] = color;
          lightness = (lightness + arg1) % 25600;
        }
      }
      break;
  }
}

// arg[0] maximum random number (see Commands::random_num() for special constant values)
// arg[1] minimum random number (default=0)
// arg[2] copied into arg[1] to allow passing another argument
void Commands::do_random_number(int arg0, int arg1, int arg2)
{
  int ran = random_num(arg0, arg1);
  command_processor->sub_args[0] = ran;
  command_processor->sub_args[1] = arg2;
  command_processor->sub_args[2] = 0;
}

void Commands::do_stop()
{
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
void Commands::do_palette(int arg0, int arg1)
{
  rgb_color * palette = Colors::get_palette();

  if(arg1 > 0)
  {
    arg0 = max(0, arg0);
    for(int i = arg1; i >= arg0; i--)
      buffer->push_color(palette[i]);                      
  } 
  else 
    buffer->push_color(palette[arg0]);                                                      
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
void Commands::do_shuffle(int arg0, int arg1, int arg2)
{
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

#ifdef USE_EXTRA_SHUFFLES
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
#endif
  }    
}

// todo: optional
// todo: move to config?
void Commands::set_black_level(int arg0, int arg1, int arg2)
{
  rgb_color black_level = {(byte)arg0, (byte)arg1, (byte)arg2};
  buffer->set_black_level(black_level);
}

// maybe:
// with only one argument, push it, as in: acc2=acc1, acc1=acc0, acc0=arg0
// with two or three arguments, just store them to the accumulators

// core
void Commands::do_store(int arg0, int arg1, int arg2)
{
  int *accumulators = command_processor->accumulators;
//  int *sub_args = command_processor->sub_args;

  if(arg0 == 0){
    // no arguments supplied, clear accumulators  
    accumulators[0] = 0;
    accumulators[1] = 0;
    accumulators[2] = 0;
  } else if(arg1 == 0 && arg2 == 0){
    // only one argument supplied, push it in at acc0
    accumulators[2] = accumulators[1];
    accumulators[1] = accumulators[0];
    accumulators[0] = arg0;
  } else {
    accumulators[2] = arg2;
    accumulators[1] = arg1;
    accumulators[0] = arg0;

//    byte nargs = command_processor->get_num_args();
//    for(int i = 0; i < nargs; i++){
//      accumulators[i] = sub_args[i];  
//    }
  }
}

// arg0 = 0                             : (no argument supplied)       restore all arguments from the accumulators

// arg0 != 0 and arg1  = 0              : (only one argument supplied) arg0=acc0    arg1=arg0    arg2=acc1

// arg0 != 0 and arg1 != 0              : (two arguments supplied) 
//                                      :   arg1 shifts -> arg2, arg0 shifts -> arg1 
//                                      :   arg0 gets set based on arg2
//                                      :   arg2 = 0 - arg0 = accumulator0
//                                      :   arg2 = 1 - arg1 = accumulator1
//                                      :   arg2 = 2 - arg2 = accumulator2
void Commands::do_recall(int arg0, int arg1, int arg2)
{
  int *accumulators = command_processor->accumulators;
  int *sub_args = command_processor->sub_args;

  // no arguments supplied, restore all arguments
  if(arg0 == 0)
  {
      byte nargs = command_processor->get_num_args();
      for(int i = 0; i < nargs; i++)
        sub_args[i] = accumulators[i];
  }

  // one or more arguments supplied  
  else
  {
    // incoming arg0 gets shifts to arg1 in each case
    sub_args[1] = arg0;

      // only one argument supplied
    if(arg1 == 0)
    {
      // set arg0 and arg2 from accumulator 0 and 2
      sub_args[0] = accumulators[0];
      sub_args[2] = accumulators[1];
    }
    else
    {
      // two or three arguments supplied  
      // arg2 always gets set from arg1
      sub_args[2] = arg1;

      // only 0, 1 and 2 are valid
      arg2 = max(0, min(2, arg2));
      sub_args[0] = accumulators[arg2];
    } 
  }
}

// arg0 the color.red value for primary color
// arg1 the color.green value for secondary color
// arg2 the color.blue value for rendering type
void Commands::do_dynamic_color(byte arg0, byte arg1, byte arg2, byte effect){
  rgb_color data = {arg0, arg1, arg2};
  buffer->push_color(data, 1, false, DYNAMIC_COLOR | effect, 0, 0, false);
}

// todo: optional
void Commands::do_fan(bool fan_on, bool auto_set)
{
  if(auto_set){
    if(fan_on){
      // turn fan on  
      set_pin(FAN_PIN, true);
    } else {
      // turn fan off if not user-enabled
      if(!user_fan_on)
        set_pin(FAN_PIN, false);
    }
  } else {
    // set fan according to setting
    set_pin(FAN_PIN, fan_on);
    user_fan_on = fan_on;
  }
}

#ifdef USE_MAPPING
void Commands::do_xy_position(byte arg0, byte arg1)
{
  byte led = maps.get_led(arg0, arg1); 
  if(led != Maps::INVALID_POS){
    buffer->set_zone(0);
    set_position(led);
  }
}
#endif

#include "dispatch_command.h"

#include "testing.h"

#endif

