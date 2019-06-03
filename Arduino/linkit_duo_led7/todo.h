// (macro) 5
// would run the macro five times
// (macro schedule) times 
// would be invalid

// mini remote propeller
//0:set:0:seq:pal
//1:set:rnd:3:blk:4,-1:cpy
//2:set:0,4:seq:era:1:run:0,16:run:flu
//3:set:0:run:flu


// can't use variable assignment in conditionals
// save table of python expressions

// delay before beep on startup


// macro return if


// what are the various syntax forms?
// $simple
// [inline ]
// [[ multi-line
// ]]
//

// idea: delimiters: first pair is the positive one and any additional ones are the negative ones

// macro_compiler can't handle raw script like:
// 10:set:39:run
// 11:set:14:run
// 12:set:app:37:run:200,69:sch
// 13:set:36:run:30000,36:sch
// 72:set:flu
// --> needed for auto-restore of original script

// have ability for default arguments to templates

// idea: if two macros are identical, keep only one of them and fix up references
// 72:set:flu
// 9:set:flu
// --> at least a warning about it at first

// don't keep this in resolved
// %include: hsl-sequencer

// what happens passing wrong things to (( ((( [[[

// these are not tested for:
//No available macro numbers available during final number assignment
//is needed as a carry-over macro but is already assigned

// DRY up delimiter strings
// standardize on dictionaries for delimiters
// parse template arguments better to avoid name clashing

// idea: simulate runtime variables using push/pop

// idea: peek at sequencer's next value

// start of a pretty xmas effect
// 0:set:rps:0,-1:seq:pal:sfd:rst
// 1:set:rps:0,-1:seq:psh:1:psh:add:pal:sfd:rst:50,250:rng:psh:1:0
// 3:set:0:seq


// new sequencer type:
// - wander
// - between bounds
// - uses step
// - stays in bounds or wraps around


// dampening
// practical use
// generate values
// pass as arg0 to filter, get new arg0 back
// uses macro #0 and #1 for data storage
// or just a small array of ints

// new concept: filters?








// ideas
// background striper
// 93-led disc manager
// 19-led disc manager
// glasses manager
// 200-led mapping manager

// a way to transofrm a dynamic color into a real one in place in the buffer

// might need a shortcut for running a macro a number of times
// (<macro-to-run> <schedule>)
// (<macro-to-run>) <times>


//  get-random-number
//  run-macro

//  get-random-number
//  (<<arg0>>)


// when starting a sequencer can you get the very first low value?

// could have argument to shuffle to allow it shuffle only N pixels

// why is a push_command extra margin of chars needed?

// make sure there are specs covering all error messages in macro compiler
// remove unnecessary verbose output

/* MUST */
// need error if macro and variable have the same name
// should raise error if more than one macro use the same fix macro number


/* NICE */
// [[[ multimacro needs name prefixing too to avoid possible name clashes ]]]
// template names should be prefixed too
// need to be able to multicast system commands, to all or maybe targetted to
// explosion animation
// maybe smarter python expression combining than jsut removing backticks?
// dynamic colors should support > palette colors
// should be able to override the default [render !] so a custom render can go in the short space
// ball-box: a wandering mode (needs dampening)
// macro compiler substitutes macro numbers with single digits, but could require double digits when programming (command string length risk)
// could make upper case only variables always mutable



/* WANT */
// report errors before compressing spaces because it confuses troubleshooting
// progmac.py report actual number of macro bytes programmed
// might need to expand multi macros before prefixing (others?) 
// prefixing is sensitive to order of include files (had to put hsl-sequencer first for hsl-sequencer-start to be recognized)
// background: darker pixels are next higher palette color
// new inquiry: whether red and green are being swapped; add spec to cover rendering in a swap situation





/* IDEA */
// remix:
// mirror alternative: reverse a set of pixels
// to remix:
// 1) apply reversing to groups of leds, groups computed as # of leds / some count, so 4 would create four groups, 1 would reverse all pixels
// 2) reverse only half of the groups, alternating reverse, not reverse (which starts first should be settable or predictable - negative number could switch to start without reversing)
// 3) do several rounds with random numbers to complete the remix, could make it animated or not

// sequencer: min,max,macro:sqr (run macro if sequencer value between min-max inclusive) val,0,macro:sqr (run macro if sequencer value = val)

// scramble buffer, either completely random or remix
// would have to be scalable and account for uneven divisions

// have a special language construction that uses memory macros 0 and 1 to perform a loop
// useful if needing to do something N times with an index
// command: 0:set:0:seq:dyn
// command: 1:set:0,18:seq:0,18:run

// sequence command to advance two at once (then math them?)

// snf set-next-frame
// take sequencer arguments
// get the sequencer value


//animation needs
//
//parent-child relationship
//position
//size
//
//------------------
//
//get next object position
//
//need to set the viewport to cover it
//
//
//pass in: seq #, object width, parent object seq #
//
//advance sequence to next object position
//push
//get parent sequencer current position
//push
//add them
//set offset
//add object width
//set window
//
//-------------------
//
//arg0 - seq #
//arg1 - parent seq #
//arg2 - object width
//
//advance sequence arg0 and save it
//get sequence arg1 and add
//set this as offset
//add width arg2
//set this as window



// flooding should overwrite the origin position (otherwise ?)


// effect to dim by varing amounts dynamically (could be like soft fade but fade ratio the wanders)

// objects
// 3 pixels center color, outer color
// white pixel
// static pixel

