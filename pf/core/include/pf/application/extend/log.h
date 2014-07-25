/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id log.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/22 21:10
 * @uses the application extend for base module log form framework, important!
 *       cn:
 *       框架基础日志扩展
 *       重要的 -- 编译时必须包含
 */
#ifndef PF_APPLICATION_EXTEND_LOG_H_
#define PF_APPLICATION_EXTEND_LOG_H_

#include "pf/application/extend/config.h"

enum {
  kLoginLogFile = kLogFileCount,
  kShareMemoryLogFile,
  kGatewayLogFile,
  kCenterLogFile,
  kServerLogFile,
  kFinalLogFileCount, //如果没有任何扩展，此值为kLogFileCount
}; //扩展的日志记录ID，从基础的kLogFileCount开始扩展

namespace pf_base {
extern const char *g_extend_log_filename[];
}; //扩展命名空间

#endif //PF_APPLICATION_EXTEND_LOG_H_
