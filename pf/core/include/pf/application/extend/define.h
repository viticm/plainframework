/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id define.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2014/07/24 16:21
 * @uses the application extend for base module define form framework, important!
 *       cn:
 *       框架基础定义扩展，宏和常量以及枚举等定义
 *       重要的 -- 编译时必须包含
 */
#ifndef PF_APPLICATION_EXTEND_DEFINE_H_
#define PF_APPLICATION_EXTEND_DEFINE_H_

//applications defined puts in here
#if defined(_PF_TEST) /* { */
#define APPLICATION_NAME "pf_simple"
#define APPLICATION_LOGFILE kDebugLogFile 
#elif defined(_VGATEWAY) /* }{ */
#define APPLICATION_NAME "gateway"
#define APPLICATION_LOGFILE kGatewayLogFile
#elif defined(_VLOGIN) /* }{ */
#define APPLICATION_NAME "login"
#define APPLICATION_LOGFILE kLoginLogFile
#elif defined(_VCENTER) /* }{ */
#define APPLICATION_NAME "center"
#define APPLICATION_LOGFILE kCenterLogFile
#elif defined(_VSERVER) /* }{ */
#define APPLICATION_NAME "server"
#define APPLICATION_LOGFILE kServerLogFile
#else /* }{ */
#define APPLICATION_NAME ""
#define APPLICATION_LOGFILE kDebugLogFile
#endif /* } */

#endif //PF_APPLICATION_EXTEND_DEFINE_H_
