
#define GLEW_STATIC

#include <GL/Glew.h>

#include "Config.hpp"
#include "Engine/Render.hpp"
#include "Engine/Timer.hpp"

bool Dirty = false;
bool Loop = true;
std::string ResourceDir;
std::string ConfigFile;
std::string InterfaceFile;
std::string RootDir;

CConfig *Config;
CLogger *Logger;

Game::CPod *Pod;
Game::CPod *ResPod;
Game::CTrack *Track;

Engine::CRender *Render;
GLFWwindow *GLWindow;

void ErrorCallback(int Error, const char* Description)
{
    Logger->Error("GLFW3 Error #" + std::to_string(Error) + ", " + std::string(Description));
}

void FocusCallback(GLFWwindow* Window, int Focus)
{
    if (Focus == GLFW_FALSE && Config->Graphics.Fullscreen) {
        glfwIconifyWindow(GLWindow);
    }
}

void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
{

    if (Key != GLFW_KEY_TAB) {
        ImGui_ImplGlfw_KeyCallback(Window, Key, Scancode, Action, Mods);
    }

    if(Key == GLFW_KEY_DELETE && Action == GLFW_PRESS) {

        if (Config->Editor.Mode == 2) {
            Render->GetTrack().BoxesObject()->AddBoxesUndo();
            for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
                for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
                    Render->GetTrack().BoxesObject()->SetUpperHeight(x + Render->GetEditBlock().x, z + Render->GetEditBlock().z, 0.0f);
                    Render->GetTrack().BoxesObject()->SetLowerHeight(x + Render->GetEditBlock().x, z + Render->GetEditBlock().z, 0.0f);
                }
            }
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 3) {
            Render->GetTrack().AddModelUndo();
            Render->GetTrack().DeleteModel(Config->Editor.Selected);
        } else if (Config->Editor.Mode == 4) {
            Render->GetTrack().Terrain->AddTextureUndo();
            Render->GetTrack().TexturePoolObject()->DeleteTexture(Config->Editor.Selected);
        } else if (Config->Editor.Mode == 5) {
            Render->GetTrack().AddCourseUndo();
            Render->GetTrack().DeleteCheckpoint(Config->Editor.Segment);
        }

    } else if(Key == GLFW_KEY_INSERT && Action == GLFW_PRESS) {

        if (Config->Editor.Mode == 2) {
            Render->GetTrack().BoxesObject()->AddBoxesUndo();
            Render->Editor(true, true);
        } else if (Config->Editor.Mode == 3) {
            Render->GetTrack().AddModelUndo();
            Render->GetTrack().AddModel(Render->GetEditBlock());
        } else if (Config->Editor.Mode == 5) {
            Render->GetTrack().AddCourseUndo();
            Render->GetTrack().InsertCheckpoint(Config->Editor.Segment,  glm::vec3(Render->GetEditBlock().x - 1.0f, Track->TerrainObject()->GetHeight(Render->GetEditBlock().x, Render->GetEditBlock().z).y, Render->GetEditBlock().z - 1.0f), glm::vec3(Render->GetEditBlock().x + 1.0f, Track->TerrainObject()->GetHeight(Render->GetEditBlock().x, Render->GetEditBlock().z).y, Render->GetEditBlock().z + 1.0f), "1", 60.0f);
        }

    } else if(Key == GLFW_KEY_TAB && Action == GLFW_PRESS) {

        Config->Editor.Index = 0;
        Config->Editor.Segment = 0;
        Config->Editor.Selected = 0;
        Config->Editor.Orientation = 0;
        Config->Editor.Rotation = 0;

        Config->Editor.Mode++;
        if(Config->Editor.Mode > 5) {
            Config->Editor.Mode = 0;
        }

        if (Config->Editor.Mode == 3) {
            Config->Editor.Selected = -1;
        }

//    } else if(Key == GLFW_KEY_N && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {
//
//        Track->New("Untitled");
//
//    } else if(Key == GLFW_KEY_O && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {
//
//        if (Config->Graphics.Fullscreen) {
//            glfwIconifyWindow(GLWindow);
//        }
//
//        std::string File = FileOpen(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//
//        if(File.size() != 0) {
//            strcpy(Config->Game.Track, File.substr(File.find_last_of("\\") + 1).c_str());
//            Config->General.Restart = true;
//            Loop = false;
//        }
//        glfwRestoreWindow(GLWindow);
//
//    } else if(Key == GLFW_KEY_S && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {
//
//        Track->Save(Config->Game.Track);
//
//    } else if(Key == GLFW_KEY_A && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {
//
//        if (Config->Graphics.Fullscreen) {
//            glfwIconifyWindow(GLWindow);
//        }
//
//        std::string File = FileSave(NULL, (char*)("World Situation Files\0*.sit\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
//
//        if(File.size() != 0) {
//            strcpy(Config->Game.Track, File.substr(File.find_last_of("\\") + 1).c_str());
//            Track->Save(Config->Game.Track);
//        }
//        glfwRestoreWindow(GLWindow);

    } else if(Key == GLFW_KEY_X && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        if (Config->Editor.Mode == 1) {
            Render->GetTrack().TerrainObject()->AddTerrainUndo();
            Render->GetTrack().TerrainObject()->CopyTerrain(Render->GetEditBlock(), true);
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 2) {
            Render->GetTrack().BoxesObject()->AddBoxesUndo();
            Render->GetTrack().BoxesObject()->CopyBoxes(Render->GetEditBlock(), true);
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 3) {
            Render->GetTrack().AddModelUndo();
            Render->GetTrack().CopyModel(true);
        } else if (Config->Editor.Mode == 4) {
            Render->GetTrack().TerrainObject()->AddTextureUndo();
            Render->GetTrack().TerrainObject()->CopyTextures(Render->GetEditBlock(), true);
            Render->SetUpdate();
        }

    } else if(Key == GLFW_KEY_C && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        if (Config->Editor.Mode == 1) {
            Render->GetTrack().TerrainObject()->CopyTerrain(Render->GetEditBlock());
        } else if (Config->Editor.Mode == 2) {
            Render->GetTrack().BoxesObject()->CopyBoxes(Render->GetEditBlock());
        } else if (Config->Editor.Mode == 3) {
            Render->GetTrack().CopyModel();
        } else if (Config->Editor.Mode == 4) {
            Render->GetTrack().TerrainObject()->CopyTextures(Render->GetEditBlock());
        }

    } else if(Key == GLFW_KEY_V && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        if (Config->Editor.Mode == 1) {
            Render->GetTrack().TerrainObject()->AddTerrainUndo();
            Render->GetTrack().TerrainObject()->PasteTerrain(Render->GetEditBlock());
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 2) {
            Render->GetTrack().BoxesObject()->AddBoxesUndo();
            Render->GetTrack().BoxesObject()->PasteBoxes(Render->GetEditBlock());
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 3) {
            Render->GetTrack().AddModelUndo();
            Render->GetTrack().PasteModel(Render->GetEditBlock());
        } else if (Config->Editor.Mode == 4) {
            Render->GetTrack().TerrainObject()->AddTextureUndo();
            Render->GetTrack().TerrainObject()->PasteTextures(Render->GetEditBlock());
            Render->SetUpdate();
        }

    } else if(Key == GLFW_KEY_S && Mods == GLFW_MOD_SHIFT && Action == GLFW_PRESS) {

        Config->General.Paused = true;
        Config->General.Properties = false;
        Config->General.Statistics = false;
        Config->General.About = false;
        Config->General.Statistics = !Config->General.Statistics;

    } else if(Key == GLFW_KEY_C && Mods == GLFW_MOD_SHIFT && Action == GLFW_PRESS) {

        Config->General.Configuration = !Config->General.Configuration;
        if (Config->General.Configuration) {
            Config->General.Properties = false;
            Config->General.Statistics = false;
            Config->General.About = false;
        }

    } else if(Key == GLFW_KEY_SPACE && Action == GLFW_PRESS) {


        if (Config->Editor.Mode == 1) {
            Config->Editor.Altitude = Render->GetTrack().TerrainObject()->GetHeight(Render->GetEditBlock().x, Render->GetEditBlock().z).y;
        } else if (Config->Editor.Mode == 2) {
            Config->Editor.Altitude = Render->GetTrack().BoxesObject()->GetUpperHeight(Render->GetEditBlock().x, Render->GetEditBlock().z);
            Config->Editor.Bottom = Render->GetTrack().BoxesObject()->GetLowerHeight(Render->GetEditBlock().x, Render->GetEditBlock().z);
        } else if (Config->Editor.Mode == 3) {
            if (Render->GetTrack().NumModels() != 0) {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                Render->Models(true);
                if (Config->Editor.Selected != -1) {
                    Config->Editor.Index = Render->GetTrack().Models[Config->Editor.Selected].Index;
                }
            }
        } else if (Config->Editor.Mode == 4) {
            if (Render->GetTrack().TexturePoolObject()->NumTextures() != 0) {
                Config->Editor.Index = Render->GetTrack().TerrainObject()->GetTextureIndex(Render->GetEditBlock().x, Render->GetEditBlock().z);
            }
        }

    } else if(Key == GLFW_KEY_E && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->General.Editors = !Config->General.Editors;

    } else if(Key == GLFW_KEY_D && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->General.Debug = !Config->General.Debug;

    } else if(Key == GLFW_KEY_F && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->General.FpsCounter = !Config->General.FpsCounter;

    } else if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS) {

        Config->General.Paused = !Config->General.Paused;
        if (!Config->General.Paused) {
            Config->General.Properties = false;
            Config->General.Configuration = false;
            Config->General.About = false;
            Config->General.Statistics = false;
        }

    } else if (Key == GLFW_KEY_H && Action == GLFW_PRESS) {

        Config->General.Paused = true;
        Config->General.Help = true;

    } else if(Key == GLFW_KEY_H && Action == GLFW_RELEASE) {

        Config->General.Paused = false;
        Config->General.Help = false;

    } else if(Key == GLFW_KEY_P && Mods == GLFW_MOD_SHIFT && Action == GLFW_PRESS) {

        Config->Editor.Preview = !Config->Editor.Preview;

    } else if(Key == GLFW_KEY_GRAVE_ACCENT && Action == GLFW_PRESS) {

        Config->General.Properties = !Config->General.Properties;
        Config->General.Paused = true;
        Config->General.Statistics = false;
        Config->General.About = false;

    } else if(Key == GLFW_KEY_Q && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        glfwSetWindowShouldClose(GLWindow, GLFW_TRUE);

    } else if (Key == GLFW_KEY_Z && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        if (Config->Editor.Mode == 1) {
            Render->GetTrack().TerrainObject()->RemoveTerrainUndo();
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 2) {
            Render->GetTrack().BoxesObject()->RemoveBoxesUndo();
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 3) {
            Render->GetTrack().RemoveModelUndo();
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 4) {
            Render->GetTrack().TerrainObject()->RemoveTextureUndo();
            Render->SetUpdate();
        } else if (Config->Editor.Mode == 5) {
            Render->GetTrack().RemoveCourseUndo();
        }

    } else if(Key == GLFW_KEY_C && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->Editor.Camera++;
        if(Config->Editor.Camera > 4) {
            Config->Editor.Camera = 0;
        }

    } else if(Key == GLFW_KEY_V && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->Graphics.VSync = !Config->Graphics.VSync;
        glfwSwapInterval(Config->Graphics.VSync);

    } else if(Key == GLFW_KEY_ENTER && Mods == GLFW_MOD_ALT && Action == GLFW_PRESS) {

        Config->Graphics.Fullscreen = !Config->Graphics.Fullscreen;

        int Count, PosX, PosY;
        GLFWmonitor** Monitors = glfwGetMonitors(&Count);

        if(Config->Graphics.Fullscreen) {
            glfwSetWindowMonitor(GLWindow, Monitors[int(Config->Graphics.Monitor - 1)], 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
        } else {

//            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
            glfwSetWindowMonitor(GLWindow, NULL, 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
            const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor - 1)]);
            glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor - 1)], &PosX, &PosY);
            glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));
        }
        glfwSwapInterval(Config->Graphics.VSync);
    }

    if(Config->Editor.Mode > 0 && Config->General.Editors) {

        if(Key == GLFW_KEY_F1 && Action == GLFW_PRESS) {

            Config->Render.Culled = !Config->Render.Culled;

        } else if(Key == GLFW_KEY_F2 && Action == GLFW_PRESS) {

            Config->Render.Lighted = !Config->Render.Lighted;

        } else if(Key == GLFW_KEY_F3 && Action == GLFW_PRESS) {

            Config->Render.Textured = !Config->Render.Textured;

        } else if(Key == GLFW_KEY_F4 && Action == GLFW_PRESS) {

            Config->Render.Wireframe = !Config->Render.Wireframe;

        } else if(Key == GLFW_KEY_F5 && Action == GLFW_PRESS) {

            Config->Render.Terrain = !Config->Render.Terrain;

        } else if(Key == GLFW_KEY_F6 && Action == GLFW_PRESS) {

            Config->Render.Boxes = !Config->Render.Boxes;

        } else if(Key == GLFW_KEY_F7 && Action == GLFW_PRESS) {

            Config->Render.Models = !Config->Render.Models;

        } else if(Key == GLFW_KEY_F8 && Action == GLFW_PRESS) {

            Config->Render.Water = !Config->Render.Water;

        } else if(Key == GLFW_KEY_F9 && Action == GLFW_PRESS) {

            Config->Render.Backdrop = !Config->Render.Backdrop;

        } else if(Key == GLFW_KEY_F10 && Action == GLFW_PRESS) {

            Config->Render.Overlay = !Config->Render.Overlay;

        }
    }
}
void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
{

    if (!Render->GetIO().WantCaptureMouse) {

        if (Config->Editor.Mode == 1) {

            if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS) {
                Render->GetTrack().TerrainObject()->AddTerrainUndo();
                Config->Editor.Mouse = GLFW_MOUSE_BUTTON_LEFT;
                Render->Editor(true, true);
            } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_PRESS) {
                if (Config->Editor.Paint == false) {
                    Render->GetTrack().TerrainObject()->AddTerrainUndo();
                }
                Config->Editor.Mouse = GLFW_MOUSE_BUTTON_RIGHT;
                Config->Editor.Paint = true;
                Render->Editor(true, true);
            } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_RELEASE) {
                Render->GetTrack().TerrainObject()->CalculateNormals();
                Config->Editor.Paint = false;
                Render->SetUpdate();
            }

        } else if (Config->Editor.Mode == 2) {

            if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS) {
                Render->GetTrack().BoxesObject()->AddBoxesUndo();
                Config->Editor.Mouse = GLFW_MOUSE_BUTTON_1;
                Render->Editor(true, true);
            }

            if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_PRESS) {
                if (Config->Editor.Paint == false) {
                    Render->GetTrack().BoxesObject()->AddBoxesUndo();
                }
                Config->Editor.Paint = true;
                Render->Editor(true, true);
            } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_RELEASE) {
                Config->Editor.Paint = false;
                Render->SetUpdate();
            }



        } else if (Config->Editor.Mode == 3) {

            if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS) {

                if (Render->GetTrack().ModelPoolObject()->NumModels() != 0) {

                    Config->Editor.Mouse = -3;

                    if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT)) {
                        Render->GetTrack().AddModelUndo();
                        Render->GetTrack().AddModel(glm::vec3(Render->GetEditBlock().x, Render->GetTrack().TerrainObject()->GetHeight(Render->GetEditBlock().x, Render->GetEditBlock().z).y, Render->GetEditBlock().z));
                    }

                    if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {
                        Render->GetTrack().AddModelUndo();
                    }
                }

            } else if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_RELEASE) {

                Config->Editor.Mouse = 0;

            } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_PRESS) {

                if (Config->Editor.Selected != -1) {
                    if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
                        Render->GetTrack().AddModelUndo();
                        Render->GetTrack().DeleteModel(Config->Editor.Selected);
                    }

                    if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT)) {
                        Render->GetTrack().AddModelUndo();
                        Render->GetTrack().SetModel(Config->Editor.Selected, Config->Editor.Index);
                    }
                }
            }

        } else if (Config->Editor.Mode == 4) {

            if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS) {
                Render->GetTrack().TerrainObject()->AddTextureUndo();
                Config->Editor.Mouse = GLFW_MOUSE_BUTTON_LEFT;
                Render->Editor(true, true);

            } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_PRESS) {

                if (Config->Editor.Paint == false) {
                    Render->GetTrack().TerrainObject()->AddTextureUndo();
                }
                Config->Editor.Mouse = GLFW_MOUSE_BUTTON_RIGHT;
                Config->Editor.Paint = true;
                Render->Editor(true, true);
            } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_RELEASE) {
                Config->Editor.Paint = false;
                Render->SetUpdate();
            }


        } else if (Config->Editor.Mode == 5) {

            if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS){
                Config->Editor.Mouse = -1;
                Render->GetTrack().AddCourseUndo();
                Render->Editor(true, true);
            } else if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_RELEASE) {
                Config->Editor.Mouse = 0;
            } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_PRESS){
                Config->Editor.Mouse = -2;
                Render->GetTrack().AddCourseUndo();
                Render->Editor(true, true);
            } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_RELEASE) {
                Config->Editor.Mouse = 0;
            }
        }

    }
