#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Core>
#include "ray.h"

class Camera {
  using Vec3f = Eigen::Vector3f;
 public:
  Camera (float vfov_degrees, float aspect)
      : vfov_degrees_(vfov_degrees), aspect_(aspect){
    RecomputeAllTheThings();
  }

  Ray GetRay(float u, float v) const {
    return Ray(origin_,
               lower_left_corner_ + u * horizontal_ + v * vertical_ - origin_);
  }

 private:
  void RecomputeAllTheThings() {
    float theta = vfov_degrees_ * M_PI / 180.0;
    float half_height = std::tan(theta / 2);
    float half_width = aspect_ * half_height;
    origin_ = Vec3f(0.f, 0.f, 0.f);
    lower_left_corner_ = Vec3f(-half_width, -half_height, -1.f);
    horizontal_ = Vec3f(2 * half_width, 0.f, 0.f);
    vertical_ = Vec3f(0.f, 2 * half_height, 0.f);
  }

  float vfov_degrees_;
  float aspect_;
  Vec3f origin_;
  Vec3f lower_left_corner_;
  Vec3f horizontal_;
  Vec3f vertical_;
};
#endif // CAMERA_H_