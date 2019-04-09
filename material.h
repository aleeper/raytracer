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
    float cosine;
    Vec3f outward_normal;
    if (ray_in.direction().dot(hit.normal) > 0) {
      outward_normal = -hit.normal;
      ni_over_nt = refractive_index_;
      cosine = refractive_index_ * ray_in.direction().dot(hit.normal) / ray_in.direction().norm();
    } else {
      outward_normal = hit.normal;
      ni_over_nt = 1.0 / refractive_index_;
      cosine = -ray_in.direction().dot(hit.normal) / ray_in.direction().norm();
    }

    Vec3f refracted_direction;
    bool did_refract = Refract(ray_in.direction(), outward_normal, ni_over_nt,
                               &refracted_direction);    
    float reflect_probability = did_refract ?
        ComputeSchlick(cosine, refractive_index_) : 1.0f;
      
    *attenuation = Vec3f(1.0, 1.0, 1.0);
    
    if (drand48() < reflect_probability) {
      Vec3f reflected_direction = Reflect(ray_in.direction(), hit.normal);
      *scattered_ray = Ray(hit.point, reflected_direction);
    } else {
      *scattered_ray = Ray(hit.point, refracted_direction);
    }
    return true;
  }

 float refractive_index_;
 float fuzz_;
};

#endif // MATERIAL_H_