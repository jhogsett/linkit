#ifndef CONFIG_H
#define CONFIG_H

// enable to change settings for the 8-LED wearable version
// #define WEARABLE

// enable to change settings for the 93-LED disc
// #define DISC93

// enable to change settings for the 93-LED disc plus a strip
#define DISC93_AND_STRIP

// enable to change settings for a single 50-LED strand
// #define STRAND1

// enable to change settings for two 50-LED strands
// #define STRAND2

// enable to test limits of memory usage
// #define MAXLEDS

// enable to test 3 72-LED strips
// #define STRIP3

// enable to test 2 72-LED strips
// #define STRIP2

// should have the auto-brightness range here
#define AUTO_BRIGHTNESS_MIN 3
#define AUTO_BRIGHTNESS_MAX 33

#if defined(WEARABLE) || defined(STRAND1) || defined(STRAND2) || defined(DISC93)
#define USE_1_DISPLAYS
#elif  defined(STRIP2) || defined(DISC93_AND_STRIP)
#define USE_2_DISPLAYS
#elif defined(STRIP3) || defined(MAXLEDS)
#define USE_3_DISPLAYS
#else
#define USE_1_DISPLAYS
#endif

#if defined(USE_3_DISPLAYS)
#define NUM_DISPLAYS 3           
#define NUM_BUFFERS 3            
#elif defined(USE_2_DISPLAYS)
#define NUM_DISPLAYS 2           
#define NUM_BUFFERS 2            
#else
#define NUM_DISPLAYS 1           
#define NUM_BUFFERS 1            
#endif

#if defined(DISC93) || defined(DISC93_AND_STRIP)
#define DISPLAY_PIN1 11
#define DISPLAY_PIN2 12
#define DISPLAY_PIN3 10
#else
#define DISPLAY_PIN1 12
#define DISPLAY_PIN2 11
#define DISPLAY_PIN3 10
#endif

#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG
#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting

// standard brightness
#if defined(WEARABLE)
#define DEFAULT_BRIGHTNESS_PERCENT 10
#elif defined(DISC93) || defined(DISC93_AND_STRIP)
#define DEFAULT_BRIGHTNESS_PERCENT 15
#elif defined(STRAND1) || defined(STRAND2)
#define DEFAULT_BRIGHTNESS_PERCENT 25
#elif defined(STRIP2) || defined(STRIP3)
#define DEFAULT_BRIGHTNESS_PERCENT 15
#else
#define DEFAULT_BRIGHTNESS_PERCENT 25
#endif

#define MINIMUM_BRIGHTNESS_PERCENT 2  // brightness scale for blinking leds in the off state
#define FADE_RATE 0.9999

// visible led count
#if defined(WEARABLE)
#define ANIM_LED_COUNT  8             
#elif defined(DISC93) || defined(DISC93_AND_STRIP)
#define ANIM_LED_COUNT 93
#elif defined(STRAND1)
#define ANIM_LED_COUNT 50
#elif defined(STRAND2)
#define ANIM_LED_COUNT 100
#elif defined(MAXLEDS)
#define ANIM_LED_COUNT 250
#else
#define ANIM_LED_COUNT 72
#endif

#if defined(WEARABLE)
#define DEMO_OBJECT_SIZE 1
#define DEMO_GAP_SIZE 0
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 125
#elif defined(DISC93)
#define DEMO_OBJECT_SIZE 3
#define DEMO_GAP_SIZE 0
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 35
#elif defined(DISC93_AND_STRIP)
#define DEMO_OBJECT_SIZE 7
#define DEMO_GAP_SIZE 0
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 20
#elif defined(STRAND2)
#define DEMO_OBJECT_SIZE 10
#define DEMO_GAP_SIZE 0
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 35
#else
#define DEMO_OBJECT_SIZE 3
#define DEMO_GAP_SIZE 1
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 0
#endif

#define EASE_ANIM_MARGIN 10           // safety margin for visual effects that go past the end of the LEDs

#ifdef NON_WORKING_INVALID
#define BAUD_RATE 230400              // did not work in testing
#else
#define BAUD_RATE 115200              // speed for communicating with the MT7688
#endif

#if defined(USE_1_DISPLAYS)
#if defined(WEARABLE)
#define BLINK_PERIOD 12000
#define BREATHE_PERIOD 800
#else
#define BLINK_PERIOD 6000
#define BREATHE_PERIOD 400
#endif
#elif defined(USE_2_DISPLAYS)
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 200
#elif defined(USE_3_DISPLAYS)
#define BLINK_PERIOD 2000
#define BREATHE_PERIOD 133
#endif

class Config
{
  public:
  static const byte num_displays = NUM_DISPLAYS;
  static const byte num_buffers = NUM_BUFFERS;

  static constexpr byte display_pins[NUM_DISPLAYS] = 
  {
    DISPLAY_PIN1
#if defined(USE_2_DISPLAYS)
    , DISPLAY_PIN2
#endif
#if defined(USE_3_DISPLAYS)
    , DISPLAY_PIN3
#endif
  };  

  
  static const byte random_seed_pin = RANDOM_SEED_PIN;
  static const byte light_sensor_pin = LIGHT_SENSOR_PIN;
  static const byte default_brightness_percent = DEFAULT_BRIGHTNESS_PERCENT;
  static const byte minimum_brightness_percent = MINIMUM_BRIGHTNESS_PERCENT;
  static constexpr float fade_rate = FADE_RATE;
  static const byte visible_led_count = ANIM_LED_COUNT;
  static const byte ease_anim_margin = EASE_ANIM_MARGIN;
  static const byte led_count = visible_led_count + ease_anim_margin;
  static const int blink_period = BLINK_PERIOD;
  static const int breathe_period = BREATHE_PERIOD;
};

#endif
  
