#ifndef DISPATCH_COMMAND_H
#define DISPATCH_COMMAND_H

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
      do_rotate(command_processor->sub_args[0], command_processor->sub_args[1], false); 
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
      pause(command_processor->sub_args[0]);
      reset_args = true;
      break;
    case CMD_CONTINUE:  
      resume(command_processor->sub_args[0]);
      reset_args = true;
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
    case CMD_ANIM_ROTATE:    
      // arg[0] # times to rotate, default = 1
      // arg[1] # rotation steps each time, default = 1
      do_rotate(command_processor->sub_args[0], command_processor->sub_args[1], true); 
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
    case CMD_TEST:
      do_test(command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      reset_args = true;
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
    case CMD_SETFADERATE:      
      set_fade_rate(command_processor->sub_args[0]);                                                                                                                                    
      reset_args = true;
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
      scheduler.set_schedule(command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
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
      scheduler.reset_all_schedules();
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
    case CMD_SEQ_WHEEL:
      command_processor->sub_args[0] = do_sequence(SEQUENCE_WHEEL, command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      command_processor->sub_args[1] = 0;
      command_processor->sub_args[2] = 0;
      // don't reset args
      break;
    case CMD_SEQ_SWING:
      command_processor->sub_args[0] = do_sequence(SEQUENCE_SWING, command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      command_processor->sub_args[1] = 0;
      command_processor->sub_args[2] = 0;
      // don't reset args
      break;
    case CMD_SEQ_DRIVE:
      command_processor->sub_args[0] = do_sequence(SEQUENCE_DRIVE, command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      command_processor->sub_args[1] = 0;
      command_processor->sub_args[2] = 0;
      // don't reset args
      break;
    case CMD_SEQ_BOUND:
      command_processor->sub_args[0] = do_sequence(SEQUENCE_BOUND, command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      command_processor->sub_args[1] = 0;
      command_processor->sub_args[2] = 0;
      // don't reset args
      break;
    case CMD_SEQ_WHLSN:
      command_processor->sub_args[0] = do_sequence(SEQUENCE_WHEEL_SINE, command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      command_processor->sub_args[1] = 0;
      command_processor->sub_args[2] = 0;
      // don't reset args
      break;
      
    case CMD_SEQ_SWGSN:
      command_processor->sub_args[0] = do_sequence(SEQUENCE_SWING_SINE, command_processor->sub_args[0], command_processor->sub_args[1], command_processor->sub_args[2]);
      command_processor->sub_args[1] = 0;
      command_processor->sub_args[2] = 0;
      // don't reset args
      break;
  }

  if(reset_args)
    command_processor->reset_args();   

  return continue_dispatching;
}
#endif