// vary
// draw mode



// combine refresh and reset somehow maybe {{{{ }}}}
//    {{
//      {{{
//        do-animated-rotation
//      }}}
//    }}

// fast mirror, flood

// box sequencer or sequence next box command
// needs:
// position
// width
// spill over edges (internally keeps illegal buffer value, but returns a safe value
// 

// new math: min, max

// directive to spit out a message

// pm command to dump out overridable variables from a script

// if a variable value is a string, it needs to be surrounded by quotes when being evaluated by python


// app should do these
//    <CONFIG-FADE-RATE> = <fade-rate>
//
//    # set the preset brightness level
//    <brightness>
//    set-brightness

// new sequence advance types
// advance, unless on low limit  (and run a macro if supplied)
// advance, unless on high limit (and run a macro if supplied)

// a standard macro like this is 15 bytes + terminator
//50:set:2,75:swc:56:run:150,59:sch
//51:set:
// after arg storage change, no carryover macro
// 14:set:1,75:sws:18:run:25,21:sch

// call this instant template?
//$num-instances 3
//[[[ <num-instances> INSTANCE COLOR `<RED>+INSTANCE` REPEATS `INSTANCE`
//        COLOR
//        add-palette-color
          INSTANCE
          repeat
//]]]
//
//would become
//
//0
//add-palette-color
//1
//add-palette-color
//2
//add-palette-color
//
//
//
//0:pal:1:pal:2:pal
//


// magic zoom (-1?) zooms to fill the current width

//         {seq-X}, `X*(<NUM-LEDS>/6)+(<NUM-LEDS>/6)`, `X*(<NUM-LEDS>/6)`
// in place macros?
//
//         &
//         {seq-X}, `X*(<NUM-LEDS>/6)+(<NUM-LEDS>/6)`, `X*(<NUM-LEDS>/6)`

// sequencer for drawing and erasing objects

// something like snw but makes it easy to erase before drawing next segment

// change fade effect to be a cross fade to the current black level

// are data/render buffers mirrored in program memory for initialization?

// time metrics: fastest/slowest test/group

// alternate specs led display: green, yellow, red success dots

// document minimum 2 zone requirement

// add additional argument to expect_... for a default led display to avoid blank times during testing

// add timing metrics to led specs

// led_specs.py: 

// something like 100:red or 500:red can cause the device to lock up


// dynamic rendering hints


// setting window, have a way to make it relative, like -10 means offset+10

// add ability to get different brightness levels for testing

// add final command to sunrise

// simple script run check: run be able to run, stop, and regain control over the device. (start/stop each app macro)

// a simple way to invoke a sequencer that uses a macro for math
// {seq-hsl hsl-macro}

// # requires use of pre-defined variables
//
// <<< `<NUM-LEDS>>=72`
//   $num-instances 6
// >>>
//
// <<< `<NUM-LEDS><72`
//   $num-instances 4
// >>>

// <<< DEVICE-NAME apollo
//   $num-instances 6
// >>>



// ununused brackets || // \\ ^^ 
// << something related to variables? or conditional compilation? or spit out text? >>
// unused operators - _ ~ !



// conditionals would need to be held onto and handled for each macro expansion

// reusable components: 
// -- need to share, or namespace, render routines
// -- need to share sequencers

// report line number of error (would require not adding/subtracting lines, or saving an original line identity per new lines
// could make each line a dictionary instead of string then keep properties along with each line; methods to simplify using as strings

// must document language

// -1 is common so maybe it should have it's own number encoding with zero data bytes
// could have magic arg mapping 11xxxx xx11xx xxxx11

// need command to quit macro early under certain conditions
  // something like: default: exit if zero, exit if non-zero, exit if (conditions of various kinds)

// idea: if acc0 == acc1 exit macro
//       
// specs
  // math, mirroring

// start experimenting with using an arduino to tend batteries:
    // disconnect when above or below a preset voltage

// memory macro space could be used for:
    // dampening
    // storing variables for use in macros (even just as-is "1:set:19:1:run")    

// dampening should be a sequencer type
// some kind of incoming parameters
// on advancing would always specify new value
// would use memory macro space (or RAM?) to keep values for a moving average (other kinds?)

// gravity could be a similar sequencer type where its fed new incoming values


// weather data shows rain as main forecast even when rain amount section is missing
// in these cases consider the conditions clear

// idea: dampening generator, for instance, averaging, lamp burning out, side-gravity

// figure out a scheme for spec randomization

// name match should apply to to group and/or test description

// handle -127 to -1 with a single byte instead of two

// maybe return # of cmd chars processed, at least when the command must be retryable 

// integers need to be passed in to meta-templates, but there ought to be a way to pre-process variables and python expressions so those can be resolved prior to expansion
//   what if part of pre-processing is trying to resolve all variables and python expressions?
//   problem: python expressions within metatemplates using passed in values can't be evaluated, 
//     because the upper case name gets used, but there's no marking of those variables to be able to detect the line as unresolved
//     one possible fix would be to consider upper-case chars as unresolved

// show the device uptime at the bottom of the html page (or have an admin page to view it)

// add a way to reverse a swing sequencer, to flip the direction any time



// need to inquire mapping bounds

// idea: in addition to xy positioning, have angle/radius positioning
// how would that work?
// 0..359 (angle), 0..100 (percent)
// radius max is half the mapping width
// angle computed using cos()/sin() instead of lookup table

// need test for saved stack on sequencer macro math call

