
#include "Render.hpp"

using namespace Engine;

CRender::CRender(void) : EditBlock(0.0f)
{
}

CRender::~CRender(void)
{
    if (Dirty) {
        if(MessageBox(NULL, "Would You Like To Save Changes?", "Save", MB_ICONQUESTION | MB_YESNO) == IDYES) {
            if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
                std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
                if(File.size() != 0) {
                    strcpy(Config->Game.Track, File.substr(File.find_last_of("\\") + 1).c_str());
                    Track->Save(Config->Game.Track);
                }
            } else {
                Track->Save(Config->Game.Track);
            }
        }
        glfwRestoreWindow(GLWindow);
    }

    if(Track) {
        delete Track;
        Track = NULL;
        Logger->Text("Destroyed Track Object");
    }

    if(Pod) {
        delete Pod;
        Pod = NULL;
        Logger->Text("Destroyed Pod Pool Object");
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


bool CRender::Create(Game::CPod &Pod, Game::CTrack &Track)
{
    this->Pod = &Pod;
    this->Track = &Track;

    Weather.SetWeather(Config->Game.Weather);

    if(QuadTree.Create(&Track, Track.Dimensions() / 8)) {
        Logger->Text("Created Terrain QuadTree");
    } else {
        Logger->Error("Error Creating Terrain QuadTree");
    }

    Config->Cameras[0].Position = Track.GetStartPosition(0);
    Config->Cameras[0].Position.y += 1.5f;

    LogoTexture.LoadImage("Logo.png", "Game\\");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;// | ImGuiConfigFlags_NavEnableSetMousePos | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
    io.WantCaptureMouse = true;
    io.MouseDrawCursor = false;
    io.IniFilename = &InterfaceFile.c_str()[0];
    ImGui_ImplGlfw_InitForOpenGL(GLWindow, true);
    ImGui_ImplOpenGL3_Init(NULL);

    ImGuiStyle& Style = ImGui::GetStyle();

    Style.WindowRounding = 2.0f;
    Style.FrameRounding = 2.0f;
    Style.ScrollbarRounding = 2.0f;
    Style.WindowBorderSize = 2.0f;
    Style.FrameBorderSize = 2.0f;

    Style.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    Style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.75f);
    Style.Colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    Style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    Style.Colors[ImGuiCol_Border]                 = ImVec4(0.61f, 0.61f, 0.61f, 0.50f);
    Style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    Style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
    Style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.29f, 0.30f, 0.31f, 0.40f);
    Style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.49f, 0.46f, 0.46f, 0.67f);
    Style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    Style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.66f, 0.19f, 0.19f, 1.00f);
    Style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    Style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    Style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    Style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.87f, 0.37f, 0.38f, 1.00f);
    Style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
    Style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.90f, 0.15f, 0.15f, 1.00f);
    Style.Colors[ImGuiCol_Button]                 = ImVec4(0.45f, 0.45f, 0.45f, 0.40f);
    Style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.84f, 0.17f, 0.17f, 1.00f);
    Style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    Style.Colors[ImGuiCol_Header]                 = ImVec4(0.52f, 0.52f, 0.52f, 0.31f);
    Style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.80f, 0.11f, 0.11f, 0.80f);
    Style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    Style.Colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    Style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.39f, 0.39f, 0.39f, 0.78f);
    Style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    Style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    Style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.49f, 0.49f, 0.49f, 0.67f);
    Style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.50f, 0.10f, 0.10f, 0.95f);
    Style.Colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.98f, 0.26f, 0.26f, 0.48f);
    Style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(0.57f, 0.57f, 0.57f, 0.90f);
    Style.Colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.27f, 0.27f, 1.00f);
    Style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    Style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    Style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glAlphaFunc(GL_GEQUAL, 0.75f);
    glEnable(GL_ALPHA_TEST);

//    glEnable(GL_POLYGON_OFFSET_FILL);
//    glPolygonOffset(1.0f, 1.0f);

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT, GL_FILL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_NORMALIZE);
    glLightfv(GL_LIGHT0, GL_POSITION, &Track.GetSunPosition()[0]);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    Resize();

    Timer.Reset();

    Editor(false, true);

    Config->General.Paused = false;

    return true;

}

void CRender::Scene(void)
{

    if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) && Config->Editor.Mode != 5) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Models(true);
    }

    if(Config->Editor.Mode) {

        Config->Render.Wireframe ? glPolygonMode(GL_FRONT, GL_LINE) : glPolygonMode(GL_FRONT, GL_FILL);
        Config->Render.Textured ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
        Config->Render.Culled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        Config->Render.Lighted ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);

    } else {

        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
    }

    glCallList(PerspectiveView);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMultMatrixf(glm::value_ptr(Camera.Update(Config, Timer.Update())));
    Frustum.Update();

    glLightfv(GL_LIGHT0, GL_POSITION, &Track->GetSunPosition()[0]);

    if(Config->Render.Backdrop) {
        Backdrop();
    }

    Weather.ApplyTint();

    Terrain(QuadTree.GetRootNode(), glm::vec3(0.0f, Config->Cameras[Config->Editor.Camera].Position.y, 0.0f), false);

    if(Config->Render.Water) {
        Water(Timer.GetDelta(), Track->WaterHeight);
    }

    if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
        EditBlock = Camera.GetEditBlock(ProjectionMatrix);
    }

    if(Config->Render.Models) {
        Models();
    }

    Update = false;

    if (Config->Render.Overlay) {

        glLineWidth(2.0f);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        if (Config->Editor.Mode == 1 || Config->Editor.Mode == 2 || Config->Editor.Mode == 4) {

            Terrain(QuadTree.GetRootNode(), glm::vec3(0.0f, Config->Cameras[Config->Editor.Camera].Position.y, 0.0f), true);

        } else if (Config->Editor.Mode == 3) {

            if(Config->Render.Models) {
                Models(false, true);
            }
        } else if (Config->Editor.Mode == 5) {

            if(Config->Render.Course) {
                Course(Config->Editor.Index);
            }
        }

    }

    if (Config->Editor.Mode != 0 && Config->Editor.Mode != 3 ) {
        Editor(false, true);
    }

    Interface();

    glfwSwapBuffers(GLWindow);

}

void CRender::Editor(bool Apply, bool Refresh)
{

    static float Lower, Upper;
    static int MirrorX, MirrorY, Rotation;
    static float Y, Radius, OffsetY;
    static int OffsetX, OffsetZ, EvenX, EvenZ;
    static std::vector< std::vector<float> >Grid;
    static std::vector< std::vector<float> >Smooth;

    glLineWidth(2.5f);
    glDisable(GL_CULL_FACE);
//    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

    if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
        Config->Editor.Preview = true;
    } else {
        Config->Editor.Preview = false;
    }


    if (Refresh) {

        Config->Editor.Smooth = glm::clamp(Config->Editor.Smooth, 0.0f, 10.0f);
        Config->Editor.Altitude = glm::clamp(Config->Editor.Altitude, 0, 255);
        Config->Editor.GridWidth = glm::clamp(Config->Editor.GridWidth, 1, 64);
        Config->Editor.GridLength = glm::clamp(Config->Editor.GridLength, 1, 64);

        Y = 0.0f;
        OffsetY = 0.05f;
        EvenX = (Config->Editor.GridWidth % 2);
        EvenZ = (Config->Editor.GridLength % 2);
        Radius = (Config->Editor.GridWidth + Config->Editor.GridLength) / 4.0f;

        Grid.resize(Config->Editor.GridWidth + 1);
        Smooth.resize(Config->Editor.GridWidth + 1);

        for (int x = 0; x < Config->Editor.GridWidth + 1; x++) {
            Grid[x].resize(Config->Editor.GridLength + 1);
            Smooth[x].resize(Config->Editor.GridLength + 1);

            for (int z = 0; z < Config->Editor.GridLength + 1; z++) {
                Grid[x][z] = 0.0f;
                Smooth[x][z] = 0.0f;
            }
        }

        switch (Config->Editor.Tool) {

        case 0: // Raise

            for (int x = 1; x < Config->Editor.GridWidth; x++) {
                for (int z = 1; z < Config->Editor.GridLength; z++) {
                    Grid[x][z] = Config->Editor.Altitude;
                }
            }

            break;

        case 1: // Lower
            for (int x = 1; x < Config->Editor.GridWidth; x++) {
                for (int z = 1; z < Config->Editor.GridLength; z++) {
                    Grid[x][z] = -Config->Editor.Altitude;
                }
            }

            break;

        case 2: // Plane

            for (int x = 1; x < Config->Editor.GridWidth; x++) {
                for (int z = 1; z < Config->Editor.GridLength; z++) {
                    OffsetX = x - Config->Editor.GridWidth / 2 + EditBlock.x;
                    OffsetZ = z - Config->Editor.GridLength / 2 + EditBlock.z;
                    Grid[x][z] = Config->Editor.Altitude / 16.0f - Track->TerrainObject()->GetHeight(OffsetX, OffsetZ).y;
                }
            }

            break;

        case 3: // Slope

            switch (Config->Editor.Orientation) {

            case 0:

                for (int x = 1; x < Config->Editor.GridWidth; x++) {
                    Y += Config->Editor.Altitude / (Config->Editor.GridWidth - 1);

                    for (int z = 1; z < Config->Editor.GridLength; z++) {
                        Grid[x][z] = Y;
                    }
                }

                break;

            case 1:

                for (int z = Config->Editor.GridLength - 1; z > 0; z--) {
                    Y += Config->Editor.Altitude / (Config->Editor.GridLength - 1);

                    for (int x = Config->Editor.GridWidth - 1; x > 0 ; x--) {
                        Grid[x][z] = Y;
                    }
                }

                break;

            case 2:

                for (int x = Config->Editor.GridWidth - 1; x > 0 ; x--) {
                    Y += Config->Editor.Altitude / (Config->Editor.GridWidth - 1);

                    for (int z = Config->Editor.GridLength - 1; z > 0; z--) {
                        Grid[x][z] = Y;
                    }
                }

                break;

            case 3:

                for (int z = 1; z < Config->Editor.GridLength; z++) {
                    Y += Config->Editor.Altitude / (Config->Editor.GridLength - 1);

                    for (int x = 1; x < Config->Editor.GridWidth; x++) {
                        Grid[x][z] = Y;
                    }
                }

                break;

            }

            break;

        case 4: // Point

            Grid[Config->Editor.GridWidth / 2][Config->Editor.GridLength / 2] = Config->Editor.Altitude;
            break;

        case 5: // Bumps

            for (int x = 1; x < Config->Editor.GridWidth; x++) {
                for (int z = 1; z < Config->Editor.GridLength; z++) {
                    Grid[x][z] = (Config->Editor.Altitude - 0.0f) * ((((float) rand()) / (float) RAND_MAX)) + 0.0f;
                }
            }

            break;

        case 6: // Mound

            switch (Config->Editor.Orientation) {

            case 0:

                Y = Config->Editor.Altitude / (Config->Editor.GridLength / 2);

                for (int z = 1; z < Config->Editor.GridLength / 2; z++) {
                    for (int x = 1; x < Config->Editor.GridWidth; x++) {
                        Grid[x][z] = Y;
                    }

                    Y += Config->Editor.Altitude / (Config->Editor.GridLength / 2);
                }

                for (int z = Config->Editor.GridLength / 2; z < Config->Editor.GridLength; z++) {
                    for (int x = 1; x < Config->Editor.GridWidth; x++) {
                        Grid[x][z] = Y;
                    }

                    Y -= Config->Editor.Altitude / (Config->Editor.GridLength / 2);
                }

                break;

            case 1:

                Y = Config->Editor.Altitude / (Config->Editor.GridWidth / 2);

                for (int x = 1; x < Config->Editor.GridWidth / 2; x++) {
                    for (int z = 1; z < Config->Editor.GridLength; z++) {
                        Grid[x][z] = Y;
                    }

                    Y += Config->Editor.Altitude / (Config->Editor.GridWidth / 2);
                }

                for (int x = Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth; x++) {
                    for (int z = 1; z < Config->Editor.GridLength; z++) {
                        Grid[x][z] = Y;
                    }

                    Y -= Config->Editor.Altitude / (Config->Editor.GridWidth / 2);
                }

                break;

            case 2:

                Y = Config->Editor.Altitude / (Config->Editor.GridLength / 2);

                for (int z = 1; z < Config->Editor.GridLength / 2; z++) {
                    for (int x = 1; x < Config->Editor.GridWidth; x++) {
                        Grid[x][z] = Y;
                    }

                    Y += Config->Editor.Altitude / (Config->Editor.GridLength / 2);
                }

                for (int z = Config->Editor.GridLength / 2; z < Config->Editor.GridLength; z++) {
                    for (int x = 1; x < Config->Editor.GridWidth; x++) {
                        Grid[x][z] = Y;
                    }

                    Y -= Config->Editor.Altitude / (Config->Editor.GridLength / 2);
                }

                break;

            case 3:

                Y = Config->Editor.Altitude / (Config->Editor.GridWidth / 2);

                for (int x = 1; x < Config->Editor.GridWidth / 2; x++) {
                    for (int z = 1; z < Config->Editor.GridLength; z++) {
                        Grid[x][z] = Y;
                    }

                    Y += Config->Editor.Altitude / (Config->Editor.GridWidth / 2);
                }

                for (int x = Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth; x++) {
                    for (int z = 1; z < Config->Editor.GridLength; z++) {
                        Grid[x][z] = Y;
                    }

                    Y -= Config->Editor.Altitude / (Config->Editor.GridWidth / 2);
                }

                break;
            }

            break;

        case 7: // Hill

            for (int x = 1; x < Config->Editor.GridWidth; x++) {
                for (int z = 1; z < Config->Editor.GridLength; z++) {

                    Y = (float(Radius * Radius) - float((Config->Editor.GridWidth / 2.0f - x) * (Config->Editor.GridWidth / 2.0f - x) + (Config->Editor.GridLength / 2.0f - z) * (Config->Editor.GridLength / 2.0f - z)));
                    if (Y > 0) {
                        Grid[x][z] = Config->Editor.Altitude * Y / float(Radius * Radius);
                    }
                }
            }

            break;
        }

        if (Config->Editor.Smooth != 0.0f) {
            if(Config->Editor.Tool == 0 || Config->Editor.Tool == 1 || Config->Editor.Tool == 3 || Config->Editor.Tool == 6 || Config->Editor.Tool == 5) {

                for (int x = 1; x < Config->Editor.GridWidth; x++) {
                    for (int z = 1; z < Config->Editor.GridLength; z++) {
                        Smooth[x][z] = ((Grid[x - 1][z - 1] + Grid[x][z - 1] + Grid[x + 1][z - 1] +
                                     Grid[x - 1][z - 1] + Grid[x][z] + Grid[x + 1][z + 1] +
                                     Grid[x - 1][z + 1] + Grid[x][z + 1] + Grid[x + 1][z + 1]) / 9) / Config->Editor.Smooth;
                    }
                }
                for (int x = 1; x < Config->Editor.GridWidth; x++) {
                    for (int z = 1; z < Config->Editor.GridLength; z++) {
                        Grid[x][z] = Smooth[x][z];
                    }
                }
            }
        }
    }



    if (Config->Editor.Mode == 1) {

        glEnable(GL_BLEND);
        glColor4f(1.0f, 0.0f, 0.0f, 0.25f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);

        for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + EvenX; x++) {
            for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + EvenZ; z++) {

                OffsetX = x + EditBlock.x;
                OffsetZ = z + EditBlock.z;
                glVertex3f(OffsetX, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ).y + Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] / 16 + OffsetY, OffsetZ);
                glVertex3f(OffsetX, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ + 1).y + Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2 + 1] / 16 + OffsetY, OffsetZ + 1);
                glVertex3f(OffsetX + 1, Track->TerrainObject()->GetHeight(OffsetX + 1, OffsetZ + 1).y + Grid[x + Config->Editor.GridWidth / 2 + 1][z + Config->Editor.GridLength / 2 + 1] / 16 + OffsetY, OffsetZ + 1);
                glVertex3f(OffsetX + 1, Track->TerrainObject()->GetHeight(OffsetX + 1, OffsetZ).y + Grid[x + Config->Editor.GridWidth / 2 + 1][z + Config->Editor.GridLength / 2] / 16 + OffsetY, OffsetZ);

                if (Config->Editor.Paint) {
                    Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] /= 16;
                }
                if (Apply == true || Config->Editor.Paint) {
//                    if (x != -Config->Editor.GridWidth / 2 && z != -Config->Editor.GridLength / 2 && x != Config->Editor.GridWidth / 2 + EvenX && z != Config->Editor.GridLength / 2 + EvenZ) {
                        if (Config->Editor.Tool == 2) {
                            Track->TerrainObject()->SetHeight(OffsetX, OffsetZ, Config->Editor.Altitude / 16.0f);
                        } else {
                            Track->TerrainObject()->SetHeight(OffsetX, OffsetZ, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ).y + Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] / 16.0f);
                        }
