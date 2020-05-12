

#ifndef MODELS_HPP
#define MODELS_HPP

#include <algorithm>

#include "Pods.hpp"
#include "Textures.hpp"

extern std::string ResourceDir;
extern std::string RootDir;

namespace Game
{

class CModel
{

public:

    struct {
        int Scale;
        int Unknown1;
        int Unknown2;
        int NumVertices;
    } Header;

    struct AnimatedTexture {
        int NumTextures;
        int Delay;
        std::vector<std::string>Filenames;
    };

    struct VertInfo {
        int Index;
        glm::vec2 TexCoord;
    };

    struct Face {
        int Type;
        int TextureIndex;
        std::vector<VertInfo>Vertices;
    };

    std::vector<glm::i32vec3>Buffer;
    std::vector<glm::vec3> Vertices;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec3> FaceNormals;
    std::vector<std::string> Textures;
    std::vector<AnimatedTexture> AnimatedTextures;
    std::vector<glm::vec4> Colors;
    std::vector<Face> Faces;


    int NumFrames;
    std::vector<CModel> Frames;


    int ModelID;
    float BoundingSphere;
    glm::vec3 Min, Max;
    glm::vec3 Center;

    GLuint GLModel;
    Game::CTexturePool TexturePool;
    std::string Filename;

    bool Build(void);
    void CalculateNormals(void);

public:

    CModel(void);
    ~CModel(void);

    bool Load(CPod &Pod, std::string Filename, std::string Animated = "");
    void Destroy(void);

    float CalculateBounds(glm::vec3 &Min, glm::vec3 &Max);
    glm::vec3 GetBounds(glm::vec3 &Min, glm::vec3 &Max);

    glm::vec3 &GetCenter(void) {
        return Center;
    }
    const float &GetBoundingSphere(void)
    {
        return BoundingSphere;
    }
    const std::string &GetFilename(void)
    {
        return Filename;
    }
    char StrFilename(void)
    {
        return Filename.c_str()[0];
    }
    const GLuint &GetModel(void)
    {
        return GLModel;
    }

};

class CModelPool
{

public:

    CPod *Pod;
    std::vector<CModel> Models;

public:

    CModelPool(void);
    ~CModelPool(void);

    bool Create(CPod &Pod, std::vector<std::string> &Filenames);

    CModel &GetModel(int Index)
    {
        return Models[Index];
    }
    void AddModel(std::string Filename) {
        Models.resize(Models.size() + 1);
        Models[Models.size() - 1].Load(*Pod, Filename);
        Config->Editor.Index = Models.size() - 1;
    }

    void DeleteModel(int Index) {

        if (Models.size() > 0) {
            std::swap(Models[Index], Models[Models.size() - 1]);
            Models.resize(Models.size() - 1);
            Config->Editor.Index = Models.size() - 1;
        } else {
            Models.clear();
            Config->Editor.Index = 0;
        }

    }

    void DeleteAllModels(void) {
        Models.clear();
        Config->Editor.Index = 0;
    }

    const float &GetBoundingSphere(int Index)
    {
        return Models[Index].GetBoundingSphere();
    }
    const glm::vec3 GetBounds(int Index, glm::vec3 &Min, glm::vec3 &Max)
    {
        return Models[Index].GetBounds(Min, Max);
    }
//    const GLuint GetModel(int Index)
//    {
//        return Models[Index].GetModel();
//    }
    const std::string &GetFilename(int Index)
    {
        return Models[Index].GetFilename();
    }
    const int NumModels(void)
    {
        return Models.size();
    }

};

}

#endif

