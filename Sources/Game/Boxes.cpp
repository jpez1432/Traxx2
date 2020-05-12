
#include "Boxes.hpp"

using namespace Game;

CBoxes::CBoxes(void) : Dimensions(0)
{


}

CBoxes::~CBoxes(void)
{

    if(TextureInfo.empty() == false) {
        TextureInfo.clear();
        Logger->Text("Released Box Texture Information Memory");
    }

    if(LowerHeights.empty() == false) {
        LowerHeights.clear();
        Logger->Text("Released Box Lower Height Memory");
    }

    if(UpperHeights.empty() == false) {
        UpperHeights.clear();
        Logger->Text("Released Box Upper Height Memory");
    }

}

bool CBoxes::Load(CPod &Pod, std::string LowerFilename, std::string UpperFilename, std::string TexturesInfo)
{

    int Offset;

    Pod.FindFile(LowerFilename, Dimensions, Offset);

    LowerFilename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(LowerFilename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Dimensions = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Dimensions);
    LowerHeights.resize(Dimensions * Dimensions);

    StreamIn.read((char*)&LowerHeights[0], LowerHeights.size());
    StreamIn.close();

    Pod.FindFile(UpperFilename, Dimensions, Offset);

    UpperFilename.insert(0, RootDir + ResourceDir);
    StreamIn.open(UpperFilename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Dimensions = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Dimensions);
    UpperHeights.resize(Dimensions * Dimensions);

    StreamIn.read((char*)&UpperHeights[0], UpperHeights.size());
    StreamIn.close();


    Pod.FindFile(TexturesInfo, Dimensions, Offset);

    TexturesInfo.insert(0, RootDir + ResourceDir);
    StreamIn.open(TexturesInfo.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Dimensions = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Dimensions / 12);
    TextureInfo.resize(Dimensions * Dimensions);

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {
            for(int i = 0; i < 6; i++) {

                TextureInfo[IndexAt(x, z)].Indices[i] = StreamIn.get();
                TextureInfo[IndexAt(x, z)].Rotations[i] = StreamIn.get();

            }
        }
    }

    StreamIn.close();

    return true;

}

bool CBoxes::Save(CPod &Pod, std::string LowerFilename, std::string UpperFilename, std::string TexturesInfo)
{

//    int Offset;


//    Pod.FindFile(LowerFilename, Dimensions, Offset);
    LowerFilename.insert(0, "Data\\");
    LowerFilename.insert(0, RootDir + ResourceDir);
    std::ofstream StreamOut(LowerFilename.c_str(), std::ios::binary | std::ios::out);

    if(StreamOut.fail()) {
        return false;
    }

//    if(Offset == 0) {
//
//        StreamIn.seekg(0, std::ios::end);
//        Dimensions = StreamIn.tellg();
//        StreamIn.seekg(0, std::ios::beg);
//
//    } else {
//
//        StreamIn.seekg(Offset, std::ios::beg);
//
//    }

//    Dimensions = sqrt(Dimensions);
//    LowerHeights.resize(Dimensions * Dimensions);

    StreamOut.write((char*)&LowerHeights[0], LowerHeights.size());
    StreamOut.close();

//    Pod.FindFile(UpperFilename, Dimensions, Offset);
    UpperFilename.insert(0, "Data\\");
    UpperFilename.insert(0, RootDir + ResourceDir);
    StreamOut.open(UpperFilename.c_str(), std::ios::binary | std::ios::out);

    if(StreamOut.fail()) {
        return false;
    }

//    if(Offset == 0) {
//
//        StreamIn.seekg(0, std::ios::end);
//        Dimensions = StreamIn.tellg();
//        StreamIn.seekg(0, std::ios::beg);
//
//    } else {
//
//        StreamIn.seekg(Offset, std::ios::beg);
//
//    }

//    Dimensions = sqrt(Dimensions);
//    UpperHeights.resize(Dimensions * Dimensions);

    StreamOut.write((char*)&UpperHeights[0], UpperHeights.size());
    StreamOut.close();

//    Pod.FindFile(TexturesInfo, Dimensions, Offset);
    TexturesInfo.insert(0, "Data\\");
    TexturesInfo.insert(0, RootDir + ResourceDir);
    StreamOut.open(TexturesInfo.c_str(), std::ios::binary | std::ios::out);

    if(StreamOut.fail()) {
        return false;
    }

//    if(Offset == 0) {
//
//        StreamIn.seekg(0, std::ios::end);
//        Dimensions = StreamIn.tellg();
//        StreamIn.seekg(0, std::ios::beg);
//
//    } else {
//
//        StreamIn.seekg(Offset, std::ios::beg);
//
//    }

//    Dimensions = sqrt(Dimensions / 12);
//    TextureInfo.resize(Dimensions * Dimensions);

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {
            for(int i = 0; i < 6; i++) {

                StreamOut.put(TextureInfo[IndexAt(x, z)].Indices[i]);
                StreamOut.put(TextureInfo[IndexAt(x, z)].Rotations[i]);

            }
        }
    }

    StreamOut.close();

    return true;

}