// -p show # bytes and/or show all macro bytes don't stop on 255 to check on overrun macros

// can't use long command names for passing values to template

// would be nice not to have to use additional macros just to assign 10,11,12 as numbers.
// maybe pass macro number in include line? (don't want extra args)
// maybe a directive

// lines with # are still getting processed in some cases, see test_script23.mac

// maybe prefix macro names with filenane on include to avoid name clashes

// idea: command line switch that lists out settable command line presets per program along with their current value

/* slightly odd
------------------------------------------------------
Final Macro Numbers:
------------------------------------------------------
    4: 4
    5: 6
    6: 7
    7: 12
    8: 14
    9: 16
 ->  10: 19
   11: 22
   12: 25
   13: 28
   14: 9
   15: 31
   16: 32
   17: 33
   18: 34
   19: 35
   20: 36
   21: 37
   22: 39
   23: 41
   24: 42
   25: 43
   26: 44
   27: 45
->   10: 10
10-11: 11
10-20: 20
10-21: 21
 */

// arduino side dosn't handle compketely full macro without terminator
// so this happens due to using macro bytes - 1
/*
 * 11:set:run:20:run:23:run:22:run:7:run:50,18:sch
 * 12:set:
 * 13:set:4:sto:5:run:rcl
 */



//maybe measure all macros first, then do fitting




/*
(((worm 3 `X+1`)))

[[[worm NUM-FINE-ZONES]]]


idea: do replacements for all device properties, incase they're used

---------------------------------



could always assume the argument is a python expression (`3` is OK) but it needs delimiters


*/
 
// can't use long names in macro expansions

// error message if device config values come back as zero

// directives in macros are not immutable (this may be good)

// constants with evaluated python caused problems if they themselves are used in calculations, probably due to ` mismatch

// idea: color command accepts long commands

// to ease multi sequencing, could have sequencers that have base parent
//   it's always an add operation because it sets the sequencers basis
// . this can be a command similar to :snw:
// . takes two sequence numbers, get's current on one, and get's next on the other and adds them to form basis

// :s__:

// idea: number ring, like the other number generators, except you feed it numbers to generate; could be used for dampening
// .   fixed size

// why does a double return from a macro wipe out arg0? mayube by design?

// print out fixed macro numbers / apps

// display device profile only

//need an easy way to see bytes per script line (save used bytes)

// need a way to make the meta template expansion count variable, settable via command line or by including script

// simple script run check: run be able to run, stop, and regain control over the device. (start/stop each app macro)
// . , test each app


// long command names don't work in variables assignments
  // since these can be specified on the command line, it's probably better to require real commands

// table - list out sequencers

// idea: for an included module, have a way to specify an "init" macro that must be run when any app starts up
//   could be a directive
// . adds (calls it) along with each "app" command
// %init-macro hsl-sequencer-start
//
// when seen, add to a list of init macro names

// before script is consolidated, 


// add directives to shown tables

// fix up macros table with final macro number assignments for printout

// print out macros with fixed macro numbers

// idea: run the test script for a few seconds and make sure the device can be taken control of


// a simple way to invoke a sequencer that uses a macro for math

// ununused brackets || <<>> '' "" // \\ 
// unused operators = - _ ~ !

// reusable components: 
// -- need to share, or namespace, render routines
// -- need to share sequencers

// may need some concept of blending some method of components like starting

// fix: using [render !] in spec test scripts caused failure

// report line number of error (would require not adding/subtracting lines, or saving an original line identity per new lines

// todo: print out macro numbers for forced ones

// %dryrun
// % others?

// animation idea: wanderers, like red and green, could combine for a while as yellow and become one entangled wanderer. It could later split back into two separate wanders, or combine with a blue one to make white.
//   what would the rules be for when to split and combine?

// must document language

// to save macro bytes, commands should have non-negative switching parameters when possible

// -1 is common so maybe it should have it's own number encoding with zero data bytes
// negative numbers are generally small; they could be encoded as 8-bits and expanded back to 16 (if needed).

// app to collect timing metrics on various commands (can this be built into test suite?)

// need command to quit macro early under certain conditions
  // something like: default: exit if zero, exit if non-zero, exit if (conditions of various kinds)

// memory macro space could be made availble for storing values, or having queues of values

// show remaining macro bytes 

// wrapper on argparse to make it easier to add my kind of options 

// can something like a color swinger or wanderer be packaged up as a reusable object?

// specs
  // math, mirroring

// may need to pass arguments to the macro call

// start experimenting with using an arduino to tend batteries:
    // disconnect when above or below a preset voltage

// memory macro space could be used for:
    // dampening
    // storing variables for use in macros (even just as-is "1:set:19:1:run")    

// what would it take to seal the ends for the wanderers?

// what would it take to constrain a wanderer to a portion of the strip, which can be moved itself?

// weather data shows rain as main forecast even when rain amount section is missing
// in these cases consider the conditions clear

// idea: dampening generator, for instance, averaging, lamp burning out, side-gravity

// wanderer idea: do its own mirroring, allowing all 6 to travel anywhere
  // also connect the ends (can this be made automatic in sequencer?)

// idea: destination sequencer, set a point, it reaches it, linearly, or otherwise

// button presses
  // examine an input
  // if it's positive X number of times and negative < Y number of times, consider it switched on
  // could be a special type of schedule that counts pin highs, and macro gets called if pin is high

// sequencers: one-time

// figure out a scheme for spec randomization

// command to convert percentage to perfect count of leds/width/whatever to make scaling more automatic