//    if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_PRESS) {
//        Config->Editor.Mouse = -2;
//    } else if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS) {
//        Config->Editor.Mouse = -1;
//    }

}

void MouseScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{


    float Scaling = 1.0;

   if (Config->Editor.Mode == 1) {
        Scaling = 1.0f;
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT)) {
            Scaling *= 10.0f;
        }
        if (!glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {
            Config->Editor.Altitude += YOffset * Scaling;
        }
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {
            Config->Editor.Smooth += YOffset * Scaling;
        }
        Scaling = 5.0f;
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Scaling *= 0.5f;
            Config->Editor.GridWidth += YOffset * Scaling;
            Config->Editor.GridLength += YOffset * Scaling;
        }
    } else if (Config->Editor.Mode == 1 && glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT)) {
        Scaling = 5.0f;
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT)) {
            Scaling *= 0.5f;
        }
        Config->Editor.GridWidth += YOffset * Scaling;
        Config->Editor.GridLength += YOffset * Scaling;

    } else if (Config->Editor.Mode == 2) {

        if (!glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {

            Config->Editor.Index += YOffset;
            if (Config->Editor.Index >= Render->GetTrack().TexturePoolObject()->NumTextures()) {
                Config->Editor.Index = 0;
            } else if (Config->Editor.Index < 0) {
                Config->Editor.Index = Render->GetTrack().TexturePoolObject()->NumTextures() - 1;
            }
        }

        Scaling = 5.0f;
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)  && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Scaling *= 0.5f;
            Config->Editor.GridWidth += YOffset * Scaling;
            Config->Editor.GridLength += YOffset * Scaling;
        }

        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT) ) {
            Config->Editor.Selected += YOffset;
            if (Config->Editor.Selected > 5) {
                Config->Editor.Selected = 0;
            } else if (Config->Editor.Selected < 0) {
                Config->Editor.Selected = 5;
            }
        }

        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {

            Config->Editor.Rotation += YOffset;
            if (Config->Editor.Rotation > 15) {
                Config->Editor.Rotation = 0;
            } else if (Config->Editor.Rotation < 0) {
                Config->Editor.Rotation = 15;
            }
            Config->Editor.Mouse = -1;
            Render->Editor(true, true);
        }

        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Config->Editor.Altitude += YOffset;
            if (Config->Editor.Altitude < Config->Editor.Bottom) {
                Config->Editor.Altitude = Config->Editor.Bottom;
            }
        }
        if (!glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Config->Editor.Bottom += YOffset;
        }

    } else if (Config->Editor.Mode == 3) {

        if (!glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {
            Config->Editor.Index += YOffset;
            if (Config->Editor.Index >= Render->GetTrack().ModelPoolObject()->NumModels() - 1) {
                Config->Editor.Index = 0;
            } else if (Config->Editor.Index <= 0) {
                Config->Editor.Index = Render->GetTrack().ModelPoolObject()->NumModels() - 1;
            }
        }

        Scaling = 0.01f;
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT)) {
            Scaling *= 10.0f;
        }

        if (Config->Editor.Selected != -1) {
            if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {
                Render->GetTrack().AddModelUndo();
                Render->GetTrack().SetModel(Config->Editor.Selected, Config->Editor.Index);
            } else if (glfwGetKey(GLWindow, GLFW_KEY_Y) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
                Render->GetTrack().Models[Config->Editor.Selected].Rotation.x += YOffset * Scaling;
            } else if (glfwGetKey(GLWindow, GLFW_KEY_X) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
                Render->GetTrack().Models[Config->Editor.Selected].Rotation.z += YOffset * Scaling;
            } else if (glfwGetKey(GLWindow, GLFW_KEY_Z) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
                Render->GetTrack().Models[Config->Editor.Selected].Rotation.y += YOffset * Scaling;
            } else if (glfwGetKey(GLWindow, GLFW_KEY_Y) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
                Render->GetTrack().Models[Config->Editor.Selected].Position.y += YOffset * Scaling;
            } else if (glfwGetKey(GLWindow, GLFW_KEY_X) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
                Render->GetTrack().Models[Config->Editor.Selected].Position.x += YOffset * Scaling;
            } else if (glfwGetKey(GLWindow, GLFW_KEY_Z) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
                Render->GetTrack().Models[Config->Editor.Selected].Position.z += YOffset * Scaling;
            }


        }
    } else if (Config->Editor.Mode == 4) {

        Scaling = 5.0f;
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Scaling *= 0.5f;
            Config->Editor.GridWidth += YOffset * Scaling;
            Config->Editor.GridLength += YOffset * Scaling;
        }

        if (!glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {

            Config->Editor.Index += YOffset;
            if (Config->Editor.Index >= Render->GetTrack().TexturePoolObject()->NumTextures()) {
                Config->Editor.Index = 0;
            } else if (Config->Editor.Index < 0) {
                Config->Editor.Index = Render->GetTrack().TexturePoolObject()->NumTextures() - 1;
            }
        } else if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {

            Config->Editor.Index += YOffset;
            if (Config->Editor.Index >= Render->GetTrack().TexturePoolObject()->NumTextures()) {
                Config->Editor.Index = 0;
            } else if (Config->Editor.Index < 0) {
                Config->Editor.Index = Render->GetTrack().TexturePoolObject()->NumTextures() - 1;
            }
            Config->Editor.Mouse = -1;
            Render->Editor(true, true);
        }
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {

            Config->Editor.Rotation += YOffset;
            if (Config->Editor.Rotation > 15) {
                Config->Editor.Rotation = 0;
            } else if (Config->Editor.Rotation < 0) {
                Config->Editor.Rotation = 15;
            }
            Config->Editor.Mouse = -2;
            Render->Editor(true, true);
        }

    } else if (Config->Editor.Mode == 5) {

        Scaling = 0.01f;
        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT)) {
            Scaling *= 10.0f;
        }

        if (glfwGetKey(GLWindow, GLFW_KEY_Y) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Render->GetTrack().Vehicles[Config->Editor.Selected].Rotation.x += YOffset * Scaling;
        } else if (glfwGetKey(GLWindow, GLFW_KEY_X) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Render->GetTrack().Vehicles[Config->Editor.Selected].Rotation.z += YOffset * Scaling;
        } else if (glfwGetKey(GLWindow, GLFW_KEY_Z) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Render->GetTrack().Vehicles[Config->Editor.Selected].Rotation.y += YOffset * Scaling;
        }

        if (glfwGetKey(GLWindow, GLFW_KEY_X) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Render->GetTrack().Vehicles[Config->Editor.Selected].Position.x += YOffset * Scaling;
        } else if (glfwGetKey(GLWindow, GLFW_KEY_Y) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Render->GetTrack().Vehicles[Config->Editor.Selected].Position.y += YOffset * Scaling;
        } else if (glfwGetKey(GLWindow, GLFW_KEY_Z) && glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {
            Render->GetTrack().Vehicles[Config->Editor.Selected].Position.z += YOffset * Scaling;
        }

        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_ALT)) {

            Config->Editor.Selected += YOffset;
            if (Config->Editor.Selected > 7) {
                Config->Editor.Selected = 0;
            } else if (Config->Editor.Selected < 0) {
                Config->Editor.Selected = 7;
            }
        }

        if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL)) {

            Config->Editor.Segment += YOffset;
            if ((unsigned int)Config->Editor.Segment > Render->GetTrack().Courses[Config->Editor.Index].Checkpoints.size() - 1) {
                Config->Editor.Segment = 0;
            } else if (Config->Editor.Segment < 0) {
                Config->Editor.Segment = Render->GetTrack().Courses[Config->Editor.Index].Checkpoints.size() - 1;
            }
        }

    }
    Render->Editor(false, true);

}

