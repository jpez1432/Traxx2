

#ifndef RENDER_HPP
#define RENDER_HPP

#include <gtc/constants.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_access.hpp>

#include "../Interface/imgui.h"
#include "../Interface/imgui_impl_glfw.h"
#include "../Interface/imgui_impl_opengl3.h"

#include "QuadTree.hpp"
#include "Camera.hpp"
#include "Frustum.hpp"
#include "Timer.hpp"
#include "../Config.hpp"
#include "../Game/Track.hpp"

namespace Engine
{

class CRender
{

private:

    bool Update;
    glm::vec3 EditBlock;

    int Width, Height;

    GLUquadric *Object;

    glm::mat4x4 ProjectionMatrix;
    glm::mat4x4 OrthoMatrix;

    GLuint PerspectiveView;
    GLuint OrthographicalView;

    Game::CTrack *Track;
    Game::CPod *Pod;
    Game::CWeather Weather;
    Game::CTexture LogoTexture;
    Game::CTexture WaterTexture;

    Engine::CTimer Timer;
    Engine::CQuadTree QuadTree;
    Engine::CCamera Camera;
    Engine::CFrustum Frustum;

public:

    CRender(void);
    ~CRender(void);

    bool Create(Game::CPod &Pod, Game::CTrack &Track);

    void Scene(void);

    void Editor(bool Apply, bool Refresh);
    void Interface(void);
    void Logo(void);

    void Shadows(void);
    void Reflections(void);
    void Backdrop(void);
    void Models(bool Picking = false, bool Overlay = false);
    void Water(float Delta, float Height = 0.0f);
    void Terrain(CNode *Node, glm::vec3 Offset, bool Overlay);
    void Box(glm::vec3 Position, glm::vec3 Dimensions);
    void Course(int Index);

    void SetUpdate(void) { Update = true; }
    ImGuiIO &GetIO(void) { return ImGui::GetIO(); }
    Game::CTrack &GetTrack(void) { return *Track; }
    glm::vec3 &GetEditBlock(void) { return Camera.GetEditBlock(ProjectionMatrix); }
    Game::CTexture &GetWaterTexture(void) { return WaterTexture; }
    Game::CWeather &GetWeather(void) { return Weather; }
    void Resize(void);

};

}

#endif
