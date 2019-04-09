#include <iostream>
#include <random>
#include <Eigen/Core>
#include "camera.h"
#include "ray.h"
#include "sphere.h"
#include "hitable.h"
#include "util.h"

using Vec3f = Eigen::Vector3f;

constexpr float kMinHitDistance = 0.001f;

Vec3f GetColor(const Ray& ray, Hitable* world) {
  HitResult hit_result;
  if (world->ComputeHit(ray, kMinHitDistance, MAXFLOAT, &hit_result)) {
    // return 0.5 * Vec3f(hit_result.normal.x() + 1, hit_result.normal.y() + 1,
    //     hit_result.normal.z() + 1);
    Vec3f target =
        hit_result.point + hit_result.normal + GetRandomVecInUnitSphere();
    return 0.5 * GetColor(Ray(hit_result.point, target - hit_result.point),
                          world);
  } else {
    Vec3f unit_direction = ray.direction().normalized();
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0f - t) * Vec3f(1.0f, 1.0f, 1.0f) + t * Vec3f(0.5f, 0.7f, 1.0f);  
  }
}

int main() {
  int nx = 200;
  int ny = 100;
  int ns = 10;
  Camera camera;
  
  //TODO memory!
  Hitable* list[2];
  list[0] = new Sphere(Vec3f(0.f, 0.f, -1.f), 0.5);
  list[1] = new Sphere(Vec3f(0.f, -100.5f, -1.f), 100);
  Hitable* world = new HitableList(list, 2);

  std::cout << "P3\n" << nx << " " << ny << "\n255\n";
  for (int y = ny - 1; y >= 0; y--) {
    for (int x = 0; x < nx; x++) {
      Vec3f color(0, 0, 0);
      if (ns == 1) {
        float u = float(x) / float(nx);
        float v = float(y) / float(ny);  
        Ray ray = camera.GetRay(u, v);
        color = GetColor(ray, world);
      } else {
        for (int s = 0; s < ns; ++s) {
          float u = float(x + drand48() - 0.5f) / float(nx);
          float v = float(y + drand48() - 0.5f) / float(ny);  
          Ray ray = camera.GetRay(u, v);
          color += GetColor(ray, world);
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