// add min/max fixing mth op

// name match should apply to to group and/or test description

// need a command for flushing the serial output buffer for debugging macros

// test: chaining math operations, leaving results in args

// it might simplify things if there was a separate command to retrieve sequence numbers, so seq becomes sqw for setting, and then seq pulls the value

// add a timing test on fade and crossfade, should happen in less than x

// sequencer that calls macro should restrain to bounds (?)

//type of sequencer
//    no limits
//    instead: macro #1: advancing, macro #2 painting
//    has built-in next window support
//    advancing does any arbitrary positions
//    how to deal with wheel problem? how does it know when to do it? (wrapping around is different from a large rebound, that arrives at the same position)
//    might need a wheel vs. swing macro sequencer to deal with brushing

// snw should do the pos and extra drawing if necessary

// could make a form of flood that wraps around the end of the width to the beginning

// mir modes, like quarters insted of half, etc.

// default sequencers to num leds

// swing sequencer is reliant on a mode, and needs to be sensitive to the mode changing when stepping negatively

// era doesn't work when width is one pixel

// math: min/max

// could use accumulators to do if/then decisions in macros
  // x,m1,m2:iff: m1, m2 are macros, m1 is run unless x is zero, then it's m2
  //
  // alternate: 
  // x,mf,ml:swt: mf,ml are first,last macros in range, x specifies switch (but in this case, ml isn't needed since x can be constrained)
    // could have ml be redefined as: macro to call if x is zero (is this needed?)

// write latest failures to ./.led_specs so they can be rerun easily

// be able to update sequencer keeping present width (offset the sequencer)

// add rgb terminal colors

// move helpers into led_command.py

// dynamic blink based on 1-6
  // blue dynamic color channel lower six bits specify which ones are on
  // or double to 1-12 and use green & blue channels

// calibrate timing setting by use testing timer to compare
      // could be part of the start-up procedure
      // make this a calibration program

// can a sequencer be used to simulate the effect of a spinning light intensity profile?

// macros need to set up effects pausing
  // annunciator needs fix

// command document last modified date should be the later of it and http_command.py




/* new features

// most needed

// super pixels
      // for 2d strips, it sets several leds with fade off ends
      // for 3d displays, it sets a circle (with fade off edges?)

// calibrate timing setting by use testing timer to compare
      // could be part of the start-up procedure
      // make this a calibration program

// now that sto and rcl are settled and args can be passed into macros, are there math opertions that would be handy on the accumulators?
// need to be able to manipulate arguments
      // example: quantize (to allow position offsets)
      // also: combine two separate random numbers into arg0, arg1 (can be done now with rcl I think)

//   also need one-time action sequencers

// introduce setting for "primary" and "secondary" display so the particular display can be changed in real time outside of running program
      // so I can softswitch wearables between external and built-in display

// need triggers, want the cosine swing and wheel to trigger other ones
//   could set trigger macros per type of sequencer (they may need different numbers of macros)
// trigger idea: when filling up and ran out of open positions
// triggers:
      // run a macro each time a sequence has an event (events: start (wheel), start, mid-way (swing))
      // how would this work?
      //   one trigger per sequencer (so, 6)
      //   triggers are built into sequencers, not separate (though a different command is used to set them up)
      //   set a trigger: t,s,m:trg 
      //     t=trigger number
      //     s=sequence number, if s is -1, the trigger is cleared
      //     m=macro number, for two-event sequences (swing) it calls this macro and m+1 
      //   each sequence incrementer will fire the trigger at bounding events
      //   the trigger will then call the macro(s)

// other sequencers:
      // brownian motion
      // power ease
      // seeded random
      // gravity bounce
      // ones useful in 3D such as triangle wave, spirograph, Lissajous curves

// command to run a range of macros, like 20,29:rmm runs macros 20 through 29 in order
  // -- would have to be a private seed (feed value back as seed?) to have a repeatable sequence

// concept of "objects"
// - hidden/shown
// - moved around
// - could be as simple as offset + window + color & effect
// - could be a complex pattern like a bell curve of lit leds around a center point
// rearrange config into device profiles
// find and replace color feature
// the infinite loop protection could also be a trigger, when all full or all empty
// triggers could be a set of slots that get set to a macro to run when triggers
// could have certain commands that end the macro under certain conditions
     // quit if ... 
// could have conditional/switch to call multiple macros
// for zones, have several configurations that can be chosen in real time by device type
// need command to set offset and window from arg0, arg1
// need to be able to choose a set of random numbers, and share them across macros
//   palette could be used for this - could hold three arguments worth of random numbers
// need to be able to have a white flash but then resume the previous color/effect
// - maybe reverts when the luminosity matches so it seems to fade out
// - could have a trigger that could detect the end of fading out; macro would need to know the position
// - there could be a standard after-fade color (default black) 
// could have a random macro command
// 0,2,5:rnm = randomly run 0, 1 or 2, with 5 as arg0
// would be nice to be able to measure the *current usage* via an analog pin
// have a way to add/subtract color
// super slow fade "decay"
//have a schedule that gets triggered upon a carry being set, could automatically wired together two zones
//-- could have other kinds of triggers
// have a non-blocking delay -- it just checks a running counter
// also: set an analog value on a pin
// show/hide by zone
// re-randomization on repeat/copy/mirror, etc.
// new effects
////////////////////
// strobe, like blink but goes from normal to high instead of from low to normal to do the blinking
// flash -- flashes bright white 1 or 3 times
// lightning 
// new commands
////////////////////
// hide / show
// move command for placement, like:
//    red:57:move
// swap command like move (or instead of)
// shutter / slide in / slide out / random change / etc. transitions
// after pause, allow breathing and blinking to come to a halt first
// reverse, inverse mirror
// full frame animation in/out using back buffer
// shooting up
// transition: all leds get sucked into one LED position
// being able to specify whether to animate for any color push

// logging

// need simple way to make decisions
  // for example n,12,13 returns 13 if n==0 otherwise 12 (or semantically might be better as n,0th,1st so if n==0 it's 12 and if n==1 its 13
// add links to program macros
// first needed trigger: run a macro when the breathing level becomes black, so the color can be changed on each breath
// it's just some defines for types of events, like breath == off, or blink == off, infinite loop broken out of, etc.
//  then each just points to a position in a byte array of macro numbers to run in response to the trigger
// try again a command to wait for blink counter = 0 (or all off instead?)
// for button presses, need pin-in command that leaves 0 or 1 as arg0
// quantization of positions and blocking (sets of leds)
// ability to display one buffer on two displays as a standard configuration
// get 2 random numbers at once









// hardware:
// radius8 - fix pixel; add linkit
// projector2 - add linkit




/* specs */
// add test for color correction when using dynamnic colors
// tests: determine if mapping is enabled (so it's tested only on x,y displays)
// need to test all sequencer modes,  I had trouble getting some to work
// is this tested? // allow setting default effect (for instnace to save having to set a slow fade on every case)



