/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id config.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm@126.com>/viticm.ti@gmail.com
 * @date 2015/03/16 14:37
 * @uses scene of server config file
 *       cn: 服务器场景配置文件
*/
#ifndef SCENE_CONFIG_H_
#define SCENE_CONFIG_H_

#include "structure/define.h"
#include "pf/base/config.h"

#define SCENE_REGION_INZONE_MAX (10)
#define SCENE_VARIABLE_MAX (64)
#define SCENE_FUBEN_DATA_PARAM_MAX (512)
#define SCENE_FUBEN_DATA_COPYMEMBER_MAX (144)
#define SCENE_TIMER_DOING_NUMBER (512)
#define SCENE_MAX (4096)
#define SCENE_MODULENAME "scene"

namespace scene {

class Scene;
class ChatPipeline;
class Grid;
class Timer;
class Map;
class PathFinder;
class Scaner;
class Thread;
class ThreadPool;

namespace manager {
class Net;
class RecycleConnection;
class DropPosition;
class Fuben;
class PKRegion;
class Region;
class Scene;
class TimerDoing;
class Variable;
class Thread;
}; //namespace manager

}; //scene

#endif //SCENE_CONFIG_H_
