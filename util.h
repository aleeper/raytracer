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


#endif // UTIL_H_