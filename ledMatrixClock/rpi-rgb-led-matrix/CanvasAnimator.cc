#include "CanvasAnimator.h"

CanvasAnimator::CanvasAnimator(RGBMatrix* c, CanvasConfig* cc):canvas(c),canvasConfig(cc)
{
    frame = canvas->CreateFrameCanvas();
}

void CanvasAnimator::spiraleAnimate()
{
    rgb_matrix::Color color = canvasConfig->color;

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

void CanvasAnimator::randomizeAnimate()
{
    const int nbPoints = 20;
    const int duration = 200;
    const int delay = 20*1000;

    clear();
    for (int i=0;i<duration;i++)
    {
        canvas->transformer()->Transform(frame)->Clear();
        frame->SetBrightness(canvas->brightness());
        for (int x=0;x<nbPoints;x++)
        {
            for (int y=0;y<16;y++)
            {
                frame->SetPixel(rand()%32,rand()%16,
                        (canvasConfig->color).r,
                        (canvasConfig->color).g,
                        (canvasConfig->color).b);
            }
        }
        frame = canvas->SwapOnVSync(frame);
        usleep(delay);
    }
    clear();
}

void CanvasAnimator::hourAnimateOpening()
{
    int r = (canvasConfig->color3).r;
    int g = (canvasConfig->color3).g;
    int b = (canvasConfig->color3).b;

    for (int i=0;i<14;i++){
        canvas->SetPixel(16+i,3,r,g,b);
        canvas->SetPixel(15-i,3,r,g,b);
        canvas->SetPixel(16+i,12,r,g,b);
        canvas->SetPixel(15-i,12,r,g,b);
        usleep(20*1000);
    }
}

void CanvasAnimator::hourAnimateClosing()
{
    int r=(canvasConfig->backgroundColor).r;
    int g=(canvasConfig->backgroundColor).g;
    int b=(canvasConfig->backgroundColor).b;

    for (int i=0;i<14;i++){
        canvas->SetPixel(16+i,3,r,g,b);
        canvas->SetPixel(15-i,3,r,g,b);
        canvas->SetPixel(16+i,12,r,g,b);
        canvas->SetPixel(15-i,12,r,g,b);
        usleep(20*1000);
    }
}

void CanvasAnimator::coolBorderAnimate()
{
    coolBorderAnimateOpening();
    coolBorderAnimateClosing();
}

void CanvasAnimator::coolBorderAnimateOpening()
{
    int ycpt = 0, xcpt=0;
    int r = (canvasConfig->color).r;
    int g = (canvasConfig->color).g;
    int b = (canvasConfig->color).b;

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

void CanvasAnimator::coolBorderAnimateClosing()
{
    Color backgroundColor = canvasConfig->backgroundColor;

    int ycpt=16, xcpt=8;
    int r=backgroundColor.r;
    int g=backgroundColor.g;
    int b=backgroundColor.b;
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

void CanvasAnimator::drawLineAnimated(bool orientation, bool direction, int x1, int y1, int length, int r, int g, int b)
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

void CanvasAnimator::clear()
{
    canvas->Clear();
    if ((canvasConfig->backgroundColor).r != 0 
            || (canvasConfig->backgroundColor).g != 0
            || (canvasConfig->backgroundColor).b != 0)
    {
        canvas->Fill((canvasConfig->backgroundColor).r, (canvasConfig->backgroundColor).g, (canvasConfig->backgroundColor).b);
    }
}

void CanvasAnimator::animateColors()
{
    for (int t=0;t<50;t++)
    {
        int r = rand()%255;
        int g = rand()%255;
        int b = rand()%255;
        canvas->transformer()->Transform(frame)->Clear();
        frame->SetBrightness(canvas->brightness());
        frame->Fill(r, g, b);
        frame = canvas->SwapOnVSync(frame);
        usleep(200*1000);
    }
}

void CanvasAnimator::animateOscillo()
{
    canvas->Clear();

    for (int t=0;t<50;t++)
    {
        canvas->transformer()->Transform(frame)->Clear();
        frame->SetBrightness(canvas->brightness());
        for (int x=0;x<32;x+=2)
        {
            drawOscillo(frame,x,rand()%16+2);
        }
        frame = canvas->SwapOnVSync(frame);
        usleep(80*1000);
    }
}

void CanvasAnimator::drawOscillo(FrameCanvas* frame, int x1, int y1)
{
    for (int y=0;y<y1;y++)
    {
        Color* color = NULL;
        if (y<=4)
        {
            color = new Color(40,255,40);
        }
        else if(y<=8)
        {
            color = new Color(255,255,0);
        }
        else if(y<=12)
        {
            color = new Color(255,80,0);
        }
        else if(y<=16)
        {
            color = new Color(255,0,0);
        }

        int r = color->r;
        int g = color->g;
        int b = color->b;
        frame->SetPixel(x1,16-y,r,g,b);
        frame->SetPixel(x1+1,16-y,r,g,b);
    }
}

void CanvasAnimator::animateRain()
{
    std::vector<std::vector<int>> waterDrops; 

    for (int i=0;i<200;i++)
    {
        std::vector<int> waterDropCoord(2);
        waterDropCoord[0]=rand()%32;
        waterDropCoord[1]=-rand()%300;
        waterDrops.push_back(waterDropCoord);
    }

    while(!isRainOver(waterDrops))
    {
        canvas->transformer()->Transform(frame)->Clear();
        frame->SetBrightness(canvas->brightness());
        for (auto & element : waterDrops) {
            frame->SetPixel(element[0],element[1]-11,0,0,80);
            frame->SetPixel(element[0],element[1]-10,0,0,80);
            frame->SetPixel(element[0],element[1]-9,0,0,80);
            frame->SetPixel(element[0],element[1]-8,0,0,80);
            frame->SetPixel(element[0],element[1]-7,0,0,80);
            frame->SetPixel(element[0],element[1]-6,0,0,80);
            frame->SetPixel(element[0],element[1]-5,0,0,80);
            frame->SetPixel(element[0],element[1]-4,0,0,80);
            frame->SetPixel(element[0],element[1]-3,0,0,80);
            frame->SetPixel(element[0],element[1]-2,0,0,120);
            frame->SetPixel(element[0],element[1]-1,0,0,120);
            frame->SetPixel(element[0],element[1],0,0,255);
            element[1]++;
        }
        frame = canvas->SwapOnVSync(frame);
        usleep(30*1000);
    }

}

bool CanvasAnimator::isRainOver(std::vector<std::vector<int>>& waterDrops)
{
    for (auto & element : waterDrops) {
        if (element[1]<16+12)
            return false;
    }
    return true; 
}

void CanvasAnimator::animateCountdown(int min){
    tm *lt;
    char date[5];
    time_t now = time(0);
    lt = localtime(&now);
    lt->tm_min = min;
    lt->tm_sec = 0;
    while (lt->tm_min > 0 || lt->tm_sec > 0)
    {
        canvas->transformer()->Transform(frame)->Clear();
        frame->SetBrightness(canvas->brightness());
        sprintf(date, "%02d:%02d", lt->tm_min, lt->tm_sec);
        rgb_matrix::DrawText(frame, canvasConfig->font, 1, 4+canvasConfig->font.baseline(), canvasConfig->color, date);
        frame = canvas->SwapOnVSync(frame);
        usleep(1000*1000);
        if (lt->tm_sec>0)
        {
            lt->tm_sec--;
        }        
        else 
        {
            lt->tm_min--;
            lt->tm_sec = 59;
        }
    }
    for (int i=0;i<5;i++)
    {
        canvas->transformer()->Transform(frame)->Clear();
        frame->SetBrightness(canvas->brightness());
        rgb_matrix::DrawText(frame, canvasConfig->font, 1, 4+canvasConfig->font.baseline(), Color(255,0,0), "00:00");
        frame = canvas->SwapOnVSync(frame);
        usleep(500*1000);
        canvas->Clear();
        usleep(500*1000);
    }
}
