// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how write text.
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "graphics.h"
#include "socket.h"

#include <iostream>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <sstream>

using namespace rgb_matrix;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

static bool parseColor(Color *c, const char *str) {
    return sscanf(str, "%hhu,%hhu,%hhu", &c->r, &c->g, &c->b) == 3;
}

void* commandListenerThread(void* vector)
{
    std::vector<std::string>* myVector = (std::vector<std::string>*)vector;
    CommandListener* commandListener = new CommandListener(*myVector);
    commandListener->startSocket(); 
    pthread_exit(NULL);
}

class LedClock
{
    public :
        LedClock();

        void spiraleAnimate();
        void randomizeAnimate();
        void hourAnimate(bool opening);
        void coolBorderAnimate(bool opening);
        void drawLineAnimated(bool orientation, bool direction, int x1, int y1, int length, int r, int g, int b);

        void scrollText(const char* text, const char* title);

        std::string pop_front(std::vector<std::string>& vec);
        void* commandListenerThread(void* vector);
        void handleCommand();

        void drawClockWithDate(tm* lt);
        void drawBinaryClock(tm* lt);
        void drawBinaryClockDigit(int x, char elem);
        void drawClock(tm* lt);

        void clear();
        void display();

        std::vector<std::string> myVector;
    private:

        rgb_matrix::Color color;
        rgb_matrix::Color color2;
        rgb_matrix::Color backgroundColor;
        RGBMatrix* canvas;
        const char *bdf_font_file;
        const char *clock_bdf_font_file;
        int x;
        int y;
        int previousMinute;
        rgb_matrix::Font font;
        rgb_matrix::Font clock_font;
        GPIO io;
        int brightness;
        int rows;
        int chain;
        int parallel;
        // mode 1 : standard (HH : MM)
        // mode 2 : binary clock
        // mode 3 : clock with day and month (dd/mm)
        int mode;
};

void LedClock::drawLineAnimated(bool orientation, bool direction, int x1, int y1, int length, int r, int g, int b)
{
    FrameCanvas* frame = canvas->SwapOnVSync(NULL);

    if (orientation) //horizontal
    {
        if (direction)
        {
            for (int x=x1;x<x1+length;x++)
            {
                frame->SetPixel(x,y1,r,g,b);
                usleep(20*1000);
            }
        }
        else 
        {
            for (int x=x1;x>x1-length;x--)
            {
                frame->SetPixel(x,y1,r,g,b);
                usleep(20*1000);
            }
        }
    }
    else //vertical
    {
        if (direction)
        {
            for (int y=y1;y<y1+length;y++)
            {
                frame->SetPixel(x1,y,r,g,b);
                usleep(20*1000);
            }
        }
        else
        {
            for (int y=y1;y>y1-length;y--)
            {
                frame->SetPixel(x1,y,r,g,b);
                usleep(20*1000);
            }
        }
    }
}

void LedClock::spiraleAnimate()
{
    drawLineAnimated(false, false,  0, 15, 16, color.r,color.g,color.b );
    drawLineAnimated(true, true,  0, 0, 31, color.r,color.g,color.b );
    drawLineAnimated(false, true,  31, 0, 16, color.r,color.g,color.b );
    drawLineAnimated(true, false,  31, 15, 31, color.r,color.g,color.b );

    drawLineAnimated(false, false,  1, 15, 15, color.r,color.g,color.b );
    drawLineAnimated(true, true,  1, 1, 30, color.r,color.g,color.b );
    drawLineAnimated( false, true,  30, 1, 15, color.r,color.g,color.b );
    drawLineAnimated( true, false,  30, 14, 30, color.r,color.g,color.b );

    drawLineAnimated( false, false,  2, 14, 14, color.r,color.g,color.b );
    drawLineAnimated( true, true,  2, 2, 29, color.r,color.g,color.b );
    drawLineAnimated( false, true,  29, 2, 14, color.r,color.g,color.b );
    drawLineAnimated( true, false,  29, 13, 29, color.r,color.g,color.b );
}

void LedClock::randomizeAnimate()
{
    int nbPoints = 20;
    int duration = 200;
    int delay = 20*1000;

    clear();
    for (int i=0;i<duration;i++)
    {
        FrameCanvas* frame = canvas->CreateFrameCanvas();
        for (int x=0;x<nbPoints;x++)
        {
            for (int y=0;y<16;y++)
            {
                frame->SetPixel(rand()%32,rand()%16,color.r,color.g,color.b);
            }
        }
        canvas->SwapOnVSync(frame);
        usleep(delay);
    }
    clear();
}

