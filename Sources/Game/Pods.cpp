
#include "Pods.hpp"

using namespace Game;

CPod::CPod(void)
{

}

CPod::~CPod(void)
{
    Directories.clear();
}

bool CPod::Create(std::string Path, std::string PodFile, std::string PodComment, std::vector<std::string> &Filenames)
{
    int Offset;
    int NumFiles;
    int Filesize;
    char Comment[80];
    char Filename[32];


    NumFiles = Filenames.size();
    sprintf(Comment, "%s\n", PodComment.c_str());

    Offset = (84 + (NumFiles) * 40);

    std::ofstream StreamOut(PodFile.c_str(), std::ios::binary | std::ios::out);

    if (StreamOut.fail()) {
        return false;
    }

    StreamOut.write((char*)&NumFiles, sizeof(NumFiles));
    StreamOut.write((char*)&Comment, sizeof(Comment));

    for (int i = 0; i < NumFiles; i++) {

        Filesize = GetFileSize(Filenames[i].c_str());

        if (Filesize == -1) {
            Logger->Error(std::string("Error Reading - ") + Filenames[i] + std::string(", For Creation Of Pod, Aborting..."), true, true, false);
            return false;
        }

        if (i > 0) Offset += GetFileSize(Filenames[i - 1].c_str());

        strcpy(Filename, Filenames[i].c_str());
        StreamOut.write((char*)&Filename, sizeof(Filename));
        StreamOut.write((char*)&Filesize, sizeof(Filesize));
        StreamOut.write((char*)&Offset, sizeof(Offset));

    }

    for (int i = 0; i < NumFiles; i++) {

        std::ifstream StreamIn(Filenames[i].c_str(), std::ios::binary | std::ios::in);

        if (StreamIn.fail()) {
            return false;
        }

        StreamIn.seekg(0, std::ios::end);
        std::streamsize Size = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

        std::vector<char> Buffer(Size);
        StreamIn.read(Buffer.data(), Size);

        StreamOut.write((char*)&Buffer[0], Buffer.size());

        Buffer.clear();
        StreamIn.close();
    }

    StreamOut.close();

    return true;
}

bool CPod::Load(std::string Filename, bool Absolute, bool Extract)
{
    this->PodFile = Filename;

    std::vector<char>Data;

    if (Absolute == false) {
        Filename.insert(0, RootDir + ResourceDir);
    }
    std::ifstream StreamIn(Filename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.read((char*)&NumFiles, sizeof(NumFiles));
    StreamIn.read((char*)&Comment[0], sizeof(Comment));

    Directories.resize(NumFiles);
    StreamIn.read((char*)&Directories[0], sizeof(Directory) * NumFiles);


    for (unsigned int i = 0; i < Directories.size(); i++) {
        strcpy(Directories[i].Filename, UpperStr(Directories[i].Filename));
        Filename = Directories[i].Filename;
        if (StrCompare(Filename.substr(Filename.size() - 4, Filename.size()), ".SIT")) {
            Tracks.push_back(Filename.substr(Filename.find_last_of("\\") + 1).c_str());
        }
    }

    if (Extract) {

        for (unsigned int i = 0; i < Directories.size(); i++) {

            Data.resize(Directories[i].Size);
            StreamIn.seekg(Directories[i].Offset, std::ios::beg);
            StreamIn.read((char*)&Data[0], Data.size());

            Filename = Directories[i].Filename;
            Filename.insert(0, RootDir + ResourceDir);
            std::ofstream StreamOut(Filename.c_str(), std::ios::binary | std::ios::out);

            if(!StreamOut.fail()) {
                StreamOut.write((char*)&Data[0], Data.size());
                StreamOut.close();
            } else {
                Logger->Error("Error Extracting File From Pod - " + Filename, true, true, false);
            }
        }

    }

    StreamIn.close();

    return true;

}

bool CPod::FindFile(std::string &Filename, int &Size, int &Offset, bool Resources)
{

    Size = Offset = 0;

    if (Resources) {
        return false;
    }

    for(unsigned int i = 0; i < Directories.size(); i++) {
        if(StrCompare(Directories[i].Filename, Filename.c_str())) {

            Filename = PodFile;
            Size = Directories[i].Size;
            Offset = Directories[i].Offset;
            return true;

        }
    }

    return false;
}
