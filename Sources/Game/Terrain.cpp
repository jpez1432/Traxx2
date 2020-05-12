
#include "Terrain.hpp"

using namespace Game;

CTerrain::CTerrain(void) : Dimensions(0)
{

}

CTerrain::~CTerrain(void)
{

    if(TextureInfo.empty() == false) {
        TextureInfo.clear();
        Logger->Text("Released Terrain Texture Index, Rotation Memory");
    }

    if(HeightmapData.empty() == false) {
        HeightmapData.clear();
        Logger->Text("Released Terrain Heightmap Memory");
    }

    if(HeightMap.empty() == false) {
        HeightMap.clear();
        Logger->Text("Released Terrain Heightmap Scaled Memory");
    }

    if(Normals.empty() == false) {
        Normals.clear();
        Logger->Text("Released Terrain Heightmap Normals Memory");
    }

}

bool CTerrain::Load(CPod &Pod, std::string Filename, std::string TexturesInfo)
{

    int Size, Offset;

    Filename.insert(0, "Data\\");
    Pod.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Size = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Size);
    HeightmapData.resize(Dimensions * Dimensions);
    HeightMap.resize(Dimensions * Dimensions);

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {
            HeightmapData[x + Dimensions * z] = StreamIn.get();
            HeightMap[z + Dimensions * x] = glm::vec3(float(x), float(HeightmapData[x + Dimensions * z] / 16.0f), float(z));
        }
    }

    StreamIn.close();

    CalculateNormals();

    TexturesInfo.insert(0, "Data\\");
    Pod.FindFile(TexturesInfo, Size, Offset);

    TexturesInfo.insert(0, RootDir + ResourceDir);
    StreamIn.open(TexturesInfo.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Size = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Size / 2);
    TextureInfo.resize(Dimensions * Dimensions);

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {
            TextureInfo[z + Dimensions * x].Index = StreamIn.get();
            TextureInfo[z + Dimensions * x].Rotation = StreamIn.get();
        }
    }

    StreamIn.close();

    return true;

}

bool CTerrain::Save(CPod &Pod, std::string Filename, std::string TexturesInfo)
{

    Filename.insert(0, "Data\\");
    Filename.insert(0, RootDir + ResourceDir);
    std::ofstream StreamOut(Filename.c_str(), std::ios::binary | std::ios::out);

    if(StreamOut.fail()) {
        return false;
    }

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {

            StreamOut.put(int(HeightMap[IndexAt(x, z)].y * 16.0f));
        }
    }

    StreamOut.close();

    TexturesInfo.insert(0, "Data\\");
    TexturesInfo.insert(0, RootDir + ResourceDir);
    StreamOut.open(TexturesInfo.c_str(), std::ios::binary | std::ios::out);

    if(StreamOut.fail()) {
        return false;
    }

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {

            StreamOut.put((char)TextureInfo[z + Dimensions * x].Index);
            StreamOut.put((char)TextureInfo[z + Dimensions * x].Rotation);

        }
    }

    StreamOut.close();

    return true;

}

void CTerrain::Generate(int MinSize, int MaxSize, int MaxHeight, int NumHills, int Seed)
{
    for(int z = 0; z < Dimensions; z++) {
        for( int x = 0; x < Dimensions; x++) {
            HeightMap[x + Dimensions * z].y = 0.0f;
        }
    }

    float X, Y;

    srand(Seed);
    for (int i = 0; i < NumHills; i++) {

        float Radius = RandomRange(MinSize, MaxSize);

        X = RandomRange(-Radius, Dimensions + Radius);
        Y = RandomRange(-Radius, Dimensions + Radius);

        float RadiusSq = Radius * Radius;
        float DistSq;
        float Height;

        int MinX = X - Radius - 1;
        int MaxX = X + Radius + 1;
        MinX = glm::clamp(MinX, 0, Dimensions - 1);
        MaxX = glm::clamp(MaxX, 0, Dimensions - 1);

        int MinY = Y - Radius - 1;
        int MaxY = Y + Radius + 1;
        MinY = glm::clamp(MinY, 0, Dimensions - 1);
        MaxY = glm::clamp(MaxY, 0, Dimensions - 1);

        for (int i = MinX; i <= MaxX; i++) {
            for (int j = MinY; j < MaxY; j++) {

                DistSq = (X - i) * (X - i) + (Y - j) * (Y - j);
                Height = RadiusSq - DistSq;

                if (Height > 0) {
                    HeightMap[i + Dimensions * j].y += Height / (255 - MaxHeight);
                }
            }
        }

    }
    CalculateNormals();
}