void LedClock::hourAnimate(bool opening)
{
    int r = 80;
    int g = 0;
    int b = 0;

    if (opening)
    {
        for (int i=0;i<14;i++){
            canvas->SetPixel(16+i,3,r,g,b);
            canvas->SetPixel(15-i,3,r,g,b);
            canvas->SetPixel(16+i,12,r,g,b);
            canvas->SetPixel(15-i,12,r,g,b);
            usleep(20*1000);
        }
    }
    if (!opening)
    {
        r=backgroundColor.r;
        g=backgroundColor.g;
        b=backgroundColor.b;
        for (int i=0;i<14;i++){
            canvas->SetPixel(16+i,3,r,g,b);
            canvas->SetPixel(15-i,3,r,g,b);
            canvas->SetPixel(16+i,12,r,g,b);
            canvas->SetPixel(15-i,12,r,g,b);
            usleep(20*1000);
        }
    }
}

void LedClock::coolBorderAnimate(bool opening)
{
    int ycpt = 0, xcpt=0;
    int r = color.r;
    int g = color.g;
    int b = color.b;

    if (opening)
    {
        while (ycpt!=16)
        {
            canvas->SetPixel(15+ycpt,0,r,g,b);
            canvas->SetPixel(15-ycpt,0,r,g,b);
            canvas->SetPixel(15+ycpt,15,r,g,b);
            canvas->SetPixel(15-ycpt,15,r,g,b);
            ycpt++;
            usleep(20*1000);
        }
        while (xcpt!=8)
        {
            canvas->SetPixel(0,xcpt,r,g,b);
            canvas->SetPixel(0,15-xcpt,r,g,b);
            canvas->SetPixel(31,xcpt,r,g,b);
            canvas->SetPixel(31,15-xcpt,r,g,b);
            xcpt++;
            usleep(20*1000);
        }
    }
    if (!opening)
    {
        ycpt=16;
        xcpt=8;
        r=backgroundColor.r;
        g=backgroundColor.g;
        b=backgroundColor.b;
        while (xcpt!=0)
        {
            canvas->SetPixel(0,xcpt,r,g,b);
            canvas->SetPixel(0,15-xcpt,r,g,b);
            canvas->SetPixel(31,xcpt,r,g,b);
            canvas->SetPixel(31,15-xcpt,r,g,b);
            xcpt--;
            usleep(20*1000);
        }
        while (ycpt!=-1)
        {
            canvas->SetPixel(15+ycpt,0,r,g,b);
            canvas->SetPixel(15-ycpt,0,r,g,b);
            canvas->SetPixel(15+ycpt,15,r,g,b);
            canvas->SetPixel(15-ycpt,15,r,g,b);
            ycpt--;
            usleep(20*1000);
        }
    }
}

void LedClock::scrollText(const char* text, const char* title)
{
    FrameCanvas* frame = canvas->SwapOnVSync(NULL);
    int len = strlen(text);
    int pixelLength = len * font.CharacterWidth('X')+32;
    for (int i=0;i<pixelLength;i++) {
        clear();
        if (title != NULL)
        {
            rgb_matrix::DrawText(frame, clock_font, 0, 1+clock_font.baseline(), color2, title);
            rgb_matrix::DrawText(frame, font, 32-i, 7+font.baseline(), color, text);
        }
        else
        {
            rgb_matrix::DrawText(frame, font, 32-i, 4+font.baseline(), color, text);
        }
        usleep(30*1000);
    }
}

std::string LedClock::pop_front(std::vector<std::string>& vec)
{
    std::string text = vec.front();
    vec.erase(vec.begin());
    return text;
}


