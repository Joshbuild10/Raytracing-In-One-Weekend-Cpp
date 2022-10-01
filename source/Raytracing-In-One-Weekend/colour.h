#ifndef COLOUR_H
#define COLOUR_H

#include "utility.h"

#include <iostream>

void write_colour(std::ostream& out, colour pixel_colour, int samples_per_pixel)
{
    // Divide the colour by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    pixel_colour *= scale;

    // Write the translated [0,255] value of each colour component.
    out << static_cast<int>(256 * clamp(pixel_colour.r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(pixel_colour.g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(pixel_colour.b, 0.0, 0.999)) << '\n';
}

// Overloaded function for back compatibility with tests
void write_colour(std::ostream& out, colour pixel_colour)
{
    // Write the translated [0,255] value of each colour component.
    out << static_cast<int>(255.999 * pixel_colour.r) << ' '
        << static_cast<int>(255.999 * pixel_colour.g) << ' '
        << static_cast<int>(255.999 * pixel_colour.b) << '\n';
}

#endif