// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how write text.
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "graphics.h"
#include "socket.h"
#include "CanvasAnimator.h"
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

using namespace rgb_matrix;
/*
class Point {
    public :
        Point(int x,int y)_x(x),_y(y){}

        int _x;
        int _y;
};*/

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
    std::vector<std::string> myVector;
    void display();

    private:
    void handleCommands();
    void handleCommand();
    void handleCommandAnimate();
    void handleCommandBrightness();
    void handleCommandColor1();
    void handleCommandColor2();
    void handleCommandBackgroundColor();
    void handleCommandText();
    void handleCommandCountdown();
    void displayClock();
    void loadFonts();
    void clear();
    
     void checkAndTriggerHourAnimate();
   
    void scrollText(const char* text, const char* title);
    std::string pop_front(std::vector<std::string>& vec);
    void initClock();
    void drawClockWithDate();
    void drawBinaryClock();
    void drawBinaryClockDigit(int x, char elem);
    void drawClock();
    
    RGBMatrix* canvas;
    const char *bdfFontFile;
    const char *bdfFontFile2;
    int x;
    int y;
    int previousMinute;
    GPIO io;
    int brightness;
    int rows;
    int chain;
    int parallel;
    // mode 1 : standard (HH : MM)
    // mode 2 : binary clock
    // mode 3 : clock with day and month (dd/mm)
    int mode;
    tm *lt;
    bool redraw;
    std::vector<std::string> currentCommand;
    CanvasAnimator* canvasAnimator;
    CanvasConfig* canvasConfig;
};

