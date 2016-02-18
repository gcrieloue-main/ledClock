// Expose key features of the lib for the project purpose
// Those can then be used by nodejs with node-ffi.

#include "led-matrix.h"
#include "graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace rgb_matrix;

extern "C" {
    bool parseColor(Color *c, const char *str) {
        return sscanf(str, "%hhu,%hhu,%hhu", &c->r, &c->g, &c->b) == 3;
    }

    int display(char *str, Color* color, int x, int y, int size) {
        // default font
        char *bdf_font_file;
        
        if (size == 6)
        {
            bdf_font_file= "/home/pi/LedClockServer/rpi-rgb-led-matrix/fonts/8x10.bdf";
        }
        else if (size == 8)
        {
            bdf_font_file= "/home/pi/LedClockServer/rpi-rgb-led-matrix/fonts/8x13B.bdf";
        }
        else if (size == 7)
        {
            bdf_font_file= "/home/pi/LedClockServer/rpi-rgb-led-matrix/fonts/7x14.bdf";
        }
        else if (size == 9)
        {
            bdf_font_file= "/home/pi/LedClockServer/rpi-rgb-led-matrix/fonts/9x15B.bdf";
        }
        else
        {
            fprintf(stderr, "size unavailable");
        }
        

        // rgb matrix config
        int rows = 16; // 16 lines
        int chain = 1; // only one matrix
        int parallel = 1; // only one matrix row

        /*
         * Load font. This needs to be a filename with a bdf bitmap font.
         */
        rgb_matrix::Font font;
        if (!font.LoadFont(bdf_font_file)) {
            fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
        }

        /*
         * Set up GPIO pins. This fails when not running as root.
         */
        GPIO io;
        if (!io.Init())
            return 1;

        /*
         * Set up the RGBMatrix. It implements a 'Canvas' interface.
         */
        RGBMatrix *canvas = new RGBMatrix(&io, rows, chain, parallel);

        bool all_extreme_colors = true;
        all_extreme_colors &= color->r == 0 || color->r == 255;
        all_extreme_colors &= color->g == 0 || color->g == 255;
        all_extreme_colors &= color->b == 0 || color->b == 255;
        if (all_extreme_colors)
            canvas->SetPWMBits(1);

        //canvas->Clear();
        rgb_matrix::DrawText(canvas, font, x, y + font.baseline(), *color, str);

        // Finished. Shut down the RGB matrix.
        //canvas->Clear();
        delete canvas;

        return 0;
    }

    int displayText(char *str, char *colorStr, int x, int y, int size) {
        Color color(255,255,255);
        parseColor(&color, colorStr);
        return display(str, &color, x, y, size);
    }
}
