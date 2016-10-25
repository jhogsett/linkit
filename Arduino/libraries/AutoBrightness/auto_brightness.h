#ifndef AUTO_BRIGHTNESS_H
#define AUTO_BRIGHTNESS_H

// Attach a 10k resistor between aref & <PIN>, and a photoresistor between <PIN> & GND

#include <Arduino.h>
#include <color_math.h>

// Automatic brightness minimum and maximum setting
#define MIN_BRIGHTNESS_PERCENT 10
#define MAX_BRIGHTNESS_PERCENT 100
#define BRIGHTNESS_PERCENT_RANGE (MAX_BRIGHTNESS_PERCENT - MIN_BRIGHTNESS_PERCENT)
#define DEFAULT_BRIGHTNESS_PERCENT MIN_BRIGHTNESS_PERCENT

// Automatic brightness ligth samples
#define LIGHT_SAMPLE_COUNT 5
#define LIGHT_SAMPLE_DELAY 10

template<unsigned char pin> class AutoBrightness
{
  static int light_samples[];

  public:
  void auto_adjust_brightness();

  private:
  int get_average_light_level();
  int get_next_light_sample();
  int get_one_light_sample();
};

template<unsigned char pin> int AutoBrightness<pin>::light_samples[LIGHT_SAMPLE_COUNT] = {0,0,0,0,0};

template<unsigned char pin> int AutoBrightness<pin>::get_average_light_level(){
  int result;
  for(int i = 0; i < LIGHT_SAMPLE_COUNT; i++){
    result = get_next_light_sample();  
  }
}

template<unsigned char pin> int AutoBrightness<pin>::get_next_light_sample(){
  int accum = 0;
  for(int i = LIGHT_SAMPLE_COUNT-1; i > 0; i--){
    light_samples[i] = light_samples[i-1];
    accum += light_samples[i];
  }
  light_samples[0] = get_one_light_sample();
  accum += light_samples[0];

  return accum / LIGHT_SAMPLE_COUNT;
}

template<unsigned char pin> int AutoBrightness<pin>::get_one_light_sample() {
  delay(LIGHT_SAMPLE_DELAY);
  return analogRead(pin);  
}

template<unsigned char pin> void AutoBrightness<pin>::auto_adjust_brightness(){
  int light_level = get_average_light_level();
  // 1023 = complete darkness, 0 = infinite brightness

  float detected_brightness = (1023.0 - light_level) / 1024.0;
  int ranged_brightness_percent = int(BRIGHTNESS_PERCENT_RANGE * detected_brightness);

  ColorMath::set_brightness(MIN_BRIGHTNESS_PERCENT + ranged_brightness_percent);
}
#endif
