
#ifndef CAMERA_HPP
#define CAMERA_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <time.h>
#include <string>

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>

#include "../Config.hpp"

extern GLFWwindow *GLWindow;

namespace Engine
{

class CCamera
{
private:

    glm::vec3 EditBlock;

    float Speed;
    double XPos, YPos;
    double XOffset, YOffset;
    int HalfWidth, HalfHeight;

    int Boundaries;

    CConfig *Config;

public:

    CCamera(void);

    glm::mat4x4 Update(CConfig *Config, const double Delta);
    glm::vec3 &GetEditBlock(glm::mat4x4 ProjectionMatrix);

};
}

#endif
