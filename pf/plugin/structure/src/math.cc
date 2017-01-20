#include <math.h>
#include "structure/define.h"
#include "structure/math.h"

namespace structure {

namespace math {

float sqrtex(const position_t *source, const position_t *target) {
  __ENTER_FUNCTION
    float result = .0f;
    double x1 = static_cast<double>(source->x);
    double z1 = static_cast<double>(source->z);
    double x2 = static_cast<double>(target->x);
    double z2 = static_cast<double>(target->z);
    result = 
      static_cast<float>(sqrt((x1 - x2) * (x1 - x2) + (z1 - z2) * (z1 - z2)));
    return result;
  __LEAVE_FUNCTION
    return .0f;
}

} //namespace math

} //namespace structure
