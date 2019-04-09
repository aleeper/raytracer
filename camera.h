#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Core>
#include "ray.h"

class Camera {
  using Vec3f = Eigen::Vector3f;
 public:
  Camera () {
    origin = Vec3f(0.f, 0.f, 0.f);
    lower_left_corner = Vec3f(-2.f, -1.f, -1.f);
    horizontal = Vec3f(4.f, 0.f, 0.f);
    vertical = Vec3f(0.f, 2.f, 0.f);
  }
  
  Ray GetRay(float u, float v) const {
    return Ray(origin,
               lower_left_corner + u * horizontal + v * vertical - origin);
  }

 private:
  Vec3f origin;
  Vec3f lower_left_corner;
  Vec3f horizontal;
  Vec3f vertical;
};
#endif // CAMERA_H_