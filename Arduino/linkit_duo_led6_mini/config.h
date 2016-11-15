#ifndef CONFIG_H
#define CONFIG_H

#define DATA_OUT_PIN 12               // data out pin for sending color data to the LEDs
#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG
#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting

#define DEFAULT_BRIGHTNESS_PERCENT 6 // standard brightness
#define MINIMUM_BRIGHTNESS_PERCENT 2  // brightness scale for blinking leds in the off state
#define FADE_RATE 0.9999

#define ANIM_LED_COUNT 8              // visible led count
#define EASE_ANIM_MARGIN 10           // safety margin for visual effects that go past the end of the LEDs

#define BAUD_RATE 115200              // speed for communicating with the MT7688

class Config
{
  public:
  static const int data_out_pin = DATA_OUT_PIN;
  static const int random_seed_pin = RANDOM_SEED_PIN;
  static const int light_sensor_pin = LIGHT_SENSOR_PIN;
  static const int default_brightness_percent = DEFAULT_BRIGHTNESS_PERCENT;
  static const int minimum_brightness_percent = MINIMUM_BRIGHTNESS_PERCENT;
  static constexpr float fade_rate = FADE_RATE;
  static const int visible_led_count = ANIM_LED_COUNT;
  static const int ease_anim_margin = EASE_ANIM_MARGIN;
  static const int led_count = visible_led_count + ease_anim_margin;
};

#endif
  
