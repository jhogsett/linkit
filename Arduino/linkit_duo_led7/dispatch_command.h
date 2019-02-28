#ifndef DISPATCH_COMMAND_H
#define DISPATCH_COMMAND_H

bool Commands::dispatch_command(int cmd, byte *dispatch_data){
  bool continue_dispatching = true;
  bool reset_args = true;
  int arg0 = command_processor->sub_args[0];
  int arg1 = command_processor->sub_args[1];
  int arg2 = command_processor->sub_args[2];
 
  switch(cmd){
    case CMD_NONE: 
      command_processor->save_args(); 
      reset_args = false; 
      break;

    case CMD_FLUSH: 
      flush(true); 
      break;

#define ERASE_SLOW_ALL 0
#define ERASE_FAST_BUFFER 1
#define ERASE_FAST_ALL 2      
    case CMD_ERASE:
      if(arg0 == ERASE_SLOW_ALL)
        buffer->erase(false); 
      else
        buffer->fast_erase(arg0 == ERASE_FAST_ALL ? true : false);
      break;
  
    case CMD_ROTATE: 
      do_rotate(arg0, arg1, false); 
      break;
      
    case CMD_REPEAT: 
      do_repeat(arg0); 
      break;
      
    case CMD_COPY: 
      do_copy(arg0, arg1, arg2); 
      break;
    
    case CMD_FLOOD: 
      do_flood(arg0); 
      break;
    
    case CMD_MIRROR: 
      do_mirror(); 
      break;
 
    case CMD_DISPLAY: 
      set_display(arg0); 
      break;

    case CMD_ZONE: 
      buffer->set_zone(arg0); 
      break;

    case CMD_WINDOW: 
      buffer->set_window_override(arg0, true); 
      break;
      
    case CMD_OFFSET: 
      buffer->set_offset_override(arg0, true); 
      break;
      
    case CMD_REVERSE: 
      buffer->set_reverse(arg0 == 1 ? true : false); 
      break;
      
    case CMD_RGBCOLOR: 
      buffer->push_rgb_color(arg0, arg1, arg2); 
      break;
    
    case CMD_HSLCOLOR: 
      buffer->push_hsl_color(arg0, arg1, arg2); 
      break;
  
    case CMD_RED: 
    case CMD_ORANGE: 
    case CMD_YELLOW: 
    case CMD_GREEN: 
    case CMD_BLUE: 
    case CMD_PURPLE: 
    case CMD_CYAN: 
    case CMD_MAGENTA: 
    case CMD_LTBLUE: 
    case CMD_LTGREEN: 
    case CMD_SEAFOAM: 
    case CMD_PINK: 
    case CMD_AMBER: 
    case CMD_OLIVE: 
    case CMD_SKYBLUE: 
    case CMD_TURQUOISE: 
    case CMD_LAVENDER: 
    case CMD_ROSE: 
    case CMD_DKGRAY: 
    case CMD_GRAY: 
    case CMD_WHITE: 
    case CMD_TUNGSTEN: 
    case CMD_NEON: 
    case CMD_SODIUM: 
      dispatch_color(cmd, arg0, arg1);
      break; 
  
    case CMD_BLACK: 
      buffer->push_color(buffer->black, arg0, arg1); 
      break; 
 
    case CMD_RANDOM: 
      do_random(arg0, arg1); 
      break; 

    case CMD_DYN_COLOR:
      do_dynamic_color(arg0, arg1, arg2);
      break;
    
    case CMD_BLEND: 
#ifdef USE_BLEND
      do_blend(arg0); 
#endif
      break;
    
//    case CMD_MAX: 
//      break;
    
    case CMD_DIM: 
      do_dim(arg0); 
      break;
    
    case CMD_BRIGHT: 
      do_bright(arg0); 
      break;

//    case CMD_STATIC: 
    
    case CMD_BLINK: 
    case CMD_BLINK1: 
    case CMD_BLINK2: 
    case CMD_BLINK3: 
    case CMD_BLINK4: 
    case CMD_BLINK5: 
    case CMD_BLINK6: 
    case CMD_BLINKA: 
    case CMD_BLINKB: 
    case CMD_BLINKD:
    case CMD_BREATHE: 
    case CMD_BREATHE_D:
    case CMD_SLOW_FADE: 
    case CMD_FAST_FADE: 
      dispatch_effect(cmd);
      break;

    case CMD_STORE: 
      do_store(arg0, arg1, arg2);
      reset_args = false; 
      break;
  
    case CMD_RECALL: 
      do_recall(arg0, arg1, arg2);
      reset_args = false; 
      break;

    case CMD_PUSH:
      do_push(arg0, arg1, arg2);
      break;
    
    case CMD_PAUSE: 
      pause(arg0); 
      break;

    case CMD_CONTINUE: 
      resume(arg0); 
      break;
      
    case CMD_RESET: 
      reset(); 
      break;
    
    case CMD_CLEAR: 
      clear(); 
      break;
    
    case CMD_LEVEL: 
      set_brightness_level(arg0, arg1); 
      break;
    
//    case CMD_FADE: 
//      do_fade(); 
//      break;
    
    case CMD_ANIM_ROTATE: 
      do_rotate(arg0, arg1, true); 
      break;
  
    case CMD_XYPOSITION:
 #ifdef USE_MAPPING
     do_xy_position(arg0, arg1, arg2);
#endif
      break;
  
    case CMD_CFADE: 
      do_crossfade(); 
      break;
    
//    case CMD_BLINKR: 
//      break;
//    
//    case CMD_EFFECTR: 
//      break;
    
    case CMD_TEST: 
      do_test(arg0, arg1, arg2); 
      break;
    
    case CMD_CONFIGURE: 
      do_configure(arg0, arg1); 
      break;
    
    case CMD_PIN: 
      set_pin(arg0, arg1); 
      break;

    case CMD_SCHEDULE: 
      scheduler.set_schedule(arg0, arg1); 
      break;
    
    case CMD_CARRY: 
      buffer->push_carry_color(); 
      break;
    
    case CMD_SET_MACRO: 
      continue_dispatching = do_set_macro(arg0, dispatch_data); 
      break;
    
    case CMD_RUN_MACRO: 
      do_run_macro(arg0, arg1, arg2);
      break;
    
    case CMD_DELAY: 
      do_delay(arg0); 
      break;
    
    case CMD_STOP: 
      do_stop();
      break; 
    
    case CMD_RANDOM_NUM: 
      do_random_number(arg0, arg1, arg2); 
      reset_args = false; 
      break;
    
    case CMD_POSITION: 
      set_position(arg0, arg1); 
      break;
    
    case CMD_RPOSITION: 
      random_position(arg0); 
      break;
    
    case CMD_PALETTE: 
      do_palette(arg0, arg1); 
      break;
    
    case CMD_SHUFFLE: 
      do_shuffle(arg0, arg1, arg2); 
      break;
    
    case CMD_SETBLACK: 
      set_black_level(arg0, arg1, arg2); 
      break;
  
    case CMD_SEQ_WHEEL: 
    case CMD_SEQ_SWING: 
    case CMD_SEQ_WHLCO: 
    case CMD_SEQ_WHLSN: 
      command_processor->reset_args();
      command_processor->sub_args[0] = do_sequence(SEQUENCE_WHEEL + (cmd - CMD_SEQ_WHEEL), arg0, arg1, arg2); 
      reset_args = false; 
      break; 
  
    case CMD_SEQ_NEXTW:
      do_next_window(arg0, arg1, arg2); 
      reset_args = false; 
      break;
  
  case CMD_CLR_SEQ_HUE:
  case CMD_CLR_SEQ_SAT:
  case CMD_CLR_SEQ_LIT:
    do_color_sequence(COLOR_SEQUENCE_HUE + (cmd - CMD_CLR_SEQ_HUE), arg0, arg1, arg2); 
    break;

  case CMD_FAN:
    do_fan(arg0);
    break;

  case CMD_APP:
    do_app_setup();
    break;

  case CMD_ADD:
  case CMD_SUB:
  case CMD_MUL:
  case CMD_DIV:
  case CMD_MOD:
  case CMD_DIF:
  case CMD_AVG:
    dispatch_math(cmd);
    reset_args = false; 
    break;

  case CMD_DRAW_MODE:
    buffer->set_draw_mode(arg0);
    break;

  case CMD_POP:
    do_pop(arg0);
    reset_args = false; 
    break;    
  }
  
  if(reset_args)
    command_processor->reset_args(); 
  
  return continue_dispatching;
}


// arg0: times
// arg1: 
void Commands::dispatch_color(byte cmd, int arg0, int arg1){
  buffer->push_color(*Colors::get_color(cmd - CMD_RED), arg0, arg1); 
}

void Commands::dispatch_effect(byte cmd){
  effects_processor->start_effect((cmd - CMD_FIRST_EFFECT) + FIRST_EFFECT); 
}

void Commands::dispatch_math(byte cmd){
  do_math((cmd - CMD_ADD) + MATH_ADD);
}

#endif