void CBoxes::CopyBoxes(glm::vec3 Center, bool Cut)
{
    BoxesSelection.resize(Config->Editor.GridWidth + 1);
    TexturesSelection.resize(Config->Editor.GridWidth + 1);
    for (int x = 0; x < Config->Editor.GridWidth + 1; x++) {
        BoxesSelection[x].resize(Config->Editor.GridLength + 1);
        TexturesSelection[x].resize(Config->Editor.GridLength + 1);
        for (int z = 0; z < Config->Editor.GridLength + 1; z++) {
            BoxesSelection[x][z] = glm::vec2(0.0f);
        }
    }

    for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
        for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
            BoxesSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2].x = LowerHeights[IndexAt(x + Center.x, z + Center.z)];
            BoxesSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2].y = UpperHeights[IndexAt(x + Center.x, z + Center.z)];
            TexturesSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] = TextureInfo[IndexAt(x + Center.x, z + Center.z)];
        }
    }
    if (Cut) {
        for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
            for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
                LowerHeights[IndexAt(x + Center.x, z + Center.z)] = 0;
                UpperHeights[IndexAt(x + Center.x, z + Center.z)] = 0;
            }
        }
    }

}

void CBoxes::PasteBoxes(glm::vec3 Center)
{
    for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
        for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
            if (BoxesSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2].x != BoxesSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2].y) {
                LowerHeights[IndexAt(x + Center.x, z + Center.z)] = BoxesSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2].x;
                UpperHeights[IndexAt(x + Center.x, z + Center.z)] = BoxesSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2].y;
                TextureInfo[IndexAt(x + Center.x, z + Center.z)] = TexturesSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2];
            }
        }
    }
}

void CBoxes::AddBoxesUndo(void)
{
    UndoTextureInfo.push_back(TextureInfo);
    UndoLowerHeights.push_back(LowerHeights);
    UndoUpperHeights.push_back(UpperHeights);

    Dirty = true;
}

void CBoxes::RemoveBoxesUndo(void)
{
    if (UndoTextureInfo.size() == 0) {
        return;
    }

    TextureInfo = UndoTextureInfo[UndoTextureInfo.size() - 1];
    UndoTextureInfo.resize(UndoTextureInfo.size() - 1);

    LowerHeights = UndoLowerHeights[UndoLowerHeights.size() - 1];
    UndoLowerHeights.resize(UndoLowerHeights.size() - 1);

    UpperHeights = UndoUpperHeights[UndoUpperHeights.size() - 1];
    UndoUpperHeights.resize(UndoUpperHeights.size() - 1);

}


const int CBoxes::IndexAt(int X, int Z)
{
    return int (((Z + Dimensions) % Dimensions) + ((X + Dimensions) % Dimensions) * Dimensions);
}

const float CBoxes::GetLowerHeight(int X, int Z)
{
    return float(LowerHeights[IndexAt(X, Z)] / 16.0f);
}

const float CBoxes::GetUpperHeight(int X, int Z)
{
    return float(UpperHeights[IndexAt(X, Z)] / 16.0f);
}

void CBoxes::SetLowerHeight(int X, int Z, float Height)
{
    LowerHeights[IndexAt(X, Z)] = Height;
}
void CBoxes::SetUpperHeight(int X, int Z, float Height)
{
    UpperHeights[IndexAt(X, Z)] = Height;
}

const int CBoxes::GetTextureIndex(int X, int Z, int Side)
{
    return int(TextureInfo[IndexAt(X, Z)].Indices[Side] + ((TextureInfo[IndexAt(X, Z)].Rotations[Side] & 1) << 8));
}

void CBoxes::SetTextureIndex(int X, int Z, int Side, int Index)
{
    if (Config->Editor.PaintAll) {
        for (int i = 0; i < 6; i++) {
            TextureInfo[IndexAt(X, Z)].Indices[i] = Index;
            TextureInfo[IndexAt(X, Z)].Rotations[i] = (Index << 8) & 1;
        }
    } else {
        TextureInfo[IndexAt(X, Z)].Indices[Side] = Index;
        TextureInfo[IndexAt(X, Z)].Rotations[Side] = (Index << 8) & 1;
    }
}

void CBoxes::GetTextureRotation(int X, int Z, int Side, int &MirrorX, int &MirrorY, int &Rotation)
{
    MirrorX = (TextureInfo[IndexAt(X, Z)].Rotations[Side] & 32) >> 5;
    MirrorY = (TextureInfo[IndexAt(X, Z)].Rotations[Side] & 16) >> 4;
    Rotation = (TextureInfo[IndexAt(X, Z)].Rotations[Side] & 192) >> 6;
}

void CBoxes::SetTextureRotation(int X, int Z, int Side, int MirrorX, int MirrorY, int Rotation)
{
    Config->Editor.MirrorX = MirrorX;
    Config->Editor.MirrorY = MirrorY;
    Config->Editor.Orientation = Rotation;

    if (Config->Editor.PaintAll) {
        for (int i = 0; i < 6; i++) {
            TextureInfo[IndexAt(X, Z)].Rotations[i] = (MirrorX << 5) & 32;
            TextureInfo[IndexAt(X, Z)].Rotations[i] += (MirrorY << 4) & 16;
            TextureInfo[IndexAt(X, Z)].Rotations[i] += (Rotation << 6) & 192;
        }
    } else {
        TextureInfo[IndexAt(X, Z)].Rotations[Side] = (MirrorX << 5) & 32;
        TextureInfo[IndexAt(X, Z)].Rotations[Side] += (MirrorY << 4) & 16;
        TextureInfo[IndexAt(X, Z)].Rotations[Side] += (Rotation << 6) & 192;
    }


}

