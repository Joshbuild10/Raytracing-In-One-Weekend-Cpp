#ifndef COLOUR_H
#define COLOUR_H

#include "vec3.h"

#include <iostream>

void write_colour(std::ostream& out, colour pixel_colour) {
    // Write the translated [0,255] value of each colour component.
    out << static_cast<int>(255.999 * pixel_colour.r) << ' '
        << static_cast<int>(255.999 * pixel_colour.g) << ' '
        << static_cast<int>(255.999 * pixel_colour.b) << '\n';
}

#endif