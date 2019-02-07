import re

def translate(line):
    regex = "(\s|[^a-z0-9-])"
    words = re.compile(regex).split(line)

    new_words = []
    for word in words:
        if word in long_commands.keys():
            new_words.append(long_commands[word])
        else:
            new_words.append(word)

    return "".join(new_words)

    #stripped_long_command = long_command.strip()
    #if stripped_long_command in long_commands.keys():
    #    return long_commands[stripped_long_command]
    #else:
    #    return long_command

long_commands = {
 "flush" : "flu",
 "reset" : "rst",
 "erase" : "era",
 "rotate" : "rot",
 "repeat" : "rep",
 "copy" : "cpy",
 "copy" : "cpy",
 "flood" : "flo",
 "mirror" : "mir",
 "display" : "dis",
 "zone" : "zon",
 "window" : "win",
 "offset" : "off",
 "reverse" : "rev",
 "red-green-blue" : "rgb",
 "hue-saturation-lightness" : "hsl",
 "red" : "red",
 "orange" : "org",
 "yellow" : "yel",
 "green" : "grn",
 "blue" : "blu",
 "purple" : "pur",
 "cyan" : "cyn",
 "magenta" : "mag",
 "light-blue" : "lbl",
 "light-green" : "lgr",
 "seafoam-green" : "sea",
 "pink" : "pnk",
 "amber" : "amb",
 "olive" : "olv",
 "sky-blue" : "sky",
 "tuquoise" : "tur",
 "lavender" : "lav",
 "rose" : "ros",
 "dark-gray" : "dgr",
 "gray" : "gry",
 "white" : "wht",
 "tungsten" : "tun",
 "neon" : "neo",
 "sodium" : "sod",
 "black" : "blk",
 "random-color" : "rnd",
 "dynamic-color" : "dyn",
 "blend" : "ble",
 "maximize" : "max",
 "dim" : "dim",
 "brighten" : "brt",
 "static" : "sta",
 "blink" : "bli",
 "blink-1" : "bl1",
 "blink-2" : "bl2",
 "blink-3" : "bl3",
 "blink-4" : "bl4",
 "blink-5" : "bl5",
 "blink-6" : "bl6",
 "blink-a" : "bla",
 "blink-b" : "blb",
 "dynamic-blink" : "bld",
 "breathe" : "bre",
 "dynamic-blink" : "brd",
 "slow-fade" : "sfd",
 "fast-fade" : "ffd",
 "store" : "sto",
 "recall" : "rcl",
 "push" : "psh",
 "pause" : "pau",
 "continue" : "cnt",
 "clear" : "clr",
 "level" : "lev",            # deprecated
 "set-brightness-level" : "lev",
 "fade" : "fad",
 "animated-rotate" : "art",
 "crossfade" : "cfa",
 "random-blink" : "blr",
 "random-effect" : "efr",
 "test" : "tst",
 "configure" : "cfg",
 "set-pin" : "pin",
 "schedule" : "sch",
 "carry-color" : "car",
 "set-macro" : "set",
 "run-macro" : "run",
 "delay" : "del",
 "stop" : "stp",
 "random-number" : "rng",
 "position" : "pos",
 "random-position" : "rps",
 "xy-position" : "xyp",
 "palette-color" : "pal",
 "shuffle" : "shf",
 "set-black-level" : "sbl",
 "sequence" : "seq",
 "wheel-sequencer" : "seq",
 "swing-sequencer" : "sqs",
 "wheel-cosine-sequencer" : "swc",
 "wheel-sine-sequencer" : "sws",
 "sequence-next-window" : "snw",
 "color-sequence-hue" : "csh",
 "color-sequence-saturation" : "css",
 "color-sequence-lightness" : "csl",
 "enable-fan" : "fan",
 "application" : "app",
 "add" : "add",
 "subtract" : "sub",
 "multiply" : "mul",
 "divide" : "div",
 "modulo" : "mod",
 "difference" : "dif",
 "average" : "avg",
 "draw-mode" : "drw",      # deprecated
 "set-draw-mode" : "drw"
}