//                    }
                }
            }
        }

        glEnd();


        if (Apply && !Config->Editor.Paint) {
            Track->TerrainObject()->CalculateNormals();
            Update = true;
        }

    } else if (Config->Editor.Mode == 2) {

        if (!Config->Editor.Preview) {
            glEnable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
            glColor4f(1.0f, 0.0f, 0.0f, 0.25f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glDisable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + EvenX; x++) {
            for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + EvenZ; z++) {

                OffsetX = x + EditBlock.x;
                OffsetZ = z + EditBlock.z;

                Upper = (Config->Editor.Altitude) / 16.0f;
                Lower = (Config->Editor.Bottom) / 16.0f;

                if ((Lower != Upper)) {

                    Track->BoxesObject()->GetTextureRotation(OffsetX, OffsetZ, 0, MirrorX, MirrorY, Rotation);

                    if (Config->Editor.PaintAll || Config->Editor.Selected == 0) {
                        Track->TexturePoolObject()->Bind(Config->Editor.Index);
                    } else {
                        Track->TexturePoolObject()->Bind(Track->BoxesObject()->GetTextureIndex(OffsetX, OffsetZ, 0));
                    }

                    glBegin(GL_QUADS);

                    glNormal3f(1, 0, 0);
                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ + 1);

                    glEnd();

                    Track->BoxesObject()->GetTextureRotation(OffsetX, OffsetZ, 1, MirrorX, MirrorY, Rotation);
                    if (Config->Editor.PaintAll || Config->Editor.Selected == 1) {
                        Track->TexturePoolObject()->Bind(Config->Editor.Index);
                    } else {
                        Track->TexturePoolObject()->Bind(Track->BoxesObject()->GetTextureIndex(OffsetX, OffsetZ, 1));
                    }
                    glBegin(GL_QUADS);

                    glNormal3f(-1, 0, 0);
                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ);

                    glEnd();

                    Track->BoxesObject()->GetTextureRotation(OffsetX, OffsetZ, 2, MirrorX, MirrorY, Rotation);
                    if (Config->Editor.PaintAll || Config->Editor.Selected == 2) {
                        Track->TexturePoolObject()->Bind(Config->Editor.Index);
                    } else {
                        Track->TexturePoolObject()->Bind(Track->BoxesObject()->GetTextureIndex(OffsetX, OffsetZ, 2));
                    }
                    glBegin(GL_QUADS);

                    glNormal3f(0, 0, -1);
                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);

                    glEnd();

                    Track->BoxesObject()->GetTextureRotation(OffsetX, OffsetZ, 3, MirrorX, MirrorY, Rotation);
                    if (Config->Editor.PaintAll || Config->Editor.Selected == 3) {
                        Track->TexturePoolObject()->Bind(Config->Editor.Index);
                    } else {
                        Track->TexturePoolObject()->Bind(Track->BoxesObject()->GetTextureIndex(OffsetX, OffsetZ, 3));
                    }
                    glBegin(GL_QUADS);

                    glNormal3f(0, 0, 1);
                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ);

                    glEnd();

                    Track->BoxesObject()->GetTextureRotation(OffsetX, OffsetZ, 4, MirrorX, MirrorY, Rotation);
                    if (Config->Editor.PaintAll || Config->Editor.Selected == 4) {
                        Track->TexturePoolObject()->Bind(Config->Editor.Index);
                    } else {
                        Track->TexturePoolObject()->Bind(Track->BoxesObject()->GetTextureIndex(OffsetX, OffsetZ, 4));
                    }
                    glBegin(GL_QUADS);

                    glNormal3f(0, -1, 0);
                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ + 1);

                    glEnd();

                    Track->BoxesObject()->GetTextureRotation(OffsetX, OffsetZ, 5, MirrorX, MirrorY, Rotation);
                    if (Config->Editor.PaintAll || Config->Editor.Selected == 5) {
                        Track->TexturePoolObject()->Bind(Config->Editor.Index);
                    } else {
                        Track->TexturePoolObject()->Bind(Track->BoxesObject()->GetTextureIndex(OffsetX, OffsetZ, 5));
                    }
                    glBegin(GL_QUADS);

                    glNormal3f(0, 1, 0);
                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);

                    glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ);

                    glEnd();

                    if (Apply == true || Config->Editor.Paint) {

                        Track->BoxesObject()->SetTextureIndex(OffsetX, OffsetZ, Config->Editor.Selected, Config->Editor.Index);

                        switch (Config->Editor.Rotation) {

                        case 0:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 0, 0, 0);
                            break;

                        case 1:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 0, 0, 1);
                            break;

                        case 2:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 0, 0, 2);
                            break;

                        case 3:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 0, 0, 3);
                            break;

                        case 4:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 1, 0, 0);
                            break;

                        case 5:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 1, 0, 1);
                            break;

                        case 6:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 1, 0, 2);
                            break;

                        case 7:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 1, 0, 3);
                            break;

                        case 8:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 0, 1, 0);
                            break;

                        case 9:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 0, 1, 1);
                            break;

                        case 10:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 0, 1, 2);
                            break;

                        case 11:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 0, 1, 3);
                            break;

                        case 12:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 1, 1, 0);
                            break;

                        case 13:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 1, 1, 1);
                            break;

                        case 14:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 1, 1, 2);
                            break;

                        case 15:
                            Track->BoxesObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.Selected, 1, 1, 3);
                            break;

                        }

                        if (Config->Editor.Mouse == 0) {

                            Track->BoxesObject()->SetLowerHeight(OffsetX, OffsetZ, Config->Editor.Bottom);
                            Track->BoxesObject()->SetUpperHeight(OffsetX, OffsetZ, Config->Editor.Altitude);

                        }

                    }
                }
            }
        }



        if (Apply && !Config->Editor.Paint) {
            Update = true;
        }

    } else if (Config->Editor.Mode == 3) {

//        if ((Config->Editor.Mouse == -3) && (Config->Editor.Selected != -1)) {
//            Track->SetModel(Config->Editor.Selected, Config->Editor.Index);
//        }

    } else if (Config->Editor.Mode == 4) {

        if (!Config->Editor.Preview) {
            glEnable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
            glColor4f(1.0f, 0.0f, 0.0f, 0.25f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glDisable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            Track->TexturePoolObject()->Bind(Config->Editor.Index);
        }

        glBegin(GL_QUADS);

        for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + EvenX; x++) {
            for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + EvenZ; z++) {

                Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] = 0.0f;
                OffsetX = x + EditBlock.x;
                OffsetZ = z + EditBlock.z;

                glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(Config->Editor.MirrorX, Config->Editor.MirrorY, Config->Editor.Orientation, 0)[0]);
                glVertex3f(OffsetX, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ).y + OffsetY, OffsetZ);

                glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(Config->Editor.MirrorX, Config->Editor.MirrorY, Config->Editor.Orientation, 1)[0]);
                glVertex3f(OffsetX + 1, Track->TerrainObject()->GetHeight(OffsetX + 1, OffsetZ).y + OffsetY, OffsetZ);

                glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(Config->Editor.MirrorX, Config->Editor.MirrorY, Config->Editor.Orientation, 2)[0]);
                glVertex3f(OffsetX + 1, Track->TerrainObject()->GetHeight(OffsetX + 1, OffsetZ + 1).y + OffsetY, OffsetZ + 1);

                glTexCoord2fv(&Track->TexturePoolObject()->GetTextureCoords(Config->Editor.MirrorX, Config->Editor.MirrorY, Config->Editor.Orientation, 3)[0]);
                glVertex3f(OffsetX, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ + 1).y + OffsetY, OffsetZ + 1);


                if (Apply == true || Config->Editor.Paint) {

                    Track->TerrainObject()->SetTextureIndex(OffsetX, OffsetZ, Config->Editor.Index);

                    switch (Config->Editor.Rotation) {

                    case 0:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 0, 0, 0);
                        break;

                    case 1:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 0, 0, 1);
                        break;

                    case 2:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 0, 0, 2);
                        break;

                    case 3:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 0, 0, 3);
                        break;

                    case 4:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 1, 0, 0);
                        break;

                    case 5:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 1, 0, 1);
                        break;

                    case 6:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 1, 0, 2);
                        break;

                    case 7:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 1, 0, 3);
                        break;

                    case 8:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 0, 1, 0);
                        break;

                    case 9:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 0, 1, 1);
                        break;

                    case 10:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 0, 1, 2);
                        break;

                    case 11:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 0, 1, 3);
                        break;

                    case 12:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 1, 1, 0);
                        break;

                    case 13:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 1, 1, 1);
                        break;

                    case 14:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 1, 1, 2);
                        break;

                    case 15:
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, 1, 1, 3);
                        break;

                    }

                    if (Config->Editor.Mouse == 0) {

                        Track->TerrainObject()->SetTextureIndex(OffsetX, OffsetZ, Config->Editor.Index);
                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.MirrorX,  Config->Editor.MirrorY, Config->Editor.Orientation);

                    }

                    if (Config->Editor.Mouse == 1) {

                        Track->TerrainObject()->SetTextureRotation(OffsetX, OffsetZ, Config->Editor.MirrorX,  Config->Editor.MirrorY, Config->Editor.Orientation);

                    }

                }

            }
        }

        glEnd();

        if (Apply && !Config->Editor.Paint) {
            Update = true;
        }

    } else if (Config->Editor.Mode == 5) {

        EditBlock = Camera.GetEditBlock(ProjectionMatrix);

        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT) && Config->Editor.Mouse == -1){
            if (Apply) {
                Track->InsertCheckpoint(Config->Editor.Segment,  glm::vec3(EditBlock.x - 1.0f, Track->TerrainObject()->GetHeight(EditBlock.x, EditBlock.z).y, EditBlock.z - 1.0f), glm::vec3(EditBlock.x + 1.0f, Track->TerrainObject()->GetHeight(EditBlock.x, EditBlock.z).y, EditBlock.z + 1.0f), "1", 60.0f);
            }
        } else if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT) && Config->Editor.Mouse == -2){
            if (Apply) {
                Track->DeleteCheckpoint(Config->Editor.Segment);
            }
        } else if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) && Config->Editor.Mouse == -1){
            Track->Vehicles[Config->Editor.Selected].Position = EditBlock;
        } else if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && Config->Editor.Mouse == -2){
            Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].Start = glm::vec3(EditBlock.x, Track->TerrainObject()->GetHeight(EditBlock.x, EditBlock.z).y, EditBlock.z);
        } else if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && Config->Editor.Mouse == -1){
            Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].End = glm::vec3(EditBlock.x, Track->TerrainObject()->GetHeight(EditBlock.x, EditBlock.z).y, EditBlock.z);
        }
    }

    if (Config->Editor.Overlay) {

        glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (Config->Editor.Mode == 2 && Config->Editor.Mode != 5) {

//            glColor4f(1.0f, 0.0f, 0.0f, 0.25f);
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glBegin(GL_QUADS);

            for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + EvenX; x++) {
                for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + EvenZ; z++) {

                    OffsetX = x + EditBlock.x;
                    OffsetZ = z + EditBlock.z;

                    Upper = (Config->Editor.Altitude) / 16.0f;
                    Lower = (Config->Editor.Bottom) / 16.0f;

                    if (Lower != Upper) {


                        glVertex3f(OffsetX, Lower, OffsetZ);
                        glVertex3f(OffsetX, Upper, OffsetZ);
                        glVertex3f(OffsetX, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX, Lower, OffsetZ + 1);

                        glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ);
                        glVertex3f(OffsetX + 1, Lower, OffsetZ);

                        glVertex3f(OffsetX, Lower, OffsetZ + 1);
                        glVertex3f(OffsetX, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);

                        glVertex3f(OffsetX + 1, Lower, OffsetZ);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ);
                        glVertex3f(OffsetX, Upper, OffsetZ);
                        glVertex3f(OffsetX, Lower, OffsetZ);

                        glVertex3f(OffsetX, Upper, OffsetZ);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX, Upper, OffsetZ + 1);

                        glVertex3f(OffsetX, Lower, OffsetZ);
                        glVertex3f(OffsetX, Lower, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Lower, OffsetZ);

                    }

                    Upper = Track->BoxesObject()->GetUpperHeight(OffsetX, OffsetZ);
                    Lower = Track->BoxesObject()->GetLowerHeight(OffsetX, OffsetZ);

                    if (Lower != Upper) {

                        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

                        glVertex3f(OffsetX, Lower, OffsetZ);
                        glVertex3f(OffsetX, Upper, OffsetZ);
                        glVertex3f(OffsetX, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX, Lower, OffsetZ + 1);

                        glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ);
                        glVertex3f(OffsetX + 1, Lower, OffsetZ);

                        glVertex3f(OffsetX, Lower, OffsetZ + 1);
                        glVertex3f(OffsetX, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);

                        glVertex3f(OffsetX + 1, Lower, OffsetZ);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ);
                        glVertex3f(OffsetX, Upper, OffsetZ);
                        glVertex3f(OffsetX, Lower, OffsetZ);

                        glVertex3f(OffsetX, Upper, OffsetZ);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ);
                        glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);
                        glVertex3f(OffsetX, Upper, OffsetZ + 1);

                        glVertex3f(OffsetX, Lower, OffsetZ);
                        glVertex3f(OffsetX, Lower, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Lower, OffsetZ);
                    }

                }
            }

            glEnd();

        } else if (Config->Editor.Mode != 5){

            glBegin(GL_QUADS);

            for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + EvenX; x++) {
                for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + EvenZ; z++) {

                    OffsetX = x + EditBlock.x;
                    OffsetZ = z + EditBlock.z;
                    if (!Config->Editor.Paint) {
                        glVertex3f(OffsetX, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ).y + Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] / 16 + OffsetY, OffsetZ);
                        glVertex3f(OffsetX, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ + 1).y + Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2 + 1] / 16 + OffsetY, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Track->TerrainObject()->GetHeight(OffsetX + 1, OffsetZ + 1).y + Grid[x + Config->Editor.GridWidth / 2 + 1][z + Config->Editor.GridLength / 2 + 1] / 16 + OffsetY, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Track->TerrainObject()->GetHeight(OffsetX + 1, OffsetZ).y + Grid[x + Config->Editor.GridWidth / 2 + 1][z + Config->Editor.GridLength / 2] / 16 + OffsetY, OffsetZ);
                    } else {
                        glVertex3f(OffsetX, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ).y + Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] + OffsetY, OffsetZ);
                        glVertex3f(OffsetX, Track->TerrainObject()->GetHeight(OffsetX, OffsetZ + 1).y + Grid[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2 + 1] + OffsetY, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Track->TerrainObject()->GetHeight(OffsetX + 1, OffsetZ + 1).y + Grid[x + Config->Editor.GridWidth / 2 + 1][z + Config->Editor.GridLength / 2 + 1] + OffsetY, OffsetZ + 1);
                        glVertex3f(OffsetX + 1, Track->TerrainObject()->GetHeight(OffsetX + 1, OffsetZ).y + Grid[x + Config->Editor.GridWidth / 2 + 1][z + Config->Editor.GridLength / 2] + OffsetY, OffsetZ);
                    }

                }
            }

            glEnd();
        }
    }

    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
}