void ResizeCallback(GLFWwindow* Window, int Width, int Height)
{
    Render->Resize();
}

bool Create(std::string ResDir, std::string CfgFile, std::string AppRoot)
{

    RootDir = AppRoot.substr(0, AppRoot.find_last_of("\\") + 1);
    ResourceDir = ResDir;
    InterfaceFile = RootDir + ResourceDir + "Interface.ini";
    ConfigFile = RootDir + ResourceDir + CfgFile;

    Logger = new CLogger(RootDir + ResourceDir + "Traxx2.log");
    Config = new CConfig(ConfigFile);

    Logger->Text(GetInfo());
    Logger->Text("Resource Directory - " + ResDir);
    Logger->Text("Configuration Filename - " + CfgFile);
    Logger->Text("Interface Filename - Interface.ini");
    Logger->Text("Root Directory - " + RootDir);

    CreateDirectory(std::string(RootDir + ResDir + "UI\\").c_str(), NULL);
    CreateDirectory(std::string(RootDir + ResDir + "ART\\").c_str(), NULL);
    CreateDirectory(std::string(RootDir + ResDir + "DATA\\").c_str(), NULL);
    CreateDirectory(std::string(RootDir + ResDir + "FOG\\").c_str(), NULL);
    CreateDirectory(std::string(RootDir + ResDir + "LEVELS\\").c_str(), NULL);
    CreateDirectory(std::string(RootDir + ResDir + "MODELS\\").c_str(), NULL);
    CreateDirectory(std::string(RootDir + ResDir + "WORLD\\").c_str(), NULL);
    CreateDirectory(std::string(RootDir + ResDir + "MUSIC\\").c_str(), NULL);


    glfwSetErrorCallback(ErrorCallback);

    if(glfwInit()) {
        Logger->Text("Initialized GLFW3 Library");
    } else {
        Logger->Error("Error Initializing GLFW3 Library");
    }

    glfwWindowHint(GLFW_SAMPLES, Config->Graphics.Antialiasing);
    glfwWindowHint(GLFW_DOUBLEBUFFER, Config->Graphics.DoubleBuffer);
    glfwWindowHint(GLFW_REFRESH_RATE, Config->Graphics.RefreshRate);

    int Count, PosX, PosY;
    GLFWmonitor** Monitors = glfwGetMonitors(&Count);

    if(Config->Graphics.Monitor > Count) {
        Logger->Error("Invalid Monitor Selection, Defaulting To Primary...", true, true, false);
        Config->Graphics.Monitor = 1;
    }

    if(Config->Graphics.Fullscreen) {
        GLWindow = glfwCreateWindow(Config->Graphics.Width, Config->Graphics.Height, GetInfo().c_str(), Monitors[int(Config->Graphics.Monitor - 1)], NULL);
    } else {
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        GLWindow = glfwCreateWindow(Config->Graphics.Width, Config->Graphics.Height, GetInfo().c_str(), NULL, NULL);
        const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor - 1)]);
        glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor - 1)], &PosX, &PosY);
        glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));
    }

    if(GLWindow) {
        Logger->Text("Created GLFW3 Window");
    } else {
        Logger->Error("Error Creating GLFW3 Window");
    }

