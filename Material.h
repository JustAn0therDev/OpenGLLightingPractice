#pragma once
#include <glm/glm.hpp>

struct Material {
  glm::vec3 diffuse; // the diffuse vec3
  glm::vec3 specular; // the specular vec3
  float shininess; // the shininess of the material (how much of the light source will be reflected on the material)
};