void CRender::Terrain(CNode *Node, glm::vec3 Offset, bool Overlay)
{
    if(Node->Leaf) {

        if (Frustum.SphereInFrustum(glm::vec3(Node->Center.x, Offset.y, Node->Center.z), QuadTree.GetLeafSize()) != CFrustum::Outside) {
            Node->Render(Overlay);
            if (Update) {
                Node->Update(*Track);
            }
        }

    } else {
        for(int i = 0; i < 4; i++) {
            Terrain(Node->Child[i], Offset, Overlay);
        }
    }
}

void CRender::Interface(void)
{

//    glAlphaFunc(GL_GEQUAL, 0.5f);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool LoadPalette = false;
    static int ComboBox_TypeItem = 0;
    static int ComboBox_SoundItem = 0;

    if (Config->General.About) {

        Logo();

        ImGui::Begin("About Traxx2...", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoTitleBar*/);

        ImGui::SetWindowSize(ImVec2(300, 256), 0);
        ImGui::SetWindowPos(ImVec2(float ((Width / 2) - 150), (float (Height / 2) - Height / 8)));

        ImGui::Text(GetInfo().c_str());
        ImGui::Text("www.Natas8.com/Traxx2");
        ImGui::Text("Jpez1432@gmail.com");

        ImGui::Separator();
        ImGui::TextWrapped("Traxx2 is built from the Project Monster Engine, it was created for simplicty, and to help the MTM community come together again... Traxx2 uses OpenGL so graphics and processing is accelerated. The original Traxx, R.I.P. Guitar Bill was an awesome tool at the time, but hopefully it will be easier to use and much faster. All questions, comments and bug reports are welcome. Keep on truckin'");
        ImGui::End();
    }


    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl - N")) {
                if (Dirty) {
                    if(MessageBox(NULL, "Would You Like To Save Changes?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
                            std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
                            if(File.size() != 0) {
                                strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
                                Track->Save(Config->Game.Track);
                            }
                        } else {
                            Track->Save(Config->Game.Track);
                        }
                    }
                }

                strcpy(Config->Game.Track, UpperStr("Untitled.sit"));

                delete Pod;
                Pod = new Game::CPod();
                Pod->Load("Untitled.pod");

                delete Track;
                Track = new Game::CTrack();
                Track->Load(*Pod, Config->Game.Track);

                QuadTree.Destroy();
                QuadTree.Create(Track, Track->Dimensions() / 8);

                Config->Cameras[Config->Editor.Camera].Position = Track->GetStartPosition(0);

                LoadPalette = false;
                ComboBox_TypeItem = atoi(Track->Type.c_str()) - 1;
                ComboBox_SoundItem = atoi(Track->AmbientSound.c_str());

                glfwRestoreWindow(GLWindow);

            }
            if (ImGui::MenuItem("Open...", "Ctrl - O")) {
                if (Dirty) {
                    if(MessageBox(NULL, "Would You Like To Save Changes?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
                            std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
                            if(File.size() != 0) {
                                strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
                                Track->Save(Config->Game.Track);
                            }
                        } else {
                            Track->Save(Config->Game.Track);
                        }
                    }
                }

                std::string File = FileOpen(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
                if(File.size() != 0) {
                    strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));

                    delete Pod;
                    Pod = new Game::CPod();
                    Pod->Load("Resources.pod");

                    delete Track;
                    Track = new Game::CTrack();
                    Track->Load(*Pod, Config->Game.Track);

                    QuadTree.Destroy();
                    QuadTree.Create(Track, Track->Dimensions() / 8);

                    Config->Cameras[Config->Editor.Camera].Position = Track->GetStartPosition(0);

                    LoadPalette = false;
                    ComboBox_TypeItem = atoi(Track->Type.c_str()) - 1;
                    ComboBox_SoundItem = atoi(Track->AmbientSound.c_str());
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl - S")) {
                if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
                    std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
                    if(File.size() != 0) {
                        strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
                        Track->Save(Config->Game.Track);
                    }
                } else {
                    Track->Save(Config->Game.Track);
                }

            }
            if (ImGui::MenuItem("Save As...")) {
                std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);

                if(File.size() != 0) {
                    strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
                    Track->Save(Config->Game.Track);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Import Pod...")) {
                if (Dirty) {
                    if(MessageBox(NULL, "Would You Like To Save Changes?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
                            std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
                            if(File.size() != 0) {
                                strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
                                Track->Save(Config->Game.Track);
                            }
                        } else {
                            Track->Save(Config->Game.Track);
                        }
                    }
                }

                std::string File = FileOpen(NULL, (char*)("MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".pod")[0]);
                if(File.size() != 0) {

                    bool Extract = false;
                    if(MessageBox(NULL, "It Is Strongly Recommended You Extract The Files First, Would You Like To Extract Them Now?", "Extract", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        Extract = true;
                    }

                    delete Pod;
                    Pod = new Game::CPod();
                    Pod->Load(File.substr(File.find_last_of("\\") + 1).c_str(), false, Extract);

                    Weather.SetWeather(Config->Game.Weather);

                    if (Pod->NumTracks() == 0) {
                        Logger->Error("Pod File Contains No Tracks, Aborting...", true, true, false);

                        delete Pod;
                        Pod = new Game::CPod();
                        Pod->Load("Untitled.pod");

                        strcpy(Config->Game.Track, UpperStr("Untitled.sit"));

                    } else {
                        if (Pod->NumTracks() > 1) {
                            Logger->Text("Pod File Contains Multiple Tracks, Loading First Found", true, true);
                        }
                        strcpy(Config->Game.Track, UpperStr(Pod->Track(0).c_str()));
                    }


                    delete Track;
                    Track = new Game::CTrack();
                    Track->Load(*Pod, Config->Game.Track);

                    QuadTree.Destroy();
                    QuadTree.Create(Track, Track->Dimensions() / 8);

                    Config->Cameras[Config->Editor.Camera].Position = Track->GetStartPosition(0);

                    LoadPalette = false;
                    ComboBox_TypeItem = atoi(Track->Type.c_str()) - 1;
                    ComboBox_SoundItem = atoi(Track->AmbientSound.c_str());

                    glfwRestoreWindow(GLWindow);
                }

            }
            if (ImGui::MenuItem("Export Pod...")) {
                if (Dirty || StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
                    if(MessageBox(NULL, "Track File Needs To Be Saved To Continue, Would You Like To Continue?", "Save", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
                            std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
                            if(File.size() != 0) {
                                strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
                                Track->Save(Config->Game.Track);
                            }
                        } else {
                            Track->Save(Config->Game.Track);
                        }
                        std::string File = FileSave(NULL, (char*)("MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".pod")[0]);
                        if(File.size() != 0) {
                            Track->Export(File.substr(File.find_last_of("\\") + 1).c_str());
                        }
                    }
                } else {
                    std::string File = FileSave(NULL, (char*)("MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".pod")[0]);
                    if(File.size() != 0) {
                        Track->Export(File.substr(File.find_last_of("\\") + 1).c_str());
                    }
                }

            }
            ImGui::Separator();
            if (ImGui::MenuItem("Configuration...", "Shift - C", Config->General.Configuration)) {
                Config->General.Configuration = !Config->General.Configuration;
                Config->General.Properties = false;
                Config->General.Statistics = false;
                Config->General.About = false;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Ctrl - Q")) {
                Loop = false;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {

            if (ImGui::MenuItem("Undo", "Ctrl - Z")) {


            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl - X")) {


            }
            if (ImGui::MenuItem("Copy", "Ctrl - C")) {

            }
            if (ImGui::MenuItem("Paste", "Ctrl - V")) {


            }

            ImGui::Separator();
            if (ImGui::MenuItem("Delete", "Delete")) {


            }
            if (ImGui::MenuItem("Insert", "Insert")) {

            }

            ImGui::EndMenu();

        }

       if (ImGui::BeginMenu("View")) {

            if (ImGui::MenuItem("Properties...", "Tilde", Config->General.Properties)) {
                Config->General.Properties = !Config->General.Properties;
                if (Config->General.Properties) {
                    Config->General.Configuration = false;
                    Config->General.About = false;
                    Config->General.Statistics = false;
                }
            }


            if (ImGui::MenuItem("Statistics...", "Shift - S", Config->General.Statistics)) {
                Config->General.Statistics = !Config->General.Statistics;
                Config->General.Configuration = false;
                Config->General.About = false;
                Config->General.Properties = false;
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Editors", "Ctrl - E", Config->General.Editors)) {
                Config->General.Editors = !Config->General.Editors ;
            }
            if (ImGui::MenuItem("Debug Info", "Ctrl - D", Config->General.Debug)) {
                Config->General.Debug = !Config->General.Debug;
            }
            ImGui::EndMenu();
        }

       if (ImGui::BeginMenu("Tools")) {

            if (ImGui::MenuItem("Extract Pod...")) {
                std::string File = FileOpen(NULL, (char*)("MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".pod")[0]);
                if(File.size() != 0) {

                    Game::CPod *ExtractPod = new Game::CPod();
                    ExtractPod->Load(File.c_str(), true, true);
                    delete ExtractPod;

                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Save Screenshot...")) {
                std::string File = FileSave(NULL, (char*)("Png Files\0*.png\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".png")[0]);
                if(File.size() != 0) {
                    SOIL_save_screenshot(File.c_str(), SOIL_SAVE_TYPE_PNG, 0, 0, Width, Height);
                }
            }

            ImGui::EndMenu();
        }

       if (ImGui::BeginMenu("Help")) {

            if (ImGui::MenuItem("About Traxx2...")) {

                Config->General.Properties = false;
                Config->General.Configuration = false;
                Config->General.Statistics = false;
                Config->General.About = true;

            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if(Config->General.Statistics) {


        ImGui::Begin("Statistics", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoTitleBar*/);

        if (Track->MTM2Track) {
            ImGui::Text("MTM2 Track");
        } else {
            ImGui::Text("MTM1 Track");
        }
        ImGui::Separator();

        int Animated = 0;
        int Boxes = 0;
        for (int i = 0; i < Track->NumModels(); i++) {
            if (Track->ModelPoolObject()->GetModel(i).NumFrames != 0) {
                Animated++;
            }
            if (Track->Models[i].Filename == "Box") {
                Boxes++;
            }
        }

        ImGui::Text("Number Of Models - %i", Track->NumModels());
        ImGui::Text("Number Of Boxes - %i", Boxes);
        ImGui::Text("Number Of Animated Models - %i", Animated);

        ImGui::Separator();
        ImGui::Text("Number Of Textures - %i", Track->TexturePoolObject()->NumTextures());

        ImGui::Separator();
        ImGui::Text("Number Of Courses - %i", Track->NumCourses);

        for (int i = 0; i < Track->NumCourses; i++) {
            ImGui::Text("Course %i Number Of Checkpoints - %i", i, (int)Track->Courses[i].Checkpoints.size());
        }

        int GroundBoxes = 0;
        for (int x = 0; x < Track->Dimensions(); x++) {
            for (int z = 0; z < Track->Dimensions(); z++) {
                if (Track->BoxesObject()->GetLowerHeight(x, z) != Track->BoxesObject()->GetUpperHeight(x, z)) {
                    GroundBoxes++;
                }
            }
        }
        ImGui::Separator();
        ImGui::Text("Number Of Ground Boxes - %i", GroundBoxes);

        ImGui::Separator();
        for (int i = 0; i < Track->BackdropCount; i++) {
            ImGui::Text("Backdrop Filename - %i, %s", i + 1, Track->Backdrops[i].GetFilename().c_str());
        }

        ImGui::SetWindowPos(ImVec2(float (Width / 2) - ImGui::GetWindowWidth() / 2.0f, float (Height / 2) - ImGui::GetWindowHeight() / 2.0f));

        ImGui::End();

    }

    if(Config->General.Configuration) {

        LoadPalette = false;
        ComboBox_TypeItem = atoi(Track->Type.c_str()) - 1;
        ComboBox_SoundItem = atoi(Track->AmbientSound.c_str());

        ImGui::Begin("Configuration", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoTitleBar*/);

        ImGui::SetWindowSize(ImVec2(300, 350), 0);
        ImGui::SetWindowPos(ImVec2(float ((Width / 2) - 150), (float (Height / 2) - 175)));


        if(ImGui::CollapsingHeader("General")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Track"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("About Traxx2"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 1);

            ImGui::PushItemWidth(140);

            if(ImGui::Button("Load Defaults...", ImVec2(138, 18))) {

                if(MessageBox(NULL, "Defaults Will Be Loaded And Restart Required, Would You Like To Continue?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                    Config->Defaults();
                    Loop = false;
                }
                glfwRestoreWindow(GLWindow);
            }

            ImGui::SameLine();
            if(ImGui::Button("Restart Traxx2", ImVec2(138, 18))) {
                Loop = false;
            }

            ImGui::Checkbox("Load Startup", &Config->General.LoadStartup);
            ImGui::SameLine();
            ImGui::Checkbox("Debug Info", &Config->General.Debug);
            ImGui::SameLine();
            ImGui::Checkbox("Fps", &Config->General.FpsCounter);
        }

        ImGui::PushItemWidth(240);

        if(ImGui::CollapsingHeader("Graphics")) {

            static bool Restart = false;

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Track"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("About Traxx2"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);


            if (Restart) {
                if(ImGui::Button("Restart Traxx2 To Apply Changes...", ImVec2(275, 18))) {
                    Restart = false;
                    Loop = false;
                }

            }

            ImGui::PushItemWidth(160);

            static int Count, PosX, PosY;
            static GLFWmonitor** Monitors = glfwGetMonitors(&Count);
            const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor - 1)]);

            static bool LastFullscreen = Config->Graphics.Fullscreen;
            ImGui::Checkbox("Toggle Fullscreen", &Config->Graphics.Fullscreen);

            if (LastFullscreen != Config->Graphics.Fullscreen) {
                LastFullscreen = Config->Graphics.Fullscreen;

                if(Config->Graphics.Fullscreen) {
                    glfwSetWindowMonitor(GLWindow, Monitors[int(Config->Graphics.Monitor - 1)], 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
                } else {
                    glfwSetWindowMonitor(GLWindow, NULL, 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
                    const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor - 1)]);
                    glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor - 1)], &PosX, &PosY);
                    glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));
                }
            }

            ImGui::SameLine();

            static bool LastVSync = Config->Graphics.VSync;
            ImGui::Checkbox("Toggle V-Sync", &Config->Graphics.VSync);

            if (LastVSync != Config->Graphics.VSync) {
                LastVSync = Config->Graphics.VSync;
                glfwSwapInterval(Config->Graphics.VSync);
            }

            static bool LastMultiSample = Config->Graphics.Multisample;
            ImGui::Checkbox("Multisampling    ", &Config->Graphics.Multisample);

            if(LastMultiSample != Config->Graphics.Multisample) {
                LastMultiSample = Config->Graphics.Multisample;

                if(Config->Graphics.Multisample) {
                    glEnable(GL_MULTISAMPLE);
                } else {
                    glDisable(GL_MULTISAMPLE);
                }
            }

            ImGui::SameLine();

            static bool LastDoubleBuffer = Config->Graphics.DoubleBuffer;
            ImGui::Checkbox("Double Buffer", &Config->Graphics.DoubleBuffer);

            if(LastDoubleBuffer != Config->Graphics.DoubleBuffer) {
                LastDoubleBuffer = Config->Graphics.DoubleBuffer;
                Restart = true;
            }

            static int ComboBox_ResItem = 0;
            static bool LoadRes = false;
            if(LoadRes == false) {
                LoadRes = true;

                if(Config->Graphics.Width == 640 && Config->Graphics.Height == 480) {
                    ComboBox_ResItem = 0;
                } else if(Config->Graphics.Width == 800 && Config->Graphics.Height == 600) {
                    ComboBox_ResItem = 1;
                } else if(Config->Graphics.Width == 900 && Config->Graphics.Height == 600) {
                    ComboBox_ResItem = 2;
                } else if(Config->Graphics.Width == 1024 && Config->Graphics.Height == 768) {
                    ComboBox_ResItem = 3;
                } else if(Config->Graphics.Width == 1440 && Config->Graphics.Height == 900) {
                    ComboBox_ResItem = 4;
                } else if(Config->Graphics.Width == 1600 && Config->Graphics.Height == 900) {
                    ComboBox_ResItem = 5;
                } else if(Config->Graphics.Width == 1600 && Config->Graphics.Height == 1200) {
                    ComboBox_ResItem = 6;
                } else if(Config->Graphics.Width == 1680 && Config->Graphics.Height == 1050) {
                    ComboBox_ResItem = 7;
                } else if(Config->Graphics.Width == 1920 && Config->Graphics.Height == 1080) {
                    ComboBox_ResItem = 8;
                }
            }

            static int LastRes = 0;
            const char* ResItems[] = { "640x480", "800x600", "900x600", "1024x768", "1440x900", "1600x900", "1600x1200", "1680x1050", "1920x1080" };

            ImGui::Combo("Resolution", &ComboBox_ResItem, ResItems, IM_ARRAYSIZE(ResItems), 8);

            if (ComboBox_ResItem != LastRes) {
                LastRes = ComboBox_ResItem;

                if (ComboBox_ResItem == 0) {
                    glfwSetWindowSize(GLWindow, 640, 480);
                } else if (ComboBox_ResItem == 1) {
                    glfwSetWindowSize(GLWindow, 800, 600);
                } else if (ComboBox_ResItem == 2) {
                    glfwSetWindowSize(GLWindow, 900, 600);
                } else if (ComboBox_ResItem == 3) {
                    glfwSetWindowSize(GLWindow, 1024, 768);
                } else if (ComboBox_ResItem == 4) {
                    glfwSetWindowSize(GLWindow, 1440, 900);
                } else if (ComboBox_ResItem == 5) {
                    glfwSetWindowSize(GLWindow, 1600, 900);
                } else if (ComboBox_ResItem == 6) {
                    glfwSetWindowSize(GLWindow, 1600, 1200);
                } else if (ComboBox_ResItem == 7) {
                    glfwSetWindowSize(GLWindow, 1680, 1050);
                } else if (ComboBox_ResItem == 8) {
                    glfwSetWindowSize(GLWindow, 1920, 1080);
                }

                glfwGetWindowSize(GLWindow, &Config->Graphics.Width, &Config->Graphics.Height);
                glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor - 1)], &PosX, &PosY);
                glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));

            }

            static int LastMonitor = Config->Graphics.Monitor;
            ImGui::DragInt("Monitor", &Config->Graphics.Monitor, 1, 1, Count, "%i");

            if(LastMonitor != Config->Graphics.Monitor) {
                LastMonitor = Config->Graphics.Monitor;

                if(Config->Graphics.Fullscreen) {
                    glfwSetWindowMonitor(GLWindow, Monitors[int(Config->Graphics.Monitor - 1)], 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
                } else {
                    glfwSetWindowMonitor(GLWindow, NULL, 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
                    const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor - 1)]);
                    glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor - 1)], &PosX, &PosY);
                    glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));
                }
            }

            static float LastClip = Config->Graphics.FarClip;
            ImGui::DragFloat("Far Clipping", &Config->Graphics.FarClip, 1.0f,16.0f, 256.0f, "%.1f");

            if(LastClip != Config->Graphics.FarClip) {
                LastClip = Config->Graphics.FarClip;
                Resize();
            }

            static float LastFOV = Config->Graphics.FieldOfVision;
            ImGui::DragFloat("Field Of Vision", &Config->Graphics.FieldOfVision, 1.0f, 0.0f, 90.0f, "%.1f");

            if(LastFOV != Config->Graphics.FieldOfVision) {
                LastFOV = Config->Graphics.FieldOfVision;
                Resize();
            }

            static int LastTexture = Config->Graphics.TextureQuality;
            ImGui::DragInt("Texture Quality", &Config->Graphics.TextureQuality, 1, 0, 2, "%i");

            if(LastTexture != Config->Graphics.TextureQuality) {
                LastTexture = Config->Graphics.TextureQuality;
                Restart = true;
            }

            static int LastAA = Config->Graphics.Antialiasing;
            float Next = pow(2, ceil(log(Config->Graphics.Antialiasing) / log(2)));

            ImGui::DragInt("Antialiasing", &Config->Graphics.Antialiasing, Next, 2, 16, "%i");

            if(LastAA != Config->Graphics.Antialiasing) {
                LastAA = Config->Graphics.Antialiasing;
                Restart = true;
            }

        }

        if(ImGui::CollapsingHeader("Editor")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Editor"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Track"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("About Traxx2"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);

            static int ComboBox_EditorItem = Config->Editor.Mode;
            const char* EditingItems[] = { "Viewing", "Terrain", "Boxes", "Models", "Textures", "Course"};

            ImGui::PushItemWidth(200);
            ImGui::Combo("Edit Mode", &ComboBox_EditorItem, EditingItems, 5, 5);
            Config->Editor.Mode = ComboBox_EditorItem;

            if (Config->Editor.Mode > 0) {

                ImGui::Text("Non-Viewing Editor Flags");

                ImGui::Separator();
                ImGui::Checkbox("Mode Overlays  ", &Config->Render.Overlay);
                ImGui::SameLine();
                ImGui::Checkbox("Editor Overlays", &Config->Editor.Overlay);
                ImGui::Separator();

                ImGui::Checkbox("Wireframe", &Config->Render.Wireframe);
                ImGui::SameLine();
                ImGui::Checkbox("Textured ", &Config->Render.Textured);
                ImGui::SameLine();
                ImGui::Checkbox("Lighting ", &Config->Render.Lighted);

                ImGui::Checkbox("Culling  ", &Config->Render.Culled);
                ImGui::SameLine();
                ImGui::Checkbox("Terrain  ", &Config->Render.Terrain);
                ImGui::SameLine();
                ImGui::Checkbox("Boxes    ", &Config->Render.Boxes);

                ImGui::Checkbox("Models   ", &Config->Render.Models);
                ImGui::SameLine();
                ImGui::Checkbox("Water    ", &Config->Render.Water);
                ImGui::SameLine();
                ImGui::Checkbox("Backdrop ", &Config->Render.Backdrop);
            }
        }

