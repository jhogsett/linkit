
#include <PololuLedStrip.h>

PololuLedStrip<12> ledStrip;

#define LED_COUNT 72
#define MAX_LED (LED_COUNT)
rgb_color colors[LED_COUNT];

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
  flood(black);
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

//// Render an object
//void draw(rgb_color color, int pos, int set, int id){
//  //int warped = WARPED[pos];
//  int final_pos = pos + (set * VLED_COUNT);
//  colors[final_pos] = add_color(colors[final_pos], color);
//  occupy_position(pos, set, id);
//}
//
//// Unrender an object
//void undraw(rgb_color color, int pos, int set, int id){
//  //int warped = WARPED[pos];
//  //colors[warped] = subtract_color(colors[warped], color);
//  vacate_position(pos, set, id);
//}

void push_color(rgb_color color, bool display = false){
  for(int i = MAX_LED - 1; i >= 1; i--){
    colors[i] = colors[i-1];
  }
  colors[0] = color;
  if(display){
    ledStrip.write(colors, LED_COUNT); 
  }
}


void setup() { 
  Serial.begin(115200);  // open serial connection to USB Serial 
                         //port(connected to your computer)
  Serial1.begin(57600);  // open internal serial connection to 
                         //MT7688
  pinMode(13, OUTPUT); // in MT7688, this maps to device 
    
  setup_colors(false);
  set_brightness();
  erase(true);
}

#define MAX_STRING_LENGTH 10

void loop() { 
  char str[MAX_STRING_LENGTH];
  rgb_color color;

  if(Serial1.available() > 0){
    //push_color(red, true);
    int c = Serial1.readBytesUntil('\0', str, MAX_STRING_LENGTH);
    str[c] = 0;

//    for(int i = 0; i < c; i++){
//      Serial.print(i);
//      Serial.print(str[i]);
//    }
//
//    for(int i = 0; i < c; i++){
//      Serial.print(i);
//      Serial.print(str[i]);
//    }

    if(strcmp(str, "red") == 0){
      push_color(red, true);
    }
    else if(strcmp(str, "green") == 0){
      push_color(green, true);
    }
    else if(strcmp(str, "blue") == 0){
      push_color(blue, true);
    }
    else if(strcmp(str, "black") == 0){
      push_color(black, true);
    }
    else if(strcmp(str, "yellow") == 0){
      push_color(yellow, true);
    }
    else if(strcmp(str, "orange") == 0){
      push_color(orange, true);
    }
    else if(strcmp(str, "purple") == 0){
      push_color(purple, true);
    }
    else if(strcmp(str, "cyan") == 0){
      push_color(cyan, true);
    }
    else if(strcmp(str, "magenta") == 0){
      push_color(magenta, true);
    }
    else if(strcmp(str, "pink") == 0){
      push_color(pink, true);
    }
    else if(strcmp(str, "white") == 0){
      push_color(white, true);
    }
    else if(strcmp(str, "erase") == 0){
      erase(true);
    }
    else {
      push_color(black, true);
    }

    //Serial.println(str);
    //Serial.print(str == "red");
    //Serial.print(str == "red\0");

//    if(str == "red"){
//
//    push_color(blue, true);
//      
//      color = red;
//    } else if(str == "green"){
//      color = green;
//    } else if(str == "yellow"){
//      color = yellow;
//    } else if(str == "black"){
//      color = black;
//    } else if(str == "blue"){
//      color = blue;
//    }
//
//
//    push_color(color);
//    ledStrip.write(colors, LED_COUNT); 
  }
}

