# JET SET WILLY

## About

Originally written in 1984 by Matthew Smith. This port is based on the original
ZX Spectrum version, written in C and using the SDL2 library.

## Game play

Game play is 100% identical. The original game had bugs which have been fixed, such as landing at the end
of a jump into a solid wall.

## Video & Audio

Some subtle improvements have been made to make the game more enjoyable:

- Per pixel colouring. This eliminates colour clashing.
- 16 colour palette.
- 2 replacement character set fonts; one small, one large.
- The title and in-game music scores have been reproduced and are polyphonic.
- The sound effects are approximately the same as in the original game and
include stereo panning effects.
- To give the music and sound effects a retro feel, a square wave generator is
used to give it a "beepy" sound.

## Cheat mode

Cheat mode is activated just like in the original game by typing the code. Once
activated, switching levels is as simple.

The keyboard numbers 1 to 0 are levels 1 to 10, letters A to T are levels
11 to 30 and the Shift key changes that to levels 31 to 60. Then press Enter
to change level. These key combinations need to be pressed simultaneously.

## Copy protection

The original game needed a "code card" to start the game after loading. This was
an early attempt at copy protection. If you don't want to experience entering
those codes, press any key during the "JetSet Willy Loading" sequence.

# Windows build?

Adding support for Windows is not planned. However, AukonDK of the [Jet Set Willy & Manic Miner community](https://jswmm.co.uk/)
has kindly provided a 64bit of both Manic Miner & JetSet Willy for Windows available here: [MM and JSW ports to SDL2](https://jswmm.co.uk/topic/791-mm-and-jsw-ports-to-sdl2/).
