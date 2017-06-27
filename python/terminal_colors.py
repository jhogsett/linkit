def black(text):                                                                                                                                                        
  return "\x1b[30m" + text + normal()                                                                                                        
                                                                                                                                                                                                           
def red(text):                                                                                                                                                                                             
  return "\x1b[31m" + text + normal()                                                                                                                                   
                                                                                                                                                                                                           
def green(text):                                                                                                                                                                                           
  return "\x1b[32m" + text + normal()                                                                                                                                                                      
                                                                                                                                                    
def yellow(text):                                                                                                                                                                                          
  return "\x1b[33m" + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def blue(text):                                                                                                                                                                                            
  return "\x1b[34m" + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def magenta(text):                                                                                                                                                                                         
  return "\x1b[35m" + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def cyan(text):                                                                                                                                                                                            
  return "\x1b[36m" + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def white(text):                                                                                                                                                                                           
  return "\x1b[37m" + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def normal():                                                                                                                                                                                              
  return "\x1b[39m"                                                                                                                                                                                        
                                                                                                                                                                                                           
def black(text):                                                                                                                                                        
  return "\x1b[40m" + text + normal()                                                                                                                                                                      
                                                                                                                                                                                                           
def redbg(text):                                                                                                                                                        
  return "\x1b[41m" + text + normalbg()                                                                                                                                 
                                                                                                                                                                        
def greenbg(text):                                                                                                                                                      
  return "\x1b[42m" + text + normalbg()                                                                                                                                 
                                                                                                                                                                                                           
def yellowbg(text):                                                                                                                                                                                        
  return "\x1b[43m" + text + normalbg()                                                                                                                                 
                                                                                                                                                                        
def bluebg(text):                                                                                              
  return "\x1b[44m" + text + normalbg()                                                                                                      
                                                                                                                                             
def magentabg(text):                                                                                                                                                                                       
  return "\x1b[45m" + text + normalbg()                                                                                                             
                                                                                                                                                                        
def cyanbg(text):                                                                                                                                                                                          
  return "\x1b[46m" + text + normalbg()                                                                                                             
                                                                                                                                             
def whitebg(text):                                                                                             
  return "\x1b[47m" + text + normalbg()                                                                                                                                                                    
                                                                                                                                                    
def normalbg():                                                                                                                                                                                            
  return "\x1b[49m"   

