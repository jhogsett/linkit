#ifndef CONFIG_H
#define CONFIG_H

#include <common.h>

#define TEST_FRAMEWORK

// #define REAL_ARDUINO

///////////////////////////
// device profile selection
///////////////////////////

// wearable and 36-LED annunciator
// #define WEARABLE_AND_ANNUNCIATOR

// single 72-LED strip
 #define LED_STRIP_72

// single 90-LED strip
#define LED_STRIP_90

// single 90-LED strip in Apollo
// #define APOLLO_DISPLAY

// 100-LED sphere
// #define SPHERE

// 200-LED overhead
// #define RADIUS8

// 93-LED projector or platform
// #define PROJECTOR

// 44-LED Wearable and Hard Hat
// #define WEARABLE_AND_HARDHAT

// 64-LED Glasses
// #define WEARABLE_AND_GLASSES

// 8-LED standalone wearable
// #define WEARABLE_8

// 19-LED Mini Disc
// #define MINI_DISC_19

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

#ifdef LED_STRIP_72
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 72
#define ZONES_72_LEDS
#endif

#ifdef LED_STRIP_90
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 90
#define ZONES_90_LEDS
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
#define ANIM_LED_COUNT 100
#define ZONES_SPHERE
#define DEFAULT_BRIGHTNESS_PERCENT 25
#define BLINK_PERIOD 1500
#define BREATHE_PERIOD 100
#endif

#ifdef RADIUS8
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 200
#define ZONES_RADIUS8
#define BLINK_PERIOD 1000
#define BREATHE_PERIOD 80
#define FADE_RATE 0.925
#define NUM_MEMORY_MACROS 2
#define NUM_SEQUENCERS 2
#endif

#ifdef PROJECTOR
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 93
#define ZONES_DISC
#define DEFAULT_BRIGHTNESS_PERCENT 100
#define BLINK_PERIOD 3000
#define BREATHE_PERIOD 150
#endif

#ifdef MINI_DISC_19
#define USE_1_DISPLAYS
#define ANIM_LED_COUNT 19
#define DEFAULT_BRIGHTNESS_PERCENT 70
#define FADE_RATE 0.925
#define FADE_PERIOD 400
#define ZONES_MINI_DISC
#endif

#ifdef WEARABLE_AND_HARDHAT
#define WEARABLE
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


