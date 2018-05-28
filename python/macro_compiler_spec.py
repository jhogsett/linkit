#!/usr/bin/python

import sys
import macro_compiler as mc

def expect(description, expected, got):
  if expected != got:
    print "failed: " + description
    print "expected: " + str(expected)
    print "got: " + str(got)

def print_script(script):
  for line in script:
    print line
  print

def test(description):
  return True

########################################################################
########################################################################

def specs():
########################################################################
# simple script
########################################################################
  if test("simple script"):
    script_text = '''
# test script

$max 10

[init]
red
<max>:rep
(render)

[doit 1]
blu

[render]
flu
    '''

    script = script_text.split("\n")
    compiled_script = mc.compile_script(script)
    print_script(compiled_script)

    expected_script = [
'14:set:red:10:rep:15:run', 
'15:set:flu', 
'1:set:blu'
    ]
    expect("compiled script #1", expected_script, compiled_script)

    mc.reset()

########################################################################
# complex script
########################################################################  
  if test("complex script"):
    script_text = '''
# red green and blue wandering pixels with mirror

#[name {number}] start macro
#(name) run macro

# variables
$max 90
$max-half 45
$render-time 50
$sequence-time 20
$fade-type ffd
#$red-seq 0
#$green-seq 1
#$blue-seq 2
$wander-range 3
$wander-neg-range -1

# use variables or recall macro numbers <name>

# command value substitutions
# these should be all caps built-in?
$get-current -1
$macro-seq -4


# fancy app
[fancy 10]
(init)
(setup-rendering)
(start-components)

# initialization
[init]
app

# rendering
[setup-rendering]
<render-time>,<render>:sch

[render]
mir
flu

# start up components
[start-components]
(start-red)
(start-green)
(start-blue)

# red wanderer
[start-red]
{red-seq},<max>,0:seq
<sequence-time>,<red-sequence>:sch

[red-sequence]
<red-seq>,<macro-seq>,<red-math>:seq
pos
<red-seq>,0,1:pal
<fade-type>
rst

[red-math]
<wander-range>:rng:psh
<wander-neg-range>:psh
add
<red-seq>,<get-current>:seq:psh
add
<max-half>:psh
mod

# green wanderer
[start-green]
{green-seq},<max>,0:seq
<sequence-time>,<green-sequence>:sch

[green-sequence]
<green-seq>,<macro-seq>,<green-math>:seq
pos
<green-seq>,0,1:pal
<fade-type>
rst

[green-math]
<wander-range>:rng:psh
<wander-neg-range>:psh
add
<green-seq>,<get-current>:seq
psh
add
<max-half>:psh
mod


# blue wanderer
[start-blue]
{blue-seq},<max>,0:seq
<sequence-time>,<blue-sequence>:sch

[blue-sequence]
<blue-seq>,<macro-seq>,<blue-math>:seq
pos
<blue-seq>,0,1:pal
<fade-type>
rst

[blue-math]
<wander-range>:rng:psh
<wander-neg-range>:psh
add
<blue-seq>,<get-current>:seq
psh
add
<max-half>:psh
mod
    '''

    script = script_text.split("\n")
    compiled_script = mc.compile_script(script)
    print_script(compiled_script)

    expected_script = [
'10:set:21:run:22:run:24:run', 
'14:set:2,-4,25:seq:pos:2,0,1:pal:ffd:rst', 
'15:set:3:rng:psh:-1:psh:add:0,-1:seq:psh:add:45:psh:mod', 
'16:set:mir:flu', 
'17:set:0,-4,15:seq:pos:0,0,1:pal:ffd:rst', 
'18:set:3:rng:psh:-1:psh:add:1,-1:seq:psh:add:45:psh:mod', 
'19:set:2,90,0:seq:20,14:sch', 
'20:set:1,90,0:seq:20,23:sch', 
'21:set:app', 
'22:set:50,16:sch', 
'23:set:1,-4,18:seq:pos:1,0,1:pal:ffd:rst', 
'24:set:26:run:20:run:19:run', 
'25:set:3:rng:psh:-1:psh:add:2,-1:seq:psh:add:45:psh:mod', 
'26:set:0,90,0:seq:20,17:sch'
    ]
    expect("compiled script #2", expected_script, compiled_script)


############################################################################

def setup():
#    initialize()
#    introduction()
  pass

def loop():
  specs() 
  sys.exit()

if __name__ == '__main__':
  setup()
#  while True:
#    try:
  loop()
#    except KeyboardInterrupt:
#      sys.exit("\nExiting...\n")
#    except Exception:
#      raise