//        if(ImGui::CollapsingHeader("General Help")) {
//
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General Help"), 1);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Terrain Editor"), 0);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Boxes Editor"), 0);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Models Editor"), 0);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Textures Editor"), 0);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Course Editor"), 0);
//
//            ImGui::Text("Escape - Pause");
//            ImGui::Text("Tilde - Menu");
//            ImGui::Text("Alt & Enter - Fullscreen");
//            ImGui::Text("Tab - Cycle Editing Modes");
//            ImGui::Text("Ctrl & Q - Exit");
//            ImGui::Text("Ctrl & V - Toggle VSync");
//            ImGui::Text("Ctrl & D - Debug Info");
//            ImGui::Text("Ctrl & F - Fps Counter");
//            ImGui::Text("Ctrl & C - Cycle Cameras");
//            ImGui::Text("W - Forwards");
//            ImGui::Text("S - Backwards");
//            ImGui::Text("A - Strafe Left");
//            ImGui::Text("D - Strafe Right");
//            ImGui::Text("Q - Down");
//            ImGui::Text("E - Up");
//            ImGui::Text("H - Editor Help");
//
//        }
//
//        if(ImGui::CollapsingHeader("About Traxx2")) {
//
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Editor"), 0);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Track"), 0);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("About Traxx2"), 1);
//            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);
//
//            ImGui::Text(GetInfo().c_str());
//            ImGui::Separator();
//            ImGui::Text("Contact: Jpez1432@gmail.com");
//            ImGui::Text("Website: www.Natas8.com");
//
//        }
        ImGui::End();
    }

    if (Config->General.Properties) {

        ImGui::Begin("Track Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoTitleBar*/);

        ImGui::SetWindowSize(ImVec2(300, 352), 0);
        ImGui::SetWindowPos(ImVec2(float ((Width / 2) - 150), (float (Height / 2) - 175)));

//        if(ImGui::CollapsingHeader("Track", ImGuiTreeNodeFlags_DefaultOpen)) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Track"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("About Traxx2"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);


//            if (ImGui::Button("Import Pod ")) {
//
//                if (Dirty) {
//                    if(MessageBox(NULL, "Would You Like To Save Changes?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
//                        if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
//                            std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//                            if(File.size() != 0) {
//                                strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
//                                Track->Save(Config->Game.Track);
//                            }
//                        } else {
//                            Track->Save(Config->Game.Track);
//                        }
//                    }
//                }
//
//                std::string File = FileOpen(NULL, (char*)("MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".pod")[0]);
//                if(File.size() != 0) {
//
//                    bool Extract = false;
//                    if(MessageBox(NULL, "It Is Strongly Recommended You Extract The Files First, Would You Like To Extract Them Now?", "Extract", MB_ICONQUESTION | MB_YESNO) == IDYES) {
//                        Extract = true;
//                    }
//
//                    delete Pod;
//                    Pod = new Game::CPod();
//                    Pod->Load(File.substr(File.find_last_of("\\") + 1).c_str(), false, Extract);
//
//                    Weather.SetWeather(Config->Game.Weather);
//
//                    if (Pod->NumTracks() == 0) {
//                        Logger->Error("Pod File Contains No Tracks, Aborting...", true, true, false);
//
//                        delete Pod;
//                        Pod = new Game::CPod();
//                        Pod->Load("Untitled.pod");
//
//                        strcpy(Config->Game.Track, UpperStr("Untitled.sit"));
//
//                    } else {
//                        if (Pod->NumTracks() > 1) {
//                            Logger->Text("Pod File Contains Multiple Tracks, Loading First Found", true, true);
//                        }
//                        strcpy(Config->Game.Track, UpperStr(Pod->Track(0).c_str()));
//                    }
//
//
//                    delete Track;
//                    Track = new Game::CTrack();
//                    Track->Load(*Pod, Config->Game.Track);
//
//                    QuadTree.Destroy();
//                    QuadTree.Create(Track, Track->Dimensions() / 8);
//
//                    Config->Cameras[Config->Editor.Camera].Position = Track->GetStartPosition(0);
//
//                    LoadPalette = false;
//                    ComboBox_TypeItem = atoi(Track->Type.c_str()) - 1;
//                    ComboBox_SoundItem = atoi(Track->AmbientSound.c_str());
//
//                    glfwRestoreWindow(GLWindow);
//                }
//            }
//
//            ImGui::SameLine();
//            if (ImGui::Button("Export Pod")) {
//
//                if (Dirty || StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
//                    if(MessageBox(NULL, "Track File Needs To Be Saved To Continue, Would You Like To Continue?", "Save", MB_ICONQUESTION | MB_YESNO) == IDYES) {
//                        if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
//                            std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//                            if(File.size() != 0) {
//                                strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
//                                Track->Save(Config->Game.Track);
//                            }
//                        } else {
//                            Track->Save(Config->Game.Track);
//                        }
//                        std::string File = FileSave(NULL, (char*)("MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".pod")[0]);
//                        if(File.size() != 0) {
//                            Track->Export(File.substr(File.find_last_of("\\") + 1).c_str());
//                        }
//                    }
//                } else {
//                    std::string File = FileSave(NULL, (char*)("MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".pod")[0]);
//                    if(File.size() != 0) {
//                        Track->Export(File.substr(File.find_last_of("\\") + 1).c_str());
//                    }
//                }
//            }
//
//            ImGui::SameLine();
//            if (ImGui::Button("Extract Pod ")) {
//                std::string File = FileOpen(NULL, (char*)("MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".pod")[0]);
//                if(File.size() != 0) {
//
//                    Game::CPod *ExtractPod = new Game::CPod();
//                    ExtractPod->Load(File.c_str(), true, true);
//                    delete ExtractPod;
//
//                }
//            }
//
//            if (ImGui::Button("New")) {
//
//                if (Dirty) {
//                    if(MessageBox(NULL, "Would You Like To Save Changes?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
//                        if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
//                            std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//                            if(File.size() != 0) {
//                                strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
//                                Track->Save(Config->Game.Track);
//                            }
//                        } else {
//                            Track->Save(Config->Game.Track);
//                        }
//                    }
//                }
//
//                strcpy(Config->Game.Track, UpperStr("Untitled.sit"));
//
//                delete Pod;
//                Pod = new Game::CPod();
//                Pod->Load("Untitled.pod");
//
//                delete Track;
//                Track = new Game::CTrack();
//                Track->Load(*Pod, Config->Game.Track);
//
//                QuadTree.Destroy();
//                QuadTree.Create(Track, Track->Dimensions() / 8);
//
//                Config->Cameras[Config->Editor.Camera].Position = Track->GetStartPosition(0);
//
//                LoadPalette = false;
//                ComboBox_TypeItem = atoi(Track->Type.c_str()) - 1;
//                ComboBox_SoundItem = atoi(Track->AmbientSound.c_str());
//
//                glfwRestoreWindow(GLWindow);
//            }
//
//            ImGui::SameLine();
//
//            if (ImGui::Button("Open")) {
//
//                if (Config->Graphics.Fullscreen) {
//                    glfwIconifyWindow(GLWindow);
//                }
//
//                if (Dirty) {
//                    if(MessageBox(NULL, "Would You Like To Save Changes?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
//                        if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
//                            std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//                            if(File.size() != 0) {
//                                strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
//                                Track->Save(Config->Game.Track);
//                            }
//                        } else {
//                            Track->Save(Config->Game.Track);
//                        }
//                    }
//                }
//
//                std::string File = FileOpen(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//                if(File.size() != 0) {
//                    strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
//
//                    delete Pod;
//                    Pod = new Game::CPod();
//                    Pod->Load("Resources.pod");
//
//                    delete Track;
//                    Track = new Game::CTrack();
//                    Track->Load(*Pod, Config->Game.Track);
//
//                    QuadTree.Destroy();
//                    QuadTree.Create(Track, Track->Dimensions() / 8);
//
//                    Config->Cameras[Config->Editor.Camera].Position = Track->GetStartPosition(0);
//
//                    LoadPalette = false;
//                    ComboBox_TypeItem = atoi(Track->Type.c_str()) - 1;
//                    ComboBox_SoundItem = atoi(Track->AmbientSound.c_str());
//
//                }
//
//                glfwRestoreWindow(GLWindow);
//            }
//
//            ImGui::SameLine();
//            if (ImGui::Button("Save")) {
//
//                if (StrCompare(Config->Game.Track, UpperStr("Untitled.sit"))) {
//                    std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//                    if(File.size() != 0) {
//                        strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
//                        Track->Save(Config->Game.Track);
//                    }
//                } else {
//                    Track->Save(Config->Game.Track);
//                }
//                glfwRestoreWindow(GLWindow);
//            }
//
//            ImGui::SameLine();
//            if (ImGui::Button("Save As")) {
//
//                std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//
//                if(File.size() != 0) {
//                    strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
//                    Track->Save(Config->Game.Track);
//                }
//                glfwRestoreWindow(GLWindow);
//            }

//            ImGui::SameLine();

            ImGui::PushItemWidth(180);

            ImGui::InputText("Track Name", &Track->Name[0], 128);

//            if (ImGui::Button(Config->Game.Track, ImVec2(185, 18))) {
//                std::string File = FileSave(NULL, (char*)("World Situation Files\0*.lvl\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Worlds\\")[0], &std::string(".SIT")[0]);
//                if (File != "") {
//                    strcpy(Config->Game.Track, UpperStr(File.substr(File.find_last_of("\\") + 1).c_str()));
//                }
//            }
//            ImGui::SameLine();
//            ImGui::Text("Track File");

            if (ImGui::Button(Track->Level.c_str(), ImVec2(181, 18))) {
                std::string File = FileSave(NULL, (char*)("World Level Files\0*.lvl\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Levels\\")[0], &std::string(".LVL")[0]);
                if (File != "") {
                    Track->Level = UpperStr(File.substr(File.find_last_of("\\") + 1).c_str());
                }
            }
            ImGui::SameLine();
            ImGui::Text("Track Level");

            if (Pod->NumTracks() > 1) {

                static int LastTrack = 0;
                static int ComboBox_TrackItem = 0;
                static char **TrackItems = new char*[Pod->NumTracks()];

                for(int i = 0; i < Pod->NumTracks(); i++) {
                    TrackItems[i] = &Pod->Track(i)[0];
                }

                ImGui::PushItemWidth(180);
                ImGui::Combo("Select Track", &ComboBox_TrackItem, TrackItems, Pod->NumTracks(), 8);

                if (ComboBox_TrackItem != LastTrack) {
                    LastTrack = ComboBox_TrackItem;

                    strcpy(Config->Game.Track, TrackItems[ComboBox_TrackItem]);

                    delete Track;
                    Track = new Game::CTrack();
                    Track->Load(*Pod, Config->Game.Track);

                    QuadTree.Destroy();
                    QuadTree.Create(Track, Track->Dimensions() / 8);

                    Config->Cameras[Config->Editor.Camera].Position = Track->GetStartPosition(0);
                    Weather.SetWeather(Config->Game.Weather);

                }
            }

            ImGui::InputText("Pod Comment", &Track->Comment[0], 80);

            static int LastTypeItem = atoi(Track->Type.c_str());
//            static int
            ComboBox_TypeItem = atoi(Track->Type.c_str());
            const char* TypeItems[] = { "Drag", "Circuit", "Rally", "Rumble" };

            ImGui::Combo("Track Type", &ComboBox_TypeItem, TypeItems, 4, 4);
            if (LastTypeItem != ComboBox_TypeItem) {
                LastTypeItem = ComboBox_TypeItem;
                Track->Type = std::to_string(ComboBox_TypeItem);
            }


            static int ComboBox_PaletteItem = 0;
            static int LastPaletteItem = 0;
            const char* PaletteItems[] = { "Farm Road", "Voodoo", "Excavation", "Scrapyard", "Heights", "Sidewinder Canyon", "Tinhorn Junction", "Tumbleweed", "Breakneck Ridge", "Arena Rumble", "Pyramid Rumble", "Hypercube Rumble", "Graveyard", "Crazy98", "Torture Pit", "MetalCr3"};
            ImGui::Combo("Color Palette", &ComboBox_PaletteItem, PaletteItems, 16, 16);

            if (LoadPalette == false) {
                LoadPalette = true;

                if (Track->Palette == "TPARK.ACT") {
                    ComboBox_PaletteItem = 0;
                } else if (Track->Palette == "MAIN.ACT") {
                    ComboBox_PaletteItem = 1;
                } else if (Track->Palette == "AZTEC.ACT") {
                    ComboBox_PaletteItem = 2;
                } else if (Track->Palette == "JUNK.ACT") {
                    ComboBox_PaletteItem = 3;
                } else if (Track->Palette == "ALASKA.ACT") {
                    ComboBox_PaletteItem = 4;
                } else if (Track->Palette == "SNAKE.ACT") {
                    ComboBox_PaletteItem = 5;
                } else if (Track->Palette == "AUSSIE.ACT") {
                    ComboBox_PaletteItem = 6;
                } else if (Track->Palette == "BAJA.ACT") {
                    ComboBox_PaletteItem = 7;
                } else if (Track->Palette == "ROCKQRY.ACT") {
                    ComboBox_PaletteItem = 8;
                } else if (Track->Palette == "ARENA.ACT") {
                    ComboBox_PaletteItem = 9;
                } else if (Track->Palette == "PYRAMID.ACT") {
                    ComboBox_PaletteItem = 10;
                } else if (Track->Palette == "HYPERCUBE.ACT") {
                    ComboBox_PaletteItem = 11;
                } else if (Track->Palette == "GRAVEYARD.ACT") {
                    ComboBox_PaletteItem = 121;
                } else if (Track->Palette == "CRAZY98.ACT") {
                    ComboBox_PaletteItem = 13;
                } else if (Track->Palette == "WAR.ACT") {
                    ComboBox_PaletteItem = 14;
                } else if (Track->Palette == "METALCR2.ACT") {
                    ComboBox_PaletteItem = 15;
                }
            }

            if (LastPaletteItem != ComboBox_PaletteItem) {
                LastPaletteItem = ComboBox_PaletteItem;

                switch (ComboBox_PaletteItem)
                {
                case 0:
                    Track->Palette = "TPARK.ACT";
                    break;
                case 1:
                    Track->Palette = "MAIN.ACT";
                    break;
                case 2:
                    Track->Palette = "AZTEC.ACT";
                    break;
                case 3:
                    Track->Palette = "JUNK.ACT";
                    break;
                case 4:
                    Track->Palette = "ALASKA.ACT";
                    break;
                case 5:
                    Track->Palette = "SNAKE.ACT";
                    break;
                case 6:
                    Track->Palette = "AUSSIE.ACT";
                    break;
                case 7:
                    Track->Palette = "BAJA.ACT";
                    break;
                case 8:
                    Track->Palette = "ROCKQRY.ACT";
                    break;
                case 9:
                    Track->Palette = "ARENA.ACT";
                    break;
                case 10:
                    Track->Palette = "PYRAMID.ACT";
                    break;
                case 11:
                    Track->Palette = "HYPERCUBE.ACT";
                    break;
                case 12:
                    Track->Palette = "GRAVEYARD.ACT";
                    break;
                case 13:
                    Track->Palette = "CRAZY98.ACT";
                    break;
                case 14:
                    Track->Palette = "WAR.ACT";
                    break;
                case 15:
                    Track->Palette = "METALCR2.ACT";
                    break;

                }
                Track->LoadPalette(Track->Palette);
            }

//            static int
            ComboBox_SoundItem = atoi(Track->AmbientSound.c_str());
            static int LastSoundItem = atoi(Track->AmbientSound.c_str());
            const char* SoundItems[] = { "Default", "Voodoo", "Excavation", "Farm Road", "Scrapyard", "Heights", "Sidewinder Canyon", "Tinhorn Junction", "Tumbleweed", "Breakneck Ridge", "Arena Rumble", "Pyramid Rumble", "Hypercube Rumble", "Graveyard", "Tweety Birds (C98)", "15"};

            ImGui::Combo("Sound Theme", &ComboBox_SoundItem, SoundItems, 16, 16);
            if (LastSoundItem != ComboBox_SoundItem) {
                LastSoundItem = ComboBox_SoundItem;
                Track->AmbientSound = std::to_string(ComboBox_SoundItem);
            }

            static int LastNumBackdrops = Track->BackdropCount;
            ImGui::DragInt("# Of Backdrops", &Track->BackdropCount, 1, 0, 2, "%i");
            if (Track->BackdropCount != LastNumBackdrops) {
                LastNumBackdrops = Track->BackdropCount;
                Track->Backdrops.resize(Track->BackdropCount);
            }

            for (int i = 0; i < Track->BackdropCount; i++) {
                if (ImGui::Button(&std::string(Track->Backdrops[i].Filename)[0], ImVec2(180, 18))) {
                    std::string File = FileOpen(NULL, (char*)("BIN Model Files\0*.bin\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Models\\")[0], &std::string(".bin")[0]);
                    if(File.size() != 0) {
                        Track->Backdrops[i].Load(*Pod, File.substr(File.find_last_of("\\") + 1).c_str());
                    }
                }
                ImGui::SameLine();
                ImGui::Text("Filename %i", i + 1);
            }

            static float LastWaterHeight = Track->WaterHeight;
            ImGui::SliderFloat("Water Height", &Track->WaterHeight, 0.0f, 16.0f, "%.2f");
            if (LastWaterHeight != Track->WaterHeight) {
                LastWaterHeight = Track->WaterHeight;
                Update = true;
            }

            ImGui::SliderInt("Complexity", &Config->Game.Scenery, 0, 3, "%i");

            if (ImGui::SliderInt("Weather Mode", &Config->Game.Weather, 0, 8, "%i")) {
                Weather.SetWeather(Config->Game.Weather);
            }

            static bool All = true;
            static bool Masked[9] = {true, true, true, true, true, true, true, true, true};
            static int WeatherMasks[9] = {1, 2, 4, 8, 16, 32, 64, 128, 256};

            for (int i = 0; i < 9; i++) {
                if (Masked[i] == false) {
                    All = false;
                }
            }

            ImGui::Checkbox("Clear    ", &Masked[0]);
            ImGui::SameLine();
            ImGui::Checkbox("Cloudy   ", &Masked[1]);
            ImGui::SameLine();
            ImGui::Checkbox("Foggy    ", &Masked[2]);

            ImGui::Checkbox("Dense Fog", &Masked[3]);
            ImGui::SameLine();
            ImGui::Checkbox("Rain     ", &Masked[4]);
            ImGui::SameLine();
            ImGui::Checkbox("Snow     ", &Masked[5]);

            ImGui::Checkbox("Dusk     ", &Masked[6]);
            ImGui::SameLine();
            ImGui::Checkbox("Night    ", &Masked[7]);
            ImGui::SameLine();
            ImGui::Checkbox("Black    ", &Masked[8]);

            if (Masked[0]) {
                Track->WeatherMask = 1;
            } else {
                Track->WeatherMask = 0;
            }

            if (!All) {
                for (int i = 0; i < 9; i++) {
                    if (Masked[i]) {
                        Track->WeatherMask |= WeatherMasks[i];
                    }
                }
            } else {
                Track->WeatherMask = 65535;
            }
//        }

        ImGui::End();
    }

    if(Config->General.Debug &&  Config->Editor.Mode != 0) {

        ImGui::SetNextWindowBgAlpha(0.35f);
        ImGui::Begin("Debug", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration  | ImGuiWindowFlags_NoCollapse);

        CConfig::Camera *Cam = &Config->Cameras[Config->Editor.Camera];

        ImGui::Text("Camera - %i", Config->Editor.Camera);
        ImGui::Text("Edit Block Info - %0.f, %0.f, %0.f", EditBlock.x, EditBlock.y, EditBlock.z);
        ImGui::Separator();

        if (Track->TexturePoolObject()->NumTextures() != 0) {
            ImGui::Text("Texture - %s", Track->TexturePoolObject()->GetFilename(Track->TerrainObject()->GetTextureIndex(EditBlock.x,  EditBlock.z)).c_str());
        }

        ImGui::Text("Altitude - %0.f", Track->TerrainObject()->GetHeight(EditBlock.x,  EditBlock.z).y * 16.0f);
        ImGui::Text("Box Top - %0.f", Track->BoxesObject()->GetUpperHeight(EditBlock.x,  EditBlock.z) * 16.0f);
        ImGui::Text("Box Bottom - %0.f", Track->BoxesObject()->GetLowerHeight(EditBlock.x,  EditBlock.z) * 16.0f);

        if (Track->NumModels() != 0 && Config->Editor.Selected != -1) {
            ImGui::Text("Model - %i, %s", Config->Editor.Selected, Track->Models[Config->Editor.Selected].Filename.c_str());
        }

        ImGui::Separator();
        ImGui::Text("Position - %0.2f, %0.2f, %0.2f", Cam->Position.x, Cam->Position.y, Cam->Position.z);
        ImGui::Text("View - %0.2f, %0.2f, %0.2f", Cam->Center.x, Cam->Center.y, Cam->Center.z);
        ImGui::Text("Pitch - %0.2f, Yaw - %0.2f", Cam->Pitch, Cam->Yaw);
        ImGui::Text("Angle - %0.2f", Cam->Angle);

        if(Config->General.FpsCounter) {
            ImGui::Separator();
            ImGui::Text("Fps - %i", Timer.GetFps());
        }

        ImGui::SetWindowPos(ImVec2(Width - ImGui::GetWindowSize().x - 4, 22));

        ImGui::End();

    }

    if(Config->General.Help) {

        ImGui::Begin("Traxx2 Help", NULL, /*ImGuiWindowFlags_AlwaysAutoResize |*/ ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        ImGui::SetWindowSize(ImVec2(420, 420), 0);
        ImGui::SetWindowPos(ImVec2(float ((Width / 2) - 210), float ((Height / 2) - 210)));

        if(ImGui::CollapsingHeader("General Help")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General Help"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Terrain Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Boxes Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Models Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Textures Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Course Editor"), 0);

            ImGui::Text("Escape - Pause");
            ImGui::Text("Tilde - Track Properties");
            ImGui::Text("Alt & Enter - Fullscreen");
            ImGui::Text("Tab - Cycle Editing Modes");
            ImGui::Text("Ctrl & Q - Exit");
            ImGui::Text("Ctrl & V - Toggle VSync");
            ImGui::Text("Ctrl & D - Debug Info");
            ImGui::Text("Ctrl & F - Fps Counter");
            ImGui::Text("Ctrl & E - Toggle Editors");
            ImGui::Text("Ctrl & C - Cycle Cameras");
            ImGui::Text("Shift & C - Configuration");
            ImGui::Text("W - Forwards");
            ImGui::Text("S - Backwards");
            ImGui::Text("A - Strafe Left");
            ImGui::Text("D - Strafe Right");
            ImGui::Text("Q - Down");
            ImGui::Text("E - Up");

            ImGui::Separator();
            ImGui::Text("Non-Viewing Mode Toggle Keys");
            ImGui::Separator();

            ImGui::Text("F1 - Culling");
            ImGui::Text("F2 - Lighting");
            ImGui::Text("F3 - Textured");
            ImGui::Text("F4 - Wireframe");
            ImGui::Text("F5 - Terrain");
            ImGui::Text("F6 - Boxes");
            ImGui::Text("F7 - Models");
            ImGui::Text("F8 - Water");
            ImGui::Text("F9 - Backdrop");
            ImGui::Text("F10 - Overlays");
            ImGui::Text("F11 - Editor Overlays");

        }

        if(ImGui::CollapsingHeader("Terrain Editor")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Terrain Editor"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Boxes Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Models Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Textures Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Course Editor"), 0);

            ImGui::Text("Left Control - Toggle Lock Editor");
            ImGui::Text("Left Shift - Apply Scaling");
            ImGui::Text("Scroll Wheel - Change Altitude");
            ImGui::Text("Scroll Wheel & Left Alt - Change Smoothing");
            ImGui::Text("Scroll Wheel & Left Control - Scale Grid");
            ImGui::Text("Left Mouse Button - Apply Tool");
            ImGui::Text("Right Mouse Button - Paint Mode");
            ImGui::Text("Ctrl & Z - Undo Last Action");
            ImGui::Text("Ctrl & X - Cut Terrain, Set Altitude");
            ImGui::Text("Ctrl & C - Copy Terrain");
            ImGui::Text("Ctrl & V - Paste Terrain");
            ImGui::Text("Spacebar - Select Terrain Altitude");


        }

        if(ImGui::CollapsingHeader("Boxes Editor")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Terrain Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Boxes Editor"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Models Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Textures Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Course Editor"), 0);

            ImGui::Text("Scroll Wheel & Left Alt & Left Shift - Upper Height");
            ImGui::Text("Scroll Wheel & Left Control & Left Shift - Lower Height");
            ImGui::Text("Left Mouse Button - Apply Heights");
            ImGui::Text("Scroll Wheel - Cycle Textures");
            ImGui::Text("Scroll Wheel & Left Shift - Scale Grid");
            ImGui::Text("Scroll Wheel & Left Alt - Cycle Rotations");
            ImGui::Text("Scroll Wheel & Left Control - Cycle Faces");
            ImGui::Text("Right Mouse Button - Paint Mode");
            ImGui::Text("Ctrl & Z - Undo Last Action");
            ImGui::Text("Delete - Delete Selected Boxes");
            ImGui::Text("Insert - Add Boxes");
            ImGui::Text("Ctrl & X - Cut Boxes");
            ImGui::Text("Ctrl & C - Copy Boxes");
            ImGui::Text("Ctrl & V - Paste Boxes");
            ImGui::Text("Spacebar - Select Boxes Heights");

        }

        if(ImGui::CollapsingHeader("Models Editor")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Terrain Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Boxes Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Models Editor"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Textures Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Course Editor"), 0);

            ImGui::Text("Left Shift - Apply Scaling");
            ImGui::Text("Left Mouse Button & Left Shift - Add Model");
            ImGui::Text("Left Mouse Button & Left Alt - Move Model");
            ImGui::Text("Right Mouse Button & Left Shift - Set Model");
            ImGui::Text("Right Mouse Button & Left Control - Delete Model");
            ImGui::Text("Scroll Wheel - Cycle Available Models");
            ImGui::Text("Scroll Wheel & Left Alt - Cycle/Set Model");
            ImGui::Text("Scroll Wheel & X - Rotate Model X Axis");
            ImGui::Text("Scroll Wheel & Y - Rotate Model Y Axis");
            ImGui::Text("Scroll Wheel & Z - Rotate Model Z Axis");
            ImGui::Text("Scroll Wheel & X & Left Control - Move Model X Axis");
            ImGui::Text("Scroll Wheel & Y & Left Control  - Move Model Y Axis");
            ImGui::Text("Scroll Wheel & Z & Left Control  - Move Model Z Axis");
            ImGui::Text("Ctrl & Z - Undo Last Action");
            ImGui::Text("Delete - Delete Selected Model");
            ImGui::Text("Insert - Add Model");
            ImGui::Text("P - Preview Index Model");
            ImGui::Text("Spacebar - Select Model Index");

        }

        if(ImGui::CollapsingHeader("Textures Editor")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Terrain Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Boxes Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Models Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Textures Editor"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Course Editor"), 0);

            ImGui::Text("Scroll Wheel & Left Alt - Cycle Rotations");
            ImGui::Text("Left Mouse Button - Apply Texture");
            ImGui::Text("Right Mouse Button - Paint Mode");
            ImGui::Text("Scroll Wheel & Left Control - Scale Grid");
            ImGui::Text("Scroll Wheel & Left Alt - Cycle/Set Texture");
            ImGui::Text("Scroll Wheel - Cycle Available Textures");
            ImGui::Text("Ctrl & Z - Undo Last Action");
            ImGui::Text("Delete - Delete Selected Texture");
            ImGui::Text("P - Preview Index Texture");
            ImGui::Text("Ctrl & X - Cut Textures");
            ImGui::Text("Ctrl & C - Copy Textures");
            ImGui::Text("Ctrl & V - Paste Textures");
            ImGui::Text("Spacebar - Select Texture Index");


        }

        if(ImGui::CollapsingHeader("Course Editor")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Terrain Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Boxes Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Models Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Textures Editor"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Course Editor"), 1);


            ImGui::Text("Left Shift - Apply Scaling");
            ImGui::Text("Left Alt & Left Mouse Button - Insert Segment");
            ImGui::Text("Left Alt & Right Mouse Button - Delete Segment");
            ImGui::Text("Left Shift & Left Mouse Button - Position Start Segment");
            ImGui::Text("Left Shift & Right Mouse Button - Position End Segment");
            ImGui::Text("Scroll Wheel & X - Rotate Vehicle X Axis");
            ImGui::Text("Scroll Wheel & Y - Rotate Vehicle Y Axis");
            ImGui::Text("Scroll Wheel & Z - Rotate Vehicle Z Axis");
            ImGui::Text("Scroll Wheel & X & Left Control - Move Vehicle X Axis");
            ImGui::Text("Scroll Wheel & Y & Left Control - Move Vehicle Y Axis");
            ImGui::Text("Scroll Wheel & Z & Left Control - Move Vehicle Z Axis");
            ImGui::Text("Left Ctrl & Left Mouse Button - Vehicle Position");
            ImGui::Text("Scroll Wheel & Left Alt - Cycle Vehicles");
            ImGui::Text("Scroll Wheel & Left Control - Cycle Checkpoints");
            ImGui::Text("Ctrl & Z - Undo Last Action");
            ImGui::Text("Delete - Delete Current Segement");
            ImGui::Text("Insert - Add Segement");
        }

