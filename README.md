NetRun
======

Overview
--------

NetRun is a roguelike I wrote for fun. It was my first biggish project, and employs lots of design concepts I had never tried before.

Design
------

NetRun has, in my opinion, a very fun design. It uses Binary Space Partitioning to generate a map of tiles. From there most game activity revolves around interacting with tiles:

- If an entity interacts with an empty tile it is moved into that square 
- If it moves into a trapped square the trap is set off 
- Moving into a door would open the door if closed, or act like an empty square otherwise
- Moving into an occupied square instead interacts with the occupying entity (usually hits a monster)
- Countless other uses

This creates a layer of modularity I've rarely seen in a roguelike, where monster and player code hardly needs to be aware of the rest of the game's design at all.

Exponential Growth
------------------

At one point I used NetRun in a math class to demonstrate exponential growth. That code has yet to be removed, so the program asks for a growth rate and makes the monsters multiply accordingly. A reasonable rate is 0.2 if you want the game to be winnable.

Dependencies
------------

Standard C and C++ libraries, and ncurses. In addition, part of the project is written in C++ 11, so you'll need a compiler that supports that.

At one point the project ran on both Linux and FreeBSD, but it may not be cross platform right now. It's known to build on Gentoo GNU/Linux with g++ 4.6.3.