/* multicasting */
// on command, show which ones acknowledged
      // the device name should come back
// don't wait forever for ack from arduino because it could conflict with http_command or a python app
// have them communicate and split the rainbow
  // they could elect a leader, and count the number of members
  // add message type (along with new machine type, machine name, existing key)
// could have device types and names and send selective multicast messages
// a form of run that assumes it was a one-off executable not still running, like runonce=
// use a back off algorithm for the repeated sends
  // can that be done in a thread?
// add way to run system command on all devices
// add new arg to http_command.py that issues multicast (or another flag to make it broadcast)
// need to run and runonce commands
// need to build into http_command to reduce loading delay
// send a selector for types of devices, or names, or certain characteristics











/* modify existing commands */
// blend could work over a numnber of pixels
// allow the rnd command to fill the available LEDs of the width
// have a sequencer that runs to one side, stops, and has to be triggered to continue to the other side
// allow setting the current value of a sequencer (to allow multiple ones running that are offset from each other)
// for palette rotation, maybe specify how many colors to involve in rotation, such as 6
// could expand rev command into insertion/operation mode and add things like: xor-remixed position insertion, other interesting insertion modes
// modify clr to reset config to defaults
// a mode when doing random position: only even, or only odd, or only mod N
// -1,0:pal could do random palette color, -1,5:pal do random within first 5 colors
// cfg command - signal to reset to default config value (for any of the configs)
// shf: need to rotate and transform palette in various ways
// the color commands could always be a shortcut to colors in the palette instead of fixed values
// need a way to set a sequence relatively, like to cover a zone
// ability to set current sequence value to a random number in range
// need a way to reset all sequncers (-1:seq?)
// need to be able to link sequencers (triggers?)
// a form of rotate that automatically shifts in carry color
// could have a non-blinking blink, just display the dim color, for background decoration
// vesions of mirror and flip that blend the colors
// would be nice to flood by adding instead of replacing colors, maybe an effect that governs how the color gets placed (whether it adds or overwrites)
// could have an advance command that sets the position to the next position
// other inputs to rnd: choose one of any of the available colors, choose one of the b&w colors
// flip command to reverse all bits in current zone
// third arg to "rng" could be the quantization ("2" means all even values, "3" means all multiples of 3)
// add time delay param to animated rotation
// need palette color to be able to pull from whole list of 25 standard colors




/* changes to consider */
// should push_rgb unscale to the configured default brightness or a standard brightness, like the 0-20 color standard




/* testing */
// negative testing: ensure things out of bounds don't crash
// rerun the test suite on various devices; there are some device-specific spec failures
// multiple displays specs
// dump accum results quitting specs early
// have a way to spit out results while running
// set exit code from led_specs depending on failures or not
// tests: display time it took to run them
// need to reset blink period after testing
// test could highlight differences between strings to reduce need to manually diff them 
// switch to list groups
// select specific test #




/* troubleshooting */
// why does changing number of displays affect program memory (any structures in progmem?)
// crash annunciator switching from wheel with next window to wheel wit position only 
// make sure sequence parameters can be changed while running without resetting and without negative effects
// for dynamically changing scheduler timing, make sure changing the time can be done without reseting it
// can the wheel sequence be switched in direction?
// sequencer will leave value in arg0, how to get it into arg1?
      // another sequencer type?
// rst should not set display back to zero
// does an empty command, ":", clear arguments?
// 0,255,255:hsl at 100:lev is less bright than red at 100:lev (hsl brightness compensation problem?)
// opposite value doesn't seem to work proper except starting a zero
// ignore out of range sequencer numbers
// need to ensure not getting same random color twice in a row (will make things prettier)
// where possible, special mode ints should be positive so they can be randomized
// for red/green swapped led strands, the carry color comes back flipped
// cpy needs to overlap the end (for proper fill)
// the fade problem is when running from within a schedule. Running a macro N times from another macro works from the command line
// the multi-run from the command line is spectacular, why is not applying the fade at the same rate?
// when setting position and placing a rubber stamp, it only places one pixel (which is expected)
      // need a way to expand the window to easily place the whole stamp
      // for instance, -6:win could mean to expand the current window +6
