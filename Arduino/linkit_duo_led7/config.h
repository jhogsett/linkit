#ifndef CONFIG_H
#define CONFIG_H

#define TEST_FRAMEWORK

// #define REAL_ARDUINO

///////////////////////////
// device profile selection
///////////////////////////

// single 90-LED strip in Apollo
// #define APOLLO_DISPLAY

// 19-LED Mini Disc
#define MINI_DISC_19

// single 144-LED strip
// #define LED_STRIP_144

// single 72-LED strip
// #define LED_STRIP_72

// 72-LED weather display
// #define WEATHER_72

// 36-LED annunciator
// #define ANNUNCIATOR

// 54-LED annunciator #2
// #define ANNUNCIATOR2

// 100-LED sphere
// #define SPHERE

// 93-LED platform
// #define PLATFORM

// 93-LED projector
// #define PROJECTOR

// 64-LED Glasses
// #define WEARABLE_AND_GLASSES
// #define WEARABLE_AND_GLASSES_TYPE2

// 200-LED overhead
// #define RADIUS8

// 44-LED Wearable and Hard Hat
// #define WEARABLE_AND_HARDHAT

// wearable and 36-LED annunciator
// #define WEARABLE_AND_ANNUNCIATOR

// wearable and 54-LED annunciator #2
// #define WEARABLE_AND_ANNUNCIATOR2

// single 90-LED strip
// #define LED_STRIP_90

// 8-LED standalone wearable
// #define WEARABLE_8

// 150-LED Outfit
// #define OUTFIT

//////////////////////////////
// devices profile definitions
//////////////////////////////

// all devices defaults
#define BAUD_RATE 115200              // speed for communicating with the MT7688
#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG
#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting
#define DISPLAY_PIN1 12
#define DISPLAY_PIN2 11
#define DISPLAY_PIN3 10
#define EASE_ANIM_MARGIN 0           // safety margin for visual effects that go past the end of the LEDs
#define AUTO_BRIGHTNESS_MIN 3
#define AUTO_BRIGHTNESS_MAX 33
#define MINIMUM_BRIGHTNESS_PERCENT 1  // brightness scale for blinking leds in the off state
#define DEFAULT_BRIGHTNESS_PERCENT 20
#define FADE_RATE 0.925
#define FADE_PERIOD 100
#define NUM_MACRO_CHARS 20
#define NUM_MEMORY_MACROS 10
#define NUM_EEPROM_MACROS 51
#define BLINK_PERIOD 6000
#define BREATHE_PERIOD 300
#define NUM_SEQUENCERS 10
#define FAN_PIN 13
#define FAN_AUTO_ON_BRIGHTNESS 50

#ifdef LED_STRIP_72
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 72
#define ZONES_72_LEDS
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 150
#endif

#ifdef LED_STRIP_90
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 90
#define ZONES_90_LEDS
#define DEFAULT_BRIGHTNESS_PERCENT 15
#define BLINK_PERIOD 1500
#define BREATHE_PERIOD 100
#endif

#ifdef LED_STRIP_144
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 144
#define ZONES_144_LEDS
#define DEFAULT_BRIGHTNESS_PERCENT 15
#define BLINK_PERIOD 1500
#define BREATHE_PERIOD 100
#endif

#ifdef APOLLO_DISPLAY
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 90
#define ZONES_90_LEDS
#define AUTO_BRIGHTNESS_MIN 5
#define AUTO_BRIGHTNESS_MAX 100
#define DEFAULT_BRIGHTNESS_PERCENT 100
#define BLINK_PERIOD 1000
#define BREATHE_PERIOD 50
#endif

#ifdef SPHERE
#define USE_1_DISPLAYS
#define SWAP_REDGREEN
#define ANIM_LED_COUNT 100
#define ZONES_SPHERE
#define DEFAULT_BRIGHTNESS_PERCENT 25
#define BLINK_PERIOD 1500
#define BREATHE_PERIOD 100
#endif

#ifdef RADIUS8
#define USE_1_DISPLAYS
#define SWAP_REDGREEN
#define ANIM_LED_COUNT 200
#define ZONES_RADIUS8
#define BLINK_PERIOD 1000
#define BREATHE_PERIOD 80
#define FADE_RATE 0.925
#define NUM_MEMORY_MACROS 2
#define NUM_SEQUENCERS 2
#define USE_MAPPING
#define RADIUS8_MAPPING
#endif

#ifdef WEATHER_72
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 72
#define ZONES_72_LEDS
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 150
#endif

#ifdef PROJECTOR
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 93
#define ZONES_DISC
#define DEFAULT_BRIGHTNESS_PERCENT 100
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 150
#endif

#ifdef PLATFORM
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 93
#define ZONES_DISC
#define DEFAULT_BRIGHTNESS_PERCENT 25
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 150
#endif

#ifdef MINI_DISC_19
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 19
#define DEFAULT_BRIGHTNESS_PERCENT 20
#define FADE_RATE 0.925
#define FADE_PERIOD 400
#define ZONES_MINI_DISC
#define BLINK_PERIOD 12000
#define BREATHE_PERIOD 1000
#endif

