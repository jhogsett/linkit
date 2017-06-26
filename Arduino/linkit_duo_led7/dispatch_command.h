#ifndef DISPATCH_COMMAND_H
#define DISPATCH_COMMAND_H

bool Commands::dispatch_command(int cmd, byte *dispatch_data){
  bool continue_dispatching = true;
  bool reset_args = false;
  int arg0 = command_processor->sub_args[0];
  int arg1 = command_processor->sub_args[1];
  int arg2 = command_processor->sub_args[2];
  
  switch(cmd){
    case CMD_NONE:        command_processor->save_args();                break;
    case CMD_FLUSH:       flush(true);                                   break;
    case CMD_ERASE:       buffer->erase(false);                          break;
    case CMD_ROTATE:      do_rotate(arg0, arg1, false);                  reset_args = true; break;
    case CMD_REPEAT:      do_repeat(arg0); reset_args = true;            break;
    case CMD_COPY:        do_copy(arg0, arg1, arg2);                     reset_args = true; break;
    case CMD_FLOOD:       do_flood();                                    break;
    case CMD_MIRROR:      do_mirror();                                   break;
    case CMD_DISPLAY:     set_display(arg0);                             reset_args = true; break;
    case CMD_ZONE:        buffer->set_zone(arg0);                        reset_args = true; break;
    case CMD_WINDOW:      buffer->set_window_override(arg0);             reset_args = true; break;
    case CMD_OFFSET:      buffer->set_offset_override(arg0);             reset_args = true; break;
    case CMD_REVERSE:     buffer->set_reverse(arg0 == 1 ? true : false); reset_args = true; break;
    case CMD_RGBCOLOR:    buffer->push_rgb_color(arg0, arg1, arg2);      reset_args = true; break;
    case CMD_HSLCOLOR:    buffer->push_hsl_color(arg0, arg1, arg2);      reset_args = true; break;
    case CMD_RED:       
    case CMD_GREEN:     
    case CMD_BLUE:      
    case CMD_BLACK:     
    case CMD_YELLOW:    
    case CMD_ORANGE:    
    case CMD_PURPLE:    
    case CMD_CYAN:      
    case CMD_MAGENTA:   
    case CMD_PINK:      
    case CMD_WHITE:     
    case CMD_GRAY:      
    case CMD_LTGREEN:   
    case CMD_SEAFOAM:   
    case CMD_LTBLUE:    
    case CMD_DKGRAY:    
    case CMD_TUNGSTEN:    
    case CMD_AMBER:
    case CMD_OLIVE:
    case CMD_SKYBLUE:
    case CMD_TURQUOISE:
    case CMD_LAVENDER:
    case CMD_ROSE:        dispatch_color(cmd);                         reset_args = true; break;
    case CMD_RANDOM:      do_random(arg0);                             reset_args = true; break;   
    case CMD_BLEND:       do_blend(arg0);                              reset_args = true; break;
    case CMD_MAX:         do_max();                                    break;
    case CMD_DIM:         do_dim();                                    break;
    case CMD_BRIGHT:      do_bright();                                 break;
    case CMD_BLINK:     
    case CMD_BLINK1:    
    case CMD_BLINK2:    
    case CMD_BLINK3:    
    case CMD_BLINK4:    
    case CMD_BLINK5:    
    case CMD_BLINK6:    
    case CMD_BLINKA:    
    case CMD_BLINKB:    
    case CMD_BLINKC:
    case CMD_BREATHE:   
    case CMD_SLOW_FADE:
    case CMD_FAST_FADE:
    case CMD_TWINKLE:
    case CMD_RAW:
    case CMD_STATIC:      dispatch_effect(cmd);                        break;
    case CMD_BLINKR:      effects_processor->start_blinking_r();       break;
    case CMD_EFFECTR:     effects_processor->start_effect_r();         break;
    case CMD_PAUSE:       pause(arg0);                                 reset_args = true; break;
    case CMD_CONTINUE:    resume(arg0);                                reset_args = true; break;
    case CMD_RESET:       reset();                                     break;
    case CMD_CLEAR:       clear();                                     break;
    case CMD_LEVEL:       set_brightness_level(arg0);                  reset_args = true; break;
    case CMD_FADE:        do_fade();                                   break;
    case CMD_WIPE:        do_wipe();                                   break;
    case CMD_ANIM_ROTATE: do_rotate(arg0, arg1, true);                 reset_args = true; break;
    case CMD_PSHIFT:      do_power_shift(arg0);                        reset_args = true; break;
    case CMD_PSHIFTO:     do_power_shift_object(arg0, arg1);           reset_args = true; break;
    case CMD_CFADE:       do_crossfade();                              break;
    case CMD_TEST:        do_test(arg0, arg1, arg2);                   reset_args = true; break;
    case CMD_CONFIGURE:   do_configure(arg0, arg1, arg2);              break;
    case CMD_PINON:       set_pin(arg0, true);                         reset_args = true; break;
    case CMD_PINOFF:      set_pin(arg0, false);                        reset_args = true; break;
    case CMD_SCHEDULE:    scheduler.set_schedule(arg0, arg1, arg2);    reset_args = true; break;
    case CMD_CARRY:       buffer->push_carry_color();                  break;
    case CMD_SET_MACRO:
      continue_dispatching = do_set_macro(arg0, dispatch_data);
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
      reset_args = true;
      break;
    case CMD_RUN_MACRO: macros.run_macro(arg0, arg1, arg2); reset_args = true; break;
    case CMD_DELAY: do_delay(arg0); reset_args = true; break;
    case CMD_STOP:
      scheduler.reset_all_schedules();
      clear();                                                            
      pause();                                                            
      break;      
    case CMD_RANDOM_NUM:
      {
        // arg[0] maximum random number (see Commands::random_num() for special constant values)
        // arg[1] minimum random number (default=0)
        // arg[2] copied into arg[1] to allow passing another argument
        int ran = random_num(arg0, arg1);
        command_processor->sub_args[0] = ran;
        command_processor->sub_args[1] = arg2;
        command_processor->sub_args[2] = 0;
        // don't reset arguments
      }
      break;
    case CMD_POSITION: set_position(arg0, arg1); reset_args = true; break;
    case CMD_RPOSITION: random_position(arg0); reset_args = true; break;
    case CMD_PALETTE:
      {
        // arg[0] the index into the palette of the color to insert, or where to stop rubberstamp insert
        // arg[1] if > 0, colors are inserted counting down from this position
        //                the counting down is done so the palette achieves a left-to-right order when inserted  
        //                in this case arg[0] is the stopping point when counting down
        // for example: a rainbow is 0,5:pal, whole palette: 0,17:pal
        
        if(arg1 > 0){
          arg0 = max(0, arg0);
          rgb_color * palette = Colors::get_palette();
          for(byte i = arg1; i >= arg0; i--){
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
        rgb_color black_level = {(byte)arg0, (byte)arg1, (byte)arg2};
        buffer->set_black_level(black_level);
        break;
      }
    case CMD_SEQ_WHEEL:
    case CMD_SEQ_SWING:
    case CMD_SEQ_WHLCO:
    case CMD_SEQ_SWGCO:
    case CMD_SEQ_WHLSN:
    case CMD_SEQ_SWGSN:
    case CMD_SEQ_WHLPW:
    case CMD_SEQ_SWGPW:
      dispatch_sequence(cmd);
      // don't reset args
      break;
    case CMD_SEQ_NEXTW:
      command_processor->sub_args[0] = do_next_window(arg0, &command_processor->sub_args[1], arg2);
      command_processor->sub_args[2] = 0;
      // don't reset args
      break;
    case CMD_SEQ_NEXTM:
      break;
    case CMD_CLR_SEQ_HUE:
      // arg0 - step angle, default = 20
      //   if arg0 < 0, the order is reverse
      // arg1 - starting hue angle 0-359, default = 0 (red)
      // arg2 - lightness, default = 255
      // (saturation = 255)
      // do_color_sequence(COLOR_SEQUENCE_HUE, arg0, arg1, arg2)
      break;
    case CMD_CLR_SEQ_LIT:
      // arg0 - step, default = 256 / 18 = 14.22222222 (magic value, others must be integers)
      //   if arg0 if < 0, the order is reversed
      // arg1 - hue angle 0-359, default = 0 (red)
      // arg2 - saturation 0-255, default = 255
      // (starting percent = 0)
      // do_color_sequence(COLOR_SEQUENCE_LIGHTNESS, arg0, arg1, arg2)
      
      break;
    case CMD_CLR_SEQ_SAT:
      // arg0 - step, default = 256 / 18 = 14.22222222 (magic value, others must be integers)
      //   if arg0 if < 0, the order is reversed
      // arg1 - hue angle 0-359, default = 0 (red)
      // arg2 - lightness 0-255, default = 255
      // do_color_sequence(COLOR_SEQUENCE_SATURATION, arg0, arg1, arg2)
      break;
  }

  if(reset_args)
    command_processor->reset_args();   

  return continue_dispatching;
}

void Commands::dispatch_color(int cmd){
  rgb_color color = BLACK;

  switch(cmd){
    case CMD_RED:       color = RED;       break;
    case CMD_GREEN:     color = GREEN;     break;
    case CMD_BLUE:      color = BLUE;      break;     
    case CMD_BLACK:     color = buffer->black; break;
    case CMD_YELLOW:    color = YELLOW;    break;
    case CMD_ORANGE:    color = ORANGE;    break;
    case CMD_PURPLE:    color = PURPLE;    break;    
    case CMD_CYAN:      color = CYAN;      break; 
    case CMD_MAGENTA:   color = MAGENTA;   break;  
    case CMD_PINK:      color = PINK;      break;
    case CMD_WHITE:     color = WHITE;     break;
    case CMD_GRAY:      color = GRAY;      break;      
    case CMD_LTGREEN:   color = LTGREEN;   break;
    case CMD_SEAFOAM:   color = SEAFOAM;   break;
    case CMD_LTBLUE:    color = LTBLUE;    break;   
    case CMD_DKGRAY:    color = DKGRAY;    break;
    case CMD_TUNGSTEN:  color = TUNGSTEN;  break;   
    case CMD_AMBER:     color = AMBER;     break;
    case CMD_OLIVE:     color = OLIVE;     break;
    case CMD_SKYBLUE:   color = SKYBLUE;   break;
    case CMD_TURQUOISE: color = TURQUOISE; break;
    case CMD_LAVENDER:  color = LAVENDER;  break;
    case CMD_ROSE:      color = ROSE;      break;
  }
  
  buffer->push_color(color, command_processor->sub_args[0], command_processor->sub_args[1]);                                                   
}

void Commands::dispatch_effect(int cmd){
//  effects_map[] = { BLINK_ON,   BLINK_ON_1, BLINK_ON_2, BLINK_ON_3, BLINK_ON_4,    
//                    BLINK_ON_5, BLINK_ON_6, BLINK_ON_A, BLINK_ON_B, BLINK_ON_C,
//                    BREATHE_ON, SLOW_FADE,  FAST_FADE,  TWINKLE_ON, RAW_ON,
//                    STATIC_ON };
//  effects_processor->start_effect(effects_map[cmd - CMD_BLINK]);

  byte effect = NO_EFFECT;
  switch(cmd){
    case CMD_BLINK:     effect = BLINK_ON;   break;
    case CMD_BLINK1:    effect = BLINK_ON_1; break;
    case CMD_BLINK2:    effect = BLINK_ON_2; break;    
    case CMD_BLINK3:    effect = BLINK_ON_3; break;    
    case CMD_BLINK4:    effect = BLINK_ON_4; break;    
    case CMD_BLINK5:    effect = BLINK_ON_5; break;   
    case CMD_BLINK6:    effect = BLINK_ON_6; break;   
    case CMD_BLINKA:    effect = BLINK_ON_A; break;   
    case CMD_BLINKB:    effect = BLINK_ON_B; break;   
    case CMD_BLINKC:    effect = BLINK_ON_C; break;
    case CMD_BREATHE:   effect = BREATHE_ON; break;
    case CMD_SLOW_FADE: effect = SLOW_FADE;  break;
    case CMD_FAST_FADE: effect = FAST_FADE;  break;
    case CMD_TWINKLE:   effect = TWINKLE_ON; break;
    case CMD_RAW:       effect = RAW_ON;     break;
    case CMD_STATIC:    effect = STATIC_ON;  break;
  }
   
  effects_processor->start_effect(effect);
}

void Commands::dispatch_sequence(int cmd){
  byte type = SEQUENCE_WHEEL;

  switch(cmd){
    case CMD_SEQ_WHEEL: type = SEQUENCE_WHEEL;        break;
    case CMD_SEQ_SWING: type = SEQUENCE_SWING;        break;
    case CMD_SEQ_WHLCO: type = SEQUENCE_WHEEL_COSINE; break;
    case CMD_SEQ_SWGCO: type = SEQUENCE_SWING_COSINE; break;
    case CMD_SEQ_WHLSN: type = SEQUENCE_WHEEL_SINE;   break;
    case CMD_SEQ_SWGSN: type = SEQUENCE_SWING_SINE;   break;
    case CMD_SEQ_WHLPW: type = SEQUENCE_WHEEL_POWER;  break;
    case CMD_SEQ_SWGPW: type = SEQUENCE_SWING_POWER;  break;
  }
    command_processor->sub_args[0] = do_sequence(type, command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
    command_processor->sub_args[1] = 0;
    command_processor->sub_args[2] = 0;
}

#endif
