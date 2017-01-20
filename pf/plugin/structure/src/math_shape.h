#ifndef _MATH_SHAPE_H_
#define _MATH_SHAPE_H_

#include <cmath>
#define PI (3.1415926)

struct point_struct {
  double x;
  double y;
  point_struct() : x{.0}, y{.0} {}
};
using point_t = point_struct;

//获得两个点的距离
inline double distance(const point_t &a, const point_t &b) {
  double x = b.x - a.x;
  double y = b.y - a.y;
  return sqrt(x * x + y * y);
}

/**
 * 有方向的相对坐标转换，x轴的方向为origin->direction
 * @param origin 坐标系的原点
 * @param direction 方向点
 * @param point 需要转换的点
 */
point_t absolute_to_relative(
    const point_t &origin, const point_t &direction, const point_t &point);

/**
 * 没有方向的相对坐标转换，x、y轴的方向与原点相同
 * @param origin 坐标系原点
 * @param point 需要转换的点
 */
point_t absolute_to_relative(const point_t &origin, const point_t &point);

/**
 * 简单极坐标计算
 * @param point 需要计算的坐标
 */
point_t to_spolar_coordinate(const point_t &point);

/**
 * 转换为极坐标（转换后的x为斜边，y为角度）
 * @param point 需要转换的点
 */
inline point_t to_polar_coordinate(const point_t &point) {
  point_t result;
  result.x = sqrt(point.x * point.x + point.y * point.y);
  result.y = (180.0 / PI) * atan2(point.y , point.x); //弧度转角度
  result.y = result.y < .0 ? result.y + 360.0 : result.y;
  return result;
}

/**
 * 判断一个点是否在矩形内（相对中心点）
 * @param L 长度
 * @param W 宽度
 * @param point 需要检查的点
 */ 
bool in_rect(double L, double W, const point_t &point);

/**
 * 判断一个点是否在扇形内（相对中心点）
 * @param center 扇形的中心点
 * @param direction 中心线的方向坐标
 * @param r 半径
 * @param angle 角度（0 < angle < 360）
 * @param point 需要检查的点
 */
bool in_circular_sector(const point_t &center, 
                        const point_t &direction, 
                        double r, 
                        int angle, 
                        const point_t &point);

#endif //_MATH_SHAPE_H_
