#ifndef UTIL_H_
#define UTIL_H_
#include <Eigen/Core>

using Vec3f = Eigen::Vector3f;

Vec3f GetRandomVecInUnitSphere() {
  Vec3f p;
  do {
    p = 2.0 * Vec3f(drand48(), drand48(), drand48()) - Vec3f(1, 1, 1);
  } while (p.squaredNorm() >= 1.0);
  return p;
}

Vec3f Reflect(const Vec3f& direction, const Vec3f& normal) {
  return direction - 2.0 * direction.dot(normal) * normal;
}

bool Refract(const Vec3f& direction, const Vec3f& normal, float ni_over_nt,
              Vec3f* refracted) {
  Vec3f unit_direction = direction.normalized();
  float dot = unit_direction.dot(normal);
  float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dot * dot);
  if (discriminant > 0) {
    *refracted = ni_over_nt * (unit_direction - normal * dot)
        - normal * std::sqrt(discriminant);
    return true;
  }
  return false;
}

#endif // UTIL_H_