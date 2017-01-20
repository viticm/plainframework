/**
 * PAP Engine ( https://github.com/viticm/pap )
 * $Id math.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2015/03/26 21:26
 * @uses structure math functions
 *       cn: 基础数据结构数据库，如果数量多可以单独做成插件
*/
#ifndef STRUCTURE_MATH_H_
#define STRUCTURE_MATH_H_

#include "structure/config.h"

namespace structure {

namespace math {

PF_PLUGIN_API float sqrtex(const position_t *source, const position_t *target);

}; //namespace math

}; //namespace structure

#endif //STRUCTURE_MATH_H_
