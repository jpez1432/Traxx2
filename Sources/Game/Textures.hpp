
#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#define GLEW_STATIC

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>
#include <Soil2/Soil2.h>

#include "Pods.hpp"

namespace Game
{

class CTexture
{

private:

    bool ActPalette;
    GLuint GLTexture;
    std::string Filename;

public:

    CTexture(void);
    ~CTexture(void);

    bool LoadRaw(CPod &Pod, std::string Filename, unsigned int *DefaultPalette = NULL);
    bool LoadBlank(GLuint Dimensions, GLuint Color, GLenum Format);
    bool LoadImage(std::string Filename, std::string AlternateDir = "");
    bool LoadMemory(GLubyte *Data, GLuint Dimensions, GLuint Bpp, GLenum Format);

    void Destroy(void);

    void SetParameters(GLint MinFilter, GLint MagFilter, GLint WrapS, GLint WrapT);

    void Bind(GLuint TextureUnit);
    void Unbind(void)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint &GetTexture(void)
    {
        return GLTexture;
    }
    std::string &GetFilename(void)
    {
        return Filename;
    }
    bool &GetPalette(void)
    {
        return ActPalette;
    }

};

class CTexturePool
{

private:

    Game::CPod *Pod;
    unsigned int DefaultPalette[256];
    glm::vec2 TextureCoords[2][2][4][4];

    int UndoIndex;
    CTexture UndoTexture;
    std::vector<CTexture> Textures;

public:

    CTexturePool(void);
    ~CTexturePool(void);

    bool Create(CPod &Pod, std::vector<std::string> &Filenames);
    bool Save(CPod &Pod, std::string Filename);

    void AddTexture(std::string Filename) {

        Textures.resize(Textures.size() + 1);
        Textures[Textures.size() - 1].LoadRaw(*Pod, Filename);
        Config->Editor.Index = Textures.size() - 1;

    }

    void DeleteAll(void) {
        Textures.resize(0);
        Config->Editor.Index = 0;
    }

    bool DeleteTexture(int Index) {

        if (Textures.size() > 0) {

            UndoIndex = Index;
            UndoTexture = Textures[Index];

            std::swap(Textures[Index], Textures[Textures.size() - 1]);
            Textures.resize(Textures.size() - 1);
            Config->Editor.Index = Textures.size() - 1;

        } else {
            Textures.resize(0);
            Config->Editor.Index = 0;
        }

        return true;
    }

    void UndoDelete(void) {
        UndoTexture.LoadRaw(*Pod, UndoTexture.GetFilename());
        Textures.push_back(UndoTexture);
        std::swap(Textures[UndoIndex], Textures[Textures.size() - 1]);
        Config->Editor.Index = UndoIndex;
    }

    void Bind(int Index)
    {
        Textures[Index].Bind(0);
    }

    const GLuint GetTexture(int Index)
    {
        return Textures[Index].GetTexture();
    }

    int NumTextures(void) {
        return Textures.size();
    }
    const std::string &GetFilename(int Index)
    {
        return Textures[Index].GetFilename();
    }
    const bool &GetPalette(int Index)
    {
        return Textures[Index].GetPalette();
    }
    glm::vec2 &GetTextureCoords(int MirrorX, int MirrorY, int Rotation, int Corner);

};

}

#endif
