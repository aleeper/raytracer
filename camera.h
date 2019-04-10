#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Core>
#include <Eigen/Geometry> 

#include "ray.h"

class Camera {
  using Vec3f = Eigen::Vector3f;
 public:
  Camera (float vfov_degrees, float aspect)
      : vfov_degrees_(vfov_degrees), aspect_(aspect),
      look_from_({0, 0, 0}), look_to_({0, 0, -1}), look_up_({0, -1, 0}) {
    RecomputeAllTheThings();
  }

  void SetLook(const Vec3f& from, const Vec3f& to, const Vec3f& up) {
    look_from_ = from;
    look_to_ = to;
    look_up_ = up;
    RecomputeAllTheThings();
  }

  Ray GetRay(float u, float v) const {
    return Ray(look_from_,
               lower_left_corner_ + u * horizontal_ + v * vertical_ - look_from_);
  }

 private:
  void RecomputeAllTheThings() {
    float theta = vfov_degrees_ * M_PI / 180.0;
    float half_height = std::tan(theta / 2);
    float half_width = aspect_ * half_height;
    Vec3f u, v, w;
    w = (look_from_ - look_to_).normalized();
    u = look_up_.cross(w).normalized();
    v = w.cross(u);
    look_from_ = look_from_;
    // lower_left_corner_ = Vec3f(-half_width, -half_height, -1.f);
    lower_left_corner_ = look_from_ - half_width * u - half_height * v - w;
    horizontal_ = 2 * half_width * u;
    vertical_ = 2 * half_height * v;
  }

  Vec3f look_from_;
  Vec3f look_to_;
  Vec3f look_up_;
  float vfov_degrees_;
  float aspect_;
  Vec3f lower_left_corner_;
  Vec3f horizontal_;
  Vec3f vertical_;
};
#endif // CAMERA_H_