//// IoT Devices
//
//// 90-LED Apollo Lights Dev indoors
//#define APOLLO_LIGHTS2_DEV
//
//// 44-LED Wearable and Hard Hat
//// #define WEARABLE_AND_HARDHAT
//
//// 100-LED Sphere
//// #define SPHERE
//
//// 200-LED Radius-8 displays
//// #define RADIUS8
//
//// 93-LED Disc Projector
//// #define PROJECTOR
//
//// 100-LED Xmas Lights
//// #define XMAS_LIGHTS
//
//// 72-LED Apollo Lights
//// #define APOLLO_LIGHTS
//
//// 90-LED Apollo Lights
//// #define APOLLO_LIGHTS2
//
//// 64-LED Wearable and Glasses
//// #define WEARABLE_AND_GLASSES
//
//// 3-72-LED Strips Status Monitor
//// #define STATUS_MONITOR
//
//// 19-LED mini disc, quartz platform
//// #define MINI_DISC_19
//
//// the non-LED repeater, just uses all defaults
//// #define REPEATER
//
//// enable to change settings for the 8-LED wearable version
//// #define WEARABLE
//
//// enable for 2nd wearable
//// #define WEARABLE2
//
//// enable for WEARABLE + 72-LED Strip
//// #define WEARABLE_AND_STRIP
//
//// 93-LED Wearable and Disc
//// #define WEARABLE_AND_DISC93
//
//// enable to change settings for the 93-LED disc
//// #define DISC93
//
//// enable to change settings for the 93-LED disc plus a strip
//// #define DISC93_AND_STRIP
//
////enable to change setings for the 93-LED disc  plus 2 strands
//// #define DISC93_AND_STRAND2
//
//// enable to change settings for a single 50-LED strand
//// #define STRAND1
//
//// enable to change settings for two 50-LED strands
//// #define STRAND2
//
//// enable to test limits of memory usage
//// #define MAXLEDS
//
//// enable to test 3 72-LED strips
//// #define STRIP3
//
//// enable to test 2 72-LED strips
//// #define STRIP2
//
//// enable 144-LED strips
//// #define DUAL_STRIP
//
//#ifdef PROJECTOR
////#define DISC93
//#define DISC93_DOUBLE
//#endif
//
//#ifdef XMAS_LIGHTS
//#define STRAND2
//#endif
//
//#if defined(MINI_DISC_19) || defined(DUAL_STRIP) || defined(DISC93)
//#define REAL_ARDUINO
//#endif
//
//#if defined(APOLLO_LIGHTS) || defined(APOLLO_LIGHTS2)
//#define USE_AUTO_BRIGHTNESS
//#endif
//
//#ifdef STATUS_MONITOR
//#define STRIP3
//#endif
//
//#if defined(APOLLO_LIGHTS) || defined(APOLLO_LIGHTS2)
//#define AUTO_BRIGHTNESS_MIN 5
//#define AUTO_BRIGHTNESS_MAX 100
//#else
//#define AUTO_BRIGHTNESS_MIN 3
//#define AUTO_BRIGHTNESS_MAX 33
//#endif
//
//#if defined(STRAND1) || defined(STRAND2) || defined(DISC93) || defined(DUAL_STRIP) || defined(WEARABLE2) || defined(APOLLO_LIGHTS) || defined(APOLLO_LIGHTS2) || defined(RADIUS8) || defined(APOLLO_LIGHTS2_DEV) || defined(SPHERE)
//#define USE_1_DISPLAYS
//#elif  defined(STRIP2) || defined(DISC93_AND_STRIP) || defined(WEARABLE) || defined(WEARABLE_AND_STRIP) || defined(WEARABLE_AND_DISC93) || defined(DISC93_AND_STRAND2) || defined(DISC93_DOUBLE)  || defined(WEARABLE_AND_GLASSES) || defined(WEARABLE_AND_HARDHAT)                                 
//#define USE_2_DISPLAYS
//#elif defined(STRIP3) || defined(MAXLEDS) //|| defined(WEARABLE_AND_GLASSES) 
//#define USE_3_DISPLAYS
//#else
//#define USE_1_DISPLAYS
//#endif
//
//#if defined(DISC93_AND_STRIP) || defined(WEARABLE_AND_DISC93) || defined(DISC93_AND_STRAND2) // || defined(WEARABLE_AND_GLASSES) defined(DISC93) || 
//#define DISPLAY_PIN1 11
//#define DISPLAY_PIN2 10
//#define DISPLAY_PIN3 12
//#elif defined(WEARABLE_AND_GLASSES) || defined(WEARABLE_AND_HARDHAT)
//#define DISPLAY_PIN1 11
//#define DISPLAY_PIN2 12
//#define DISPLAY_PIN3 10
//#elif defined(STATUS_MONITOR) || defined(STRIP3)
//#define DISPLAY_PIN1 5
//#define DISPLAY_PIN2 6
//#define DISPLAY_PIN3 7
//#else
//#define DISPLAY_PIN1 12
//#define DISPLAY_PIN2 11
//#define DISPLAY_PIN3 10
//#endif
//
//#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG
//#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting

