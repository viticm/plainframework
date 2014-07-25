/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm@126.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/06/18 12:05
 * @uses 游戏公用基础模块的基本定义文件
 */
#ifndef PF_BASE_CONFIG_H_
#define PF_BASE_CONFIG_H_

/* base type { */
#include "pf/base/type.h"
/* } base type */

#define DATE_LENGTH_MAX 20

/* base log { */
enum {
  kDebugLogFile = 0,
  kErrorLogFile = 1,
  kNetLogFile = 2,
  kFunctionLogFile = 3,
  kLogFileCount,
};
/* } base log */

#include "pf/application/extend/define.h"

//headers include order: pf/base/config.h -> sys include -> module include

#endif //PF_BASE_CONFIG_H_
