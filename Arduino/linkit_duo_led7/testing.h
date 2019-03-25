#ifndef TESTING_H
#define TESTING_H

#include "config.h"

#define TEST_TYPE_INQUIRY      0
#define TEST_TYPE_DUMP_MACRO   1
#define TEST_TYPE_DUMP_BUFFER  2
#define TEST_TYPE_DUMP_RENDER  3
#define TEST_TYPE_DUMP_EFFECTS 4
#define TEST_TYPE_DUMP_PALETTE 5
#define TEST_TYPE_FUNCTION     6
#define TEST_TYPE_DUMP_ACCUMS  7

#define TEST_INQUIRY_NUM_LEDS           0
#define TEST_INQUIRY_PALETTE_SIZE       1
#define TEST_INQUIRY_OFFSET             2
#define TEST_INQUIRY_WINDOW             3
#define TEST_INQUIRY_DEFAULT_BRIGHTNESS 4
#define TEST_INQUIRY_MINIMUM_BRIGHTNESS 5 
#define TEST_INQUIRY_REVERSE            6
#define TEST_INQUIRY_DEFAULT_FADE_RATE  7
#define TEST_INQUIRY_FADE_RATE          8
#define TEST_INQUIRY_MAPPING_ENABLED    9
#define TEST_INQUIRY_DEFAULT_LIGHTNESS 10
#define TEST_INQUIRY_MINIMUM_LIGHTNESS 11
#define TEST_INQUIRY_MAX_STRING_LENGTH 12
#define TEST_INQUIRY_TEST_FRAMEWORK_ENABLED 13
#define TEST_INQUIRY_EXTRA_SHUFFLES_ENABLED 14
#define TEST_INQUIRY_BLEND_ENABLED     15
#define TEST_INQUIRY_NUM_MACRO_CHARS   16
#define TEST_INQUIRY_NUM_MEMORY_MACROS 17
#define TEST_INQUIRY_NUM_EEPROM_MACROS 18
#define TEST_INQUIRY_FIRST_EEPROM_MACRO 19
#define TEST_INQUIRY_NUM_SEQUENCERS 20
#define TEST_INQUIRY_NUM_FINE_ZONES 21
#define TEST_INQUIRY_FIXED_PALETTE_SIZE 22

void Commands::do_test_inquiry(byte type)
{
  int result = 0;
  switch(type)
  {
    case TEST_INQUIRY_NUM_LEDS:
      result = visible_led_count;
      break;
    case TEST_INQUIRY_PALETTE_SIZE:
      result = NUM_PALETTE_COLORS;
      break;
    case TEST_INQUIRY_OFFSET:
      result = buffer->get_offset();
      break;
    case TEST_INQUIRY_WINDOW:
      result = buffer->get_window();
      break;
    case TEST_INQUIRY_DEFAULT_BRIGHTNESS:
      result = buffer->get_default_brightness();
      break;
    case TEST_INQUIRY_MINIMUM_BRIGHTNESS:
      result = renderer->get_minimum_brightness();
      break;
    case TEST_INQUIRY_REVERSE:
      result = buffer->get_reverse() ? 1 : 0;
      break;
    case TEST_INQUIRY_DEFAULT_FADE_RATE:
      // fade rate passed x 10K
      result = FADE_RATE * 10000;
      break;
    case TEST_INQUIRY_FADE_RATE:
      result = fade_effects->get_fade_rate() * 10000;
      break;
    case TEST_INQUIRY_MAPPING_ENABLED:
#ifdef USE_MAPPING
      result = 1;
#else
      result = 0;
#endif
      break;

    case TEST_INQUIRY_DEFAULT_LIGHTNESS:
      result = (255 * (buffer->get_default_brightness() / 100.0)) + 1;
      break;

    case TEST_INQUIRY_MINIMUM_LIGHTNESS:
      result = (255 * (renderer->get_minimum_brightness() / 100.0)) + 1;
      break;

    case TEST_INQUIRY_MAX_STRING_LENGTH:
      result = command_processor->get_max_string_length();
      break;
    case TEST_INQUIRY_TEST_FRAMEWORK_ENABLED:
#ifdef USE_TEST_FRAMEWORK
      result = 1;
#else
      result = 0;
#endif
      break;
    case TEST_INQUIRY_EXTRA_SHUFFLES_ENABLED:
#ifdef USE_EXTRA_SHUFFLES
      result = 1;
#else
      result = 0;
#endif
      break;
    case TEST_INQUIRY_BLEND_ENABLED:
#ifdef USE_BLEND
      result = 1;
#else
      result = 0;
#endif
      break;

    case TEST_INQUIRY_NUM_MACRO_CHARS:
      result = NUM_MACRO_CHARS;
      break;
    
    case TEST_INQUIRY_NUM_MEMORY_MACROS:
      result = NUM_MEMORY_MACROS;
      break;

    case TEST_INQUIRY_NUM_EEPROM_MACROS:
      result = NUM_EEPROM_MACROS;
      break;

    case TEST_INQUIRY_FIRST_EEPROM_MACRO:
      result = NUM_MEMORY_MACROS;
      break;
    case TEST_INQUIRY_NUM_SEQUENCERS:
      result = NUM_SEQUENCERS;
      break;

    case TEST_INQUIRY_NUM_FINE_ZONES:
      result = FINE_ZONES;
      break;

    case TEST_INQUIRY_FIXED_PALETTE_SIZE:
      result = NUM_COLORS;
      break;
    }  

  command_processor->send_int(result);    
}

