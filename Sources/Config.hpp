
#ifndef CCONFIG_HPP
#define CCONFIG_HPP

#include <Glm.hpp>

#include "Logger.hpp"

extern CLogger *Logger;
extern std::string ConfigFile;

class CConfig
{

private:

public:

    struct {
        bool About;
        bool Properties;
        bool Paused;
        bool Help;
        bool LoadStartup;
        bool Debug;
        bool FpsCounter;
        bool Editors;
        bool Configuration;
        bool Statistics;
    } General;

    struct {
        int Mouse;
        int Mode;
        int Tool;
        int Camera;
        float Smooth;
        int GridWidth;
        int GridLength;
        int Altitude;
        int Bottom;
        int Orientation;
        int Selected;
        int Index;
        int Segment;
        int Rotation;
        bool MirrorX;
        bool MirrorY;
        bool Paint;
        bool PaintAll;
        bool Locked;
        bool Random;
        bool Overlay;
        bool Preview;
        bool Dirty;
        bool Boxes;
    } Editor;

    struct {
        char Track[32];
        int Weather;
        int Scenery;
    } Game;

    struct {
        bool Wireframe;
        bool Textured;
        bool Lighted;
        bool Culled;
        bool Terrain;
        bool Boxes;
        bool Models;
        bool Water;
        bool Backdrop;
        bool Overlay;
        bool Course;
    } Render;

    struct {
        int Width;
        int Height;
        int Monitor;
        int RefreshRate;
        int Antialiasing;
        float FieldOfVision;
        float AspectRatio;
        float NearClip;
        float FarClip;
        bool VSync;
        bool DoubleBuffer;
        bool Fullscreen;
        bool Multisample;
        int TextureQuality;
    } Graphics;

    struct Camera {
        bool Invert;
        char Index;
        glm::vec3 Position;
        glm::vec3 Center;
        glm::vec3 Up;
        float Yaw;
        float Pitch;
        float Angle;
    };

    struct {
        bool MouseLook;
        float MouseSensitivity;
        float MovementSpeed;
        int Forwards;
        int Backwards;
        int StrafeLeft;
        int StrafeRight;
        int Up;
        int Down;
    } Input;

    void Defaults(std::string Filename = "");

    Camera Cameras[5];

public:

    CConfig(void);
    CConfig(std::string Filename);
    ~CConfig(void);

    bool Load(std::string Filename);
    bool Save(std::string Filename = "");

};

#endif
