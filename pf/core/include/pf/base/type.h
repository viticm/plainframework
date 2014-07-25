/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id type.h
 * @link https://github.com/viticm/pap for the canonical source repository
 * @copyright Copyright (c) 2013-2013 viticm( viticm@126.com )
 * @license
 * @user viticm<viticm@126.com>
 * @date 2014/06/18 12:07
 * @uses 基本数据定义,该文件定义不存在命名空间
 *       不能直接包含此头文件，要使用此头文件包含pf/base/config.h即可
 */
#ifndef PF_BASE_TYPE_H_
#define PF_BASE_TYPE_H_

//platform define
#ifndef __WINDOWS__
#define __WINDOWS__ (defined(_MSC_VER) || defined(__ICL))
#endif
#ifndef __LINUX__
#define __LINUX__ !(__WINDOWS__)
#endif

#if defined(GAME_CLIENT)
#define __ENTER_FUNCTION 
#define __LEAVE_FUNCTION
#endif
//network size define
#ifndef FD_SETSIZE 
#define FD_SETSIZE 4906
#endif

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif //use c99

//system include
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#if __WINDOWS__ //diffrent system include
#pragma warning (disable: 4786)
#include <windows.h>
#include <crtdbg.h>
#include <tchar.h>
#include <direct.h>
#include <io.h>
#elif __LINUX__
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <execinfo.h>
#include <signal.h>
#include <exception>
#include <setjmp.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/resource.h>
#endif
#include "pf/sys/assert.h"
#include "pf/base/io.h"
//warning the namespace can't use like this, remember it
//using namespace std;

//基本数据类型定义
//typedef unsigned char ubyte; //0~255 --use uint8_t
//typedef char byte; //-128~127 --use int8_t
typedef int16_t connectionid_t;

#define IP_SIZE 24 //max ip size
#if __LINUX__
#define HANDLE_INVALID (-1)
#elif __WINDOWS__
#define HANDLE_INVALID ((VOID*)0)
#endif
#define ID_INVALID (-1)
#define INDEX_INVALID (-1)
#define TAB_PARAM_ID_INVALID (-9999) //invalid id in excel param

#ifndef UCHAR_MAX
#define UCHAR_MIN (0)
#define UCHAR_MAX (0xFF)
#endif

#ifndef BYTE_MAX
#define BYTE_MIN UCHAR_MIN
#define BYTE_MAX UCHAR_MAX
#endif

// common define
#if __LINUX__
#ifndef LF
#define LF "\n"
#endif
#elif __WINDOWS__
#ifndef LF
#define LF "\r\n"
#endif
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

//c output/string functions
#if __WINDOWS__
#ifndef snprintf
#define snprintf _snprintf
#endif
#ifndef stricmp
#define stricmp _stricmp
#endif
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#define strtoint64(pointer,endpointer,base) \
  _strtoi64(pointer,endpointer,base)
#define strtouint64(pointer,endpointer,base) \
  _strtoui64(pointer,endpointer,base)
#elif __LINUX__
#ifndef stricmp
#define stricmp strcasecmp
#define strtoint64(pointer,endpointer,base) strtoll(pointer,endpointer,base)
#define strtouint64(pointer,endpointer,base) strtoull(pointer,endpointer,base) 
#endif
#endif

#if __WINDOWS__
#define access _access
#define mkdir(dir,mode) _mkdir(dir)
#endif

//根据指针值删除内存
#ifndef SAFE_DELETE
#if __WINDOWS__
#define SAFE_DELETE(x)	if ((x) != NULL) { \
                          Assert(_CrtIsValidHeapPointer(x)); \
                          delete (x); (x) = NULL; \
                        }

#elif __LINUX__
#define SAFE_DELETE(x)	if ((x) != NULL) { delete (x); (x) = NULL; }
#endif
#endif
//根据指针值删除数组类型内存 
//其中注意_CrtIsValidHeapPointer前的定义，在release需要定义NDEBUG
#ifndef SAFE_DELETE_ARRAY
#if __WINDOWS__
#define SAFE_DELETE_ARRAY(x) if ((x) != NULL) { \
                               Assert(_CrtIsValidHeapPointer(x)); \
                               delete[] (x); (x) = NULL; \
                             }
#elif __LINUX__
#define SAFE_DELETE_ARRAY(x) if ((x) != NULL) { delete[] (x); (x) = NULL; }
#endif
#endif

//删除指针型数据(应尽量使用SAFE_DELETE_ARRAY)
#ifndef DELETE_ARRAY
#if __WINDOWS__
#define DELETE_ARRAY(x)	if ((x) != NULL) { delete[] (x); (x) = NULL; }
#elif __LINUX__
#define DELETE_ARRAY(x)	if ((x) != NULL) { delete[] (x); (x) = NULL; }
#endif
#endif

#ifndef USE_PARAM
#define USE_PARAM(x) if (!x) {}
#endif
#define USE_PARAMEX(x) if (!(&x)) {}
//根据指针调用free接口
#ifndef SAFE_FREE
#define SAFE_FREE(x) if ((x) != NULL) { free(x); (x) = NULL; }
#endif
//根据指针调用Release接口
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	if ((x) != NULL) { (x)->release(); (x) = NULL; }
#endif

/* io { */
#ifndef ERRORPRINTF
#define ERRORPRINTF baseio_perror
#endif

#ifndef WARNINGPRINTF
#define WARNINGPRINTF baseio_pwarn
#endif

#ifndef DEBUGPRINTF
#define DEBUGPRINTF baseio_pdebug
#endif
/* } io */

#if __WINDOWS__ //normal functions
#if defined(NDEBUG)
  #define __ENTER_FUNCTION { try {
  #define __LEAVE_FUNCTION } catch(...){ AssertSpecial(false,__FUNCTION__); } }
#else
  #define __ENTER_FUNCTION { try {
  #define __LEAVE_FUNCTION } catch(...){ AssertSpecial(false,__FUNCTION__); } }
#endif

#elif __LINUX__    //linux
#define __ENTER_FUNCTION { try {
#define __LEAVE_FUNCTION } catch(...) \
                         { AssertSpecial(false,__PRETTY_FUNCTION__); } }
#endif

#endif //PF_BASE_TYPE_H_