void Commands::do_test(int type, int arg1, int arg2)
{
  switch(type)
  {
    case TEST_TYPE_INQUIRY:
      // arg1 - inquiry type
      // arg2 - (depends on inquiry)
      do_test_inquiry(arg1);
      break;
#ifdef USE_TEST_FRAMEWORK
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
    case TEST_TYPE_DUMP_ACCUMS:
      do_test_accumulators();
      break;
#endif
  }
}

// TODO only enable test code when testing

// blink period, breathe period, crossfade delay, command buffer size
 
// #define TEST_INQUIRY_WIDTH              1
//#define TEST_INQUIRY_DEVICENAME   4
//#define TEST_INQUIRY_NUM_ZONES    1
//#define TEST_INQUIRY_NUM_PALETTES 2

// # displays

// current offset, window, buffer, effects buffer


// to compute HSL lightness to match standard color brightness:
// 0,4:tst - get brightness percent
// lightness-max = (255 * (brightness percent / 100)) + 1
//command: 0,4:tst
//15k
//command: era:grn:120,255,38:hsl:2,0,2:tst
//0,19,0,0,20,0,k
//command: era:grn:120,255,39:hsl:2,0,2:tst
//0,20,0,0,20,0,k
// --> have an inquiry that returns the necessary value!

#ifdef USE_TEST_FRAMEWORK
#define TEST_FUNCTION_PROCESS_EFFECTS   0 // arg2 = number of times to run
#define TEST_FUNCTION_PROCESS_SCHEDULES 1 // arg2 = number of times to run
#define TEST_FUNCTION_TIME_MACRO        2 // arg2 = macro to run and return the time in milliseconds
#define TEST_FUNCTION_RANDOM_SEED       3 // arg2 > 0 = a specific random seed for testing random features (0 = random seed)

void Commands::do_test_function(byte type, int arg2)
{
  switch(type)
  {
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

void Commands::do_test_macro(byte macro_number)
{
  byte *position;
  byte b = macros.begin_dump_macro(macro_number, &position);
  command_processor->send_ints(b);

  // WHY IS THIS 76 BYTES?
  
  for(byte i = 0; i < NUM_MACRO_CHARS - 1; i++)
  {
    b = macros.continue_dump_macro(macro_number, &position);            
    command_processor->send_ints(b);
  }
}

void Commands::dump_buffer_colors(rgb_color * buffer, byte start, byte count, bool correct_color)
{
  // WHY IS THIS 216 BYTES?

  // 60 bytes for the for loop?
  for(int i = 0; i < count; i++)
  {
    rgb_color color;
    if(correct_color)
      color = ColorMath::correct_color(buffer[start + i]);
    else 
      color = buffer[start + i];

    // 27772
    command_processor->send_ints(color.red); // 12        // leaving only this one saves 44 bytes?
    command_processor->send_ints(color.green); // 22?   // leaving only this one saves 4 bytes?
    command_processor->send_ints(color.blue); // 22?      // leaving only this one saves 4 bytes?

//    color->red;

    // 27794
//    command_processor->send_ints(*(byte*)buffer+0); // 12        // leaving only this one saves 44 bytes?
//    command_processor->send_ints(*(byte*)buffer+1); // 22?   // leaving only this one saves 4 bytes?
//    command_processor->send_ints(*(byte*)buffer+2); // 22?      // leaving only this one saves 4 bytes?
  
  
  }
}

void Commands::do_test_buffer(byte start, byte count)
{
  dump_buffer_colors(buffer->get_buffer(), start, count);
}

void Commands::do_test_effects(byte start, byte count)
{
  byte * buf = buffer->get_effects_buffer();
  for(int i = 0; i < count; i++)
    command_processor->send_ints(buf[start + i]);
}

void Commands::do_test_render(byte start, byte count)
{
  dump_buffer_colors(buffer->get_render_buffer(), start, count);
}

void Commands::do_test_palette(byte start, byte count)
{
  dump_buffer_colors(Colors::get_palette(), start, count, false);
}

void Commands::do_test_accumulators(){
  byte nargs = command_processor->get_num_args();
  for(int i = 0; i < nargs; i++)
    command_processor->send_ints(command_processor->accumulators[i]);
}

#endif
#endif