float CTerrain::RandomRange(float Min, float Max)
{
    return( ( rand() * ( Max - Min ) / RAND_MAX ) + Min );
}

void CTerrain::CopyTerrain(glm::vec3 Center, bool Cut)
{
    TerrainSelection.resize(Config->Editor.GridWidth + 1);
    for (int x = 0; x < Config->Editor.GridWidth + 1; x++) {
        TerrainSelection[x].resize(Config->Editor.GridLength + 1);
        for (int z = 0; z < Config->Editor.GridLength + 1; z++) {
            TerrainSelection[x][z] = 0.0f;
        }
    }

    for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
        for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
            TerrainSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] = GetHeight(x + Center.x, z + Center.z).y;
        }
    }

    if (Cut) {
        for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
            for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
                SetHeight(x + Center.x, z + Center.z, Config->Editor.Altitude / 16.0f);
            }
        }
    }
}

void CTerrain::PasteTerrain(glm::vec3 Center)
{
    for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
        for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
            SetHeight(x + Center.x, z + Center.z, TerrainSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2]);
        }
    }
}

void CTerrain::CopyTextures(glm::vec3 Center, bool Cut)
{
    TextureSelection.resize(Config->Editor.GridWidth + 1);
    for (int x = 0; x < Config->Editor.GridWidth + 1; x++) {
        TextureSelection[x].resize(Config->Editor.GridLength + 1);
    }

    for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
        for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
            TextureSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2] = TextureInfo[IndexAt(x + Center.x, z + Center.z)];
        }
    }

    if (Cut) {
        for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
            for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
                TextureInfo[IndexAt(x + Center.x, z + Center.z)].Index = 0;
            }
        }
    }

}
void CTerrain::PasteTextures(glm::vec3 Center)
{
    for (int x = -Config->Editor.GridWidth / 2; x < Config->Editor.GridWidth / 2 + (Config->Editor.GridWidth % 2); x++) {
        for (int z = -Config->Editor.GridLength / 2; z < Config->Editor.GridLength / 2 + (Config->Editor.GridLength % 2); z++) {
            TextureInfo[IndexAt(x + Center.x, z + Center.z)] = TextureSelection[x + Config->Editor.GridWidth / 2][z + Config->Editor.GridLength / 2];
        }
    }
}

void CTerrain::CalculateNormals(void)
{

    Normals.resize(HeightMap.size());
    glm::vec3 TriNormals[2][Normals.size()];

    for(int z = 0; z < Dimensions; z++) {
        for( int x = 0; x < Dimensions; x++) {

            glm::vec3 Triangle0[] = { GetHeight(x, z), GetHeight((x + 1), z), GetHeight((x + 1), (z + 1)) };
            glm::vec3 Triangle1[] = { GetHeight((x + 1), (z + 1)), GetHeight(x, (z + 1)), GetHeight(x, z) };

            glm::vec3 Normal0 = glm::cross(Triangle0[0] - Triangle0[1], Triangle0[1] - Triangle0[2]);
            glm::vec3 Normal1 = glm::cross(Triangle1[0] - Triangle1[1], Triangle1[1] - Triangle1[2]);

            TriNormals[0][z + Dimensions * x] = glm::normalize(Normal0);
            TriNormals[1][z + Dimensions * x] = glm::normalize(Normal1);

        }
    }

    glm::vec3 Result(0.0f, 0.0f, 0.0f);

    for(int z = 0; z < Dimensions; z++) {
        for(int x = 0; x < Dimensions; x++) {

            Result = glm::vec3(0);

            if(z != 0 && x != 0) {
                for(int i = 0; i < 2; i++) {
                    Result += TriNormals[i][(x - 1) + Dimensions * (z - 1)];
                }
            }

            if(x != 0 && z != Dimensions - 1) {
                Result += TriNormals[0][(x - 1) + Dimensions * z];
            }

            if(x != Dimensions - 1 && z != Dimensions - 1) {
                for(int i = 0; i < 2; i++) {
                    Result += TriNormals[i][x + Dimensions * z];
                }
            }

            if(x != Dimensions - 1 && z != 0) {
                Result += TriNormals[1][x + Dimensions * (z - 1)];
            }

            Normals[x + Dimensions * z] = glm::normalize(Result);

        }
    }

}

