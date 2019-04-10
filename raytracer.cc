#include <iostream>
#include <random>
#include <memory>

#include <Eigen/Core>
#include "camera.h"
#include "ray.h"
#include "sphere.h"
#include "hitable.h"
#include "util.h"
#include "material.h"

using Vec3f = Eigen::Vector3f;

constexpr float kMinHitDistance = 0.001f;

std::unique_ptr<Hitable> GetBasicScene() {
  std::vector<Hitable*> list(5);
  list[0] = new Sphere(Vec3f(0.f, 0.f, -1.0f),    0.5, new Lambertian(Vec3f(0.1f, 0.2f, 0.5f)));
  list[1] = new Sphere(Vec3f(0.f, -100.5f, -1.f), 100, new Lambertian(Vec3f(0.8f, 0.8f, 0.f)));
  list[2] = new Sphere(Vec3f(1.f, 0.f, -1.0f),    0.5, new Metal(Vec3f(0.8f, 0.6f, 0.2f), 0.0f));
  list[3] = new Sphere(Vec3f(-1.f, 0.f, -1.0f),   0.5, new Dielectric(1.5, 0.0f));
  list[4] = new Sphere(Vec3f(-1.f, 0.f, -1.0f),  -0.45, new Dielectric(1.5, 0.0f));
  return std::unique_ptr<Hitable>(new HitableList(list));
}

Vec3f GetColor(const Ray& ray, Hitable* world, int depth) {
  HitResult hit_result;
  if (world->ComputeHit(ray, kMinHitDistance, MAXFLOAT, &hit_result)) {
    Ray scattered;
    Vec3f attentuation;
    if (depth < 50 &&
        hit_result.material->Scatter(
            ray, hit_result, &attentuation, &scattered)) {
      return attentuation.cwiseProduct(GetColor(scattered, world, depth + 1));
    } else {
      return Vec3f(0, 0, 0);
    }
  } else {
    Vec3f unit_direction = ray.direction().normalized();
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0f - t) * Vec3f(1.0f, 1.0f, 1.0f) + t * Vec3f(0.5f, 0.7f, 1.0f);  
  }
}

int main() {
  int nx = 200;
  int ny = 100;
  int ns = 100;
  Vec3f look_from(-1, 1, 0), look_to(0, 0, -1), look_up(0, 1, 0);
  float focus_distance = (look_from - look_to).norm();
  Camera camera(90, float(nx) / float(ny), 0.2, focus_distance);
  camera.SetLook(look_from, look_to, look_up);
  // camera.SetLook(Vec3f(0, 0, 0), Vec3f(0, 0, -1), Vec3f(0, 1, 0));
  
  std::unique_ptr<Hitable> world = std::move(GetBasicScene());

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  for (int y = ny - 1; y >= 0; y--) {
    for (int x = 0; x < nx; x++) {
      Vec3f color(0, 0, 0);
      if (ns == 1) {
        float u = float(x) / float(nx);
        float v = float(y) / float(ny);  
        Ray ray = camera.GetRay(u, v);
        color = GetColor(ray, world.get(), 0);
      } else {
        for (int s = 0; s < ns; ++s) {
          float u = float(x + drand48() - 0.5f) / float(nx);
          float v = float(y + drand48() - 0.5f) / float(ny);  
          Ray ray = camera.GetRay(u, v);
          color += GetColor(ray, world.get(), 0);
        }
        color /= float(ns);  
      }
      color = color.cwiseSqrt();
      
      // Eigen::Vector3f color(float(x) / float(nx), float(y) / float(ny), 0.2);
      // Ray ray(origin, lower_left_corner + u * horizontal + v * vertical);
      
      int ir = int(255.99 * color[0]);
      int ig = int(255.99 * color[1]);
      int ib = int(255.99 * color[2]);
      std::cout << ir << " " << ig << " " << ib << "\n";  
    }
  }

}