//        ImGui::SetWindowPos(ImVec2(Width / 2 - ImGui::GetWindowSize().x / 2, Height / 2 - ImGui::GetWindowSize().y / 2));


        ImGui::End();

    }

    if (Config->General.Editors) {

        if(Config->Editor.Mode == 1) {

            ImGui::Begin("Terrain Editor", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize/*| ImGuiWindowFlags_AlwaysAutoResize*/ | ImGuiWindowFlags_NoCollapse);

            ImGui::SetWindowPos(ImVec2(4, 22));

            ImGui::PushItemWidth(80);

    //        ImGui::Checkbox("Lock Editor", &Config->Editor.Locked);
            ImGui::Checkbox("Render Terrain", &Config->Render.Terrain);
            ImGui::Checkbox("Render Overlay", &Config->Render.Overlay);
    //        ImGui::Checkbox("Paint Mode", &Config->Editor.Paint);
            ImGui::Separator();

            ImGui::SliderInt("Grid Width", &Config->Editor.GridWidth, 1, 64, "%i");
            ImGui::SliderInt("Grid Length", &Config->Editor.GridLength, 1, 64, "%i");
            ImGui::SliderInt("Orientation", &Config->Editor.Orientation, 0, 3, "%i");
            ImGui::SliderInt("Altitude", &Config->Editor.Altitude, 0, 255, "%i");
            ImGui::DragFloat("Smoothing", &Config->Editor.Smooth, 0.125f, 0.0f, 16.0f, "%.2f", 1.0f);

            ImGui::Separator();

            ImGui::RadioButton("Raise   ", &Config->Editor.Tool, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Lower   ", &Config->Editor.Tool, 1);
            ImGui::RadioButton("Plane   ", &Config->Editor.Tool, 2);
            ImGui::SameLine();
            ImGui::RadioButton("Slope   ", &Config->Editor.Tool, 3);
            ImGui::RadioButton("Point   ", &Config->Editor.Tool, 4);
            ImGui::SameLine();
            ImGui::RadioButton("Bumps   ", &Config->Editor.Tool, 5);
            ImGui::RadioButton("Mound   ", &Config->Editor.Tool, 6);
            ImGui::SameLine();
            ImGui::RadioButton("Hill    ", &Config->Editor.Tool, 7);

            ImGui::Separator();

            if (ImGui::Button("Normalize Height", ImVec2(160, 18))) {
                Track->TerrainObject()->AddTerrainUndo();
                for (int x = 0; x < Track->Dimensions(); x++) {
                    for (int z = 0; z < Track->Dimensions(); z++) {
                        Track->TerrainObject()->SetHeight(x, z, Config->Editor.Altitude);
                    }
                }
                Update = true;
            }

            ImGui::Separator();

            static int MinSize = 1;
            static int MaxSize = 16;
            static int NumHills = 1000;
            static int Seed = 12345;
            static int MaxHeight = Config->Editor.Altitude;

            if (ImGui::CollapsingHeader("Generator")) {

                ImGui::SliderInt("Min Size", &MinSize, 1, MaxSize, "%i");
                ImGui::SliderInt("Max Size", &MaxSize, 1, 16, "%i");
                ImGui::SliderInt("Max Height", &MaxHeight, 1, 255, "%i");
                ImGui::SliderInt("Num Hills", &NumHills, 1, 1000, "%i");
                ImGui::SliderInt("Random Seed", &Seed, 1, 65536, "%i");

                if (ImGui::Button("Randomize", ImVec2(160, 18))) {
                    MinSize = ( rand() * ( 16 - 1 ) / RAND_MAX ) + 1;
                    MaxSize = ( rand() * ( 16 - 1 ) / RAND_MAX ) + 1;
                    MaxHeight = ( rand() * ( 255 - 1 ) / RAND_MAX ) + 1;
                    NumHills = ( rand() * ( 1000 - 1 ) / RAND_MAX ) + 1;
                    Seed = ( rand() * ( 65536 - 1 ) / RAND_MAX ) + 1;

                    Track->TerrainObject()->AddTerrainUndo();
                    Track->TerrainObject()->Generate(MinSize, MaxSize, MaxHeight, NumHills, Seed);
                    Update = true;

                }

                if (ImGui::Button("Generate", ImVec2(160, 18))) {
                    Track->TerrainObject()->AddTerrainUndo();
                    Track->TerrainObject()->Generate(MinSize, MaxSize, MaxHeight, NumHills, Seed);
                    Update = true;
                }
            }


            ImGui::End();

        } else if(Config->Editor.Mode == 2) {

            ImGui::Begin("Boxes Editor", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize/*| ImGuiWindowFlags_AlwaysAutoResize*/ | ImGuiWindowFlags_NoCollapse);

            ImGui::SetWindowPos(ImVec2(4, 22));

            ImGui::PushItemWidth(75);

    //        ImGui::Checkbox("Lock Editor", &Config->Editor.Locked);
            ImGui::Checkbox("Render Boxes", &Config->Render.Boxes);
            ImGui::Checkbox("Render Overlay", &Config->Render.Overlay);
            ImGui::Checkbox("Preview Boxes", &Config->Editor.Preview);
    //        ImGui::Checkbox("Paint Mode", &Config->Editor.Paint);
            ImGui::Separator();



            ImGui::SliderInt("Grid Width", &Config->Editor.GridWidth, 1, 64, "%i");
            ImGui::SliderInt("Grid Length", &Config->Editor.GridLength, 1, 64, "%i");
            ImGui::SliderInt("Box Tops", &Config->Editor.Altitude, 0, 255, "%i");
            ImGui::SliderInt("Box Bottoms", &Config->Editor.Bottom, 0, Config->Editor.Altitude, "%i");

            ImGui::Separator();
            ImGui::Checkbox("Paint All Sides", &Config->Editor.PaintAll);
            ImGui::SliderInt("Box Side", &Config->Editor.Selected, 0, 5, "%i");
            ImGui::SliderInt("Rotation", &Config->Editor.Orientation, 0, 3, "%i");
            ImGui::Checkbox("Mirror Texture X", &Config->Editor.MirrorX);
            ImGui::Checkbox("Mirror Texture Y", &Config->Editor.MirrorY);

            ImGui::Separator();

            ImGui::Text("Available Textures %i", Track->TexturePoolObject()->NumTextures());

            if (Track->TexturePoolObject()->NumTextures() != 0) {
                ImGui::Text("Texture: %s", Track->TexturePoolObject()->GetFilename(Config->Editor.Index).c_str());
                ImGui::Image((void*)Track->TexturePoolObject()->GetTexture(Config->Editor.Index), ImVec2(150.0f, 150.0f), ImVec2(0,0), ImVec2(1,1), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
            }

            ImGui::End();

        } else if(Config->Editor.Mode == 3) {

            ImGui::Begin("Model Editor", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize/*| ImGuiWindowFlags_AlwaysAutoResize*/ | ImGuiWindowFlags_NoCollapse);

            ImGui::SetWindowPos(ImVec2(4, 22));

            ImGui::PushItemWidth(126);

    //        ImGui::Checkbox("Lock Editor", &Config->Editor.Locked);
            ImGui::Checkbox("Render Models", &Config->Render.Models);
            ImGui::Checkbox("Render Overlay", &Config->Render.Overlay);
            ImGui::Checkbox("Preview Models", &Config->Editor.Preview);
            ImGui::Checkbox("Render Boxes", &Config->Editor.Boxes);

    //        ImGui::Checkbox("Paint Mode", &Config->Editor.Paint);
            ImGui::Separator();

            ImGui::Text("Available Models %i", Track->ModelPoolObject()->NumModels());

            if (Track->ModelPoolObject()->NumModels() != 0) {
                ImGui::Text("Model: %s", Track->ModelPoolObject()->GetFilename(Config->Editor.Index).c_str());
            }

            if (ImGui::Button("Add Model...", ImVec2(140, 18))) {
                std::string File = FileOpen(NULL, (char*)("BIN Model Files\0*.bin\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Models\\")[0], &std::string(".bin")[0]);

                if(File.size() != 0) {
                    Track->ModelPoolObject()->AddModel(File.substr(File.find_last_of("\\") + 1).c_str());
                    Config->Editor.Index = Track->ModelPoolObject()->NumModels() - 1;
                    Config->Editor.Selected = Track->Models.size() - 1;
                }
            }

            if (Track->NumModels() != 0 && Config->Editor.Selected != -1) {
                if (ImGui::Button("Delete Model", ImVec2(140, 18))) {
                    Track->DeleteModel(Config->Editor.Selected);
                }
            }

            if (Track->NumModels() != 0) {
                if (ImGui::Button("Remove All Models", ImVec2(140, 18))) {
                    if(MessageBox(NULL, "This Action Can Not Be Undone, Would You Like To Continue?", "Undo", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        Track->DeleteAllModels();
                        Track->ModelPoolObject()->DeleteAllModels();
                    }
                }
            }

            if (Track->NumModels() != 0) {
                if (ImGui::Button("Purge Model", ImVec2(140, 18))) {
                    if(MessageBox(NULL, "This Action Can Not Be Undone, Would You Like To Continue?", "Undo", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        for (unsigned int i = 0; i < Track->Models.size(); i++) {
                            if (Track->Models[i].Index == Config->Editor.Index) {
                                Track->DeleteModel(i);
                            }
                        }
                        Track->ModelPoolObject()->DeleteModel(Config->Editor.Index);
                    }
                }
            }

            if (Track->NumModels() != 0) {

                if (Config->Editor.Selected != -1) {

                    ImGui::Text("Selected: %s", Track->Models[Config->Editor.Selected].Filename.c_str());

                    ImGui::Separator();

                    if (Track->Models[Config->Editor.Selected].Filename == "Box") {
                        if (ImGui::Button("Toggle Model", ImVec2(140, 18))) {
                            Track->SetModel(Config->Editor.Selected, 0);
                        }
                    } else if (Track->Models[Config->Editor.Selected].Filename != "Box") {
                        if (ImGui::Button("Toggle Box", ImVec2(140, 18))) {
                            Track->Models[Config->Editor.Selected].Filename = "Box";
                            Track->Models[Config->Editor.Selected].Dimensions = glm::vec3(0.5f);
                            Track->Models[Config->Editor.Selected].Bounds = 1.0f;
                            Track->Models[Config->Editor.Selected].GLModel = 0;
                            Track->Models[Config->Editor.Selected].Index = 0;
                            Track->Models[Config->Editor.Selected].Mass = 0.0f;
                            Track->Models[Config->Editor.Selected].Min = glm::vec3(-0.5f);
                            Track->Models[Config->Editor.Selected].Max = glm::vec3(0.5f);
                        }
                    }

    //                ImGui::SameLine();

//                    if (ImGui::Button("Set Current Model", ImVec2(140, 18))) {
//                        Track->SetModel(Config->Editor.Selected, Config->Editor.Index);
//                    }

                    ImGui::Separator();

                    ImGui::PushItemWidth(60);

                    ImGui::DragFloat("Position X ", &Track->Models[Config->Editor.Selected].Position[0], 0.025f, 0.0f, 255.0f, "%.2f", 1);
                    ImGui::DragFloat("Position Y ", &Track->Models[Config->Editor.Selected].Position[1], 0.025f, 0.0f, 255.0f, "%.2f", 1);
                    ImGui::DragFloat("Position Z ", &Track->Models[Config->Editor.Selected].Position[2], 0.025f, 0.0f, 255.0f, "%.2f", 1);

                    ImGui::DragFloat("Rotation X ", &Track->Models[Config->Editor.Selected].Rotation[2], 0.01f, 0.0f, 6.28318f, "%.5f", 1);
                    ImGui::DragFloat("Rotation Y ", &Track->Models[Config->Editor.Selected].Rotation[0], 0.01f, 0.0f, 6.28318f, "%.5f", 1);
                    ImGui::DragFloat("Rotation Z ", &Track->Models[Config->Editor.Selected].Rotation[1], 0.01f, 0.0f, 6.28318f, "%.5f", 1);


//                    ImGui::DragFloat3("Position", &Track->Models[Config->Editor.Selected].Position[0], 0.025f, 0.0f, 255.0f, "%.2f", 1);
//                    ImGui::DragFloat3("Rotation", &Track->Models[Config->Editor.Selected].Rotation[0], 0.025f, 0.0f, 255.0f, "%.2f", 1);

                    if (Track->Models[Config->Editor.Selected].Filename == "Box"){

                        ImGui::DragFloat("Dimensions X ", &Track->Models[Config->Editor.Selected].Dimensions[0], 0.025f, 0.0f, 255.0f, "%.2f", 1);
                        ImGui::DragFloat("Dimensions Y ", &Track->Models[Config->Editor.Selected].Dimensions[1], 0.025f, 0.0f, 255.0f, "%.2f", 1);
                        ImGui::DragFloat("Dimensions Z ", &Track->Models[Config->Editor.Selected].Dimensions[2], 0.025f, 0.0f, 255.0f, "%.2f", 1);

//                        ImGui::DragFloat3("Dimensions ", &Track->Models[Config->Editor.Selected].Dimensions[0], 0.025f, 0.0f, 255.0f, "%.2f", 1);
                    }

                    ImGui::PushItemWidth(90);

                    const char* TypeItems[] = { "Default", "Sign", "Barricade", "Tree", "Cone", "Vehicle", "Checkpoint", "No Collide", "No Collide (Facing)", "Collide (Facing)", "Moving", "Post", "Ramp", "Top-Crush" };
                    char TypeFlags[3];
                    static int ComboBox_TypeItem = 0;

                    ComboBox_TypeItem =  atoi(&Track->Models[Config->Editor.Selected].TypeFlags[0]);
                    ImGui::Combo("Type", &ComboBox_TypeItem, TypeItems, 14, 14);
                    sprintf(TypeFlags, "%i,0", ComboBox_TypeItem);
                    Track->Models[Config->Editor.Selected].TypeFlags = TypeFlags;

                    ImGui::DragFloat("Mass", &Track->Models[Config->Editor.Selected].Mass, 1, 0.0f, 65536.0f, "%.2f", 1);
                    const char* SceneryItems[] = { "Sparse", "Normal", "Complex" };
                    ImGui::Combo("Scenery", &Track->Models[Config->Editor.Selected].Priority, SceneryItems, 3, 3);

                }

            }

            if (glfwGetKey(GLWindow, GLFW_KEY_P)) {

                glEnable(GL_TEXTURE_2D);
                glEnable(GL_LIGHTING);
                glEnable(GL_BLEND);
                glEnable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                glPushMatrix();
                    glm::vec3 Position = Config->Cameras[Config->Editor.Camera].Position + Config->Cameras[Config->Editor.Camera].Center * Track->ModelPoolObject()->GetModel(Config->Editor.Index).GetBoundingSphere();
                    glTranslatef(Position.x, Position.y, Position.z);
                    glCallList(Track->ModelPoolObject()->GetModel(Config->Editor.Index).GLModel);
                glPopMatrix();


            }

            ImGui::End();

        } else if(Config->Editor.Mode == 4) {

            ImGui::Begin("Textures Editor", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize/*| ImGuiWindowFlags_AlwaysAutoResize*/ | ImGuiWindowFlags_NoCollapse);

            ImGui::SetWindowPos(ImVec2(4, 22));

            ImGui::PushItemWidth(70);

    //        ImGui::Checkbox("Lock Editor", &Config->Editor.Locked);
            ImGui::Checkbox("Render Textures", &Config->Render.Textured);
            ImGui::Checkbox("Render Overlay", &Config->Render.Overlay);
            ImGui::Checkbox("Preview Textures", &Config->Editor.Preview);
    //        ImGui::Checkbox("Paint Mode", &Config->Editor.Paint);
            ImGui::Separator();

            ImGui::SliderInt("Grid Width", &Config->Editor.GridWidth, 1, 64, "%i");
            ImGui::SliderInt("Grid Length", &Config->Editor.GridLength, 1, 64, "%i");
            ImGui::SliderInt("Rotation", &Config->Editor.Orientation, 0, 3, "%i");
            ImGui::Checkbox("Mirror Texture X", &Config->Editor.MirrorX);
            ImGui::Checkbox("Mirror Texture Y", &Config->Editor.MirrorY);

            ImGui::Separator();

            ImGui::Text("Available Textures %i", Track->TexturePoolObject()->NumTextures());

            if (Track->TexturePoolObject()->NumTextures() != 0) {
                ImGui::Text("Texture: %s", Track->TexturePoolObject()->GetFilename(Config->Editor.Index).c_str());
                ImGui::Image((void*)Track->TexturePoolObject()->GetTexture(Config->Editor.Index), ImVec2(150.0f, 150.0f), ImVec2(0,0), ImVec2(1,1), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
            }

            static bool Purge = false;

            if (ImGui::Button("Add Texture", ImVec2(150, 18))) {
                std::string File = FileOpen(NULL, (char*)("RAW Texture Files\0*.raw\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Art\\")[0], &std::string(".raw")[0]);
                if(File.size() != 0) {
                    Track->TexturePoolObject()->AddTexture(File.substr(File.find_last_of("\\") + 1).c_str());
                }
                if (Track->TexturePoolObject()->NumTextures() == 1) {
                    Update = true;
                }
            }

            if (Track->TexturePoolObject()->NumTextures() != 0) {
                if (ImGui::Button("Remove All Textures", ImVec2(150, 18))) {
                    if(MessageBox(NULL, "This Action Can Not Be Undone, Would You Like To Continue?", "Undo", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        Track->TexturePoolObject()->DeleteAll();
                        for (int x = 0; x < Track->Dimensions(); x++) {
                            for (int z = 0; z < Track->Dimensions(); z++) {
                                Track->TerrainObject()->SetTextureIndex(x, z, 0);

                            }
                        }
                        Purge = false;
                        Update = true;
                    }
                }
            }

            if (Track->TexturePoolObject()->NumTextures() != 0) {
                if (ImGui::Button("Normalize Texture", ImVec2(150, 18))) {
                    Track->TerrainObject()->AddTextureUndo();
                    for (int x = 0; x < Track->Dimensions(); x++) {
                        for (int z = 0; z < Track->Dimensions(); z++) {
                            Track->TerrainObject()->SetTextureIndex(x, z, Config->Editor.Index);
                        }
                    }
                    Update = true;
                }
            }

            if (Track->TexturePoolObject()->NumTextures() != 0) {
                if (ImGui::Button("Purge Texture", ImVec2(150, 18))) {
                    if (Purge) {
                        MessageBox(NULL, "Multiple Purges May Have Undefined Behavior On Undo Purge", "Purge Undo", MB_ICONEXCLAMATION | MB_OK);
                    }
                    Track->TerrainObject()->AddTextureUndo();
                    if (Track->TexturePoolObject()->DeleteTexture(Config->Editor.Index)) {
                        Purge = true;
                    }
                }
            }


            if (Purge) {
                if (Track->TexturePoolObject()->NumTextures() != 0) {
                    if (ImGui::Button("Undo Purge", ImVec2(150, 18))) {
                        Track->TexturePoolObject()->UndoDelete();
                        Track->TerrainObject()->RemoveTextureUndo();
                        Purge = false;
                        Update = true;
                    }
                }
            }

            ImGui::End();

            if (glfwGetKey(GLWindow, GLFW_KEY_P)) {

                glCallList(OrthographicalView);
                glTranslatef(Width / 2.0f - 128, Height / 2.0f - 128, 0.0f);

                glEnable(GL_TEXTURE_2D);
                glDisable(GL_LIGHTING);
                glDisable(GL_BLEND);
                glDisable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                Track->TexturePoolObject()->Bind(Config->Editor.Index);

                glBegin(GL_QUADS);
                    glTexCoord2f(0, 0);
                    glVertex2f(0, 0);
                    glTexCoord2f(0, 1);
                    glVertex2f(0, 256);
                    glTexCoord2f(1, 1);
                    glVertex2f(256, 256);
                    glTexCoord2f(1, 0);
                    glVertex2f(256, 0);
                glEnd();

            }

        } else if (Config->Editor.Mode == 5) {

            ImGui::Begin("Course Editor", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize/*| ImGuiWindowFlags_AlwaysAutoResize*/ | ImGuiWindowFlags_NoCollapse);

            ImGui::SetWindowPos(ImVec2(4, 22));

            ImGui::PushItemWidth(70);

            ImGui::RadioButton("Main (0)", &Config->Editor.Index, 0);
            ImGui::RadioButton("Course 1", &Config->Editor.Index, 1);
//            ImGui::SameLine();
            ImGui::RadioButton("Course 2", &Config->Editor.Index, 2);
            ImGui::RadioButton("Course 3", &Config->Editor.Index, 3);
//            ImGui::SameLine();
            ImGui::RadioButton("Course 4", &Config->Editor.Index, 4);
//            ImGui::Separator();

            if (Config->Editor.Index < Track->NumCourses) {
                if ((unsigned int)Config->Editor.Segment < Track->Courses[Config->Editor.Index].Checkpoints.size()) {
                    ImGui::Text("Current Segment %i", Config->Editor.Segment);

                    ImGui::PushItemWidth(70);

                    ImGui::DragFloat("Start X", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].Start[0], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);
                    ImGui::DragFloat("Start Y", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].Start[1], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);
                    ImGui::DragFloat("Start Z", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].Start[2], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);

                    ImGui::DragFloat("End X", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].End[0], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);
                    ImGui::DragFloat("End Y", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].End[1], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);
                    ImGui::DragFloat("End Z", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].End[2], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);


//                    ImGui::DragFloat3("Start", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].Start[0], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);
//                    ImGui::DragFloat3("End", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].End[0], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);
                    ImGui::DragFloat("Speed", &Track->Courses[Config->Editor.Index].Checkpoints[Config->Editor.Segment].Speed, 0.1f, 0.0f, 100.0f, "%0.2f", 1.0f);
                    ImGui::Separator();
                }
            }
            ImGui::PushItemWidth(70);

            const char* StartItems[] = { "Vehicle 0", "Vehicle 1", "Vehicle 2", "Vehicle 3", "Vehicle 4", "Vehicle 5", "Vehicle 6", "Vehicle 7" };
            ImGui::Combo("Selected", &Config->Editor.Selected, StartItems, 8, 8);

            ImGui::Separator();
            ImGui::DragFloat("Position X", &Track->Vehicles[Config->Editor.Selected].Position[0], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);
            ImGui::DragFloat("Position Y", &Track->Vehicles[Config->Editor.Selected].Position[1], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);
            ImGui::DragFloat("Position Z", &Track->Vehicles[Config->Editor.Selected].Position[2], 0.1f, 0.0f, Track->Dimensions(), "%0.2f", 1.0f);

            ImGui::DragFloat("Rotation X", &Track->Vehicles[Config->Editor.Selected].Rotation[2], 0.01f, 0.0f, 6.28318f, "%0.5f", 1.0f);
            ImGui::DragFloat("Rotation Y", &Track->Vehicles[Config->Editor.Selected].Rotation[0], 0.01f, 0.0f, 6.28318f, "%0.5f", 1.0f);
            ImGui::DragFloat("Rotation Z", &Track->Vehicles[Config->Editor.Selected].Rotation[1], 0.01f, 0.0f, 6.28318f, "%0.5f", 1.0f);

            ImGui::End();
        }
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//    glAlphaFunc(GL_GEQUAL, 0.5f);


}

