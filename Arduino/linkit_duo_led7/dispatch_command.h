#include "dependencies.h"
#include "macros.h"

extern Dependencies dependencies;

void dispatch_command(int cmd){
  bool reset_args = false;
  
  switch(cmd){
    case CMD_NONE:      
      dependencies.command_processor.save_args();                                        
      break;
    case CMD_FLUSH:     
      dependencies.commands.flush(true);                                                              
      break;
    case CMD_ERASE:     
      dependencies.buffer.erase(false);                                                          
      break;
    case CMD_ROTATE:
      dependencies.commands.do_rotate(dependencies.command_processor.sub_args[0]); 
      reset_args = true;
      break;
    case CMD_REPEAT:    
      dependencies.commands.do_repeat(dependencies.command_processor.sub_args[0]); 
      reset_args = true;
      break;
    case CMD_COPY:   
      dependencies.commands.do_copy(dependencies.command_processor.sub_args[0], dependencies.command_processor.sub_args[1]); 
      reset_args = true;
      break;
    case CMD_FLOOD:     
      dependencies.commands.do_flood();                                                           
      break;
    case CMD_MIRROR:    
      dependencies.commands.do_mirror();                                                          
      break;
    case CMD_DISPLAY:    
      dependencies.commands.set_display(dependencies.command_processor.sub_args[0]); 
      reset_args = true;
      break;
    case CMD_ZONE:
      dependencies.buffer.set_zone(dependencies.command_processor.sub_args[0]); 
      reset_args = true;
      break;
    case CMD_WINDOW:    
      dependencies.buffer.set_window_override(dependencies.command_processor.sub_args[0]); 
      reset_args = true;
      break;
    case CMD_OFFSET:
      dependencies.buffer.set_offset_override(dependencies.command_processor.sub_args[0]); 
      reset_args = true;
      break;
    case CMD_REVERSE:
      dependencies.buffer.set_reverse(dependencies.command_processor.sub_args[0] == 1 ? true : false); 
      reset_args = true;
      break;
    case CMD_RGBCOLOR:  
      dependencies.buffer.push_rgb_color(dependencies.command_processor.sub_args[0], dependencies.command_processor.sub_args[1], dependencies.command_processor.sub_args[2]); 
      reset_args = true;
      break;
    case CMD_HSLCOLOR:  
      dependencies.buffer.push_hsl_color(dependencies.command_processor.sub_args[0], dependencies.command_processor.sub_args[1], dependencies.command_processor.sub_args[2]); 
      reset_args = true;
      break;
    case CMD_RED:       
      dependencies.buffer.push_color(RED);                                                      
      break;
    case CMD_GREEN:     
      dependencies.buffer.push_color(GREEN);                                                    
      break;
    case CMD_BLUE:      
      dependencies.buffer.push_color(BLUE);                                                     
      break;
    case CMD_BLACK:     
      dependencies.buffer.push_color(BLACK);                                                    
      break;
    case CMD_YELLOW:    
      dependencies.buffer.push_color(YELLOW);                                                   
      break;
    case CMD_ORANGE:    
      dependencies.buffer.push_color(ORANGE);                                                   
      break;
    case CMD_PURPLE:    
      dependencies.buffer.push_color(PURPLE);                                                   
      break;
    case CMD_CYAN:      
      dependencies.buffer.push_color(CYAN);                                                     
      break;
    case CMD_MAGENTA:   
      dependencies.buffer.push_color(MAGENTA);                                                  
      break; 
    case CMD_PINK:      
      dependencies.buffer.push_color(PINK);                                                     
      break; 
    case CMD_WHITE:     
      dependencies.buffer.push_color(WHITE);                                                    
      break; 
    case CMD_GRAY:      
      dependencies.buffer.push_color(GRAY);                                                     
      break;
    case CMD_LTGREEN:   
      dependencies.buffer.push_color(LTGREEN);                                                  
      break;
    case CMD_SEAFOAM:   
      dependencies.buffer.push_color(SEAFOAM);                                                  
      break;
    case CMD_LTBLUE:    
      dependencies.buffer.push_color(LTBLUE);                                                   
      break;
    case CMD_DKGRAY:    
      dependencies.buffer.push_color(DKGRAY);                                                   
      break;
    case CMD_TUNGSTEN:    
      dependencies.buffer.push_color(TUNGSTEN);                                                   
      break;
    case CMD_RANDOM:    
      dependencies.commands.do_random(dependencies.command_processor.sub_args[0]);                                                          
      reset_args = true;
      break; 
    case CMD_BLEND:     
      dependencies.commands.do_blend(dependencies.command_processor.sub_args[0]);                                                           
      reset_args = true;
      break;
    case CMD_MAX:         
      dependencies.commands.do_max();                                                             
      break;
    case CMD_DIM:       
      dependencies.commands.do_dim();                                                             
      break;
    case CMD_BRIGHT:    
      dependencies.commands.do_bright();                                                          
      break;
    case CMD_BLINK:     
      dependencies.effects_processor.start_effect(BLINK_ON);                                               
      break;
    case CMD_BLINK1:    
      dependencies.effects_processor.start_effect(BLINK_ON_1);                                             
      break;
    case CMD_BLINK2:    
      dependencies.effects_processor.start_effect(BLINK_ON_2);                                             
      break;
    case CMD_BLINK3:    
      dependencies.effects_processor.start_effect(BLINK_ON_3);                                             
      break;
    case CMD_BLINK4:    
      dependencies.effects_processor.start_effect(BLINK_ON_4);                                             
      break;
    case CMD_BLINK5:    
      dependencies.effects_processor.start_effect(BLINK_ON_5);                                            
      break;
    case CMD_BLINK6:    
      dependencies.effects_processor.start_effect(BLINK_ON_6);                                             
      break;
    case CMD_BLINKR:    
      dependencies.effects_processor.start_blinking_r();                                                   
      break;
    case CMD_BLINKA:    
      dependencies.effects_processor.start_effect(BLINK_ON_A);                                             
      break;
    case CMD_BLINKB:    
      dependencies.effects_processor.start_effect(BLINK_ON_B);                                             
      break;
    case CMD_BLINKC:
      dependencies.effects_processor.start_effect(BLINK_ON_C);                                               
      break;
    case CMD_BREATHE:   
      dependencies.effects_processor.start_effect(BREATHE_ON);                                             
      break;
    case CMD_EFFECTR:   
      dependencies.effects_processor.start_effect_r();                                                     
      break;
    case CMD_STATIC:    
      dependencies.effects_processor.start_effect(STATIC_ON);                                                                                                                      
      break;
    case CMD_PAUSE:     
      dependencies.commands.pause();
      break;
    case CMD_CONTINUE:  
      dependencies.commands.resume();
      break;
    case CMD_RESET:     
      dependencies.commands.reset();                                                              
      break;
    case CMD_CLEAR:      
      dependencies.commands.clear();                                                            
      break;
    case CMD_LEVEL:
      dependencies.commands.set_brightness_level(dependencies.command_processor.sub_args[0]); 
      reset_args = true;
      break;
    case CMD_FADE:      
      dependencies.commands.do_fade();                                                            
      break;
    case CMD_WIPE:     
      dependencies.commands.do_wipe();                                                     
      break;
    case CMD_ESHIFT:    
      dependencies.commands.do_elastic_shift(dependencies.command_processor.sub_args[0]); 
      reset_args = true;
      break;
    case CMD_PSHIFT:    
      dependencies.commands.do_power_shift(dependencies.command_processor.sub_args[0]);  
      reset_args = true;
      break;
    case CMD_PSHIFTO:   
      dependencies.commands.do_power_shift_object(dependencies.command_processor.sub_args[0], dependencies.command_processor.sub_args[1]); 
      reset_args = true;
      break;
    case CMD_CFADE:      
      dependencies.commands.do_crossfade();                                                            
      break;
    case CMD_LOPOWER:
      dependencies.commands.low_power();                                               
      break;
    case CMD_HIPOWER:
      dependencies.commands.high_power();                                               
      break;
    case CMD_PINON:    
      dependencies.commands.set_pin(dependencies.command_processor.sub_args[0], true); 
      reset_args = true;
      break;
    case CMD_PINOFF:    
      dependencies.commands.set_pin(dependencies.command_processor.sub_args[0], false); 
      reset_args = true;
      break;
    case CMD_DEMO:      
      dependencies.commands.do_demo();                                                                                                                                    
      break;
    case CMD_SETBLINKC:
      dependencies.blink_effects.set_custom_blink(dependencies.command_processor.sub_args[0]);
      reset_args = true;
      break;
    case CMD_SETBLINKP:
      dependencies.blink_effects.set_blink_period(dependencies.command_processor.sub_args[0]);
      reset_args = true;
      break;
    case CMD_SCHEDULE:
      // set or clear a schedule
      break;
    case CMD_CARRY:
      dependencies.buffer.push_carry_color();
      break;
    case CMD_SETBREATHET:
      dependencies.breathe_effects.set_breathe_time(dependencies.command_processor.sub_args[0]);
      reset_args = true;
      break;
    case CMD_SET_MACRO:
      ::set_macro(dependencies.command_processor.sub_args[0]);
      reset_args = true;
      break;
    case CMD_RUN_MACRO:
      ::run_macro(dependencies.command_processor.sub_args[0], dependencies.command_processor.sub_args[1]);
      reset_args = true;
      break;
  }

  if(reset_args)
    dependencies.command_processor.reset_args();                                 
}


