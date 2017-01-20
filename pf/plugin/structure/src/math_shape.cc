#include <cstdlib>
#include <iostream>
#include "math_shape.h"

/**
 *    Y
 *    |                 *           
 *    |                *          CX
 *    |  CY   (point)C*  a       *
 *    |   *          *   *    *
 *    |     *     b *      *B(direction) 
 *    |       *    *     *   
 *    |         * *   *  c     
 *    |  (origin)A *
 *    |           
 *    |
 *    |
 *    O------------------------------------------------- X
 */
point_t absolute_to_relative(
    const point_t &origin, const point_t &direction, const point_t &point) {
  point_t result;
  if (origin.x == direction.x && origin.y == direction.y) {
    result.x = point.x - origin.x;
    result.y = point.y - origin.y;
  } else {
    auto a = distance(direction, point);
    auto b = distance(point, origin);
    auto c = distance(direction, origin);
    auto cosA = (b * b + c * c - a * a) / (2 * b * c); //余弦定理
    result.x = b * cosA;
    result.y = sqrt(b * b - result.x * result.x); //勾股定理
  }
  return result;
}


/**
 *    Y
 *    |
 *    |      CY
 *    |      |
 *    |      |    .point
 *    |      |
 *    |      |
 *    |  origin----------------- CX
 *    |
 *    |
 *    O--------------------------------------------------- X
 *
 */
point_t absolute_to_relative(const point_t &origin, const point_t &point) {
  point_t result;
  result.x = point.x - origin.x;
  result.y = point.y - origin.y;
  return result;
}

/**
 *    Y
 *    |
 *    |      
 *    |      
 *    |           .point
 *    |         * *
 *    |  (x)  *   *
 *    |     *     *
 *    |   *       *
 *    | *  (y)    *
 *    O--------------------------------------------------- X
 *
 */
/*
point_t to_polar_coordinate(const point_t &point) {
  point_t result;
  if (.0 == point.x == point.y) return result;
  if (.0 == point.x) {
    result.x = abs(point.y);
    if (point.y < .0)  {
      result.y = 270.0;
    } else if(point.y > .0) {
      result.y = 90.0;
    } else {
      result.y = .0;
    }
  } else if (.0 == point.y) {
    result.y = point.x > .0 ? .0 : 180.0;
  } else {
    result.x = sqrt(point.x * point.x + point.y * point.y);
    result.y = (180.0 / PI) * atan2(point.y , point.x); //弧度转角度
  }
  return result;
}
**/

/**
 *    Y
 *    |     CY
 *    |     | 
 *    |     | 
 *    |     |______________L_____________     
 *    |     |                           |
 *    |     CO--------------------------W------------------------ CX
 *    |     |_____________L_____________|
 *    |
 *    | 
 *    O--------------------------------------------------- X
 *
 */
bool in_rect(double L, double W, const point_t &point) {
  auto xmin = .0, xmax = L, ymin = -W/2, ymax = W/2;
  auto x = point.x, y = point.y;
  return x >= xmin && x <= xmax && y >= ymin && y <= ymax;
}

point_t to_spolar_coordinate(const point_t &point) {
  point_t result;
  result.x = 0;
  result.y = -1;
  if (0 == point.x == point.y) {
    result.y = 0;
    return result;
  }
  if (0 == point.y) {
    result.x = abs(point.x);
    result.y = point.x > 0 ? 0 : 180;
    return result; 
  }
  if (0 == point.x) {
    result.x = abs(point.y);
    result.y = point.y > 0 ? 90 : 270;
    return result;
  }
  if (abs(point.x) == abs(point.y)) {
    result.x = 1.41421 * abs(point.x);
    if (point.x > 0 && point.y > 0) {
      result.y = 45;
    } else if (point.x < 0 && point.y > 0) {
      result.y = 135;
    } else if (point.x < 0 && point.y < 0) {
      result.y = 225;
    } else if (point.x > 0 && point.y < 0) {
      result.y = 315;
    }
  }
  return result;
}

bool in_circular_sector(const point_t &center, 
                        const point_t &direction, 
                        double r, 
                        int angle,
                        const point_t &point) {
  //实际使用中，我们会把方向点的极坐标放到外部进行计算
  point_t d_rpoint = absolute_to_relative(center, direction);
  point_t d_pc_point = to_spolar_coordinate(d_rpoint);
  if (-1 == d_pc_point.y) { //简单的如果转换不出，则需要调用角度函数计算
    d_pc_point = to_polar_coordinate(d_rpoint);
  }
  point_t rpoint = absolute_to_relative(center, point); //相对坐标
  point_t pc_point = to_polar_coordinate(rpoint); //极坐标
  if (pc_point.x > r) return false;
  bool result = false;
  auto half_angle = angle / 2;
  auto angle_counter = d_pc_point.y - half_angle; //中心线顺时针方向的范围
  auto angle_clockwise = d_pc_point.y + half_angle; //中心线逆时针方向的范围
  /*
  std::cout << "angle_counter: " << angle_counter << " angle_clockwise: " 
            << angle_clockwise << " d_pc_point.y" << d_pc_point.y << std::endl;
  */
  if (0 == d_pc_point.y || angle_counter < 0 || angle_clockwise > 360) {
    angle_counter = angle_counter < 0 ? angle_counter + 360 : angle_counter;
    angle_clockwise = angle_clockwise > 360 ? angle_counter - 360 : angle_counter;
    if (pc_point.y >= 0 && pc_point.y <= angle_counter) {
      result = true;
    } else if (pc_point.y >= angle_clockwise && pc_point.y <= 360) {
      result = true;
    }
  } else {
    result = angle_counter <= pc_point.y && angle_clockwise >= pc_point.y;
  }
  return result;
}
