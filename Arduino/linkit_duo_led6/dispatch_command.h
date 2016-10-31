extern CommandProcessor command_processor;

void dispatch_command(int cmd){
  switch(cmd){
    case CMD_NONE:      
      command_processor.save_args();                                        
      break;
    case CMD_PAUSE:     
      paused = true;                                                        
      break;
    case CMD_CONTINUE:  
      paused = false;                                                       
      break;
    case CMD_ERASE:     
      buffer.erase(true);                                                          
      break;
    case CMD_BLINK:     
      effects_processor.start_effect(BLINK_ON);                                               
      break;
    case CMD_BLINK1:    
      effects_processor.start_effect(BLINK_ON_1);                                             
      break;
    case CMD_BLINK2:    
      effects_processor.start_effect(BLINK_ON_2);                                             
      break;
    case CMD_BLINK3:    
      effects_processor.start_effect(BLINK_ON_3);                                             
      break;
    case CMD_BLINK4:    
      effects_processor.start_effect(BLINK_ON_4);                                             
      break;
    case CMD_BLINK5:    
      effects_processor.start_effect(BLINK_ON_5);                                            
      break;
    case CMD_BLINK6:    
      effects_processor.start_effect(BLINK_ON_6);                                             
      break;
    case CMD_BLINKR:    
      effects_processor.start_blinking_r();                                                   
      break;
    case CMD_BLINKA:    
      effects_processor.start_effect(BLINK_ON_A);                                             
      break;
    case CMD_BLINKB:    
      effects_processor.start_effect(BLINK_ON_B);                                             
      break;
    case CMD_BREATHE:   
      effects_processor.start_effect(BREATHE_ON);                                             
      break;
    case CMD_EFFECTR:   
      effects_processor.start_effect_r();                                                     
      break;
    case CMD_FLUSH:     
      flush();                                                              
      break;
    case CMD_BLEND:     
      do_blend();                                                           
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
    case CMD_FADE:      
      do_fade();                                                            
      break;
    case CMD_EFADE:     
      do_exhale_fade();                                                     
      break;
    case CMD_FLOOD:     
      do_flood();                                                           
      break;
    case CMD_RANDOM:    
      do_random();                                                          
      break; 
    case CMD_MIRROR:    
      do_mirror();                                                          
      break;
    case CMD_ESHIFT:    
      do_elastic_shift(command_processor.sub_args[0]); 
      command_processor.reset_args();                          
      break;
    case CMD_PSHIFT:    
      do_power_shift(command_processor.sub_args[0]);  
      command_processor.reset_args();                            
      break;
    case CMD_PSHIFTO:   
      do_power_shift_object(command_processor.sub_args[0], command_processor.sub_args[1]); 
      command_processor.reset_args();           
      break;
    case CMD_WINDOW:    
      buffer.set_window(command_processor.sub_args[0]); 
      command_processor.reset_args();                                
      break;
    case CMD_RESET:     
      reset();                                                              
      break;
    case CMD_DEMO:      
      do_demo();                                                                                                                                    
      break;
    case CMD_STATIC:    
      effects_processor.start_effect(STATIC_ON);                                                                                                                      
      break;
    case CMD_RGBCOLOR:  
      buffer.push_rgb_color(command_processor.sub_args[0], command_processor.sub_args[1], command_processor.sub_args[2]); 
      command_processor.reset_args();  
      break;
    case CMD_HSLCOLOR:  
      buffer.push_hsl_color(command_processor.sub_args[0], command_processor.sub_args[1], command_processor.sub_args[2]); 
      command_processor.reset_args();  
      break;
    case CMD_RED:       
      buffer.push_color(red);                                                      
      break;
    case CMD_GREEN:     
      buffer.push_color(green);                                                    
      break;
    case CMD_BLUE:      
      buffer.push_color(blue);                                                     
      break;
    case CMD_BLACK:     
      buffer.push_color(black);                                                    
      break;
    case CMD_YELLOW:    
      buffer.push_color(yellow);                                                   
      break;
    case CMD_ORANGE:    
      buffer.push_color(orange);                                                   
      break;
    case CMD_PURPLE:    
      buffer.push_color(purple);                                                   
      break;
    case CMD_CYAN:      
      buffer.push_color(cyan);                                                     
      break;
    case CMD_MAGENTA:   
      buffer.push_color(magenta);                                                  
      break; 
    case CMD_PINK:      
      buffer.push_color(pink);                                                     
      break; 
    case CMD_WHITE:     
      buffer.push_color(white);                                                    
      break; 
    case CMD_GRAY:      
      buffer.push_color(gray);                                                     
      break;
    case CMD_LTGREEN:   
      buffer.push_color(ltgreen);                                                  
      break;
    case CMD_SEAFOAM:   
      buffer.push_color(seafoam);                                                  
      break;
    case CMD_LTBLUE:    
      buffer.push_color(ltblue);                                                   
      break;
    case CMD_DKGRAY:    
      buffer.push_color(dkgray);                                                   
      break;
    case CMD_REPEAT:    
      do_repeat(command_processor.sub_args[0]); 
      command_processor.reset_args();                                 
      break;
  }
}

