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
 "refresh-display" : "flu",
 "reset-buffer" : "rst",
 "erase-buffer" : "era",
 "rotate" : "rot",
 "repeat" : "rep",
 "copy" : "cpy",
 "paste" : "cpy",
 "duplicate" : "cpy",
 "flood" : "flo",
 "fill" : "flo",
 "mirror" : "mir",
 "set-display" : "dis",
 "set-zone" : "zon",
 "set-window" : "win",
 "set-offset" : "off",
 "set-reverse" : "rev",
 "add-rgb-color" : "rgb",
 "add-hsl-color" : "hsl",
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
 "turquoise" : "tur",
 "lavender" : "lav",
 "rose" : "ros",
 "dark-gray" : "dgr",
 "gray" : "gry",
 "white" : "wht",
 "tungsten" : "tun",
 "neon" : "neo",
 "sodium" : "sod",
 "black" : "blk",
 "add-random-color" : "rnd",
 "add-dynamic-color" : "dyn",
 "blend-colors" : "ble",
 "maximize-brightness" : "max",
 "darken" : "dim",
 "brighten" : "brt",
 "add-static" : "sta",
 "set-blink" : "bli",
 "set-blink-1" : "bl1",
 "set-blink-2" : "bl2",
 "set-blink-3" : "bl3",
 "set-blink-4" : "bl4",
 "set-blink-5" : "bl5",
 "set-blink-6" : "bl6",
 "set-blink-a" : "bla",
 "set-blink-b" : "blb",
 "set-dynamic-blink" : "bld",
 "set-breathe" : "bre",
 "set-dynamic-breate" : "brd",
 "slow-fade" : "sfd",			# keep as a type
 "set-slow-fade" : "sfd",
 "fast-fade" : "ffd",                   # keep as a type
 "set-fast-fade" : "ffd",
 "store" : "sto",
 "recall" : "rcl",
 "push" : "psh",
 "pause" : "pau",
 "pause-effects" : "1:pau",
 "pause-schedules" : "2:pau",
 "continue" : "cnt",
 "continue-effects" : "1:cnt",
 "continue-schedules" : "2:cnt",
 "clear" : "clr",
 "set-brightness" : "lev",
 "do-fade-animation" : "fad",
 "do-animated-rotation" : "art",
 "do_crossfade" : "cfa",
 "set-random-blink" : "blr",
 "set-random-effect" : "efr",
 "test" : "tst",
 "inquire" : "tst",
 "configure" : "cfg",
 "set-pin" : "pin",
 "schedule" : "sch",
 "set-schedule" : "sch",
 "add-carry-color" : "car",
 "set-macro" : "set",
 "run-macro" : "run",
 "delay" : "del",
 "stop" : "stp",
 "get-random-number" : "rng",
 "set-position" : "pos",
 "set-random-position" : "rps",
 "set-xy-position" : "xyp",
 "add-palette-color" : "pal",
 "add-palette-colors" : "pal", 
 "shuffle-palette" : "shf",
 "reset-palette" : "1:shf",
 "set-black-level" : "sbl",
 "advance-sequence" : "seq",
 "get-sequence" : "seq",
 "wheel-sequencer" : "seq",		# keep as a type
 "start-wheel-sequence" : "seq",
 "swing-sequencer" : "sqs",              # keep as a type
 "start-swing-sequence" : "sqs",
 "wheel-cosine-sequencer" : "swc",      # keep as a type
 "cosine-sequencer" : "swc",             # keep as a type
 "start-cosine-sequence" : "swc",
 "wheel-sine-sequencer" : "sws",        # keep as a type
 "sine-sequencer" : "sws",              # keep as a type
 "start-sine-sequence" : "sws",
 "sequence-next-window" : "snw",
 "get-next-window" : "snw",
 "set-palette-by-hue" : "csh",
 "set-palette-by-saturation" : "css",
 "set-palette-by-lightness" : "csl",
 "set-fan" : "fan",
 "start-application" : "app",
 "add-equals" : "add",
 "sub-equals" : "sub",
 "subtract-equals" : "sub",
 "mul-equals" : "mul",
 "multiply-equals" : "mul",
 "div-equals" : "div",
 "divide-equals" : "div",
 "mod-equals" : "mod",
 "modulo-equals" : "mod",
 "dif-equals" : "dif",
 "difference-equals" : "dif",
 "avg-equals" : "avg",
 "average-equals" : "avg",
 "set-draw-mode" : "drw",
 "clear-schedules" : "-1:sch",
 "true" : "1",
 "false" : "0",
 "enable" : "1",
 "disable" : "0",
 "on" : "1",
 "off" : "0"
}

