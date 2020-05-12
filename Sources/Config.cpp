
#include "Config.hpp"

CConfig::CConfig(void)
{
}

CConfig::CConfig(std::string Filename)
{
    Load(Filename);
}

CConfig::~CConfig(void)
{
}

void CConfig::Defaults(std::string Filename)
{

    if(Filename == "") {
        Filename = ConfigFile;
    }

    General.About = false;
    General.Properties = false;
    General.Help = false;
    General.Paused = false;
    General.LoadStartup = false;
    General.Debug = true;
    General.FpsCounter = true;
    General.Editors = true;
    General.Configuration = false;
    General.Statistics = false;

    Editor.Mouse = 0;
    Editor.Mode = 1;
    Editor.Tool = 8;
    Editor.Camera = 0;
    Editor.Smooth = 4.0f;
    Editor.GridWidth = 4;
    Editor.GridLength = 4;
    Editor.Altitude = 8;
    Editor.Bottom = 4;
    Editor.Orientation = 0;
    Editor.Selected = 0;
    Editor.Index = 0;
    Editor.Segment = 0;
    Editor.Rotation = 0;
    Editor.MirrorX = false;
    Editor.MirrorY = false;
    Editor.Paint = false;
    Editor.PaintAll = true;
    Editor.Locked = true;
    Editor.Random = false;
    Editor.Overlay = true;
    Editor.Preview = true;
    Editor.Boxes = false;

    strcpy(Game.Track, "UNTITLED.SIT");
    Game.Weather = 0;
    Game.Scenery = 3;

    Render.Wireframe = false;
    Render.Textured = true;
    Render.Lighted = true;
    Render.Culled = true;
    Render.Terrain = true;
    Render.Boxes = true;
    Render.Models = true;
    Render.Water = true;
    Render.Backdrop = true;
    Render.Overlay = true;
    Render.Course = true;

    Graphics.Width = 900;
    Graphics.Height = 600;
    Graphics.Monitor = 1;
    Graphics.RefreshRate = 60;
    Graphics.Antialiasing = 2;
    Graphics.FieldOfVision = 60.0f;
    Graphics.AspectRatio = 1.5f;
    Graphics.NearClip = 0.01f;
    Graphics.FarClip = 96.0f;
    Graphics.VSync = true;
    Graphics.DoubleBuffer = true;
    Graphics.Fullscreen = false;
    Graphics.Multisample = true;
    Graphics.TextureQuality = 2;

    for (unsigned int i = 0; i < 5; i++) {
        Cameras[i].Invert = true;
        Cameras[i].Position = glm::vec3(128.0f, 16.0f, 128.0f);
        Cameras[i].Center = glm::vec3(0.0f, 0.0f, 1.0f);
        Cameras[i].Up = glm::vec3(0.0f, 1.0f, 0.0f);
        Cameras[i].Yaw = 0.0f;
        Cameras[i].Pitch = 0.0f;
        Cameras[i].Angle = 0.0f;
    }

    Input.MouseLook = true;
    Input.MouseSensitivity = 0.1f;
    Input.MovementSpeed = 2.5f;
    Input.Forwards = GLFW_KEY_W;
    Input.Backwards = GLFW_KEY_S;
    Input.StrafeLeft = GLFW_KEY_A;
    Input.StrafeRight = GLFW_KEY_D;
    Input.Up = GLFW_KEY_E;
    Input.Down = GLFW_KEY_Q;

    Logger->Text("Loaded Default Configuration...");

    Save(Filename);

}

bool CConfig::Load(std::string Filename)
{

//    Defaults(Filename);
//    return true;

    std::ifstream CfgFile(Filename.c_str(), std::ios::in | std::ios::binary);

    if(CfgFile.is_open()) {

        CfgFile.read((char*) &General, sizeof(General));
        CfgFile.read((char*) &Editor, sizeof(Editor));
        CfgFile.read((char*) &Game, sizeof(Game));
        CfgFile.read((char*) &Render, sizeof(Render));
        CfgFile.read((char*) &Graphics, sizeof(Graphics));
        CfgFile.read((char*) &Input, sizeof(Input));
        CfgFile.read((char*) &Cameras, sizeof(Camera) * 5);

        CfgFile.close();
        Logger->Text("Loaded Configuration");

    } else {
        Logger->Error("Error Loading Configuration, Generating Defaults...", true, true, false);
        Defaults(Filename);
    }

    Editor.Mouse = 0;
    Editor.Camera = 0;
    Editor.Orientation = 0;
    Editor.Selected = 0;
    Editor.Index = 0;
    Editor.Rotation = 0;
    Editor.Segment = 0;
    Editor.Paint = false;
    Editor.PaintAll = true;
    Editor.Locked = true;
    Editor.Random = false;
    Editor.Boxes = false;

    General.About = false;
    General.Configuration = false;
    General.Properties = false;


    return true;
}

bool CConfig::Save(std::string Filename)
{

    std::ofstream CfgFile(Filename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

    if(CfgFile.is_open()) {

        CfgFile.write((char*) &General, sizeof(General));
        CfgFile.write((char*) &Editor, sizeof(Editor));
        CfgFile.write((char*) &Game, sizeof(Game));
        CfgFile.write((char*) &Render, sizeof(Render));
        CfgFile.write((char*) &Graphics, sizeof(Graphics));
        CfgFile.write((char*) &Input, sizeof(Input));
        CfgFile.write((char*) &Cameras, sizeof(Camera) * 5);

        CfgFile.close();
        Logger->Text("Saved Configuration");

    } else {
        Logger->Error("Error Saving Configuration, Aborting...");
    }

    return true;
}

