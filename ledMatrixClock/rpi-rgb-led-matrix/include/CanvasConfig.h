
#ifndef CANVAS_CONFIG_H
#define CANVAS_CONFIG_H

#include "led-matrix.h"
#include "graphics.h"

#include <iostream>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <sstream>
#include <thread>

using namespace rgb_matrix;

class CanvasConfig
{
    private:

    public:
    CanvasConfig();
    Color color;
    Color color2;
    Color color3;
    Color backgroundColor;
    Font font;
    Font clock_font;
};

#endif
