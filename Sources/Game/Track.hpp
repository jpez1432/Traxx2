
#ifndef TRACK_HPP
#define TRACK_HPP

#include "../Common.hpp"

#include "Pods.hpp"
#include "Terrain.hpp"
#include "Textures.hpp"
#include "Boxes.hpp"
#include "Models.hpp"
#include "Weather.hpp"

namespace Game
{

class CTrack
{

public:

    bool MTM2Track;

    typedef struct {
        std::string Filename;
        glm::vec3 Position;
        glm::vec3 Rotation;
    } Vehicle;

    typedef struct {
        std::string Filename;
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Dimensions;
        float Mass;
        std::string TypeFlags;
        int Priority;
        glm::vec3 Min;
        glm::vec3 Max;
        float Bounds;
        GLuint GLModel;
        int Index;
    } Model;

    typedef struct {
        std::string Type;
        glm::vec3 Start;
        glm::vec3 End;
        float Speed;
    } Checkpoint;

    typedef struct {
        std::vector<Checkpoint> Checkpoints;
    } Course;

    std::string Level;
    std::string Name;
    std::string Locale;
    std::string Logo;
    std::string Map;
    std::string Description;
    std::string Type;
    std::string WeatherTexture;
    std::string WeatherPalette;
    std::string TextFile;
    std::string Backdrop;
    std::string HeightMap;
    std::string TextureInfo;
    std::string Palette;
    std::string TextureList;
    std::string LightMap;
    std::string Animation;
    std::string LowerBoxes;
    std::string UpperBoxes;
    std::string TextureBoxes;
    std::string Zero;
    std::string TempLevel;
    std::string Comment;
    std::string FogMap;

    std::vector<CModel> Backdrops;
    std::vector<Vehicle> Vehicles;
    std::vector<Model> Models;
    std::vector<Course> Courses;
    std::vector<std::vector<Model> > UndoModels;
    std::vector<std::vector<Course> > UndoCourses;
    Model ModelSelection;

    std::string Filename;

    glm::vec3 SunPosition;
    int BackdropCount;
    int NumCourses;
    float WaterHeight;
    long WeatherMask;
    std::string AmbientSound;
    unsigned char TrackPalette[768];
    unsigned char Fog[32768];

    Game::CTexturePool *TexturePool;
    Game::CModelPool *ModelPool;
    Game::CTerrain *Terrain;
    Game::CBoxes *Boxes;

    Game::CPod *Pod;

    std::string FindMarker(std::ifstream &Stream, std::string Marker1, std::string Marker2 = "");
    bool NewFile(std::string Filename, unsigned char Fill, long Size);

public:

    CTrack(void);
    ~CTrack(void);

    bool Load(CPod &Pod, std::string Filename);
    bool Save(std::string Filename);
    void Destroy(void);

    void LoadPalette(std::string Filename);
    bool Export(std::string Filename);
    void SetLevel(void);

    void AddModelUndo(void);
    void RemoveModelUndo(void);

    void AddCourseUndo(void);
    void RemoveCourseUndo(void);

    CTexturePool *TexturePoolObject(void)
    {
        return TexturePool;
    }

    void DrawModel(int Index)
    {
        glCallList(Models[Index].GLModel);
    }

    const int NumModels(void)
    {
        return Models.size();
    }

    CModelPool *ModelPoolObject(void)
    {
        return ModelPool;
    }
    CModel &GetModel(int Index)
    {
        return ModelPool->GetModel(Models[Index].Index);
    }

    void SetModel(int Old, int New)
    {
        if (Models.size() > 0) {
            Models[Old].GLModel = ModelPool->GetModel(New).GLModel;
            Models[Old].Filename = ModelPool->GetModel(New).Filename;
        }
    }

    void InsertCheckpoint(int Current, glm::vec3 Start, glm::vec3 End, std::string Type, float Speed = 60.0f)
    {
        Course Course;
        Checkpoint Checkpoint;
        Checkpoint.Start = Start;
        Checkpoint.End = End;
        Checkpoint.Type = Type;
        Checkpoint.Speed = Speed;

        for (int i = 0; i < Current; i++) {
            Course.Checkpoints.push_back(Courses[Config->Editor.Index].Checkpoints[i]);
        }
        Course.Checkpoints.push_back(Checkpoint);
        for (unsigned int i = Current; i < Courses[Config->Editor.Index].Checkpoints.size(); i++) {
            Course.Checkpoints.push_back(Courses[Config->Editor.Index].Checkpoints[i]);
        }
        Courses[Config->Editor.Index] = Course;
    }

