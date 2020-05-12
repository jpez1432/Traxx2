
#include "Camera.hpp"

using namespace Engine;

CCamera::CCamera(void) : EditBlock(0.0f)
{

}

glm::mat4x4 CCamera::Update(CConfig *Config, double Delta)
{
    this->Config = Config;

    CConfig::Camera *Cam = &Config->Cameras[Config->Editor.Camera];

    if(Config->General.Paused) {
        return glm::lookAt(Cam->Position, Cam->Position + Cam->Center, Cam->Up);
    }

    glfwGetFramebufferSize(GLWindow, &HalfWidth, &HalfHeight);
    HalfWidth /= 2;
    HalfHeight /= 2;

    Cam->Angle = glm::degrees(atan2f(Cam->Position.z - Cam->Center.z, Cam->Position.x - Cam->Center.x));
    Cam->Angle -= 360.0f * std::floor(Cam->Angle * (1.0f / 360.0f));

    Speed = Config->Input.MovementSpeed * Delta;

    if(glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(GLWindow, GLFW_KEY_RIGHT_SHIFT)) {
        Speed *= 10.0f;
    }

    if(Config->Input.MouseLook) {

        glfwGetCursorPos(GLWindow, &XPos, &YPos);
        glfwSetCursorPos(GLWindow, HalfWidth, HalfHeight);

        XOffset = (XPos - HalfWidth) * Config->Input.MouseSensitivity;
        YOffset = (YPos - HalfHeight) * Config->Input.MouseSensitivity;

        if(Cam->Invert) {
            YOffset *= -1;
        }

        Cam->Yaw += XOffset;
        Cam->Pitch += YOffset;

        Cam->Yaw -= 360.0f * std::floor(Cam->Yaw * (1.0f / 360.0f));
        Cam->Pitch -= 360.0f * std::floor(Cam->Pitch * (1.0f / 360.0f));

        Cam->Center.x = glm::cos(glm::radians(Cam->Yaw)) * cos(glm::radians(Cam->Pitch));
        Cam->Center.y = glm::sin(glm::radians(Cam->Pitch));
        Cam->Center.z = glm::sin(glm::radians(Cam->Yaw)) * cos(glm::radians(Cam->Pitch));
    }

    if(glfwGetKey(GLWindow, Config->Input.Forwards)) {
        Cam->Position += Speed * Cam->Center;
    }


    if(glfwGetKey(GLWindow, Config->Input.Backwards)) {
        Cam->Position -= Speed * Cam->Center;
    }


    if(glfwGetKey(GLWindow, Config->Input.StrafeLeft)) {
        Cam->Position -= Speed * glm::normalize(glm::cross(Cam->Center, Cam->Up));
    }


    if(glfwGetKey(GLWindow, Config->Input.StrafeRight)) {
        Cam->Position += Speed * glm::normalize(glm::cross(Cam->Center, Cam->Up));
    }

    if(glfwGetKey(GLWindow, Config->Input.Up)) {
        Cam->Position.y += Speed;
    }

    if(glfwGetKey(GLWindow, Config->Input.Down)) {
        Cam->Position.y -= Speed;
    }

    return glm::lookAt(Cam->Position, Cam->Position + Cam->Center, Cam->Up);
}

glm::vec3 &CCamera::GetEditBlock(glm::mat4x4 ProjectionMatrix)
{
    float Pixel = 0.0f;
    CConfig::Camera *Cam = &Config->Cameras[Config->Editor.Camera];

    glfwGetCursorPos(GLWindow, &XPos, &YPos);
    YPos = Config->Graphics.Height - YPos;

    glReadPixels(XPos, YPos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Pixel);
    EditBlock = glm::unProject(glm::vec3(XPos, YPos, Pixel), glm::lookAt(Cam->Position, Cam->Center + Cam->Position, Cam->Up), ProjectionMatrix, glm::uvec4(0, 0, Config->Graphics.Width, Config->Graphics.Height));

    return EditBlock;
}

