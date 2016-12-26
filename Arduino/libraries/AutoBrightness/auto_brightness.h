#ifndef AUTO_BRIGHTNESS_H
#define AUTO_BRIGHTNESS_H

// todo: fix how it set the brightness amount

// todo: see if the static ligth sample array is needed or can be on the stack

// Attach a 10k resistor between aref & <PIN>, and a photoresistor between <PIN> & GND

#include <Arduino.h>
#include <color_math.h>

// Automatic brightness minimum and maximum setting
#define DEFAULT_MIN_BRIGHTNESS 10
#define DEFAULT_MAX_BRIGHTNESS 90
#define DEFAULT_BRIGHTNESS_RANGE (DEFAULT_MAX_BRIGHTNESS - DEFAULT_MIN_BRIGHTNESS)

// Automatic brightness ligth samples
#define LIGHT_SAMPLE_COUNT 5
#define LIGHT_SAMPLE_DELAY 10

class AutoBrightnessBase
{
  public:
  void virtual begin(byte min_brightness = DEFAULT_MIN_BRIGHTNESS, byte max_brightness = DEFAULT_MAX_BRIGHTNESS) = 0;
  void virtual auto_adjust_brightness() = 0;
  byte virtual get_auto_brightness_level() = 0;
};

template<byte pin> class AutoBrightness : public AutoBrightnessBase
{
  public:
  void begin(byte min_brightness = DEFAULT_MIN_BRIGHTNESS, byte max_brightness = DEFAULT_MAX_BRIGHTNESS);
  void auto_adjust_brightness();
  byte get_auto_brightness_level();

  private:
  static byte min_brightness;
  static byte max_brightness;
  static byte brightness_range;
  static int light_samples[];
  int get_average_light_level();
  int get_next_light_sample();
  int get_one_light_sample();
};

template<byte pin> int AutoBrightness<pin>::light_samples[LIGHT_SAMPLE_COUNT] = {0,0,0,0,0};
template<byte pin> byte AutoBrightness<pin>::min_brightness = DEFAULT_MIN_BRIGHTNESS;
template<byte pin> byte AutoBrightness<pin>::max_brightness = DEFAULT_MAX_BRIGHTNESS;
template<byte pin> byte AutoBrightness<pin>::brightness_range = DEFAULT_BRIGHTNESS_RANGE;

template<byte pin> void AutoBrightness<pin>::begin(byte min_brightness, byte max_brightness){
  this->min_brightness = min_brightness;
  this->max_brightness = max_brightness;
  this->brightness_range = max_brightness - min_brightness;
}

template<byte pin> int AutoBrightness<pin>::get_average_light_level(){
  int result;
  for(int i = 0; i < LIGHT_SAMPLE_COUNT; i++){
    result = get_next_light_sample();  
  }
}

template<byte pin> int AutoBrightness<pin>::get_next_light_sample(){
  int accum = 0;
  for(int i = LIGHT_SAMPLE_COUNT-1; i > 0; i--){
    light_samples[i] = light_samples[i-1];
    accum += light_samples[i];
  }
  light_samples[0] = get_one_light_sample();
  accum += light_samples[0];

  return accum / LIGHT_SAMPLE_COUNT;
}

template<byte pin> int AutoBrightness<pin>::get_one_light_sample() {
  delay(LIGHT_SAMPLE_DELAY);
  return analogRead(pin);  
}

// 1023 = complete darkness, 0 = infinite brightness
template<byte pin> byte AutoBrightness<pin>::get_auto_brightness_level(){
  int light_level = get_average_light_level();
  float detected_brightness = (1023.0 - light_level) / 1024.0;
  return min_brightness + int(brightness_range * detected_brightness);
}

template<byte pin> void AutoBrightness<pin>::auto_adjust_brightness(){
  //ColorMath::set_brightness(get_auto_brightness_level());
}
#endif
