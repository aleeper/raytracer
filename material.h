#ifndef MATERIAL_H_
#define MATERIAL_H_
#include <Eigen/Core>
#include "util.h"


class Material {
 protected:
  using Vec3f = Eigen::Vector3f;
 public:
  virtual bool Scatter(const Ray& ray_in, const HitResult& hit,
                       Vec3f* attenutation, Ray* scattered_ray) const = 0;
};

class Lambertian : public Material {
 public:
  Lambertian(const Vec3f& albedo) : albedo_(albedo) {}
  
  bool Scatter(const Ray& ray_in, const HitResult& hit,
                       Vec3f* attenutation, Ray* scattered_ray) const override {
    Vec3f direction = hit.normal + GetRandomVecInUnitSphere();
    *scattered_ray = Ray(hit.point, direction);
    *attenutation = albedo_;
    return true;
  }

 Vec3f albedo_;
};

class Metal : public Material {
 public:
  Metal(const Vec3f& albedo, float fuzz) : albedo_(albedo) {
    fuzz_ = std::max(std::min(fuzz, 1.f), 0.f);
  }
  
  bool Scatter(const Ray& ray_in, const HitResult& hit,
                       Vec3f* attenutation, Ray* scattered_ray) const override {
    Vec3f reflected = Reflect(ray_in.direction().normalized(), hit.normal);
    if (fuzz_ > 0) {
      reflected += fuzz_ * GetRandomVecInUnitSphere();
    }
    *scattered_ray = Ray(hit.point, reflected);
    *attenutation = albedo_;
    return (scattered_ray->direction().dot(hit.normal) > 0);
  }

 Vec3f albedo_;
 float fuzz_;
};


#endif // MATERIAL_H_