#pragma once
#include <glm/glm.hpp>

struct Light {
  glm::vec3 position; // the light source's position
  glm::vec3 ambient; // the ambient vec3
  glm::vec3 diffuse; // the diffuse vec3
  glm::vec3 specular; // and the specular vec3
};