    void DeleteCheckpoint(int Index)
    {
        if (Courses[Config->Editor.Index].Checkpoints.size() > 0) {
            std::swap(Courses[Config->Editor.Index].Checkpoints[Index], Courses[Config->Editor.Index].Checkpoints[Courses[Config->Editor.Index].Checkpoints.size() - 1]);
            Courses[Config->Editor.Index].Checkpoints.resize(Courses[Config->Editor.Index].Checkpoints.size() - 1);
            Config->Editor.Selected = Courses[Config->Editor.Index].Checkpoints.size() - 1;
        } else {
            Courses[Config->Editor.Index].Checkpoints.clear();
            Config->Editor.Selected = 0;
        }
    }

    void CopyModel(bool Cut = false) {
        if (Config->Editor.Selected != -1) {
            ModelSelection = Models[Config->Editor.Selected];
            if (Cut) {
                DeleteModel(Config->Editor.Selected);
            }
        }
    }

    void PasteModel(glm::vec3 Position) {
        ModelSelection.Position = Position + ModelSelection.Max.y;
        Models.push_back(ModelSelection);
    }

    void AddModel(glm::vec3 Position, bool Box = false) {

        Model Model;

        if (Box) {
            Position.y += 1.0f;
            Model.Filename = "Box";
            Model.Position = Position;
            Model.Rotation = glm::vec3(0);
            Model.GLModel = -1;
            Model.Priority = 0;
            Model.Bounds = 1.0f;
            Model.Dimensions = glm::vec3(1.0f);
            Model.Index = 0;
            Model.Mass = 100.0f;
            Model.Max = glm::vec3(0.5f);
            Model.Min = glm::vec3(-0.5f);
            Models.push_back(Model);
        } else {
            if (ModelPool->NumModels() != 0) {
                Position.y += ModelPool->GetModel(Config->Editor.Index).Max.y;
                Model.Bounds = ModelPool->GetModel(Config->Editor.Index).BoundingSphere;
                Model.Dimensions = ModelPool->GetModel(Config->Editor.Index).Max - ModelPool->GetModel(Config->Editor.Index).Min;
                Model.Filename = ModelPool->GetModel(Config->Editor.Index).Filename;
                Model.GLModel = ModelPool->GetModel(Config->Editor.Index).GLModel;
                Model.Index = Config->Editor.Index;
                Model.Mass = 100.0f;
                Model.Max = ModelPool->GetModel(Config->Editor.Index).Max;
                Model.Min = ModelPool->GetModel(Config->Editor.Index).Min;
                Model.Position = Position;
                Model.Priority = 0;
                Model.Rotation = glm::vec3(0);
                Models.push_back(Model);
            }
        }
        Config->Editor.Selected = Models.size() - 1;
    }

    void DeleteModel(int Index) {

        if (Models.size() > 0) {
            std::swap(Models[Index], Models[Models.size() - 1]);
            Models.resize(Models.size() - 1);
            Config->Editor.Selected = Models.size() - 1;
        } else {
            Models.clear();
            Config->Editor.Selected = -1;
        }
    }

    void DeleteAllModels(void) {
        Models.clear();
        Config->Editor.Selected = -1;
    }

    char &ModelFilename(int Index)
    {
        return Models[Index].Filename[0];
    }

    const glm::vec3 &GetModelPosition(int Index)
    {
        return Models[Index].Position;
    }
    const glm::vec3 &GetModelRotation(int Index)
    {
        return Models[Index].Rotation;
    }
    glm::vec3 &GetModelDimensions(int Index)
    {
        return Models[Index].Dimensions;
    }
    const std::string &GetModelTypeFlags(int Index)
    {
        return Models[Index].TypeFlags;
    }
    const int &GetModelPriority(int Index)
    {
        return Models[Index].Priority;
    }
    const float GetModelBounds(int Index, glm::vec3 &Min, glm::vec3 &Max)
    {
        Min = Models[Index].Min;
        Max = Models[Index].Max;
        return Models[Index].Bounds;
    }

    int Dimensions(void)
    {
        return Terrain->GetDimensions();
    }
    CTerrain *TerrainObject(void)
    {
        return Terrain;
    }

    CBoxes *BoxesObject(void)
    {
        return Boxes;
    }
    void DrawBackground(int Index)
    {
        glCallList(Backdrops[Index].GetModel());
    }
    int GetBackdropCount(void)
    {
        return Backdrops.size();
    }

    const glm::vec4 GetSunPosition(void)
    {
        return glm::vec4(SunPosition, 1.0f);
    }

    const float &GetWaterHeight(void)
    {
        return WaterHeight;
    }

    const glm::vec3 &GetStartPosition(int Index)
    {
        return Vehicles[Index].Position;
    }
    const glm::vec3 &GetStartRotation(int Index)
    {
        return Vehicles[Index].Rotation;
    }

};

}

#endif
