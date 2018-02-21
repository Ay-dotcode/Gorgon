# Gorgon Game Engine

Gorgon Game Engine (GGE) is a C++ game engine that handles many tasks.

GGE is a multi-module, interface driven, C++ game engine that handles graphics, animations, sound, multimedia, input, operating system, resource management, simple effects and user interface. Its primary aim is to be easy to program on.

https://sourceforge.net/p/gorgon-ge/

# GGE 3.x (default branch)

## Requirements
* On Linux:
  * GCC 4.8.1+
  * CMake 2.8+
  * OpenAL
  * X11
  * pthread
  * rt

* On windows:
  * Visual Studio 2013 with multi-byte patch (not tested), Visual Studio 2015
  * OpenALSoft
  * CMake 2.8+

## How to
Use the examples provided in the Examples directory.

# GGE 4.x (4.x-dev branch)

GGE 4.x is still in development and is not feature complete.

## Requirements
* GCC 4.8.1+ / Visual Studio 2015
* CMake 2.8
* libX11, libXinerama, libXrandr, libXext (Linux)
* libpulse
* pthreads (Linux)
* doxygen
* OpenGL
* libCurl (For HTTP transport)

## Documentation
http://darkgaze.org/GGEDoc/index.html or can be built using doxygen

# License
[GPL](http://www.gnu.org/copyleft/gpl.html) (commercial use might be allowed depending on the project, please send a mail for details)

# Contributing
All contributions should be filed on the [sf.net/gorgon-ge](https://hg.code.sf.net/p/gorgon-ge/code) repository.

* Fork the repo, push your changes to your fork, and submit a pull request.
* If something does not work, please report it using [Sourceforge Tickets](https://sourceforge.net/p/gorgon-ge/tickets/) or [Issues](https://bitbucket.org/darkgazeorg/gorgon-game-engine/issues). These issues will then moved to project management system. You will be notified when the issue is solved.