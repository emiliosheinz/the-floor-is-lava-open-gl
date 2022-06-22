#ifndef SlideView_h
#define SlideView_h

#include "TilemapView.h"
#include <iostream>
using namespace std;

class DiamondView : public TilemapView
{
public:
    void computeDrawPosition(const int col, const int row, const float tw, const float th, float &targetx, float &targety) const
    {
        targetx = col * tw / 2 + row * tw / 2;
        targety = col * th / 2 - row * th / 2;
    }

    void computeTileWalking(int &col, int &row, const int direction) const
    {
        switch (direction)
        {
        case DIRECTION_NORTH:
            col++;
            break;
        case DIRECTION_EAST:
            row++;
            break;
        case DIRECTION_SOUTH:
            col--;
            break;
        case DIRECTION_WEST:
            row--;
            break;
        }
    }
};

#endif
