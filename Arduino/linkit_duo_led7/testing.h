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
//      do_test_palette(arg1, arg2);
      break;
    case TEST_TYPE_FUNCTION:
      // arg1 - function type
      // arg2 - (depends on function)
      do_test_function(arg1, arg2);
      break;
  }
}

// TODO only enable test code when testing

#define TEST_INQUIRY_NUM_LEDS     0
#define TEST_INQUIRY_WIDTH        1
#define TEST_INQUIRY_OFFSET       2
#define TEST_INQUIRY_WINDOW       3
//#define TEST_INQUIRY_NUM_ZONES    1
//#define TEST_INQUIRY_NUM_PALETTES 2
//#define TEST_INQUIRY_NUM_LEDS     3
// current offset, window, buffer, effects buffer

void Commands::do_test_inquiry(byte type, int arg2){
  switch(type){
    case TEST_INQUIRY_NUM_LEDS:
      command_processor->send_int(visible_led_count);
      break;
    case TEST_INQUIRY_OFFSET:
      command_processor->send_int(buffer->get_offset());
      break;
    case TEST_INQUIRY_WINDOW:
      command_processor->send_int(buffer->get_window());
      break;
  }
}

#define TEST_FUNCTION_PROCESS_EFFECTS   0
#define TEST_FUNCTION_PROCESS_SCHEDULES 1

void Commands::do_test_function(byte type, int arg2){
  switch(type){
    case TEST_FUNCTION_PROCESS_EFFECTS:
      effects_processor->process_effects();
      break;
    case TEST_FUNCTION_PROCESS_SCHEDULES:
      scheduler.process_schedules();
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

void Commands::do_test_buffer(byte start, byte count){
  rgb_color * buf = buffer->get_buffer();
  for(int i = 0; i < count; i++){
    command_processor->send_ints(buf[start + i].red);
    command_processor->send_ints(buf[start + i].green);
    command_processor->send_ints(buf[start + i].blue);
  }
}

void Commands::do_test_effects(byte start, byte count){
  byte * buf = buffer->get_effects_buffer();
  for(int i = 0; i < count; i++){
    command_processor->send_ints(buf[start + i]);
  }
}

void Commands::do_test_render(byte start, byte count){
  rgb_color * buf = buffer->get_render_buffer();
  for(int i = 0; i < count; i++){
    command_processor->send_ints(buf[start + i].red);
    command_processor->send_ints(buf[start + i].green);
    command_processor->send_ints(buf[start + i].blue);
  }
}

#endif

#endif

/*

simple
can set a color, can set a different color

CMD_FLUSH         1
renders the display buffer

CMD_ERASE         2
erases the render buffer

CMD_ROTATE        3
rotates the entire display
obeys direction
obeys step option
obeys flush option
obeys offset and window

CMD_REPEAT        4
repeats once
existing colors shifted
repeats multiple
includes effect 
works in reverse

CMD_COPY          5


CMD_FLOOD         6
CMD_MIRROR        7
CMD_DISPLAY       8
CMD_ZONE          9
CMD_WINDOW       10
CMD_OFFSET       11
CMD_REVERSE      12
CMD_RGBCOLOR     13
CMD_HSLCOLOR     14
CMD_RED          15
CMD_GREEN        16
CMD_BLUE         17
CMD_BLACK        18
CMD_YELLOW       19
CMD_ORANGE       20
CMD_PURPLE       21
CMD_CYAN         22
CMD_MAGENTA      23
CMD_PINK         24
CMD_WHITE        25
CMD_GRAY         26
CMD_LTGREEN      27
CMD_SEAFOAM      28
CMD_LTBLUE       29
CMD_DKGRAY       30
CMD_TUNGSTEN     31
CMD_AMBER        32
CMD_OLIVE        33
CMD_SKYBLUE      34
CMD_TURQUOISE    35
CMD_LAVENDER     36
CMD_ROSE         37
CMD_RANDOM       38
CMD_BLEND        39
CMD_MAX          40
CMD_DIM          41
CMD_BRIGHT       42
CMD_BLINK        43
CMD_BLINK1       44
CMD_BLINK2       45
CMD_BLINK3       46
CMD_BLINK4       47
CMD_BLINK5       48
CMD_BLINK6       49
CMD_BLINKR       50
CMD_BLINKA       51
CMD_BLINKB       52
CMD_BLINKC       53
CMD_BREATHE      54
CMD_SLOW_FADE    55
CMD_FAST_FADE    56
CMD_TWINKLE      57
CMD_RAW          58 
CMD_STATIC       59
CMD_EFFECTR      60
CMD_PAUSE        61
CMD_CONTINUE     62
CMD_RESET        63
CMD_CLEAR        64
CMD_LEVEL        65
CMD_FADE         66
CMD_WIPE         67
CMD_ANIM_ROTATE  68
CMD_PSHIFT       69
CMD_PSHIFTO      70
CMD_CFADE        71
CMD_TEST         72
CMD_HIPOWER_OPEN      73
CMD_PINON        74
CMD_PINOFF       75
CMD_SETFADERATE  76
CMD_SETBLINKC    77
CMD_SETBLINKP    78
CMD_SCHEDULE     79
CMD_CARRY        80
CMD_SETBREATHET  81
CMD_SET_MACRO    82
CMD_RUN_MACRO    83
CMD_DELAY        84
CMD_STOP         85
CMD_RANDOM_NUM   86
CMD_POSITION     87
CMD_RPOSITION    88
CMD_PALETTE      89
CMD_SHUFFLE      90
CMD_SETBLACK     91
CMD_SEQ_WHEEL    92
CMD_SEQ_SWING    93
CMD_SEQ_DRIVE    94
CMD_SEQ_BOUND    95
CMD_SEQ_WHLCO    96
CMD_SEQ_SWGCO    97
CMD_SEQ_WHLSN    98
CMD_SEQ_SWGSN    99


 
 */
