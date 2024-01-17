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

Cheat mode is activated just like in the orginal by typing the cheat code. Once
activated, switching levels is as simple as typing the level number followed by
ENTER. Levels lower than 10 do not need to be preceded by a zero, i.e. 01 or 1
has the same effect.

## Copy protection

The original game needed a "code card" to start the game after loading. This was
an early attempt at copy protection. This port defaults to not requiring you to
enter any codes. If you want to experience entering those codes, you can comment
out the line in the Makefile "DEFINES += -DNOCODES" with a proceeding hash (#).
