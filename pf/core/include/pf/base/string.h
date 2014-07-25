/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id string.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/18 13:45
 * @uses base string module
 */
#ifndef PF_BASE_STRING_H_
#define PF_BASE_STRING_H_

#include <map> 
#include "pf/base/config.h"

namespace pf_base {

namespace string {

/*typedef {*/
typedef std::map<std::string, std::string> map; //my map
/*typedef }*/

char get_base64char(int index);

void replace_all(std::string& str, 
                 const std::string source, 
                 const std::string destination);

bool toint16(const char* source, 
             int16_t& result, 
             uint8_t converted_length = 0, 
             bool ignored_zero = false);

bool toint32(const char* source, 
             int32_t& result, 
             uint8_t converted_length = 0, 
             bool ignored_zero = false);
int64_t toint64(const char *str);
uint64_t touint64(const char *str);

void encrypt(const char* in, char* out, int32_t out_length);

void decrypt(const char* in, char* out, int32_t out_length);

char* safecopy(char* dest, const char* src, size_t size);

int32_t charset_convert(const char* from, 
                        const char* to, 
                        char* save, 
                        int32_t save_length, 
                        const char* src, 
                        int32_t src_length) ;

bool get_escapechar(char in, char& out);
bool getescape(const char *in, size_t insize, char *out, size_t outsize);

}; //namespace string

}; //namespace pf_base

#endif //PF_BASE_STRING_H_