void LedClock::handleCommand()
{
    std::string text = pop_front(myVector);
    clear();
    if (text.find(":")!=std::string::npos)
    {
        std::vector<std::string> sp = split(text, ':');
        if (sp.front()=="bright")
        {
            int brightness = atoi(sp.at(1).c_str());
            canvas->SetBrightness(brightness);
        }
        else if (sp.front()=="color")
        {
            int r = atoi(sp.at(1).c_str());
            int g = atoi(sp.at(2).c_str());
            int b = atoi(sp.at(3).c_str());
            color.r = r;
            color.g = g;
            color.b = b;
        }
        else if (sp.front()=="color2")
        {
            int r = atoi(sp.at(1).c_str());
            int g = atoi(sp.at(2).c_str());
            int b = atoi(sp.at(3).c_str());
            color2.r = r;
            color2.g = g;
            color2.b = b;
        }
        else if (sp.front()=="backgroundColor")
        {
            int r = atoi(sp.at(1).c_str());
            int g = atoi(sp.at(2).c_str());
            int b = atoi(sp.at(3).c_str());
            backgroundColor.r = r;
            backgroundColor.g = g;
            backgroundColor.b = b;
        }
        else if (sp.front()=="text")
        {
            const char* content = sp.at(1).c_str();
            int repetition = atoi(sp.at(2).c_str());
            const char* title = NULL;
            if (sp.size() > 3)
            {
            title = sp.at(3).c_str();
            }
            for (int i=0;i<repetition;i++)
            {
                scrollText(content, title);
            }
        }
        else if (sp.front()=="animate")
        {
            const char* content = sp.at(1).c_str();
            if (strcmp(content,"randomize")==0)
            {
                randomizeAnimate();
                clear();
            }
            else if (strcmp(content,"spirale")==0)
            {
                spiraleAnimate();
                clear();
            }
        }
        else if (sp.front()=="mode")
        {
            mode = atoi(sp.at(1).c_str());
        }
    }
    else
    {
        if (text.length()<=5)
        {
            rgb_matrix::DrawText(canvas, font, x, y + font.baseline(), color, text.c_str());
            sleep(2);
        }
        else
        {
            scrollText(text.c_str(), NULL);
        }
        clear();
    }
}

void LedClock::drawBinaryClockDigit(int x, char elem)
{
    Color c1(200,80,80);
    Color c2(80,80,200);
    int r, g, b;
    if (elem == '1' || elem == '3' || elem == '5' || elem == '7' || elem == '9')
    {
        r=c1.r; g=c1.g; b=c1.b;
    }
    else
    {
        r=c2.r; g=c2.g; b=c2.b;
    }
    canvas->SetPixel(x, 16-4, r,g,b);

    if (elem == '2' || elem == '3' || elem == '6' || elem == '7')
    {
        r=c1.r; g=c1.g; b=c1.b;
    }
    else
    {
        r=c2.r; g=c2.g; b=c2.b;
    }
    canvas->SetPixel(x, 16-7, r,g,b);

    if (elem == '4' || elem == '5' || elem == '6' || elem == '7')
    {
        r=c1.r; g=c1.g; b=c1.b;
    }
    else
    {
        r=c2.r; g=c2.g; b=c2.b;
    }
    canvas->SetPixel(x, 16-10, r,g,b);

    if (elem == '8' || elem == '9')
    {
        r=c1.r; g=c1.g; b=c1.b;
    }
    else
    {
        r=c2.r; g=c2.g; b=c2.b;
    }
    canvas->SetPixel(x, 16-13, r,g,b);
}

void LedClock::drawBinaryClock(tm* lt)
{
    char hour[2];
    char minute[2];
    char second[2];
    sprintf(hour, "%02d", lt->tm_hour);
    sprintf(minute, "%02d", lt->tm_min);
    sprintf(second, "%02d", lt->tm_sec);

    drawBinaryClockDigit(6, hour[0]);
    drawBinaryClockDigit(9, hour[1]);
    drawBinaryClockDigit(14, minute[0]);
    drawBinaryClockDigit(17, minute[1]);
    drawBinaryClockDigit(22, second[0]);
    drawBinaryClockDigit(25, second[1]);
}

void LedClock::drawClockWithDate(tm* lt)
{
    clear();

    char hour[2];
    char minute[2];
    sprintf(hour, "%02d", lt->tm_hour);
    sprintf(minute, "%02d", lt->tm_min);
    rgb_matrix::DrawText(canvas, font, 3, 0 + font.baseline(), color, hour);
    rgb_matrix::DrawText(canvas, font, 3, 8 + font.baseline(), color, minute);

    char day[2];
    char mon[2];
    sprintf(day, "%02d", lt->tm_mday);
    sprintf(mon, "%02d", lt->tm_mon+1);
    rgb_matrix::DrawText(canvas, font, 18, 0 + font.baseline(), color2, day);
    rgb_matrix::DrawText(canvas, font, 18, 8 + font.baseline(), color2, mon);
}

