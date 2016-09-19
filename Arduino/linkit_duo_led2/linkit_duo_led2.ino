
#include <PololuLedStrip.h>

PololuLedStrip<12> ledStrip;

#define LED_COUNT 72
#define MAX_LED (LED_COUNT)
rgb_color colors[LED_COUNT];
rgb_color render[LED_COUNT];

int states[LED_COUNT];

#define BRIGHTNESS_DIVISOR 20.0
#define DEFAULT_BRIGHTNESS_PERCENT 20

// Standard colors
rgb_color black = {0, 0, 0};
rgb_color gray = {10, 10, 10};
rgb_color white = {20, 20, 20};
rgb_color red = {20, 0, 0};
rgb_color orange = {20, 10, 0};
rgb_color yellow = {20, 20, 0};
rgb_color ltgreen = {10, 20, 0};
rgb_color green = {0, 20, 0};
rgb_color seafoam = {0, 20, 10};
rgb_color cyan = {0, 20, 20};
rgb_color ltblue = {0, 10, 20};
rgb_color blue = {0, 0, 20};
rgb_color purple = {10, 0, 20};
rgb_color magenta = {20, 0, 20};
rgb_color pink = {20, 0, 10};

#define NPALETTE 6
#define NPRETTY_COLORS 6
rgb_color palette[NPALETTE] = { red, orange, yellow, green, blue, purple };
//rgb_color palette[NPALETTE] = { red, orange, pink, magenta,      yellow, green, seafoam, ltgreen,       ltblue, cyan, blue, purple,        white }; 

rgb_color adjusted_palette[NPALETTE];

rgb_color scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale, 
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
}

void set_brightness(int brightness_percent = DEFAULT_BRIGHTNESS_PERCENT){
  float percent = brightness_percent / 100.0;
  for(int i = 0; i < NPALETTE; i++){
    adjusted_palette[i] = scale_color(palette[i], percent);
  }
}

void flood(rgb_color color){
  for(int i = 0; i < LED_COUNT; i++){
    colors[i] = color;
  }
}

void erase(bool display = false){
  for(int i = 0; i < LED_COUNT; i++){
    colors[i] = black;
    states[i] = 0;
  }
  if(display){
    ledStrip.write(colors, LED_COUNT); 
  }
}

rgb_color add_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = min(255, color1.red + color2.red);
  new_color.green = min(255, color1.green + color2.green);
  new_color.blue = min(255, color1.blue + color2.blue);
  return new_color;
}

rgb_color subtract_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = max(0, color1.red - color2.red);
  new_color.green = max(0, color1.green - color2.green);
  new_color.blue = max(0, color1.blue - color2.blue);
  return new_color;
}

void setup_colors(bool swap = true){
  if(swap == true){
    for(int i = 0; i < NPALETTE; i++){
      unsigned char value = palette[i].red;
      palette[i].red = palette[i].green;  
      palette[i].green = value;
    }
  }
  memcpy(adjusted_palette, palette, sizeof(palette));
}

#define NO_STATE 0
#define BLINK_ON 1
//#define BLINK_OFF 2

void push_color(rgb_color color, bool display = false){
  for(int i = MAX_LED - 1; i >= 1; i--){
    colors[i] = colors[i-1];
    states[i] = states[i-1];
  }
  colors[0] = color;
  states[0] = NO_STATE;
  
  if(display){
    ledStrip.write(colors, LED_COUNT); 
  }
}

#define MAX_BLINK 20000
int blink_counter = 0;
bool blink_state = false;

void start_blinking(){
  states[0] = BLINK_ON;
}

//void toggle_blink_state(){
//  for(int i = 0; i < LED_COUNT; i++){
//    if(states[i] == BLINK_ON){
//      states[i] = BLINK_OFF;
//    } else if(states[i] == BLINK_OFF){
//      states[i] = BLINK_ON;
//    }
//  }
//}

void setup_buffer(){
  for(int i = 0; i < LED_COUNT; i++){
    if(states[i] == BLINK_ON){
      if(blink_state){
        render[i] = colors[i];  
      } else {
        render[i] = black;  
      }
    } else {
      render[i] = colors[i];
    }
  }
}

void display_buffer(){
  ledStrip.write(render, LED_COUNT);  
}

void flush(){
  setup_buffer();
  display_buffer();
}

void setup() { 
  Serial.begin(115200);  // open serial connection to USB Serial 
                         //port(connected to your computer)
  Serial1.begin(57600);  // open internal serial connection to 
                         //MT7688
  //pinMode(13, OUTPUT); // in MT7688, this maps to device 
    
  setup_colors(false);
  set_brightness();
  erase(true);
}

#define MAX_STRING_LENGTH 10

void loop() { 
  char str[MAX_STRING_LENGTH];
  rgb_color color;

  if(Serial1.available() > 0){
    int c = Serial1.readBytesUntil('\0', str, MAX_STRING_LENGTH);
    str[c] = 0;

    // reset the blink state so the automatic render won't interfere
    blink_state = false;
    blink_counter = 0;

    if(strcmp(str, "erase") == 0){
      erase(true);
    }
    else if(strcmp(str, "red") == 0){
      push_color(red);
    }
    else if(strcmp(str, "green") == 0){
      push_color(green);
    }
    else if(strcmp(str, "blue") == 0){
      push_color(blue);
    }
    else if(strcmp(str, "black") == 0){
      push_color(black);
    }
    else if(strcmp(str, "yellow") == 0){
      push_color(yellow);
    }
    else if(strcmp(str, "orange") == 0){
      push_color(orange);
    }
    else if(strcmp(str, "purple") == 0){
      push_color(purple);
    }
    else if(strcmp(str, "cyan") == 0){
      push_color(cyan);
    }
    else if(strcmp(str, "magenta") == 0){
      push_color(magenta);
    }
    else if(strcmp(str, "pink") == 0){
      push_color(pink);
    }
    else if(strcmp(str, "white") == 0){
      push_color(white);
    }
    else if(strcmp(str, "gray") == 0){
      push_color(gray);
    }
    else if(strcmp(str, "ltgreen") == 0){
      push_color(ltgreen);
    }
    else if(strcmp(str, "seafoam") == 0){
      push_color(seafoam);
    }
    else if(strcmp(str, "ltblue") == 0){
      push_color(ltblue);
    }
    else if(strcmp(str, "blink") == 0){
      start_blinking();
    }
    else if(strcmp(str, "flush") == 0){
      flush();
    }
  } else {
    blink_counter = (blink_counter + 1) % MAX_BLINK;
    if(blink_counter == 0){
      if(blink_state){
        blink_state = false;
      } else {
        blink_state = true;
      }
      flush();
    }
  }
}