// need a way to pause, and then restore the previous continued state, for instance, without effects
// need a way to clear carry color or not set it during a buffer shift
// slow fade doesn't work when running a macro repeatedly like 1000,0:run. It works when running it manually over serial.
// 0:set:20,0,0:rgb:0,20,0:rgb:0,0,20:rgb:flu
// this resutls in red, followed by two blacks
//    maybe args getting reset at wrong time
// if macro run more often than fade period, no fading at all happens
// visual representation of eeprom memory
// wasn't able to schedule macro #0
// setting blink period to zero didn't reset to default
// if paused, bre effect renders black, causing probems transitioning between animations and breathing (blink renders ok)
// problems with offset and window
// pshifto - starts at [0]
// can't figure out how to pass two arguments into a macro and using them separately
// wasn't able to use arg1 with n,-2:seq or n,-1:seq - just got nothing
// blink rate of annunciator (1) is twice too fast








/* performance */
// cache some things when rendering, for instance, the states for each blink effect, the breathe step, so they're not computed constantly
// share code between palette and buffer rotation
      // maybe allow any buffer operation on the palette
// could have 8-bit versions of sequencers for speed
// ideas for reducing code footprint
      // remove commands that are effecively the same like rep and cpy (keep cpy)
      // remove overly-protective guard code
      // move additional settings to the cfg command
      // segment features so they can be enabled/disabled easily
      // remove all floating point constants / math
      // pass a pointer to dependencies instead of passing individual arguments if/where possible
// maybe alternate between blink, breathe and fade processing to allow scheduling more often
// move mechanical buffer operations like mirror to Buffer class
// maybe making some pointers const would save memory
// immediately assume it's not a command if the string is not three chars (to save command look up time when it's just arguments)
// improvement: when the input buffer has too many incoming chars
// unget the current building command so it can be gotten next
// or discard the current building command and the rest of the buffer
// or ignore the entire request as potentially truncated (though it's already processed commands)
// need to be able to clear all macros
// maybe a flush is implied when running a macro, or maybe it would in certain cases (even number macros auto-flush
// there could be a special delimiter that takes the first byte of the buffer as a pre-looked-up command number
// the delimiter could mark a series of bytes to be processed that way
// macros would be stored this way for reduced memory use
// python programs would be able to use the delimiter when setting macros with binary values
// -- the delimiter is problematic
// instead accept a series of binary bytes of data followed by a special command for storing binary macros
// -- would require holding onto the input buffer (adding this back)
// could use the top 3 bits of the effects array to store the carry bits from color math overruns
// -- could probably get the same results by preventing the numbers from exceeding 0-255
// shortcut for continuing with another macro
// can delays in macros be made non-blocking?
// should be able to rerun the last set of commands with an automatic macro
// maybe have a special schedule that can run even if paused, would allow pausing and unpausing automatically
//-- figure out when to automatically reset blink period to default, like on clear
// need to be able to set offset override to zero (case: a zone is selected and want to override with the value zero)
// send a different acknowledgement code when first started up, so the script can detect and refresh (for instance, an uppercase K)
// see if auto brigtness needs to use that ram for its buffer
// quit the crossfade early if things stop changing, for faster switch between similar colors
// randomly scheduled macro
// 0:set:rnd:500:rng:sch (it could stop itself)
// 5:set:2:rng:rev:rnd:flu:500,1,5:rng:sch:rst (can't stop itself and also not macro #0)
// to simplify macros
// rps combine 0:rng:pos into a random position command
// rzn combine -2,-2,rng:zon into a random fine zone command
// combine 2:rng:rev into a random reversal command
// a way to simplify random scheduling such as 500,1,5:rng:sch
// designate pin #13 as the "fan" pin
  // add to the brightness setting code to turn the fan on over a certain brightness level
 // when rendering with dynamic palette colors, could temporarily pre-render the palette colors to avoid doing the math for every case
 // max brightness
// add argument when setting macro: byte offset, would allow tigher packing of macros







/* python */
// backtick to run command in color_command; also up arrow for previous commands
// color command ignore lines starting with pound
// each device should periodically check for new versions of http_command.py and http_command.html and copy to /root any new versions
// how often? 4 hours?
// allow stop command to accept a program name so I can stop something 
// that had been run over ssh
// sparkle: specify width of segments and gaps
// for wearable, have both ways to launch circleci7
// add autobrightness to circleci7
// add a command line switch to turn off logging for circleci7
// html: have links to other pages on same device, and other devices
// python:
// ci-skip build status is "not_run"
// fade based on build time
// change refresh rate based on time of day (afterhours it doesn't need to check that often)
// show something if the data hasn't updated in a long time
// show something if the data has been unchanged for some time, like after hours
// brief flash, or something, to alert to a new (green?) build
// some way to visually tell which are changes from a little while ago (like the fading yellow highlight on web pages)
// should time out after a while if the ACK is not received and just assume it, could be the Arduino side was reset 
/*python: 
if there's no new builds, display an alternate low power version
- dim for all the colors
- breathing and blinking still happen normally
seems like it would be better to let the arduino handle the low power rendering so the python script isn't in charge of an effect.
lowpower command means:
 - could be: normal display effects, but all-but-one LEDs are blacked out (or not rendered).
 - the position rendered would vary slowly and go low-high (not back and forth)
 opposite name? normal
 flush can call the alternate low-power rendering 
// Python: also show the high power mode if any of the statuses change*/
// need to close a running app before starting circleci
// need to stop circleci if running an app
// would be nice to do something graphicallty once linux is running
// in verose mode, show the commands being transmnitted
// trippy form of circleci with cosine sweeps
// program to compute timing of blinks and breathe for calibration
// pretty wifi output
// wifi mode change




