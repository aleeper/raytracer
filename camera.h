#ifndef CAMERA_H_
#define CAMERA_H_

#include <Eigen/Core>
#include <Eigen/Geometry> 

#include "ray.h"
#include "util.h"

class Camera {
  using Vec3f = Eigen::Vector3f;
 public:
  Camera (float vfov_degrees, float aspect, float aperture, float focus_dist)
      : vfov_degrees_(vfov_degrees), aspect_(aspect),
      lens_radius_(aperture/2.f), focus_distance_(focus_dist),
      look_from_({0, 0, 0}), look_to_({0, 0, -1}), look_up_({0, -1, 0}) {
    RecomputeAllTheThings();
  }

  void SetLook(const Vec3f& from, const Vec3f& to, const Vec3f& up) {
    look_from_ = from;
    look_to_ = to;
    look_up_ = up;
    RecomputeAllTheThings();
  }

  Ray GetRay(float x, float y) const {
    Vec3f effective_look_from_ = look_from_;
    if (lens_radius_ > 0) {
      Vec3f rd = lens_radius_ * GetRandomVecInUnitDisk();
      Vec3f offset = u_ * rd.x() + v_ * rd.y();
      effective_look_from_ = look_from_ + offset;
    }
    return Ray(effective_look_from_,
               lower_left_corner_ + x * horizontal_ + y * vertical_ - effective_look_from_);
  }

 private:
  void RecomputeAllTheThings() {
    float theta = vfov_degrees_ * M_PI / 180.0;
    float half_height = std::tan(theta / 2);
    float half_width = aspect_ * half_height;
    w_ = (look_from_ - look_to_).normalized();
    u_ = look_up_.cross(w_).normalized();
    v_ = w_.cross(u_);
    look_from_ = look_from_;
    lower_left_corner_ = look_from_ - half_width * focus_distance_ * u_
        - half_height * focus_distance_ * v_ - focus_distance_ * w_;
    horizontal_ = 2 * half_width * focus_distance_ * u_;
    vertical_ = 2 * half_height * focus_distance_ * v_;
  }

  Vec3f look_from_;
  Vec3f look_to_;
  Vec3f look_up_;
  Vec3f u_, v_, w_;
  float vfov_degrees_;
  float aspect_;
  float lens_radius_;
  float focus_distance_;
  Vec3f lower_left_corner_;
  Vec3f horizontal_;
  Vec3f vertical_;
};
#endif // CAMERA_H_