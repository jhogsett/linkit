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
      erase(true);                                                          
      break;
    case CMD_BLINK:     
      start_effect(BLINK_ON);                                               
      break;
    case CMD_BLINK1:    
      start_effect(BLINK_ON_1);                                             
      break;
    case CMD_BLINK2:    
      start_effect(BLINK_ON_2);                                             
      break;
    case CMD_BLINK3:    
      start_effect(BLINK_ON_3);                                             
      break;
    case CMD_BLINK4:    
      start_effect(BLINK_ON_4);                                             
      break;
    case CMD_BLINK5:    
      start_effect(BLINK_ON_5);                                            
      break;
    case CMD_BLINK6:    
      start_effect(BLINK_ON_6);                                             
      break;
    case CMD_BLINKR:    
      start_blinking_r();                                                   
      break;
    case CMD_BLINKA:    
      start_effect(BLINK_ON_A);                                             
      break;
    case CMD_BLINKB:    
      start_effect(BLINK_ON_B);                                             
      break;
    case CMD_BREATHE:   
      start_effect(BREATHE_ON);                                             
      break;
    case CMD_EFFECTR:   
      start_effect_r();                                                     
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
      power_shift_object(command_processor.sub_args[0], command_processor.sub_args[1]); 
      command_processor.reset_args();           
      break;
    case CMD_WINDOW:    
      set_window(command_processor.sub_args[0]); 
      command_processor.reset_args();                                
      break;
    case CMD_RESET:     
      reset();                                                              
      break;
    case CMD_DEMO:      
      do_demo();                                                                                                                                    
      break;
    case CMD_STATIC:    
      start_effect(STATIC_ON);                                                                                                                      
      break;
    case CMD_RGBCOLOR:  
      push_rgb_color(command_processor.sub_args[0], command_processor.sub_args[1], command_processor.sub_args[2]); 
      command_processor.reset_args();  
      break;
    case CMD_HSLCOLOR:  
      push_hsl_color(command_processor.sub_args[0], command_processor.sub_args[1], command_processor.sub_args[2]); 
      command_processor.reset_args();  
      break;
    case CMD_RED:       
      push_color(red);                                                      
      break;
    case CMD_GREEN:     
      push_color(green);                                                    
      break;
    case CMD_BLUE:      
      push_color(blue);                                                     
      break;
    case CMD_BLACK:     
      push_color(black);                                                    
      break;
    case CMD_YELLOW:    
      push_color(yellow);                                                   
      break;
    case CMD_ORANGE:    
      push_color(orange);                                                   
      break;
    case CMD_PURPLE:    
      push_color(purple);                                                   
      break;
    case CMD_CYAN:      
      push_color(cyan);                                                     
      break;
    case CMD_MAGENTA:   
      push_color(magenta);                                                  
      break; 
    case CMD_PINK:      
      push_color(pink);                                                     
      break; 
    case CMD_WHITE:     
      push_color(white);                                                    
      break; 
    case CMD_GRAY:      
      push_color(gray);                                                     
      break;
    case CMD_LTGREEN:   
      push_color(ltgreen);                                                  
      break;
    case CMD_SEAFOAM:   
      push_color(seafoam);                                                  
      break;
    case CMD_LTBLUE:    
      push_color(ltblue);                                                   
      break;
    case CMD_DKGRAY:    
      push_color(dkgray);                                                   
      break;
    case CMD_REPEAT:    
      do_repeat(command_processor.sub_args[0]); 
      command_processor.reset_args();                                 
      break;
  }
}