void CTerrain::AddTerrainUndo(void)
{
    UndoTerrain.push_back(HeightMap);

    Dirty = true;
}

void CTerrain::RemoveTerrainUndo(void)
{
    if (UndoTerrain.size() == 0) {
        return;
    }

    HeightMap = UndoTerrain[UndoTerrain.size() - 1];
    UndoTerrain.resize(UndoTerrain.size() - 1);

    CalculateNormals();
}

void CTerrain::AddTextureUndo(void)
{
    UndoTextures.push_back(TextureInfo);

    Dirty = true;
}

void CTerrain::RemoveTextureUndo(void)
{
    if (UndoTextures.size() == 0) {
        return;
    }

    TextureInfo = UndoTextures[UndoTextures.size() - 1];
    UndoTextures.resize(UndoTextures.size() - 1);

}

const unsigned int CTerrain::IndexAt(int X, int Z)
{
    X = glm::clamp(X, 0, int(Dimensions));
    Z = glm::clamp(Z, 0, int(Dimensions));

    return int(((Z + Dimensions) % Dimensions) + ((X + Dimensions) % Dimensions) * Dimensions);
}

const unsigned int CTerrain::GetTextureIndex(int X, int Z)
{
    return TextureInfo[IndexAt(X, Z)].Index + ((TextureInfo[IndexAt(X, Z)].Rotation & 1) << 8);
}

void CTerrain::SetTextureIndex(int X, int Z, int Index)
{
    if (Index > 255) {
        TextureInfo[IndexAt(X, Z)].Index = Index;
        TextureInfo[IndexAt(X, Z)].Rotation = (255 + ((255 - Index) << 8)) & 1;
    } else {
        TextureInfo[IndexAt(X, Z)].Index = Index;
    }
}

void CTerrain::GetTextureRotation(int X, int Z, int &MirrorX, int &MirrorY, int &Rotation)
{
    MirrorX = (TextureInfo[IndexAt(X, Z)].Rotation & 32) >> 5;
    MirrorY = (TextureInfo[IndexAt(X, Z)].Rotation & 16) >> 4;
    Rotation = (TextureInfo[IndexAt(X, Z)].Rotation & 192) >> 6;
}

void CTerrain::SetTextureRotation(int X, int Z, int MirrorX, int MirrorY, int Rotation)
{
    Config->Editor.MirrorX = MirrorX;
    Config->Editor.MirrorY = MirrorY;
    Config->Editor.Orientation = Rotation;

    TextureInfo[IndexAt(X, Z)].Rotation += (MirrorX << 5) & 32;
    TextureInfo[IndexAt(X, Z)].Rotation += (MirrorY << 4) & 16;
    TextureInfo[IndexAt(X, Z)].Rotation += (Rotation << 6) & 192;
}

glm::vec3 CTerrain::GetHeight(int X, int Z)
{
    return glm::vec3(X, HeightMap[IndexAt(X, Z)].y, Z);
}

void CTerrain::SetHeight(int X, int Z, float Height)
{
    HeightMap[IndexAt(X, Z)].y = Height;

}

const glm::vec3 &CTerrain::GetNormal(int X, int Z)
{

    return Normals[IndexAt(X, Z)];
}
