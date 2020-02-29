#include "terraingraph/Utility.h"

#include <cmath>

namespace terraingraph
{

float Utility::RangedRandom(float f1, float f2)
{
    return (f1 + (f2 - f1)*((float)rand()) / ((float)RAND_MAX));
}

}