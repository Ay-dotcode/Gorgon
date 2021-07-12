
GGE 4.x is still in development and is not feature complete.

## Requirements

### Compiler
* GCC 8+
* Visual Studio 2019
* Clang (not fully supported)

### Tools
* CMake 3.5
* Doxygen

### Common
* OpenGL

### Linux
* libX11, libXinerama, libXrandr, libXext
* libpulse
* pthreads
* fontconfig

### Optional (built-in available)
* FreeType2 (For font support, bitmap fonts do not need freetype)
* libCurl (For HTTP transport, built-in not yet available, disabled by default)
* FLAC (Lossless audio)
* Vorbis (Audio)

----

### Fedora 32/33 on 64bit system dependencies
Run the command below to install all the dependencies on Fedora distros after Fedora 26.

```$ sudo dnf install gcc cmake cmake-gui libX11 libXinerama-devel libXrandr-devel libXext doxygen freetype freetype-devel pulseaudio-libs-devel fontconfig-devel libcurl flac-devel libvorbis-devel```

----

### Ubuntu 18.04 64 bit system dependencies
Run the command below to install all the dependencies on Ubuntu 18.04.

```$ sudo apt-get install gcc cmake cmake-gui libx11-dev libxinerama-dev libxrandr-dev libxtst-dev doxygen freetype libfifechan-dev libfreetype6-dev libpulse-dev libfontconfig1-dev libflac-dev libvorbis-dev```


----
    
## Install Gorgon using cmake (Linux)

 1. Navigate to the Gorgon directory through terminal.

 2. Inside the Gorgon Directory create a folder called build. 

    ```$ mkdir build```

 3. Then change directory to build.

    ```$ cd build```

 4. Type in the following command to setup the cmake file system.

    ```$ cmake -S /Path/To/Gorgon -B /Path/To/Gorgon/build```

 5. Next run the command below to open the cmake gui config file

    ```$ cmake-gui```

    a. Set the source code path to /Path/To/Gorgon and the build binaries path to /Path/To/Gorgon/build if not done already, and press Configure if paths were not set. (skip step otherwise)
    b. Set CMAKE_BUILD_TYPE TO Debug
    c. Select desired modules and tools to be installed
    d. If you do not want to install libraries, you may select built-in.
    e. Then click Configure and Generate and close the cmake gui.

 6. Now run the command below to make files.

    ```$ make```
    
    **Note:** Add -j8, -j4 for the number of CPU cores you would like to use to build Gorgon as it can take time with a sigle core. 

 7. Finally install Gorgon using sudo users.

    ```$ sudo make install```
    