void CRender::Logo(void)
{
    Resize();

    glCallList(OrthographicalView);

    float Size = (Width + Height) / 10;

    glTranslatef(Width / 2, Height / 5, 0.0f);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    LogoTexture.Bind(0);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-Size, Size);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(Size, Size);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(Size, -Size);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-Size, -Size);

    glEnd();

}

void CRender::Water(float Delta, float Height)
{
    glCullFace(GL_BACK);

    if(Config->Render.Textured) {
        Weather.ApplyWaterTint();
    }

    static float Scroll = 0.0f;
    Scroll += Delta * 0.025f;

    WaterTexture.Bind(0);
    glBegin(GL_QUADS);

    glNormal3f(0.0f, -1.0f, 0.0f);

    glTexCoord2f(Track->Dimensions() + Scroll, 0.0f + Scroll);
    glVertex3f(Track->Dimensions(), Height, 0.0f);

    glTexCoord2f(Track->Dimensions() + Scroll, Track->Dimensions() + Scroll);
    glVertex3f(Track->Dimensions(), Height, Track->Dimensions());

    glTexCoord2f(0.0f + Scroll, Track->Dimensions() + Scroll);
    glVertex3f(0.0f, Height, Track->Dimensions());

    glTexCoord2f(0.0f + Scroll, 0.0f + Scroll);
    glVertex3f(0.0f, Height, 0.0f);

    glEnd();

}

