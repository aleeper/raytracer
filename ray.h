#ifndef RAY_H_
#define RAY_H_
#include <Eigen/Core>

class Ray {
  using Vec3f = Eigen::Vector3f;
 public:
  Ray () {}
  Ray (const Vec3f& point, const Vec3f& direction) :
    point_(point), direction_(direction) {}
  Vec3f point() const { return point_; }

  // Not guaranteed to be unit length.
  Vec3f direction() const { return direction_; }
  Vec3f GetPointAtParameter(float t) const { return point_ + t * direction_; }

 private:
  Vec3f point_;
  Vec3f direction_;
};
#endif // RAY_H_