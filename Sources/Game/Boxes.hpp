
#ifndef BOXES_HPP
#define BOXES_HPP

#include "Pods.hpp"

extern std::string ResourceDir;
extern std::string RootDir;

namespace Game
{

class CBoxes
{

private:

    int Dimensions;

    typedef struct {
        unsigned char Indices[6];
        unsigned char Rotations[6];
    } Information;

    std::vector<Information> TextureInfo;
    std::vector<unsigned char> LowerHeights;
    std::vector<unsigned char> UpperHeights;

    std::vector<std::vector<glm::vec2> >BoxesSelection;
    std::vector<std::vector<Information> >TexturesSelection;

    std::vector<std::vector<Information> >UndoTextureInfo;
    std::vector<std::vector<unsigned char> >UndoLowerHeights;
    std::vector<std::vector<unsigned char> >UndoUpperHeights;

    const int IndexAt(int X, int Z);

public:

    CBoxes(void);
    ~CBoxes(void);

    bool Load(Game::CPod &Pod, std::string LowerFilename, std::string UpperFilename, std::string TexturesInfo);
    bool Save(Game::CPod &Pod, std::string LowerFilename, std::string UpperFilename, std::string TexturesInfo);

    void CopyBoxes(glm::vec3 Center, bool Cut = false);
    void PasteBoxes(glm::vec3 Center);

    void AddBoxesUndo(void);
    void RemoveBoxesUndo(void);

    const int GetTextureIndex(int X, int Z, int Side);
    void SetTextureIndex(int X, int Z, int Side, int Index);

    void GetTextureRotation(int X, int Z, int Side, int &MirrorX, int &MirrorY, int &Rotation);
    void SetTextureRotation(int X, int Z, int Side, int MirrorX, int MirrorY, int Rotation);

    const float GetLowerHeight(int X, int Z);
    const float GetUpperHeight(int X, int Z);

    void SetLowerHeight(int X, int Z, float Height);
    void SetUpperHeight(int X, int Z, float Height);


};

}

#endif