void CRender::Backdrop(void)
{

    if(Config->Game.Weather == 2 || Config->Game.Weather == 3) {
        return;
    }

    Weather.ApplyBackdropTint();

    glPushMatrix();

    glDisable(GL_DEPTH_TEST);

    glTranslatef(Config->Cameras[Config->Editor.Camera].Position.x, Config->Cameras[Config->Editor.Camera].Position.y, Config->Cameras[Config->Editor.Camera].Position.z);

    Weather.DrawSkyDome();
    for (int i = 0; i < Track->BackdropCount; i++) {
        Track->DrawBackground(i);
    }

    glEnable(GL_DEPTH_TEST);

    glPopMatrix();

}

void CRender::Models(bool Picking, bool Overlay)
{

    if (!Overlay && Config->Editor.Mouse == -3 && Config->Editor.Mode == 3 && Track->NumModels() != 0) {

        EditBlock = Camera.GetEditBlock(ProjectionMatrix);

        glEnable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT, GL_FILL);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT)) {
            Track->Models[Config->Editor.Selected].Position = glm::vec3(EditBlock.x, Track->TerrainObject()->GetHeight(EditBlock.x, EditBlock.z).y + Track->ModelPoolObject()->GetModel(Config->Editor.Index).Max.y, EditBlock.z);
        }

        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT) && Config->Editor.Selected != -1) {
            Track->Models[Config->Editor.Selected].Position = glm::vec3(EditBlock.x, Track->TerrainObject()->GetHeight(EditBlock.x, EditBlock.z).y + Track->Models[Config->Editor.Selected].Max.y, EditBlock.z);
        }

    }

    float Dimensions = 0;
    glm::vec3 Min(0), Max(0);

    if (Picking) {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT, GL_FILL);
    } else {
        Weather.ApplyTint();
    }

    if (Overlay) {
        glDisable(GL_LIGHTING);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }

    for(int i = 0; i < Track->NumModels(); i++) {

        if (Picking) {
            glColor4f(float((i + 1) &0xFF) / 255.0f, float(((i + 1) >> 8) &0xFF) / 255.0f, float(((i + 1) >> 16) &0xFF) / 255.0f, 1.0f);
        }

        glm::vec3 Position = Track->GetModelPosition(i);
        glm::vec3 Rotation = Track->GetModelRotation(i);
        Dimensions = Track->GetModelBounds(i, Min, Max);

        if(Track->GetModelPriority(i) >= (int)Config->Game.Scenery) {
            continue;
        }

        if (Track->GetModelTypeFlags(i) == "6,0") {
            continue;
        }

        if(Frustum.SphereInFrustum(Position, Dimensions) > CFrustum::Outside) {

            glPushMatrix();

            glTranslatef(Position.x, Position.y, Position.z);

            if (Track->GetModelTypeFlags(i) != "8,0" && Track->GetModelTypeFlags(i) != "9,0") {

                glRotatef(270.0f - glm::degrees(Rotation.x), 0.0f, 1.0f, 0.0f);

            } else {

                double Theta = 0.0f;

                if(Position.z - Config->Cameras[Config->Editor.Camera].Position.z == 0.0f) {

                    if(Position.x < Config->Cameras[Config->Editor.Camera].Position.x) {
                        Theta = (glm::pi<float>());
                    }

                } else {

                    Theta = atan((Position.x - Config->Cameras[Config->Editor.Camera].Position.x) / (Position.z - Config->Cameras[Config->Editor.Camera].Position.z));

                    if(Position.z > Config->Cameras[Config->Editor.Camera].Position.z) {
                        Theta = (glm::pi<float>()) / 2.0f - Theta;
                    } else {
                        Theta = (glm::pi<float>()) * 1.5f - Theta;
                    }

                }

                glRotatef(270.0f - glm::degrees(Theta), 0.0f, 1.0f, 0.0f);

            }

            glRotatef(glm::degrees(-Rotation.z), 1.0f, 0.0f, 0.0f);
            glRotatef(glm::degrees(Rotation.y), 0.0f, 0.0f, 1.0f);

//            glCullFace(GL_BACK);

            if (Track->Models[i].Filename == "Box" && Config->Editor.Boxes) {
//                glDisable(GL_LIGHTING);
//                glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
                Box(Track->Models[i].Position, Track->Models[i].Dimensions);
            } else {
                Track->DrawModel(i);
            }

            glPopMatrix();
        }
    }

    if (Picking) {

        double XPos, YPos;
        unsigned char Array[4];

        glfwGetCursorPos(GLWindow, &XPos, &YPos);
        glReadPixels(XPos, YPos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, Array);

        if ((0x0 | (0x0<<8) | (0x0<<16)) == ((Array[0]) | (Array[1] << 8) | (Array[2] << 16))) {
            Config->Editor.Selected = 0;
        } else {
            Config->Editor.Selected = (Array[0]) | (Array[1] << 8) | (Array[2] << 16);
            Config->Editor.Selected--;
        }

    }

    if(Config->Editor.Mode == 3 && Config->Editor.Selected != -1) {

        glm::vec3 Position = Track->GetModelPosition(Config->Editor.Selected);
        glm::vec3 Rotation = Track->GetModelRotation(Config->Editor.Selected);
        Dimensions = Track->GetModelBounds(Config->Editor.Selected, Min, Max);

        glDisable(GL_LIGHTING);
        glColor4f(1.0f, 0.0f, 0.0f, 0.85f);

        glPushMatrix();

        glTranslatef(Position.x, Position.y, Position.z);

        if (Track->GetModelTypeFlags(Config->Editor.Selected) != "8,0" && Track->GetModelTypeFlags(Config->Editor.Selected) != "9,0") {

            glRotatef(270.0f - glm::degrees(Rotation.x), 0.0f, 1.0f, 0.0f);

        } else {

            double Theta = 0.0f;

            if(Position.z - Config->Cameras[Config->Editor.Camera].Position.z == 0.0f) {

                if(Position.x < Config->Cameras[Config->Editor.Camera].Position.x) {
                    Theta = (glm::pi<float>());
                }

            } else {

                Theta = atan((Position.x - Config->Cameras[Config->Editor.Camera].Position.x) / (Position.z - Config->Cameras[Config->Editor.Camera].Position.z));

                if(Position.z > Config->Cameras[Config->Editor.Camera].Position.z) {
                    Theta = (glm::pi<float>()) / 2.0f - Theta;
                } else {
                    Theta = (glm::pi<float>()) * 1.5f - Theta;
                }

            }

            glRotatef(270.0f - glm::degrees(Theta), 0.0f, 1.0f, 0.0f);

        }

        glRotatef(glm::degrees(-Rotation.z), 1.0f, 0.0f, 0.0f);
        glRotatef(glm::degrees(Rotation.y), 0.0f, 0.0f, 1.0f);

        glCullFace(GL_BACK);

        if (Track->Models[Config->Editor.Selected].Filename == "Box" && Config->Editor.Boxes) {
            Box(Track->Models[Config->Editor.Selected].Position, Track->Models[Config->Editor.Selected].Dimensions);
        } else {
            Track->DrawModel(Config->Editor.Selected);
        }

        glPopMatrix();
    }

}

