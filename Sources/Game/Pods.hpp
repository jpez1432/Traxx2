
#ifndef PODS_HPP
#define PODS_HPP

#include "../Common.hpp"
#include <string>
#include <vector>

#include "../Logger.hpp"
#include "../Config.hpp"

extern CLogger *Logger;
extern CConfig *Config;

extern std::string ResourceDir;
extern std::string RootDir;

namespace Game
{

class CPod
{

private:

    int NumFiles;
    char Comment[80];
    std::string PodFile;

    struct Directory {
        char Filename[32];
        int Size;
        int Offset;
    };

    std::vector<std::string> Tracks;
    std::vector<Directory> Directories;

public:

    CPod(void);
    ~CPod(void);

    bool Create(std::string Path, std::string PodFile, std::string PodComment, std::vector<std::string> &Filenames);
    bool Load(std::string Filename, bool Absolute = false, bool Extract = false);
    bool FindFile(std::string &Filename, int &Size, int &Offset, bool Resources = false);

    std::string &Filename(void)
    {
        return PodFile;
    }
    int NumTracks(void)
    {
        return Tracks.size();
    }
    std::string &Track(int Index) {
        return Tracks[Index];
    }
    const int &NumberOfFiles(void)
    {
        return NumFiles;
    }
    const char *PodComment(void)
    {
        return Comment;
    }
    const char *Filename(int Index)
    {
        return Directories[Index].Filename;
    }
    const int &Filesize(int Index)
    {
        return Directories[Index].Size;
    }
    const int &FileOffset(int Index)
    {
        return Directories[Index].Offset;
    }

};

}

#endif
