set(Local
    X11.h
    X11.cpp
    X11Keysym.h
    Clipboard.cpp
    DnD.cpp
    Monitor.cpp
    Input.cpp
    Window.cpp
)

FIND_PACKAGE(X11 REQUIRED)

LIST(APPEND Libs ${X11_LIBRARIES} ${X11_Xrandr_LIB} ${X11_Xinerama_LIB})