//    GLFWimage Icon;
//    std::string IconFile = RootDir + ResDir + "//Game//Icon.png";
//    Icon.pixels = SOIL_load_image(IconFile.c_str(), &Icon.width, &Icon.height, 0, SOIL_LOAD_RGBA);
//    glfwSetWindowIcon(GLWindow, 1, &Icon);
//    SOIL_free_image_data(Icon.pixels);

    glfwMakeContextCurrent(GLWindow);
    glfwSwapInterval(Config->Graphics.VSync);

    if(Config->Graphics.Multisample) {
        glEnable(GL_MULTISAMPLE);
    }

    if(GLEW_OK == glewInit()) {
        Logger->Text("Initialized GLEW");
    } else {
        Logger->Error("Error Initializing GLEW");
    }

    Logger->Text("GLEW - v" + std::string((char*)glewGetString(GLEW_VERSION)));
    Logger->Text("GLFW3 - v" + std::string((char*)glfwGetVersionString()));
    Logger->Text("OpenGL Renderer - " + std::string((char*)glGetString(GL_RENDERER)));
    Logger->Text("OpenGL Vendor - " + std::string((char*)glGetString(GL_VENDOR)));
    Logger->Text("OpenGL Version - v" + std::string((char*)glGetString(GL_VERSION)));

    Pod = new Game::CPod();
    Pod->Load("Resources.pod", false, true);
    delete Pod;

