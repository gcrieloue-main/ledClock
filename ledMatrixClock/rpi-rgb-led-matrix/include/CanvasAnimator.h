
#ifndef CANVAS_ANIMATOR_H
#define CANVAS_ANIMATOR_H

#include "led-matrix.h"
#include "graphics.h"
#include "CanvasConfig.h"

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
#include <vector>

using namespace rgb_matrix;

class CanvasAnimator
{
    private:
        RGBMatrix* canvas;
        CanvasConfig* canvasConfig;
        FrameCanvas* frame;
        bool isRainOver(std::vector<std::vector<int>>&);
        void drawOscillo(FrameCanvas* frame, int x1, int x2);

    public:
        CanvasAnimator(RGBMatrix* c, CanvasConfig* cc);

        void spiraleAnimate();
        void randomizeAnimate();
        void animateOscillo();
        void animateRain();
        void animateColors();
        void animateCountdown(int min);

        void hourAnimateOpening();
        void hourAnimateClosing();

        void coolBorderAnimate();
        void coolBorderAnimateOpening();
        void coolBorderAnimateClosing();
        void drawLineAnimated(bool orientation, bool direction, int x1, int y1, int length, int r, int g, int b);
        void clear();
};

#endif
