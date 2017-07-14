#ifndef TESTING_H
#define TESTING_H

#include "config.h"

#ifdef TEST_FRAMEWORK

#define TEST_TYPE_INQUIRY      0
#define TEST_TYPE_DUMP_MACRO   1
#define TEST_TYPE_DUMP_BUFFER  2
#define TEST_TYPE_DUMP_RENDER  3
#define TEST_TYPE_DUMP_EFFECTS 4
#define TEST_TYPE_DUMP_PALETTE 5
#define TEST_TYPE_FUNCTION     6

void Commands::do_test(int type, int arg1, int arg2){
  switch(type){
    case TEST_TYPE_INQUIRY:
      // arg1 - inquiry type
      // arg2 - (depends on inquiry)
      do_test_inquiry(arg1, arg2);
      break;
    case TEST_TYPE_DUMP_MACRO:
      // arg1 - macro number
      do_test_macro(arg1);
      break;
    case TEST_TYPE_DUMP_BUFFER:
      // arg1 - start
      // arg2 - count
      do_test_buffer(arg1, arg2);
      break;
    case TEST_TYPE_DUMP_RENDER:
      // arg1 - start
      // arg2 - count
      do_test_render(arg1, arg2);
      break;
    case TEST_TYPE_DUMP_EFFECTS:
      // arg1 - start
      // arg2 - count
      do_test_effects(arg1, arg2);
      break;
    case TEST_TYPE_DUMP_PALETTE:
      do_test_palette(arg1, arg2);
      break;
    case TEST_TYPE_FUNCTION:
      // arg1 - function type
      // arg2 - (depends on function)
      do_test_function(arg1, arg2);
      break;
  }
}

// TODO only enable test code when testing

#define TEST_INQUIRY_NUM_LEDS           0
#define TEST_INQUIRY_PALETTE_SIZE       1
#define TEST_INQUIRY_OFFSET             2
#define TEST_INQUIRY_WINDOW             3
#define TEST_INQUIRY_DEFAULT_BRIGHTNESS 4
#define TEST_INQUIRY_MINIMUM_BRIGHTNESS 5 
#define TEST_INQUIRY_REVERSE            6

// #define TEST_INQUIRY_WIDTH              1
//#define TEST_INQUIRY_DEVICENAME   4
//#define TEST_INQUIRY_NUM_ZONES    1
//#define TEST_INQUIRY_NUM_PALETTES 2

// current offset, window, buffer, effects buffer

void Commands::do_test_inquiry(byte type, int arg2){
  switch(type){
    case TEST_INQUIRY_NUM_LEDS:
      command_processor->send_int(visible_led_count);
      break;
    case TEST_INQUIRY_PALETTE_SIZE:
      command_processor->send_int(NUM_PALETTE_COLORS);
      break;
    case TEST_INQUIRY_OFFSET:
      command_processor->send_int(buffer->get_offset());
      break;
    case TEST_INQUIRY_WINDOW:
      command_processor->send_int(buffer->get_window());
      break;
    case TEST_INQUIRY_DEFAULT_BRIGHTNESS:
      command_processor->send_int(buffer->get_default_brightness());
      break;
    case TEST_INQUIRY_MINIMUM_BRIGHTNESS:
      command_processor->send_int(renderer->get_minimum_brightness());
      break;
    case TEST_INQUIRY_REVERSE:
      command_processor->send_int(buffer->get_reverse() ? 1 : 0);
      break;
  }
}

#define TEST_FUNCTION_PROCESS_EFFECTS   0 // arg2 = number of times to run
#define TEST_FUNCTION_PROCESS_SCHEDULES 1 // arg2 = number of times to run
#define TEST_FUNCTION_TIME_MACRO        2 // arg2 = macro to run and return the time in milliseconds
#define TEST_FUNCTION_RANDOM_SEED       3 // arg2 > 0 = a specific random seed for testing random features (0 = random seed)

void Commands::do_test_function(byte type, int arg2){
  switch(type){
    case TEST_FUNCTION_PROCESS_EFFECTS:
      {
        int times = max(1, arg2);
        for(int i = 0; i < times; i++)
          effects_processor->process_effects();
      }
      break;
    case TEST_FUNCTION_PROCESS_SCHEDULES:
      {
        int times = max(1, arg2);
        for(int i = 0; i < times; i++)
          scheduler.process_schedules();
      }
      break;
    case TEST_FUNCTION_TIME_MACRO:
      {
        unsigned long start = millis();
        macros.run_macro(arg2);
        unsigned long now = millis();
        command_processor->send_int(now - start);        
      }
      break;
    case TEST_FUNCTION_RANDOM_SEED:
      randomSeed(arg2);
      break;
  }  
}

void Commands::do_test_macro(byte macro_number){
  byte *position;
  byte b = macros.begin_dump_macro(macro_number, &position);
  command_processor->send_ints(b);
  for(byte i = 0; i < NUM_MACRO_CHARS - 1; i++){
    b = macros.continue_dump_macro(macro_number, &position);            
    command_processor->send_ints(b);
  }
}

void Commands::dump_buffer_colors(rgb_color * buffer, byte start, byte count, bool correct_color){
  for(int i = 0; i < count; i++){
    rgb_color color;
    if(correct_color){
      color = ColorMath::correct_color(buffer[start + i]);
    } else {
      color = buffer[start + i];
    }
    command_processor->send_ints(color.red);
    command_processor->send_ints(color.green);
    command_processor->send_ints(color.blue);
  }
}

void Commands::do_test_buffer(byte start, byte count){
  dump_buffer_colors(buffer->get_buffer(), start, count);
}

void Commands::do_test_effects(byte start, byte count){
  byte * buf = buffer->get_effects_buffer();
  for(int i = 0; i < count; i++){
    command_processor->send_ints(buf[start + i]);
  }
}

void Commands::do_test_render(byte start, byte count){
  dump_buffer_colors(buffer->get_render_buffer(), start, count);
}

void Commands::do_test_palette(byte start, byte count){
  dump_buffer_colors(Colors::get_palette(), start, count, false);
}

#endif
#endif