void CRender::Course(int Index)
{
    Object = gluNewQuadric();
    gluQuadricNormals(Object, 0);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(2.5f);

    if ((unsigned int)Index < Track->Courses.size() && Track->Courses[Index].Checkpoints.size() != 0) {
        glBegin(GL_LINES);
            for (unsigned int i = 0; i < Track->Courses[Index].Checkpoints.size(); i++) {
                glVertex3fv(&Track->Courses[Index].Checkpoints[i].Start[0]);
                glVertex3fv(&Track->Courses[Index].Checkpoints[i].End[0]);
            }
        glEnd();


        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
            glVertex3fv(&Track->Courses[Index].Checkpoints[Config->Editor.Segment].Start[0]);
            glVertex3fv(&Track->Courses[Index].Checkpoints[Config->Editor.Segment].End[0]);
        glEnd();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

        if ((unsigned int)Index < Track->Courses.size()) {
            for (unsigned int i = 0; i < Track->Courses[Index].Checkpoints.size(); i++) {

                glPushMatrix();
                glTranslatef(Track->Courses[Index].Checkpoints[i].Start.x, Track->Courses[Index].Checkpoints[i].Start.y, Track->Courses[Index].Checkpoints[i].Start.z);
                gluSphere(Object, 0.25f, 10, 10);
                glPopMatrix();

                glPushMatrix();
                glTranslatef(Track->Courses[Index].Checkpoints[i].End.x, Track->Courses[Index].Checkpoints[i].End.y, Track->Courses[Index].Checkpoints[i].End.z);
                gluSphere(Object, 0.25f, 10, 10);
                glPopMatrix();
            }
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

        if ((unsigned int)Config->Editor.Segment < Track->Courses[Config->Editor.Index].Checkpoints.size()) {
            glPushMatrix();
            glTranslatef(Track->Courses[Index].Checkpoints[Config->Editor.Segment].Start.x, Track->Courses[Index].Checkpoints[Config->Editor.Segment].Start.y, Track->Courses[Index].Checkpoints[Config->Editor.Segment].Start.z);
            gluSphere(Object, 0.25f, 10, 10);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(Track->Courses[Index].Checkpoints[Config->Editor.Segment].End.x, Track->Courses[Index].Checkpoints[Config->Editor.Segment].End.y, Track->Courses[Index].Checkpoints[Config->Editor.Segment].End.z);
            gluSphere(Object, 0.25f, 10, 10);
            glPopMatrix();
        }

    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    for (unsigned int i = 0; i < Track->Vehicles.size(); i++) {
        glPushMatrix();

            glTranslatef(Track->Vehicles[i].Position.x, Track->Vehicles[i].Position.y, Track->Vehicles[i].Position.z);
            glRotatef(270.0f - glm::degrees(Track->Vehicles[i].Rotation.x), 0.0f, 1.0f, 0.0f);
            glRotatef(glm::degrees(-Track->Vehicles[i].Rotation.z), 1.0f, 0.0f, 0.0f);
            glRotatef(glm::degrees(Track->Vehicles[i].Rotation.y), 0.0f, 0.0f, 1.0f);

            gluCylinder(Object, 0.0f, 0.25f, 0.5f, 10, 10);
            glTranslatef(0.0f, 0.0f, 0.5f);
            gluDisk(Object, 0.0f, 0.25f, 10, 10);

        glPopMatrix();
    }


    glPushMatrix();

        glTranslatef(Track->Vehicles[Config->Editor.Selected].Position.x, Track->Vehicles[Config->Editor.Selected].Position.y, Track->Vehicles[Config->Editor.Selected].Position.z);
        glRotatef(270.0f - glm::degrees(Track->Vehicles[Config->Editor.Selected].Rotation.x), 0.0f, 1.0f, 0.0f);
        glRotatef(glm::degrees(-Track->Vehicles[Config->Editor.Selected].Rotation.z), 1.0f, 0.0f, 0.0f);
        glRotatef(glm::degrees(Track->Vehicles[Config->Editor.Selected].Rotation.y), 0.0f, 0.0f, 1.0f);

        glDisable(GL_LIGHTING);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        gluCylinder(Object, 0.0f, 0.25f, 0.5f, 10, 10);
        glTranslatef(0.0f, 0.0f, 0.5f);
        gluDisk(Object, 0.0f, 0.25f, 10, 10);

    glPopMatrix();

    gluDeleteQuadric(Object);

}

void CRender::Box(glm::vec3 Position, glm::vec3 Dimensions)
{

    float Width = Dimensions[2] * -0.5f;
    float Height = Dimensions[0] * -0.5f;
    float Length = Dimensions[1] * -0.5f;

    glBegin(GL_TRIANGLE_STRIP);
        glNormal3f (-1,0,0);
        glVertex3f (-Width,-Height,-Length);
        glVertex3f (-Width,-Height,Length);
        glVertex3f (-Width,Height,-Length);
        glVertex3f (-Width,Height,Length);
        glNormal3f (0,1,0);
        glVertex3f (Width,Height,-Length);
        glVertex3f (Width,Height,Length);
        glNormal3f (1,0,0);
        glVertex3f (Width,-Height,-Length);
        glVertex3f (Width,-Height,Length);
        glNormal3f (0,-1,0);
        glVertex3f (-Width,-Height,-Length);
        glVertex3f (-Width,-Height,Length);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glNormal3f (0,0,1);
        glVertex3f (-Width,-Height,Length);
        glVertex3f (Width,-Height,Length);
        glVertex3f (Width,Height,Length);
        glVertex3f (-Width,Height,Length);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glNormal3f (0,0,-1);
        glVertex3f (-Width,-Height,-Length);
        glVertex3f (-Width,Height,-Length);
        glVertex3f (Width,Height,-Length);
        glVertex3f (Width,-Height,-Length);
    glEnd();

//    Weather->ApplyTint();

}

void CRender::Resize(void)
{
    if(glfwGetWindowAttrib(GLWindow, GLFW_ICONIFIED)) {
        return;
    }

    glfwGetFramebufferSize(GLWindow, &Width, &Height);
    glViewport(0, 0, Width, Height);

    OrthographicalView = glGenLists(1);
    glNewList(OrthographicalView, GL_COMPILE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    OrthoMatrix = glm::ortho(0.0f, float(Width), float(Height), 0.0f);
    glLoadMatrixf(glm::value_ptr(OrthoMatrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEndList();

    PerspectiveView = glGenLists(1);
    glNewList(PerspectiveView, GL_COMPILE);

    glViewport(0, 0, Width, Height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    Config->Graphics.AspectRatio = float(Width) / float(Height);
    ProjectionMatrix = glm::perspective(glm::radians(Config->Graphics.FieldOfVision), Config->Graphics.AspectRatio, Config->Graphics.NearClip, Config->Graphics.FarClip);
    glLoadMatrixf(glm::value_ptr(ProjectionMatrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEndList();

}
