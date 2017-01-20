/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.ti@gmail.com>
 * @date 2016/05/05 20:28
 * @uses Base module type config.
*/
#ifndef PF_BASIC_TYPE_CONFIG_H_
#define PF_BASIC_TYPE_CONFIG_H_

#include "pf/basic/config.h"

namespace pf_basic {

namespace type {

struct variable_struct;

//Commonly used definitions.
typedef PF_API variable_struct variable_t;
typedef PF_API std::vector< variable_t > variable_array_t;
typedef PF_API std::map< std::string, variable_t > variable_set_t;

}; //namespace type

}; //namespace pf_basic

#endif //PF_BASIC_TYPE_CONFIG_H_
