
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
* FreeType2 (For font support, bitmap fonts do not need freetype)


----
### Fedora 32 on 64bit system dependencies
Run the command below to install all the dependencies on Fedora distros after Fedora 26.

```$ sudo dnf install gcc cmake cmake-gui libX11 libXinerama-devel.x86_64 libXrandr-devel.x86_64 libXext doxygen freetype fifechan-opengl-devel.x86_64 mingw64-winpthreads.noarch freetype-devel.x86_64 pulseaudio-libs-devel.x86_64```
    
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
    d. Then click Configure and Generate and close the cmake gui.

 6. Now run the command below to make files.

    ```$ make```
    
    **Note:** Add -j8, -j4 for the number of CPU cores you would like to use to build Gorgon as it can take time with a sigle core. 

7. Finally install Gorgon using sudo users.

    ```$ sudo make install```
    
