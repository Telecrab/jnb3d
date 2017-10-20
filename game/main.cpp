#include "window.h"

int main()
{
    return Window::getInstance(640, 640, false)->mainLoop();
}


