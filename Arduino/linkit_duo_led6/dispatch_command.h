#include "dependencies.h"
extern Dependencies dependencies;

void dispatch_command(int cmd){
  switch(cmd){
    case CMD_NONE:      
      dependencies.command_processor.save_args();                                        
      break;
    case CMD_PAUSE:     
      dependencies.commands.pause();
      break;
    case CMD_CONTINUE:  
      dependencies.commands.resume();
      break;
    case CMD_ERASE:     
      dependencies.buffer.erase(true);                                                          
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
    case CMD_BREATHE:   
      dependencies.effects_processor.start_effect(BREATHE_ON);                                             
      break;
    case CMD_EFFECTR:   
      dependencies.effects_processor.start_effect_r();                                                     
      break;
    case CMD_FLUSH:     
      dependencies.commands.flush(true);                                                              
      break;
    case CMD_BLEND:     
      dependencies.commands.do_blend();                                                           
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
    case CMD_FADE:      
      dependencies.commands.do_fade();                                                            
      break;
    case CMD_WIPE:     
      dependencies.commands.do_wipe();                                                     
      break;
    case CMD_FLOOD:     
      dependencies.commands.do_flood();                                                           
      break;
    case CMD_RANDOM:    
      dependencies.commands.do_random();                                                          
      break; 
    case CMD_MIRROR:    
      dependencies.commands.do_mirror();                                                          
      break;
    case CMD_ESHIFT:    
      dependencies.commands.do_elastic_shift(dependencies.command_processor.sub_args[0]); 
      dependencies.command_processor.reset_args();                          
      break;
    case CMD_PSHIFT:    
      dependencies.commands.do_power_shift(dependencies.command_processor.sub_args[0]);  
      dependencies.command_processor.reset_args();                            
      break;
    case CMD_PSHIFTO:   
      dependencies.commands.do_power_shift_object(dependencies.command_processor.sub_args[0], dependencies.command_processor.sub_args[1]); 
      dependencies.command_processor.reset_args();           
      break;
    case CMD_WINDOW:    
      dependencies.buffer.set_window(dependencies.command_processor.sub_args[0]); 
      dependencies.command_processor.reset_args();                                
      break;
    case CMD_RESET:     
      dependencies.commands.reset();                                                              
      break;
    case CMD_DEMO:      
      dependencies.commands.do_demo();                                                                                                                                    
      break;
    case CMD_STATIC:    
      dependencies.effects_processor.start_effect(STATIC_ON);                                                                                                                      
      break;
    case CMD_RGBCOLOR:  
      dependencies.buffer.push_rgb_color(dependencies.command_processor.sub_args[0], dependencies.command_processor.sub_args[1], dependencies.command_processor.sub_args[2]); 
      dependencies.command_processor.reset_args();  
      break;
    case CMD_HSLCOLOR:  
      dependencies.buffer.push_hsl_color(dependencies.command_processor.sub_args[0], dependencies.command_processor.sub_args[1], dependencies.command_processor.sub_args[2]); 
      dependencies.command_processor.reset_args();  
      break;
    case CMD_RED:       
      dependencies.buffer.push_color(red);                                                      
      break;
    case CMD_GREEN:     
      dependencies.buffer.push_color(green);                                                    
      break;
    case CMD_BLUE:      
      dependencies.buffer.push_color(blue);                                                     
      break;
    case CMD_BLACK:     
      dependencies.buffer.push_color(black);                                                    
      break;
    case CMD_YELLOW:    
      dependencies.buffer.push_color(yellow);                                                   
      break;
    case CMD_ORANGE:    
      dependencies.buffer.push_color(orange);                                                   
      break;
    case CMD_PURPLE:    
      dependencies.buffer.push_color(purple);                                                   
      break;
    case CMD_CYAN:      
      dependencies.buffer.push_color(cyan);                                                     
      break;
    case CMD_MAGENTA:   
      dependencies.buffer.push_color(magenta);                                                  
      break; 
    case CMD_PINK:      
      dependencies.buffer.push_color(pink);                                                     
      break; 
    case CMD_WHITE:     
      dependencies.buffer.push_color(white);                                                    
      break; 
    case CMD_GRAY:      
      dependencies.buffer.push_color(gray);                                                     
      break;
    case CMD_LTGREEN:   
      dependencies.buffer.push_color(ltgreen);                                                  
      break;
    case CMD_SEAFOAM:   
      dependencies.buffer.push_color(seafoam);                                                  
      break;
    case CMD_LTBLUE:    
      dependencies.buffer.push_color(ltblue);                                                   
      break;
    case CMD_DKGRAY:    
      dependencies.buffer.push_color(dkgray);                                                   
      break;
    case CMD_REPEAT:    
      dependencies.commands.do_repeat(dependencies.command_processor.sub_args[0]); 
      dependencies.command_processor.reset_args();                                 
      break;
  }
}

