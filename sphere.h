#ifndef SPHERE_H_
#define SPHERE_H_
#include <Eigen/Core>
#include "hitable.h"

using Vec3f = Eigen::Vector3f;

float ComputeSphereHit(const Ray& ray, float t_min, float t_max,
                       const Vec3f& center, float radius, HitResult* result) {
  Vec3f p_o_c = ray.point() - center;
  float a = ray.direction().dot(ray.direction());
  float b = p_o_c.dot(ray.direction());
  float c = p_o_c.dot(p_o_c) - radius * radius;
  float discriminant = b * b - a * c;
  if (discriminant > 0) {
    float temp = (-b - std::sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      result->t = temp;
      result->point = ray.GetPointAtParameter(temp);
      result->normal = (result->point - center) / radius;
      return true;
    }
    temp = (-b + std::sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      result->t = temp;
      result->point = ray.GetPointAtParameter(temp);
      result->normal = (result->point - center) / radius;
      return true;
    }
  }
  return false;
}

class Sphere : public Hitable {
  using Vec3f = Eigen::Vector3f;
 public:
  Sphere() {}
  Sphere(Vec3f center, float radius, Material* material)
      : center_(center), radius_(radius), material_(material) {}
  bool ComputeHit(const Ray& ray, float t_min, float t_max, HitResult* result)
      const override;
 private:
  Vec3f center_;
  float radius_;
  Material* material_;
};

bool Sphere::ComputeHit(
    const Ray& ray, float t_min, float t_max, HitResult* result) const {
  if (ComputeSphereHit(ray, t_min, t_max, center_, radius_, result)) {
    result->material = material_;
    return true;
  } else {
    return false;
  }
}
#endif // SPHERE_H_