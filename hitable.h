#ifndef HITABLE_H_
#define HITABLE_H_
#include <Eigen/Core>

class Material;

struct HitResult {
  using Vec3f = Eigen::Vector3f;
  float t;
  Vec3f point;
  // By convention, always unit length.
  Vec3f normal;
  Material* material;
};

class Hitable {
 public:
  virtual bool ComputeHit(
      const Ray& ray, float t_min, float t_max, HitResult* result) const = 0;
};

class HitableList : public Hitable {
 public:
  HitableList() {}
  HitableList(Hitable** list, int n) : list_(list), list_size_(n) {}
  bool ComputeHit(const Ray& ray, float t_min, float t_max, HitResult* result)
      const override;
  Hitable **list_;
  int list_size_;
};

bool HitableList::ComputeHit(const Ray& ray, float t_min, float t_max,
                             HitResult* result) const {
  HitResult temp_result;
  bool hit_anything = false;
  float closest_so_far = t_max;
  for (int i = 0; i < list_size_; ++i) {
    if (list_[i]->ComputeHit(ray, t_min, closest_so_far, &temp_result)) {
      hit_anything = true;
      closest_so_far = temp_result.t;
      *result = temp_result;
    }
  }
  return hit_anything;
}
#endif // HITABLE_H_