void LedClock::drawClock(tm* lt)
{
    clear();
    if (mode == 0)
    {
    coolBorderAnimate(true);
    }

    char date[5];
    sprintf(date, "%02d:%02d", lt->tm_hour, lt->tm_min);

    if (mode == 3)
    {
        rgb_matrix::DrawText(canvas, clock_font, 3, 2+font.baseline(), color, date);
    }
    else
    {
        rgb_matrix::DrawText(canvas, font, x, y+font.baseline(), color, date);
    }

    if (mode == 3)
    {
        int x;
        int y;
        int colorDelta = 60;
        for(x=1;x<31;x++)
        {
                canvas->SetPixel(x,2,color.r-colorDelta,color.g-colorDelta,color.b-colorDelta);
        }
        for(y=3;y<12;y++)
        {
                canvas->SetPixel(1,y,color.r-colorDelta,color.g-colorDelta,color.b-colorDelta);
        }
        for(x=1;x<31;x++)
        {
                canvas->SetPixel(x,12,color.r-colorDelta,color.g-colorDelta,color.b-colorDelta);
        }
        for(y=3;y<12;y++)
        {
                canvas->SetPixel(30,y,color.r-colorDelta,color.g-colorDelta,color.b-colorDelta);
        }
    }

    if (mode == 0)
    {
        coolBorderAnimate(false);
        if (lt->tm_min == 0 || lt->tm_min == 15 || lt->tm_min == 30 || lt->tm_min == 45)
        {
            hourAnimate(true);
            hourAnimate(false);
        }
    }
}

void LedClock::display()
{
    spiraleAnimate();
    canvas->SetBrightness(70);
    clear();

    scrollText("GCR CUSTOM CLOCK", NULL);

    while (1)
    {
        bool redraw = false;
        while (!myVector.empty())
        {
            try
            {
                handleCommand();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what(); 
            }
            redraw = true;
        }
        time_t now = time(0);
        tm *lt = localtime(&now);

        if (mode == 0 || mode == 3)
        {
            if (lt->tm_min != previousMinute || redraw)
            {
                drawClock(lt);
            }
        }
        else if (mode == 1)
        {
            drawBinaryClock(lt);
        }
        else if (mode == 2)
        {
            if (lt->tm_min != previousMinute || redraw)
            {
                drawClockWithDate(lt);
            }
        }

        if (lt->tm_min != previousMinute)
        {
            previousMinute = lt->tm_min;
        }

        sleep(0.2);
    }

    // Finished. Shut down the RGB matrix.
    delete canvas;
}

LedClock::LedClock():color(80,80,200), color2(200,80,80),
    backgroundColor(0,0,0),
    bdf_font_file("fonts/6x9.bdf"),
    clock_bdf_font_file("fonts/6x7GCR.bdf"),
    x(1),y(4),brightness(30),
    rows(16),chain(1),parallel(1),
    mode(3), previousMinute(0)
{
    if (!io.Init())
        fprintf(stderr, "IO error\n");

    canvas = new RGBMatrix(&io, rows, chain, parallel);

    bool all_extreme_colors = true;
    all_extreme_colors &= color.r == 0 || color.r == 255;
    all_extreme_colors &= color.g == 0 || color.g == 255;
    all_extreme_colors &= color.b == 0 || color.b == 255;
    if (all_extreme_colors)
        canvas->SetPWMBits(1);

    if (!font.LoadFont(bdf_font_file)) {
        fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
    }
    if (!clock_font.LoadFont(clock_bdf_font_file)) {
        fprintf(stderr, "Couldn't load clock font '%s'\n", clock_bdf_font_file);
    }
}

void LedClock::clear()
{
    canvas->Clear();
    if (backgroundColor.r != 0 
            || backgroundColor.g != 0
            || backgroundColor.b != 0)
    {
        canvas->Fill(backgroundColor.r, backgroundColor.g, backgroundColor.b);
    }
}


int main(int argc, char *argv[]) {
    LedClock clock;

    pthread_t thread;

    if(pthread_create(&thread, NULL, commandListenerThread, &(clock.myVector)) == -1) {
        perror("pthread_create");
        std::cout<<"busy socket"<<std::endl;
    }

    clock.display();
    return 0;
}

