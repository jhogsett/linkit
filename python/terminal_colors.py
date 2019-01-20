
global no_colors_mode
no_colors_mode = None

def begin(no_colors=False):
  global no_colors_mode
  no_colors_mode = no_colors

def color_code(code):
  if no_colors_mode:
    return ''
  else:
    return "\x1b[" + str(code) + "m"

def black(text):                                                                                                                                                        
  return color_code(30) + text + normal()                                                                                                        
                                                                                                                                                                                                           
def red(text):                                                                                                                                                                                             
  return color_code(31) + text + normal()                                                                                                                                   
                                                                                                                                                                                                           
def green(text):                                                                                                                                                                                           
  return color_code(32) + text + normal()                                                                                                                                                                      
                                                                                                                                                    
def yellow(text):                                                                                                                                                                                          
  return color_code(33) + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def blue(text):                                                                                                                                                                                            
  return color_code(34) + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def magenta(text):                                                                                                                                                                                         
  return color_code(35) + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def cyan(text):                                                                                                                                                                                            
  return color_code(36) + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def white(text):                                                                                                                                                                                           
  return color_code(37) + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def normal():                                                                                                                                                                                              
  return color_code(39)                                                                                                                                                                                        
                                                                                                                                                                                                           
def blackbg(text):                                                                                                                                                        
  return color_code(40) + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def redbg(text):                                                                                                                                                        
  return color_code(41) + text + normalbg()                                                                                                                                 
                                                                                                                                                                        
def greenbg(text):                                                                                                                                                      
  return color_code(42) + text + normalbg()                                                                                                                                 
                                                                                                                                                                                                           
def yellowbg(text):                                                                                                                                                                                        
  return color_code(43) + text + normalbg()                                                                                                                                 
                                                                                                                                                                        
def bluebg(text):                                                                                              
  return color_code(44) + text + normalbg()                                                                                                      
                                                                                                                                             
def magentabg(text):                                                                                                                                                                                       
  return color_code(45) + text + normalbg()                                                                                                             
                                                                                                                                                                        
def cyanbg(text):                                                                                                                                                                                          
  return color_code(46) + text + normalbg()                                                                                                             
                                                                                                                                             
def whitebg(text):                                                                                             
  return color_code(47) + text + normalbg()                                                                                                                                                                    
                                                                                                                                                    
def normalbg():                                                                                                                                                                                            
  return color_code(49)   

#bold SGR1             Turn bold mode on                      ^[[1m
#lowint SGR2           Turn low intensity mode on             ^[[2m
#underline SGR4        Turn underline mode on                 ^[[4m
#blink SGR5            Turn blinking mode on                  ^[[5m
#reverse SGR7          Turn reverse video on                  ^[[7m
#invisible SGR8        Turn invisible text mode on            ^[[8m
#clearscreen ED2       Clear entire screen                    ^[[2J