/* documentation */
// working example of passing params to a macro
        // command: 0:set:rcl:pos:red:flo:flu:rst
        // command: stp
        // command: 10,20:sto:0:run
// documentation to wiki
// hardware triggers, such as button presses, will be called something else, like Actions
// the fade counter only decides when to automatically flush, not the rate at which fading happens; repeated renders will automatically fade the buffer color
// wiki page with sample macros
// update read.me with udp_command




/* visual goals */
// sequence crawling from zone to zone (triggers) 
// move a sequencing zone back and forth slowly (value safety upon arbitrary changes)
// something speeding up and slowing down (should be able to adjust schedule timing without it resetting)
// shooting star problem
      // need to continuously advance the position and place the latest color at the leading edge
// would be cool to somehow do mapping where the zone rotations on radius8 could be done on the opposite axis
// with mapping in place you should be able to apply rotations to rows and colums, in fact multiple rows and columns
      // it's possible that when mapping is in place, there's no such thing as zones
      // unmapped pixels just would be ignored
      // the expectation is it can be treated as an X x Y grid
      // radius8: w16 x h16
      // sphere: w10 x h17
      // disc (cylinder): w6 x h32 (this could simplify some effects on the projector)
      // disc mapping
      // col 6: full 0-31                 01234567890123456789012345678901 
      // col 5: 0-23 interpolated to 0-31 01223455678890112344567789001233
      // col 4: 0-15 interpolated to 0-31 00112233445566778899001122334455
      // col 3: 0-11 interpolated to 0-31 00011222333445556667788899900111
      // col 2: 0-7 interpolated to 0-31  00001111222233334444555566667777
      // col 1: 0 interpolated to 0-31    00000000000000000000000000000000
      // mapping would need to be done at the buffer level
      // pos = map(x,y)
      // x,y:pos 
      // if a second argument is provided, it could mean  use x,y mapping
      // in the case of the zero default, the x positions would still be valid positions
      // need to implement mapping for radius8 & sphere
// shooting star would be
// 5:set:wht:brt:brt:sfd:flu
// rps - random starting position
// 2:rng:rev - random direction
// 20,1,5:rng - random # between 1 and 19 with arg1 = 5
// seq - sequence arg0 from 0 to n-1 and run macro 5 each time
// apply ease animation timing to rotations
// blast through a random section
// pick a random starting position in current width
// pick a random ending position, in the going direction, up to the end of the current width
// -- could have a command that chooses a random range in current width going in current direction
//      -x:rps
// have virtual colors that always point to a position in the palette
// how? colors are stored as RGB
// can't add a flag indicating to treat it as an integer
// could have an effects bit that means get from palette
// would be nice  have a way to choose two colors that look nice together instead of arbitrary colors
// idea: a brightness curve applied across a zone; perhaps with a sweeping animation
// sweeping crossfade effect
/*
 * travel down width
 * must pass all points beginning to end of the crossfade to fade the edges properly
 * each animation pass sequences all crossfade steps spread across the same numnber of leds
 * 
 */
/* glasses
high tech color choices (green rings with spaced out single reds looks high tech)
Movements that appear Borg-like
  one eye, radar/clock-ish constantly moving 
  other eye, occasional rotation, varying direction and duration
  sides - some back and forth, and blinking
  occasional fast spinning or bright flashing
figure-8
the cops
ggggggg*/
/* car lights ideas
fast flashing colors
slow crossfading colors
nighttime sleeping
cron job to turn on and off in the middle of the night
random lengths of colors
very slow/occasional blinking / deterrent
multiple rotating zones
short white burst like once a minute
kitt kar like back and forth
  with hsl color*/
// add color groups
// strobe / flash
// random flashes more like a mechanical blinking light
// b&w static, flame
// invert hue, swing hue, back and forth between two arbitrary colors, rgb cube
/// have a way to choose random colors including black and materials
// from a sequence get just the direction, 1 or -1, to allow stepping something else  
// need a fade from black
//    maybe invert the otherwise fade to black color (subtract from color standard of 20)
// six-color dynamic blink: uses palette colors 0-15 stored in rgb







/* ideas */
// could build upon macros to form the concept of apps
      // apps:
      // - have a way to start up
      // - have a way to shut down
      // - perhaps other common behaviors
// could have an effect that runs a macro at render time to do whatever it wants, with the rendering pixel as the current window
      // - this could be very slow
      // - perhaps call the macro one time per scan line rather than per incidence of effect in buffer
      // - example of use: 
      //   - could have a dynamic display without needing to have a macro running all the time
      // --> beginning of rendering could be a trigger
// how to simplify switching?
//    ::process_commands_P(F("19:stm:-1:sch:30000,19,20:sch:17:run"));
//    ::process_commands_P(F("20:stm:-1:sch:60000,19,21:sch:18:run"));
//    ::process_commands_P(F("21:stm:-1:sch:60000,19,19:sch:23:run"));
 // could have a special form of run that sequences a series of arg[0] values
 // 3,10,5:eun = run macro three for each value from 5 to 10 
