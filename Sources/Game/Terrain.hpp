
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <vector>
#include <string>

#include "Pods.hpp"

namespace Game
{

class CTerrain
{

public:

    int Dimensions;

    typedef struct {
        unsigned char Index;
        unsigned char Rotation;
    } Information;

    std::vector<Information> TextureInfo;
    std::vector<glm::vec3> HeightMap;
    std::vector<glm::vec3> Normals;

    std::vector<std::vector<Information> > UndoTextures;
    std::vector<std::vector<glm::vec3> > UndoTerrain;

    std::vector<std::vector<float> >TerrainSelection;
    std::vector<std::vector<Information> >TextureSelection;

    std::vector<unsigned char> HeightmapData;

    const unsigned int IndexAt(int X, int Z);

    float RandomRange(float Min, float Max);


public:

    CTerrain(void);
    ~CTerrain(void);

    bool Load(CPod &Pod, std::string Filename, std::string TexturesInfo);
    bool Save(CPod &Pod, std::string Filename, std::string TexturesInfo);

    void Generate(int MinSize, int MaxSize, int MaxHeight, int NumHills, int Seed);

    void CalculateNormals(void);

    void CopyTerrain(glm::vec3 Center, bool Cut = false);
    void PasteTerrain(glm::vec3 Center);

    void CopyTextures(glm::vec3 Center, bool Cut = false);
    void PasteTextures(glm::vec3 Center);

    void AddTerrainUndo(void);
    void RemoveTerrainUndo(void);

    void AddTextureUndo(void);
    void RemoveTextureUndo(void);

    int GetDimensions(void)
    {
        return Dimensions;
    }

    const unsigned int GetTextureIndex(int X, int Z);
    void SetTextureIndex(int X, int Z, int Index);

    void GetTextureRotation(int X, int Z, int &MirrorX, int &MirrorY, int &Rotation);
    void SetTextureRotation(int X, int Z, int MirrorX, int MirrorY, int Rotation);

    glm::vec3 GetHeight(int X, int Z);
    void SetHeight(int X, int Z, float Height);

    const glm::vec3 &GetNormal(int X, int Z);

    const unsigned char &GetHeightData(void)
    {
        return HeightmapData[0];
    }

};

}

#endif
