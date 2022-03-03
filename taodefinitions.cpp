
#include "taodefinitions.h"

PointMatchDist::PointMatchDist()
{
}

PointMatchDist::PointMatchDist(Point inputPoint, Point foundPoint, float dist)
{
    curPoint = inputPoint;
    matchPoint = foundPoint;
    matchDist = dist;
}


float PointMatchDist::getMatchDist()
{
    return matchDist;
}
