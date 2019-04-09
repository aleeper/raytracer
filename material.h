#ifndef MATERIAL_H_
#define MATERIAL_H_
#include <Eigen/Core>
#include "util.h"


class Material {
 protected:
  using Vec3f = Eigen::Vector3f;
 public:
  virtual bool Scatter(const Ray& ray_in, const HitResult& hit,
                       Vec3f* attenuation, Ray* scattered_ray) const = 0;
};

class Lambertian : public Material {
 public:
  Lambertian(const Vec3f& albedo) : albedo_(albedo) {}
  
  bool Scatter(const Ray& ray_in, const HitResult& hit,
                       Vec3f* attenuation, Ray* scattered_ray) const override {
    Vec3f direction = hit.normal + GetRandomVecInUnitSphere();
    *scattered_ray = Ray(hit.point, direction);
    *attenuation = albedo_;
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
                       Vec3f* attenuation, Ray* scattered_ray) const override {
    Vec3f reflected = Reflect(ray_in.direction().normalized(), hit.normal);
    if (fuzz_ > 0) {
      reflected += fuzz_ * GetRandomVecInUnitSphere();
    }
    *scattered_ray = Ray(hit.point, reflected);
    *attenuation = albedo_;
    return (scattered_ray->direction().dot(hit.normal) > 0);
  }

 Vec3f albedo_;
 float fuzz_;
};

class Dielectric : public Material {
 public:
  Dielectric(float refractive_index, float fuzz)
      : refractive_index_(refractive_index), fuzz_(fuzz) {
    fuzz_ = std::max(std::min(fuzz, 1.f), 0.f);
  }
  
  bool Scatter(const Ray& ray_in, const HitResult& hit,
                       Vec3f* attenuation, Ray* scattered_ray) const override {
    float ni_over_nt;
    Vec3f outward_normal;
    if (ray_in.direction().dot(hit.normal) > 0) {
      outward_normal = -hit.normal;
      ni_over_nt = refractive_index_;
    } else {
      outward_normal = hit.normal;
      ni_over_nt = 1.0 / refractive_index_;
    }

    *attenuation = Vec3f(1.0, 1.0, 1.0);
    // TODO is normalization the bug?
    Vec3f reflected = Reflect(ray_in.direction(), hit.normal);
    Vec3f refracted;
    if (Refract(ray_in.direction(), outward_normal, ni_over_nt, &refracted)) {
      *scattered_ray = Ray(hit.point, refracted);
      return true;
    } else {
      *scattered_ray = Ray(hit.point, reflected);
      return false;
    }
  }

 float refractive_index_;
 float fuzz_;
};

#endif // MATERIAL_H_