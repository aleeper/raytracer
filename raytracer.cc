#include <iostream>
#include <fstream>
#include <random>
#include <memory>
#include <omp.h>

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

std::unique_ptr<Hitable> GetDemoScene(int range) {
  constexpr int n = 500;
  std::vector<Hitable*> list(n + 1);
  list[0] = new Sphere(Vec3f(0.f, -1000.f, 0.f), 1000, new Lambertian(Vec3f(0.5f, 0.5f, 0.5f)));
  int i = 1;
  constexpr float random_offset = 0.9f;
  constexpr float small_radius = 0.2f;
  constexpr float large_radius = 1.0f;
  auto rand_diffuse = []() { return drand48() * drand48(); };
  auto rand_metal = []() { return 0.5f * (1 + drand48()); };

  for (int a = -range; a < range; ++a) {
    for (int b = -range; b < range; ++b) {
      Vec3f center(a + random_offset * drand48(), small_radius, b + random_offset * drand48());
      if ((center - Vec3f(4, small_radius, 0)).norm() < 0.9) {
        continue;
      }
      float choose_material = drand48();
      if (choose_material < 0.8f) {  // Diffuse.
        auto fn = []() { return drand48() * drand48(); };
        Vec3f random_color(rand_diffuse(), rand_diffuse(), rand_diffuse());
        list[i++] = new Sphere(center, small_radius, new Lambertian(random_color));
      } else if (choose_material > 0.95f) {  // Metal.
        Vec3f random_color(rand_metal(), rand_metal(), rand_metal());
        list[i++] = new Sphere(center, small_radius, new Metal(random_color, 0.5f * drand48()));
      } else {  // Glass.
        list[i++] = new Sphere(center, small_radius, new Dielectric(1.5, 0));
      }
    }
  }
  list[i++] = new Sphere(Vec3f(0, large_radius, 0), large_radius, new Dielectric(1.5, 0));
  list[i++] = new Sphere(Vec3f(-4, large_radius, 0), large_radius, new Lambertian(Vec3f(0.4, 0.2, 0.1)));
  list[i++] = new Sphere(Vec3f(4, large_radius, 0), large_radius, new Metal(Vec3f(0.7, 0.6, 0.5), 0.0));
  list.resize(i);
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

int main(int argc, char** argv) {
  int nx = atoi(argv[1]);
  int ny = atoi(argv[2]);
  int ns = atoi(argv[3]);
  int range = atoi(argv[4]);
  float aperture = atof(argv[5]);

  // Vec3f look_from(-1, 1, 0), look_to(0, 0, -1), look_up(0, 1, 0);
  Vec3f look_from(8, 2, 2), look_to(0, -0.5, 0), look_up(0, 1, 0);
  float focus_distance = (look_from - Vec3f(4, 1, 0)).norm();
  Camera camera(45, float(nx) / float(ny), aperture, focus_distance);
  camera.SetLook(look_from, look_to, look_up);
  // camera.SetLook(Vec3f(0, 0, 0), Vec3f(0, 0, -1), Vec3f(0, 1, 0));
  
  std::unique_ptr<Hitable> world = std::move(GetDemoScene(range));

  int image[nx][ny][3];
  // #pragma omp parallel for num_threads(4)
  for (int y = ny - 1; y >= 0; y--) {
    if (y % 50 == 0) {
      std::cout << "Current row: " << y << std::endl;
    }
    #pragma omp parallel for num_threads(4)
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
      // Gamma correction.
      color = color.cwiseSqrt();
      
      int ir = int(255.99 * color[0]);
      int ig = int(255.99 * color[1]);
      int ib = int(255.99 * color[2]);
      image[x][y][0] = ir;
      image[x][y][1] = ig;
      image[x][y][2] = ib;
    }
  }
  std::cout << "Writing file..." << std::endl;
  std::ofstream myfile;
  myfile.open ("image.ppm");
  myfile << "P3\n" << nx << " " << ny << "\n255\n";
  for (int y = ny - 1; y >= 0; y--) {
    for (int x = 0; x < nx; x++) {
      myfile << image[x][y][0] << " " << image[x][y][1] << " " << image[x][y][2] << "\n";
    }
  }
  myfile.close();
}