//    Pod = new Game::CPod();
//    Pod->Load("Untitled.pod", false, true);
//    delete Pod;

    if (Config->General.LoadStartup) {
        std::string File = FileOpen(NULL, (char*)("World Situation Files\0*.sit\0MTM Pod Files\0*.pod\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "World\\")[0], &std::string(".SIT")[0]);
        if(File.size() != 0) {
            if (StrCompare(File.substr(File.size() - 4, File.size()), ".SIT")) {
                strcpy(Config->Game.Track, File.substr(File.find_last_of("\\") + 1).c_str());
                Pod = new Game::CPod();
                Pod->Load("Resources.pod");
            } else if (StrCompare(File.substr(File.size() - 4, File.size()), ".POD")) {
                Pod = new Game::CPod();
                File = File.substr(File.find_last_of("\\") + 1).c_str();
                bool Extract = false;
                if(MessageBox(NULL, "It Is Strongly Recommended You Extract The Files First, Would You Like To Extract Them Now?", "Extract", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                    Extract = true;
                }
                if(Pod->Load(File, false, Extract)) {
                    Logger->Text("Loaded Pod Information - " + File);
                } else {
                    Logger->Error("Error Loading Pod Information - " + File);
                }
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
            }
        } else {
            strcpy(Config->Game.Track, UpperStr("Untitled.sit"));
            Pod = new Game::CPod();
            if(Pod->Load("Untitled.pod")) {
                Logger->Text("Loaded Pod Information - Untitled.pod");
            } else {
                Logger->Error("Error Loading Pod Information - Untitled.pod");
            }
        }
    } else {
        Pod = new Game::CPod();
        if(Pod->Load("Untitled.pod")) {
            Logger->Text("Loaded Pod Information - Untitled.pod");
        } else {
            Logger->Error("Error Loading Pod Information - Untitled.pod");
        }
        strcpy(Config->Game.Track, UpperStr("Untitled.sit"));
    }

//    Pod = new Game::CPod();
//    Pod->Load("Crazy98.pod");
//    strcpy(Config->Game.Track, UpperStr("Crazy98.sit"));

    Track = new Game::CTrack();
    if(Track->Load(*Pod, UpperStr(Config->Game.Track))) {
        Logger->Text("Loaded Track - " + std::string(Config->Game.Track));
        glfwSetWindowTitle(GLWindow, std::string(GetInfo() + " - " + Config->Game.Track).c_str());
    } else {
        Logger->Error("Error Loading  Track - " + std::string(Config->Game.Track) + ", Aborting...");
    }

    Render = new Engine::CRender();
    if(Render->Create(*Pod, *Track)) {
        Logger->Text("Created Rendering Engine");
    } else {
        Logger->Error("Error Creating Rendering Engine");
    }

    ResPod = new Game::CPod();
    if(ResPod->Load("Resources.pod")) {
        Logger->Text("Loaded Pod Resources Information - Resources.pod");
    } else {
        Logger->Error("Error Loading Pod Resources Information - Resources.pod");
    }

    Render->GetWaterTexture().LoadRaw(*ResPod, UpperStr("Rippl100.raw"));
    Render->GetWeather().Create(*ResPod);

    glfwSetWindowFocusCallback(GLWindow,(GLFWwindowfocusfun)FocusCallback);
    glfwSetKeyCallback(GLWindow, (GLFWkeyfun)KeyCallback);
    glfwSetWindowSizeCallback(GLWindow, (GLFWwindowsizefun)ResizeCallback);
    glfwSetScrollCallback(GLWindow, (GLFWscrollfun)MouseScrollCallback);
    glfwSetMouseButtonCallback(GLWindow, (GLFWmousebuttonfun)MouseButtonCallback);

    glfwSetCursor(GLWindow, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));

    Logger->Text("Traxx2 Created");

    return true;

}

bool Destroy(void)
{

    strcpy(Config->Game.Track, UpperStr("Untitled.sit"));

    if(Render) {
        delete Render;
        Render = NULL;
        Logger->Text("Destroyed Rendering Object");
    }

    if(Config) {
        Config->Save(ConfigFile);
        delete Config;
        Config = NULL;
        Logger->Text("Destroyed Configuration Object");
    }

    if(GLWindow) {
        glfwTerminate();
        Logger->Text("Destroyed OpenGL Framework 3 Window");
    }

    Logger->Text("Traxx2 Destroyed");

    if(Logger) {
        delete Logger;
        Logger = NULL;
    }

    return true;

}

int main(int argc, char* argv[])
{
    while(Loop) {

        Loop = Create("Resources\\", "Traxx2.cfg", argv[0]);
//        Loop = Create("..\\Resources\\", "Traxx2.cfg", argv[0]);

        do {

            glfwPollEvents();
            Render->Scene();

        } while(Loop && !glfwWindowShouldClose(GLWindow));

        Destroy();
        Loop = !Loop;

    };

    return EXIT_SUCCESS;

}