LedClock::LedClock(): 
bdfFontFile("fonts/6x9.bdf"),
bdfFontFile2("fonts/6x7GCR.bdf"),
x(1),
y(4),
previousMinute(0),
brightness(30),
rows(16),chain(1),parallel(1),mode(0)
{
    if (!io.Init())
        fprintf(stderr, "IO error\n");

    canvas = new RGBMatrix(&io, rows, chain, parallel);
    canvasConfig = new CanvasConfig();
    canvasConfig->color = Color(80,80,200);
    canvasConfig->color2 = Color(200,80,80);
    canvasConfig->color3 = Color(80,0,0);
    canvasConfig->backgroundColor = Color(0,0,0);
    canvasAnimator = new CanvasAnimator(canvas, canvasConfig);

    bool all_extreme_colors = true;
    Color color = canvasConfig->color;
    all_extreme_colors &= color.r == 0 || color.r == 255;
    all_extreme_colors &= color.g == 0 || color.g == 255;
    all_extreme_colors &= color.b == 0 || color.b == 255;
    if (all_extreme_colors)
        canvas->SetPWMBits(1);

    loadFonts();
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

void LedClock::loadFonts()
{
    canvasConfig->font = Font();
    canvasConfig->clock_font = Font();
    if (!(canvasConfig->font).LoadFont(bdfFontFile)) {
        fprintf(stderr, "Couldn't load font '%s'\n", bdfFontFile);
    }
    if (!(canvasConfig->clock_font).LoadFont(bdfFontFile2)) {
        fprintf(stderr, "Couldn't load clock font '%s'\n", bdfFontFile2);
    }
}

void LedClock::display()
{
    try {
    initClock();
    while (1)
    {
        handleCommands();
        displayClock();
        sleep(0.2);
    }

    // Finished. Shut down the RGB matrix.
    delete canvas;
    
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what(); 
        }
}


void LedClock::handleCommand()
{
    std::string text = pop_front(myVector);
    clear();
    std::cout<<"handleCommand : "<<text<<std::endl;
    if (text.find(":")!=std::string::npos)
    {
        std::vector<std::string> sp = split(text, ':');
        currentCommand =sp;
        if (sp.front()=="bright")
        {
            int brightness = atoi(sp.at(1).c_str());
            canvas->SetBrightness(brightness);
        }
        else if (sp.front()=="color")
        {
            handleCommandColor1();
        }
        else if (sp.front()=="color2")
        {
            handleCommandColor2();
        }
        else if (sp.front()=="backgroundColor")
        {
            handleCommandBackgroundColor();
        }
        else if (sp.front()=="text")
        {
            handleCommandText();
        }
        else if (sp.front()=="animate")
        {
            handleCommandAnimate();
        }
        else if(sp.front()=="countdown")
        {
            handleCommandCountdown();
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
            rgb_matrix::DrawText(canvas, canvasConfig->font, x, y + canvasConfig->font.baseline(), canvasConfig->color, text.c_str());
            sleep(2);
        }
        else
        {
            scrollText(text.c_str(), NULL);
        }
        clear();
    }
}

void LedClock::handleCommandCountdown(){
    int min = atoi(currentCommand.at(1).c_str());
        canvasAnimator->animateCountdown(min);
        clear();
}

void LedClock::handleCommandAnimate()
{
    const char* content = currentCommand.at(1).c_str();
    if (strcmp(content,"randomize")==0)
    {
        canvasAnimator->randomizeAnimate();
        clear();
    }
    else if (strcmp(content,"spirale")==0)
    {
        canvasAnimator->spiraleAnimate();
        clear();
    }
    else if(strcmp(content,"oscillo")==0)
    {
        canvasAnimator->animateOscillo();
        clear();
    }
    else if(strcmp(content,"rain")==0)
    {
        canvasAnimator->animateRain();
        clear();
    }
    else if(strcmp(content,"colors")==0)
    {
        canvasAnimator->animateColors();
        clear();
    }
}

void LedClock::handleCommandColor1()
{
    int r = atoi(currentCommand.at(1).c_str());
    int g = atoi(currentCommand.at(2).c_str());
    int b = atoi(currentCommand.at(3).c_str());
    (canvasConfig->color).r = r;
    (canvasConfig->color).g = g;
    (canvasConfig->color).b = b;
}

void LedClock::handleCommandColor2()
{
    int r = atoi(currentCommand.at(1).c_str());
    int g = atoi(currentCommand.at(2).c_str());
    int b = atoi(currentCommand.at(3).c_str());
    (canvasConfig->color2).r = r;
    (canvasConfig->color2).g = g;
    (canvasConfig->color2).b = b;
}

void LedClock::handleCommandBackgroundColor()
{
    int r = atoi(currentCommand.at(1).c_str());
    int g = atoi(currentCommand.at(2).c_str());
    int b = atoi(currentCommand.at(3).c_str());
    (canvasConfig->backgroundColor).r = r;
    (canvasConfig->backgroundColor).g = g;
    (canvasConfig->backgroundColor).b = b;
}

void LedClock::handleCommandText()
{
    const char* content = currentCommand.at(1).c_str();
    int repetition = atoi(currentCommand.at(2).c_str());
    const char* title = NULL;
    if (currentCommand.size() > 3)
    {
        title = currentCommand.at(3).c_str();
    }
    for (int i=0;i<repetition;i++)
    {
        scrollText(content, title);
    }
}

void LedClock::displayClock()
{
    try {
    time_t now = time(0);
    lt = localtime(&now);

    if (mode == 0 || mode == 3)
    {
        if (lt->tm_min != previousMinute || redraw)
        {
            drawClock();
        }
    }
    else if (mode == 1)
    {
        drawBinaryClock();
    }
    else if (mode == 2)
    {
        if (lt->tm_min != previousMinute || redraw)
        {
            drawClockWithDate();
        }
    }

    if (lt->tm_min != previousMinute)
    {
        previousMinute = lt->tm_min;
    }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what(); 
        }
}

void LedClock::handleCommands()
{
    redraw = false;
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
}

void LedClock::initClock()
{
    canvas->SetBrightness(brightness);
    canvasAnimator->spiraleAnimate();
    clear();
    scrollText("GCR CUSTOM CLOCK", NULL);
}

void LedClock::clear()
{
    canvas->Clear();
    if ((canvasConfig->backgroundColor).r != 0 
        || (canvasConfig->backgroundColor).g != 0
        || (canvasConfig->backgroundColor).b != 0)
    {
        canvas->Fill((canvasConfig->backgroundColor).r, (canvasConfig->backgroundColor).g, (canvasConfig->backgroundColor).b);
    }
}

void LedClock::drawClock()
{
    Color color = canvasConfig->color;
    Color color2 = canvasConfig->color2;
    
    clear();
    if (mode == 0)
    {
        canvasAnimator->coolBorderAnimateOpening();
    }

    char date[5];
    sprintf(date, "%02d:%02d", lt->tm_hour, lt->tm_min);

    if (mode == 3)
    {
        rgb_matrix::DrawText(canvas, canvasConfig->clock_font, 3, 2+canvasConfig->font.baseline(), canvasConfig->color, date);
    }
    else
    {
        rgb_matrix::DrawText(canvas, canvasConfig->font, x, y+canvasConfig->font.baseline(), canvasConfig->color, date);
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
        canvasAnimator->coolBorderAnimateClosing();
        checkAndTriggerHourAnimate();
    }
}

void LedClock::checkAndTriggerHourAnimate()
{
    if (lt->tm_min == 0 || lt->tm_min == 15 || lt->tm_min == 30 || lt->tm_min == 45)
    {
        canvasAnimator->hourAnimateOpening();
        canvasAnimator->hourAnimateClosing();
    }
}

void LedClock::drawClockWithDate()
{
    Color color = canvasConfig->color;
    Color color2 = canvasConfig->color2;
    clear();

    char hour[2];
    char minute[2];
    sprintf(hour, "%02d", lt->tm_hour);
    sprintf(minute, "%02d", lt->tm_min);
    rgb_matrix::DrawText(canvas, canvasConfig->font, 3, 0 + canvasConfig->font.baseline(), color, hour);
    rgb_matrix::DrawText(canvas, canvasConfig->font, 3, 8 + canvasConfig->font.baseline(), color, minute);

    char day[2];
    char mon[2];
    sprintf(day, "%02d", lt->tm_mday);
    sprintf(mon, "%02d", lt->tm_mon+1);
    rgb_matrix::DrawText(canvas, canvasConfig->font, 18, 0 + canvasConfig->font.baseline(), color2, day);
    rgb_matrix::DrawText(canvas, canvasConfig->font, 18, 8 + canvasConfig->font.baseline(), color2, mon);
}

void LedClock::drawBinaryClock()
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

void LedClock::drawBinaryClockDigit(int x, char elem)
{
    Color color = canvasConfig->color;
    Color color2 = canvasConfig->color2;
    
    int r, g, b;
    if (elem == '1' || elem == '3' || elem == '5' || elem == '7' || elem == '9')
    {
        r=color.r; g=color.g; b=color.b;
    }
    else
    {
        r=color2.r; g=color2.g; b=color2.b;
    }
    canvas->SetPixel(x, 16-4, r,g,b);

    if (elem == '2' || elem == '3' || elem == '6' || elem == '7')
    {
        r=color.r; g=color.g; b=color.b;
    }
    else
    {
        r=color2.r; g=color2.g; b=color2.b;
    }
    canvas->SetPixel(x, 16-7, r,g,b);

    if (elem == '4' || elem == '5' || elem == '6' || elem == '7')
    {
        r=color.r; g=color.g; b=color.b;
    }
    else
    {
        r=color2.r; g=color2.g; b=color2.b;
    }
    canvas->SetPixel(x, 16-10, r,g,b);

    if (elem == '8' || elem == '9')
    {
        r=color.r; g=color.g; b=color.b;
    }
    else
    {
        r=color2.r; g=color2.g; b=color2.b;
    }
    canvas->SetPixel(x, 16-13, r,g,b);
}

std::string LedClock::pop_front(std::vector<std::string>& vec)
{
    std::string text = vec.front();
    vec.erase(vec.begin());
    return text;
}

void LedClock::scrollText(const char* text, const char* title)
{
    Color color = canvasConfig->color;
    Color color2 = canvasConfig->color2;
    int len = strlen(text);
    int pixelLength = len * canvasConfig->font.CharacterWidth('X')+32;
    clear();
    for (int i=0;i<pixelLength;i++) {
        FrameCanvas* frame = canvas->CreateFrameCanvas();
        if (title != NULL)
        {
            rgb_matrix::DrawText(frame, canvasConfig->clock_font, 0, 1+canvasConfig->clock_font.baseline(), color2, title);
            rgb_matrix::DrawText(frame, canvasConfig->font, 32-i, 7+canvasConfig->font.baseline(), color, text);
        }
        else
        {
            rgb_matrix::DrawText(frame, canvasConfig->font, 32-i, 4+canvasConfig->font.baseline(), color, text);
        }
        canvas->SwapOnVSync(frame);
        usleep(30*1000);
    }
    clear();
}
