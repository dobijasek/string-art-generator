#pragma once
#include "wire.h"
#include <vector>

struct nail_t
{
    int x;
    int y;
    int edge;
};

int getNailsIndex(int start, int end, int points_count);
std::vector<nail_t> createRectangle( int points, int w, int h );
std::vector<nail_t> createPolygon( int points, int sides, int w, int h );
std::vector<nail_t> generateNails(int points, int sides, int w, int h);