// standard brightness
//#if defined(WEARABLE) || defined(WEARABLE_AND_STRIP) || defined(WEARABLE_AND_GLASSES)
//#define DEFAULT_BRIGHTNESS_PERCENT 10
//#elif defined(DISC93) || defined(DISC93_AND_STRIP) || defined(STRIP2) || defined(STRIP3) || defined(DUAL_STRIP) || defined(WEARABLE_AND_DISC93) || defined(DISC93_AND_STRAND2) || defined(APOLLO_LIGHTS2_DEV) //  || defined(DISC93_DOUBLE) 
//#define DEFAULT_BRIGHTNESS_PERCENT 15
//#elif defined(STRAND1) || defined(STRAND2) || defined(APOLLO_LIGHTS) || defined(APOLLO_LIGHTS2) || defined(SPHERE) || defined(WEARABLE_AND_HARDHAT)
//#define DEFAULT_BRIGHTNESS_PERCENT 25
//#elif defined(MINI_DISC_19)
//#define DEFAULT_BRIGHTNESS_PERCENT 70
//#elif defined(DISC93_DOUBLE) 
//#define DEFAULT_BRIGHTNESS_PERCENT 50
//#else
//#define DEFAULT_BRIGHTNESS_PERCENT 20
//#endif
//
//#define MINIMUM_BRIGHTNESS_PERCENT 1  // brightness scale for blinking leds in the off state
//
//// visible led count
//#if defined(WEARABLE) || defined(WEARABLE2)
//#define ANIM_LED_COUNT  8
//#elif defined(WEARABLE_AND_STRIP)
//#define ANIM_LED_COUNT 72
//#elif defined(WEARABLE_AND_GLASSES)
//#define ANIM_LED_COUNT 64
//#elif defined(DISC93) || defined(DISC93_AND_STRIP) || defined(WEARABLE_AND_DISC93) || defined(DISC93_DOUBLE)
//#define ANIM_LED_COUNT 93
//#elif defined(STRAND1)
//#define ANIM_LED_COUNT 50
//#elif defined(STRAND2) || defined(DISC93_AND_STRAND2) || defined(SPHERE)
//#define ANIM_LED_COUNT 100
//#elif defined(DUAL_STRIP)
//#define ANIM_LED_COUNT 144
//#elif defined(MAXLEDS)
//#define ANIM_LED_COUNT 250
//#elif defined(APOLLO_LIGHTS2) || defined(APOLLO_LIGHTS2_DEV)
//#define ANIM_LED_COUNT 90
//#elif defined(RADIUS8)
//#define ANIM_LED_COUNT 200
//#elif defined(MINI_DISC_19)
//#define ANIM_LED_COUNT 19
//#elif defined(WEARABLE_AND_HARDHAT)
//#define ANIM_LED_COUNT 44
//#else
//#define ANIM_LED_COUNT 72
//#endif
//
//#if defined(DUAL_STRIP) || defined(RADIUS8)
//#define EASE_ANIM_MARGIN 0           // safety margin for visual effects that go past the end of the LEDs
//#else
//#define EASE_ANIM_MARGIN 0           // safety margin for visual effects that go past the end of the LEDs
//#endif
//
//#ifdef NON_WORKING_INVALID
//#define BAUD_RATE 230400              // did not work in testing
//#else
//#define BAUD_RATE 115200              // speed for communicating with the MT7688
//#endif
//
//// blink and breathe timing
//#if defined(REAL_ARDUINO)
//#define BLINK_PERIOD 18000
//#define BREATHE_PERIOD 700
//#elif defined(WEARABLE)
//#define BLINK_PERIOD 12000
//#define BREATHE_PERIOD 600
//#elif defined(WEARABLE_AND_STRIP) || defined(WEARABLE_AND_GLASSES) || defined(WEARABLE_AND_DISC93) || defined(APOLLO_LIGHTS2) || defined(APOLLO_LIGHTS2_DEV) || defined(SPHERE)
//#define BLINK_PERIOD 1500
//#define BREATHE_PERIOD 100
//#elif defined(DUAL_STRIP)
//#define BLINK_PERIOD 3000
//#define BREATHE_PERIOD 100
//#elif defined(USE_1_DISPLAYS) || defined(WEARABLE_AND_HARDHAT)
//#define BLINK_PERIOD 6000
//#define BREATHE_PERIOD 300
//#elif defined(USE_2_DISPLAYS)
//#define BLINK_PERIOD 3000
//#define BREATHE_PERIOD 150
//#elif defined(USE_3_DISPLAYS) || defined(RADIUS8)
//#define BLINK_PERIOD 1000
//#define BREATHE_PERIOD 80
//#else
//#define BLINK_PERIOD 6000
//#define BREATHE_PERIOD 300
//#endif
//
//#if defined(MINI_DISC_19)
//#define FADE_RATE 0.925
//#define FADE_PERIOD 400
//#else
//#define FADE_RATE 0.925
//#define FADE_PERIOD 100
//#endif
//
//#if defined(RADIUS8) || defined(DISC93_DOUBLE) || defined(DUAL_STRIP) || defined(DISC93)
//#define NUM_MEMORY_MACROS 4
//#else
//#define NUM_MEMORY_MACROS 10
//#endif

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