#ifdef WEARABLE_AND_HARDHAT
#define SWAP_REDGREEN
#define WEARABLE_TYPE2
#define ANIM_LED_COUNT 44
#define ZONES_HARDHAT
#define DEFAULT_BRIGHTNESS_PERCENT 15
#endif

#ifdef WEARABLE_AND_GLASSES
#define WEARABLE
#define ANIM_LED_COUNT 64
#define ZONES_GLASSES
#define DEFAULT_BRIGHTNESS_PERCENT 10
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 150
#endif

#ifdef WEARABLE_AND_GLASSES_TYPE2
#define WEARABLE_TYPE2
#define ANIM_LED_COUNT 64
#define ZONES_GLASSES
#define DEFAULT_BRIGHTNESS_PERCENT 10
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 150
#endif

#ifdef WEARABLE_8
#define WEARABLE_TYPE3
#define ANIM_LED_COUNT 72
#define ZONES_72_LEDS
#define DEFAULT_BRIGHTNESS_PERCENT 10
//#define BLINK_PERIOD 6000
//#define BREATHE_PERIOD 150
//#define ONE_WAY_MACROS
#endif

#ifdef WEARABLE_AND_ANNUNCIATOR
#define WEARABLE
#define ANIM_LED_COUNT 36
#define ZONES_ANNUNCIATOR
#define DEFAULT_BRIGHTNESS_PERCENT 25
#define BLINK_PERIOD 2000
#define BREATHE_PERIOD 125
#endif

#ifdef WEARABLE_AND_ANNUNCIATOR2
//#define WEARABLE
#define ANIM_LED_COUNT 54
#define ZONES_ANNUNCIATOR2
#define DEFAULT_BRIGHTNESS_PERCENT 25
#define BLINK_PERIOD 2000
#define BREATHE_PERIOD 125
#endif

#ifdef ANNUNCIATOR
#define ANIM_LED_COUNT 36
#define ZONES_ANNUNCIATOR
#define DEFAULT_BRIGHTNESS_PERCENT 33
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 125
#endif

#ifdef ANNUNCIATOR2
#define ANIM_LED_COUNT 54
#define ZONES_ANNUNCIATOR2
#define DEFAULT_BRIGHTNESS_PERCENT 33
#define BLINK_PERIOD 2000
#define BREATHE_PERIOD 125
#endif

#ifdef OUTFIT
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 150
#define ZONES_OUTFIT
#define BLINK_PERIOD 1000
#define BREATHE_PERIOD 40
#define FADE_RATE 0.825
#define DEFAULT_BRIGHTNESS_PERCENT 20
#endif

// first wearable: internal is pin 12
#ifdef WEARABLE
#define USE_2_DISPLAYS
#define DISPLAY_PIN1 11
#define DISPLAY_PIN2 12
#define DISPLAY_PIN3 10
#endif

// internal as first display
#ifdef WEARABLE_TYPE2
#define USE_2_DISPLAYS
#define DISPLAY_PIN1 12
#define DISPLAY_PIN2 11
#define DISPLAY_PIN3 10
#endif

// internal as only display
#ifdef WEARABLE_TYPE3
#define USE_1_DISPLAYS
#define DISPLAY_PIN1 12
#define DISPLAY_PIN2 11
#define DISPLAY_PIN3 10
#endif

// set number of displays and buffers based on number of configured displays
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

// finalize total # of macros
#define EEPROM_STARTING_MACRO (NUM_MEMORY_MACROS)
#define NUM_MACROS (NUM_MEMORY_MACROS + NUM_EEPROM_MACROS)

class Config
{
  public:
    static const byte num_displays = NUM_DISPLAYS;
    static const byte num_buffers = NUM_BUFFERS;

    static constexpr byte display_pins[NUM_DISPLAYS] =
    {
      DISPLAY_PIN1
#if defined(USE_2_DISPLAYS) || defined(USE_3_DISPLAYS)
      , DISPLAY_PIN2
#endif
#if defined(USE_3_DISPLAYS)
      , DISPLAY_PIN3
#endif
    };

    // todo: are these used?
    static const byte random_seed_pin = RANDOM_SEED_PIN;
    static const byte light_sensor_pin = LIGHT_SENSOR_PIN;
    static const byte default_brightness_percent = DEFAULT_BRIGHTNESS_PERCENT;
    static const byte minimum_brightness_percent = MINIMUM_BRIGHTNESS_PERCENT;
    static const byte visible_led_count = ANIM_LED_COUNT;
    static const byte ease_anim_margin = EASE_ANIM_MARGIN;
    static const byte led_count = visible_led_count + ease_anim_margin;
    static const int blink_period = BLINK_PERIOD;
    static const int breathe_period = BREATHE_PERIOD;
    static constexpr float fade_rate = FADE_RATE;
    static const int fade_period = FADE_PERIOD;
};

#endif