// or 10,3,5:run - allowing the count to be random
// 20:run - runs macro #0 20 times, with arg0 from 0-19
// 20,1:run = runs macro #1 20 times
// 20,2,5:run = runs macro 2 15 times, from 5 to 19
// how would I draw a line?
      // 3,4:pos - to set the starting position
      // 10,11:lin - sets the ending position
      //  but what would it do next?
      // idea: 10,11,7:lin - sets the ending position, and calls macro 7 for each distinct point
// idea: per-zone settable settings that affect rendering
// - brightness, black level, direction, palette offset
// - when you set certain values, they stick to the zone you're on
// ability to pause the writing to the display (but not pause rendering; would allow for quiet testing)
// could have commands that take the rest of the input and do something with them iteratively, like an instant macro
// 9:mac:commands:commands -> would run the commands 9 times
// this could save macro space for one-off special repeat functions
// 9,100:mac:commands -> would run the commands 9 times with 100ms delay between each
// could have conditional macro running
// arg0 = 0 or 1
// arg1 = macro to run if 1
// arg2 = macro to run if 0
// other canned values
// current zone, current offset, current window, current display, current reversal
// modulo could be used to toggle:
// 2:rng:1:add:2:mod
// remix
// add reboot command
// units that have sensors could broadcast info, like the current temperature or light level






/* new colors */
// add a 15,15,15 gray
// water other recognizable colors are common?
      // xenon white
      // argon purple
      // green light green
      // yelllow light amber
// add remaining 5's colors (they may be took dark)







// to compute HSL lightness to match standard color brightness:
// 0,4:tst - get brightness percent
// lightness-max = (255 * (brightness percent / 100)) + 1
//command: 0,4:tst
//15k
//command: era:grn:120,255,38:hsl:2,0,2:tst
//0,19,0,0,20,0,k
//command: era:grn:120,255,39:hsl:2,0,2:tst
//0,20,0,0,20,0,k


// rolling crossfade
// leading edge does step #1
// leading edge keeps advancing doing step #1
// goes back to trailing edge (trailing edge starts at zero)
// each step back advances to the next crossfade step
// once the trailing edge seens the last crossfast step start advancing the trailing edge
// the leading edge stops advancing when it hits the edge
// 
// leading edge starts at zero, goes to limit + count of crossfade steps
//
// 0 1 2 3 4 5 6 7 8 9
// X X X X X X X X X X 
// 0
// 1 0
// 2 1 0
// 3 2 1 0
// 4 3 2 1 0
// 5 4 3 2 1 0
// 5 5 4 3 2 1 0
// 5 5 5 4 3 2 1 0
// 5 5 5 5 4 3 2 1 0
// 5 5 5 5 5 4 3 2 1 0
// 5 5 5 5 5 5 4 3 2 1
// 5 5 5 5 5 5 5 4 3 2
// 5 5 5 5 5 5 5 5 4 3
// 5 5 5 5 5 5 5 5 5 4
// 5 5 5 5 5 5 5 5 5 5

// leading and trailing edge iterate through each led position
// leading edge stays at max once it hits it
// trailing edge stays at zero until leading edge hits count of crossfade steps
// trailing edge goes all the way to max
// leading edge could go past max + crossfade steps
//   led position computed from it but maxes out at limit
//   crossfade step computed from it, as (leading edge position - led position) maxing out at max crossfade step
//
//def at_most(a,b):
//  return a if a <= b else b  
//
//num_leds = 10
//max_led = num_leds - 1
//num_steps = 5
//max_step = num_steps - 1
//max_loops = max_led + max_step + 1
//
//for i in range(max_loops):
//  last_led_pos = at_most(i, max_led)
//  for led_pos in range(last_led_pos + 1):
//    step = at_most(i - led_pos, max_step)
//    print str(led_pos) + ":" + str(step), 
//  print
//
//0:0
//0:1 1:0
//0:2 1:1 2:0
//0:3 1:2 2:1 3:0
//0:4 1:3 2:2 3:1 4:0
//0:4 1:4 2:3 3:2 4:1 5:0
//0:4 1:4 2:4 3:3 4:2 5:1 6:0
//0:4 1:4 2:4 3:4 4:3 5:2 6:1 7:0
//0:4 1:4 2:4 3:4 4:4 5:3 6:2 7:1 8:0
//0:4 1:4 2:4 3:4 4:4 5:4 6:3 7:2 8:1 9:0
//0:4 1:4 2:4 3:4 4:4 5:4 6:4 7:3 8:2 9:1
//0:4 1:4 2:4 3:4 4:4 5:4 6:4 7:4 8:3 9:2
//0:4 1:4 2:4 3:4 4:4 5:4 6:4 7:4 8:4 9:3
//0:4 1:4 2:4 3:4 4:4 5:4 6:4 7:4 8:4 9:4
//
// 
//
//
//
//
//
//
//def do_led(led_pos, cfstep):
//  print str(led_pos) + ":" + str(cfstep),
//
//def do_step(step):
//  last_led_pos = at_most(step, max_led)
//  for led_pos in range(last_led_pos + 1):
//    cfstep = at_most(step - led_pos, max_step)
//    do_led(led_pos + offset, cfstep)
//  print
//
//offset = 10
//window = 19
//num_leds = window - offset + 1
//max_led = num_leds - 1
//num_steps = 6
//max_step = num_steps - 1
//max_loops = max_led + max_step + 1
//
//for step in range(max_loops):
//  do_step(step)
