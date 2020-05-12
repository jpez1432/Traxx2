
#include "Track.hpp"

using namespace Game;

CTrack::CTrack(void) : TexturePool(NULL), ModelPool(NULL), Terrain(NULL), Boxes(NULL)
{

}

CTrack::~CTrack(void)
{
    Destroy();
}

bool CTrack::Load(CPod &Pod, std::string Filename)
{

    this->Pod = &Pod;
    this->Comment = Pod.PodComment();
    this->Filename = Filename;

    std::string Line;
    int Size, Offset;

    Filename.insert(0, "World\\");
    Pod.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, std::ios::beg);

    std::getline(StreamIn, Line);
    Level = UpperStr(&Line[0]);

    Name = FindMarker(StreamIn, "!Race Track Name");
    Locale = FindMarker(StreamIn, "Race Track Locale");
    Logo = FindMarker(StreamIn, "Track Logo .BMP file");
    Map = FindMarker(StreamIn, "Track Map .BMP file");
    Description = FindMarker(StreamIn, "Track Description .TXT file");
    Type = FindMarker(StreamIn, "Track Race Type");

    std::getline(StreamIn, Line);
    std::getline(StreamIn, Line);
    std::getline(StreamIn, Line);

    AmbientSound = "1";
    if (Line == "!ambient sound,track length,weather mask") {
        std::getline(StreamIn, Line);
        AmbientSound = Line.substr(0, Line.find_first_of(",")).c_str();
        WeatherMask = atoi(Line.substr(Line.find_last_of(",") + 1, Line.length()).c_str());
        MTM2Track = true;
    } else {
        MTM2Track = false;
    }

    Line = FindMarker(StreamIn, "*** Vehicles ***");
    Vehicles.resize(atoi(Line.c_str()));
    std::getline(StreamIn, Line);

    for(unsigned int i = 0; i < Vehicles.size(); i++) {

        Vehicles[i].Filename = FindMarker(StreamIn, "truckFile");
        ParseVector(FindMarker(StreamIn, "ipos"), Vehicles[i].Position);
        ParseVector(FindMarker(StreamIn, "theta,phi,psi"), Vehicles[i].Rotation);

        Vehicles[i].Position /= 32.0f;
    }

    Line = FindMarker(StreamIn, "*** Boxes ***");
    Models.resize(atoi(Line.c_str()));
    std::getline(StreamIn, Line);

    for(unsigned int i = 0; i < Models.size(); i++) {

        ParseVector(FindMarker(StreamIn, "ipos"), Models[i].Position);
        ParseVector(FindMarker(StreamIn, "theta,phi,psi"), Models[i].Rotation);

        if (Models[i].Position.x < 0) { Models[i].Position.x = abs((256 * 32) + Models[i].Position.x); }
    //    if (Vector.y < 0) { Vector.y = abs((256 * 32) + Vector.y); }
        if (Models[i].Position.z < 0) { Models[i].Position.z = abs((256 * 32) + Models[i].Position.z); }

        Models[i].Position /= 32.0f;

        std::cout << Models[i].Position.x << ", " << Models[i].Position.y << ", " << Models[i].Position.z << std::endl;

        Line = FindMarker(StreamIn, "model", "length,width,height");

        if (StrCompare(Line.substr(Line.size() - 4, Line.size()), ".BIN")) {
            Models[i].Filename = Line;
        } else {
            ParseVector(Line, Models[i].Dimensions);
            Models[i].Dimensions /= 32.0f;
            Models[i].Filename = "Box";

        }

        Models[i].Mass = atof(FindMarker(StreamIn, "mass").c_str());
        Models[i].TypeFlags  = FindMarker(StreamIn, "!type,flags");
        Models[i].Priority = atoi(FindMarker(StreamIn, "priority").c_str());

    }

    Course Course;
    Line = FindMarker(StreamIn, "*** Course ***");
    std::getline(StreamIn, Line);
    Course.Checkpoints.resize(atoi(Line.substr(0, Line.find_first_of(",")).c_str()));

    for (unsigned int i = 0; i < Course.Checkpoints.size(); i++) {
        Line = FindMarker(StreamIn, "ctype,cspeed_type");
        Course.Checkpoints[i].Type = Line.substr(0, Line.find_first_of(",")).c_str();
        ParseVector(FindMarker(StreamIn, "cstart"), Course.Checkpoints[i].Start);
        Course.Checkpoints[i].Start /= 32.0f;
        ParseVector(FindMarker(StreamIn, "cend"), Course.Checkpoints[i].End);
        Course.Checkpoints[i].End /= 32.0f;
    }
    Courses.push_back(Course);

    Line = FindMarker(StreamIn, "@*********** Extended Course Definitions *************");
    NumCourses = atoi(Line.c_str());

    for (int i = 0; i < NumCourses; i++) {
        std::getline(StreamIn, Line);
        std::getline(StreamIn, Line);

        Course.Checkpoints.resize(atoi(Line.substr(0, Line.find_first_of(",")).c_str()));

        for (unsigned int i = 0; i < Course.Checkpoints.size(); i++) {
            Line = FindMarker(StreamIn, "ctype,cspeed_type");
            Course.Checkpoints[i].Type = Line.substr(0, Line.find_first_of(",")).c_str();
            ParseVector(FindMarker(StreamIn, "cstart"), Course.Checkpoints[i].Start);
            Course.Checkpoints[i].Start /= 32.0f;
            ParseVector(FindMarker(StreamIn, "cend"), Course.Checkpoints[i].End);
            Course.Checkpoints[i].End /= 32.0f;
            for(int i = 0; i < 4; i++) {
                std::getline(StreamIn, Line);
            }
        }
        Courses.push_back(Course);
    }

    Line = FindMarker(StreamIn, "*** Stadium ***");
//    if(StrCompare(Line.c_str(), "!stadiumFlag,x,z,sx,sz,stadiumModelName")) {
//        MTM2Track = true;
//    }

    Backdrop = FindMarker(StreamIn, "backdropType,backdropCount");
    BackdropCount = atoi(Backdrop.substr(Backdrop.find_last_of(",") + 1, Backdrop.length()).c_str());
    std::getline(StreamIn, Backdrop);

    Backdrops.resize(BackdropCount);
    for(int i = 0; i < BackdropCount; i++) {
        std::getline(StreamIn, Backdrop);

        if(Backdrops[i].Load(Pod, UpperStr(Backdrop))) {
            Logger->Text(std::string("Loaded Track Background Model - ") + UpperStr(Backdrop));
        } else {
            Logger->Error(std::string("Error Track Map Background Model - ") + UpperStr(Backdrop));
        }
    }

    StreamIn.close();

    Filename = UpperStr(Level);
    std::string LowerBoxes = "Data\\" + Filename.substr(0, Filename.length() - 3) + "RA0";
    std::string UpperBoxes = "Data\\" + Filename.substr(0, Filename.length() - 3) + "RA1";
    std::string TextureBoxes = "Data\\" + Filename.substr(0, Filename.length() - 3) + "CL0";


    Filename = "Levels\\" + std::string(Level);
    Pod.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    StreamIn.open(Filename.c_str(), std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, StreamIn.beg);

    std::getline(StreamIn, Line);
    std::getline(StreamIn, TextFile);
    std::getline(StreamIn, HeightMap);
    std::getline(StreamIn, TextureInfo);

    std::getline(StreamIn, Palette);
    Palette = UpperStr(Palette);

    std::getline(StreamIn, TextureList);

    for(int i = 0; i < 4; i++) {
        std::getline(StreamIn, Line);
    }

    std::getline(StreamIn, WeatherTexture);
    std::getline(StreamIn, WeatherPalette);

    for(int i = 0; i < 3; i++) {
        std::getline(StreamIn, Line);
    }

    std::getline(StreamIn, FogMap);
    FogMap = FogMap.substr(0, FogMap.find_last_of(".")) + ".MAP";

    std::getline(StreamIn, LightMap);

    std::getline(StreamIn, Line);
    ParseVector(Line, SunPosition);
    SunPosition /= 64.0f;

    WaterHeight = 0.0f;

    for(int i = 0; i < 6; i++) {
        std::getline(StreamIn, Line);
    }

    WaterHeight = atof(Line.c_str()) / 64.0f;

    StreamIn.close();

    if (MTM2Track) {

        Filename = "Fog\\" + FogMap;
        Pod.FindFile(Filename, Size, Offset);

        Filename.insert(0, RootDir + ResourceDir);
        StreamIn.open(Filename.c_str(), std::ios::in | std::ios::binary);

        if(StreamIn.fail()) {
            return false;
        }

        StreamIn.seekg(Offset, StreamIn.beg);
        StreamIn.read((char*)&Fog[0], sizeof(unsigned char) * 32768);
        StreamIn.close();

    }
    Filename = "Art\\" + Palette;
    Pod.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    StreamIn.open(Filename.c_str(), std::ios::in | std::ios::binary);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, StreamIn.beg);
    StreamIn.read((char*)&TrackPalette[0], sizeof(unsigned char) * 768);
    StreamIn.close();


    Filename = "Data\\" + TextureList;
    Pod.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    StreamIn.open(Filename.c_str(), std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, StreamIn.beg);

    unsigned int Count;
    std::vector<std::string> Filenames;

    std::getline(StreamIn, Line);
    Count = atoi(Line.c_str());

    std::getline(StreamIn, Line);
    while (!StreamIn.eof() && Line != "0" && Line != "" && Filenames.size() < Count) {
        Filenames.push_back(UpperStr(Line));
        std::getline(StreamIn, Line);
    }

    StreamIn.close();

    TexturePool = new CTexturePool();
    if(TexturePool->Create(Pod, Filenames)) {
        Logger->Text("Loaded Map Textures");
    } else {
        Logger->Error("Error Loading Map Textures");
    }

    Terrain = new CTerrain();
    if(Terrain->Load(Pod, HeightMap, TextureInfo)) {
        Logger->Text("Loaded Terrain Heightmap & Texture Info");
    } else {
        Logger->Error("Error Loading Terrain Heightmap & Texture Info");
    }

    Boxes = new CBoxes();
    if(Boxes->Load(Pod, LowerBoxes, UpperBoxes, TextureBoxes)) {
        Logger->Text("Loaded Ground Boxes & Texture Info");
    } else {
        Logger->Error("Error Loading Ground Boxes & Texture Info");
    }

    Filenames.resize(0);
    for(unsigned int i = 0; i < Models.size(); i++) {
        if(Models[i].Filename != "Box") {
            Filenames.push_back(UpperStr(Models[i].Filename));
        }
    }

    std::sort(Filenames.begin(), Filenames.end());
    Filenames.erase(std::unique(Filenames.begin(), Filenames.end()), Filenames.end());

    ModelPool = new CModelPool();
    if(ModelPool->Create(Pod, Filenames)) {
        Logger->Text("Loaded Map Models");
    } else {
        Logger->Error("Error Loading Map Models");
    }

    for(unsigned int j = 0; j < Models.size(); j++) {
        for(unsigned int i = 0; i < Filenames.size(); i++) {
            if((StrCompare(ModelPool->GetFilename(i).c_str(), Models[j].Filename.c_str()))) {

                Models[j].Index = i;
                Models[j].GLModel = ModelPool->GetModel(i).GetModel();
                Models[j].Bounds = ModelPool->GetBoundingSphere(i);
                ModelPool->GetBounds(i, Models[j].Min, Models[j].Max);
            }
        }
    }


    return true;

}

void CTrack::LoadPalette(std::string Filename)
{

    Filename.insert(0, "Art\\");
//    Pod.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::in | std::ios::binary);

    if(StreamIn.fail()) {
        return;
    }

    StreamIn.read((char*)&TrackPalette[0], sizeof(unsigned char) * 768);
    StreamIn.close();

}

bool CTrack::Save(std::string Filename)
{
    SetLevel();

    Filename.insert(0, "World\\");
    Filename.insert(0, RootDir + ResourceDir);
    std::ofstream StreamOut(Filename.c_str(),  std::ios::out | std::ios::trunc);

    if(StreamOut.fail()) {
        return false;
    }

    StreamOut << Level << std::endl;
    StreamOut << "!Race Track Name" << std::endl;
    StreamOut << Name << std::endl;
    StreamOut << "Race Track Locale" << std::endl;
    StreamOut << "Traxx2 USA" << std::endl;
    StreamOut << "Track Longtitude, Latitude" << std::endl;
    StreamOut << "7947984,7947988" << std::endl;
    StreamOut << "Track Logo .BMP file" << std::endl;
    StreamOut << Logo << std::endl;
    StreamOut << "Track Map .BMP file" << std::endl;
    StreamOut << Map << std::endl;
    StreamOut << "Track Fly-By .AVI file" << std::endl;
    StreamOut << "Sonic" << std::endl;
    StreamOut << "Track Announcer .WAV file" << std::endl;
    StreamOut << "Track" << std::endl;
    StreamOut << "Track Description .TXT file" << std::endl;
    StreamOut << Description << std::endl;
    StreamOut << "Track Race Type" << std::endl;
    std::cout << std::to_string(atoi(Type.c_str()) + 1) << ", " << Type << std::endl;
    StreamOut << std::to_string(atoi(Type.c_str()) + 1) << std::endl;
    StreamOut << "@Redbook Audio Track" << std::endl;
    StreamOut << "2" << std::endl;
    StreamOut << "!ambient sound,track length,weather mask" << std::endl;
    StreamOut << AmbientSound << ",5000.000000," << WeatherMask << std::endl;
    StreamOut << "viewmode,spotd,spotp,spoth,zoom" << std::endl;
    StreamOut << "0,16384,-16383,24832,98304" << std::endl;
    StreamOut << "$racetime, raceStartTime, dragDebugTimer" << std::endl;
    StreamOut << "0.000000,0.000000,0.000000" << std::endl;
    StreamOut << "controlflag, autoShift, autoStage, bothStaged, bothStagedPrev" << std::endl;
    StreamOut << "0,1,0,0,0" << std::endl;
    StreamOut << "stageComFlag, bonusLapFlag" << std::endl;
    StreamOut << "0,0" << std::endl;

    StreamOut << "*** Your Truck (Not used anymore) ***" << std::endl;
    StreamOut << "*********************************************" << std::endl;
    StreamOut << "truckFile" << std::endl;
    StreamOut << "powerbig.trk" << std::endl;
    StreamOut << "ipos" << std::endl;
    StreamOut << "4076.00,200.00,4332.00" << std::endl;
    StreamOut << "bvel" << std::endl;
    StreamOut << "0.000000,0.000000,0.000000" << std::endl;
    StreamOut << "theta,phi,psi" << std::endl;
    StreamOut << "0.00,0.00,0.00" << std::endl;
    StreamOut << "p,q,r" << std::endl;
    StreamOut << "0.000000,0.000000,0.000000" << std::endl;
    StreamOut << "faxle.angle,faxle.steering_angle" << std::endl;
    StreamOut << "0.000000,0.000000" << std::endl;
    StreamOut << "faxle.rtire.on_gnd,faxle.ltire.on_gnd" << std::endl;
    StreamOut << "-1,1" << std::endl;
    StreamOut << "raxle.angle,raxle.steering_angle" << std::endl;
    StreamOut << "0.000000,0.000000" << std::endl;
    StreamOut << "raxle.rtire.on_gnd,raxle.ltire.on_gnd" << std::endl;
    StreamOut << "-1,1" << std::endl;
    StreamOut << "xm.gear" << std::endl;
    StreamOut << "4" << std::endl;
    StreamOut << "ap.autopilot,ap.cnumber" << std::endl;
    StreamOut << "0,1" << std::endl;
    StreamOut << "ap.speed_control,ap.course_control,ap.lasterror" << std::endl;
    StreamOut << "0.000000,0.000000,0.000000" << std::endl;
    StreamOut << "!ap.courseToFollow" << std::endl;
    StreamOut << "0" << std::endl;
    StreamOut << "$heliTimer,heliTheta,heliPhi,heliPsi" << std::endl;
    StreamOut << "0.000000,0.000000,0.000000,0.000000" << std::endl;
    StreamOut << "heliPos" << std::endl;
    StreamOut << "0.000000,0.000000,0.000000" << std::endl;
    StreamOut << "^segments,laps,staged,bonusLaps,finishedRace,nextcheckpoint" << std::endl;
    StreamOut << "0,0,0,0,0,0" << std::endl;
    StreamOut << "totalracetime,fastestLap,dragTimer" << std::endl;
    StreamOut << "0.000000,0.000000,0.000000" << std::endl;

    for (int i = 0; i < 20; i++) {
        StreamOut << "***Lap time***" << std::endl;
        StreamOut << "0.000000" << std::endl;
        StreamOut << "*****Checkpoint times*****" << std::endl;
        for (int j = 0; j < 20; j++) {
            StreamOut << "0.000000" << std::endl;
        }
    }

    StreamOut << "*** Vehicles ***" << std::endl;
    StreamOut << Vehicles.size() << std::endl;

    for (unsigned int i = 0; i < Vehicles.size(); i++) {
        StreamOut << "*********************************************" << std::endl;
        StreamOut << "truckFile" << std::endl;
        StreamOut << Vehicles[i].Filename << std::endl;
        StreamOut << "ipos" << std::endl;
        StreamOut << Vehicles[i].Position.z * 32.0f << "," << Vehicles[i].Position.y * 32.0f << "," << Vehicles[i].Position.x * 32.0f << std::endl;
        StreamOut << "bvel" << std::endl;
        StreamOut << "0.000000,0.000000,0.000000" << std::endl;
        StreamOut << "theta,phi,psi" << std::endl;
        StreamOut << Vehicles[i].Rotation.z << "," << Vehicles[i].Rotation.y << "," << Vehicles[i].Rotation.x << std::endl;
        StreamOut << "p,q,r" << std::endl;
        StreamOut << "0.000000,0.000000,0.000000" << std::endl;
        StreamOut << "faxle.angle,faxle.steering_angle" << std::endl;
        StreamOut << "0.000000,0.000000" << std::endl;
        StreamOut << "faxle.rtire.on_gnd,faxle.ltire.on_gnd" << std::endl;
        StreamOut << "-1,1" << std::endl;
        StreamOut << "raxle.angle,raxle.steering_angle" << std::endl;
        StreamOut << "0.000000,0.000000" << std::endl;
        StreamOut << "raxle.rtire.on_gnd,raxle.ltire.on_gnd" << std::endl;
        StreamOut << "-1,1" << std::endl;
        StreamOut << "xm.gear" << std::endl;
        StreamOut << "4" << std::endl;
        StreamOut << "ap.autopilot,ap.cnumber" << std::endl;
        StreamOut << "0,1" << std::endl;
        StreamOut << "ap.speed_control,ap.course_control,ap.lasterror" << std::endl;
        StreamOut << "0.000000,0.000000,0.000000" << std::endl;
        StreamOut << "!ap.courseToFollow" << std::endl;
        StreamOut << "0" << std::endl;
        StreamOut << "$heliTimer,heliTheta,heliPhi,heliPsi" << std::endl;
        StreamOut << "0.000000,0.000000,0.000000,0.000000" << std::endl;
        StreamOut << "heliPos" << std::endl;
        StreamOut << "0.000000,0.000000,0.000000" << std::endl;
        StreamOut << "^segments,laps,staged,bonusLaps,finishedRace,nextcheckpoint" << std::endl;
        StreamOut << "0,0,0,0,0,0" << std::endl;
        StreamOut << "totalracetime,fastestLap,dragTimer" << std::endl;
        StreamOut << "0.000000,0.000000,0.000000" << std::endl;

        for (int j = 0; j < 20; j++) {
            StreamOut << "***Lap time***" << std::endl;
            StreamOut << "0.000000" << std::endl;
            StreamOut << "*****Checkpoint times*****" << std::endl;
            for (int k = 0; k < 20; k++) {
                StreamOut << "0.000000" << std::endl;
            }
        }
    }

    StreamOut << "*** Ramps ***" << std::endl;
    StreamOut << "0" << std::endl;

    StreamOut << "*** Boxes ***" << std::endl;
    StreamOut << Models.size() << std::endl;

    for (unsigned int i = 0; i < Models.size(); i++) {
        StreamOut << "*********************************************" << std::endl;
        StreamOut << "ipos" << std::endl;
        StreamOut << Models[i].Position.z * 32.0f << "," << Models[i].Position.y * 32.0f << "," << Models[i].Position.x * 32.0f << std::endl;
        StreamOut << "theta,phi,psi" << std::endl;
        StreamOut << Models[i].Rotation.z << "," << Models[i].Rotation.y << "," << Models[i].Rotation.x << std::endl;
        if (!StrCompare(Models[i].Filename, "Box")) {
            StreamOut << "model" << std::endl;
            StreamOut << Models[i].Filename << std::endl;
        } else {
            StreamOut << "length,width,height" << std::endl;
            StreamOut << Models[i].Dimensions.z * 32.0f << "," << Models[i].Dimensions.y * 32.0f << "," << Models[i].Dimensions.x * 32.0f << std::endl;
        }
        StreamOut << "mass" << std::endl;
        StreamOut << Models[i].Mass << std::endl;
        StreamOut << "bvel" << std::endl;
        StreamOut << "0.000000,0.000000,0.000000" << std::endl;
        StreamOut << "p,q,r" << std::endl;
        StreamOut << "0.000000,0.000000,0.000000" << std::endl;
        StreamOut << "!type,flags" << std::endl;
        StreamOut << Models[i].TypeFlags << std::endl;
        StreamOut << "priority" << std::endl;
        StreamOut << Models[i].Priority << std::endl;
        StreamOut << "@sound effect entries" << std::endl;
        StreamOut << "NULL.WAV" << std::endl;
        StreamOut << "NULL.WAV" << std::endl;
        StreamOut << "0,0" << std::endl;
    }

    StreamOut << "*** Cylinders ***" << std::endl;
    StreamOut << "0" << std::endl;

    StreamOut << "*** Top Crush ***" << std::endl;
    StreamOut << "0" << std::endl;

    StreamOut << "*** Course ***" << std::endl;
    StreamOut << "c1Count,course_direction" << std::endl;
    StreamOut << std::to_string(Courses[0].Checkpoints.size()) << ",0" << std::endl;

    for (unsigned int i = 0; i < Courses[0].Checkpoints.size(); i++) {
        StreamOut << "********************************************* " << std::to_string(i + (i + 1)) << std::endl;
        StreamOut << "ctype,cspeed_type" << std::endl;
        StreamOut << Courses[0].Checkpoints[i].Type << ",0" << std::endl;
        StreamOut << "cstart" << std::endl;
        StreamOut << Courses[0].Checkpoints[i].Start.z * 32.0f << "," << Courses[0].Checkpoints[i].Start.y * 32.0f << "," << Courses[0].Checkpoints[i].Start.x * 32.0f << std::endl;
        StreamOut << "cend" << std::endl;
        StreamOut << Courses[0].Checkpoints[i].End.z * 32.0f << "," << Courses[0].Checkpoints[i].End.y * 32.0f << "," << Courses[0].Checkpoints[i].End.x * 32.0f << std::endl;
        StreamOut << "cdec_point,cspeed,lastentry" << std::endl;
        StreamOut << "30.000000," << std::to_string(Courses[0].Checkpoints[i].Speed) << ",0" << std::endl;
        StreamOut << "&cSpeedLimit,cTrackWidth" << std::endl;
        StreamOut << "0.000000,0.000000" << std::endl;
    }

    StreamOut << "@*********** Extended Course Definitions *************" << std::endl;
    StreamOut << "4" << std::endl;

    for (int i = 0; i < NumCourses; i++) {
        StreamOut << "[Course " << std::to_string(i) << "] c1Count,course_direction" << std::endl;
        StreamOut << std::to_string(Courses[i + 1].Checkpoints.size()) << ",0" << std::endl;
        for (unsigned int j = 0; j < Courses[i + 1].Checkpoints.size(); j++) {
            StreamOut << "********************************************* " << std::to_string(j + (j + 1)) << std::endl;
            StreamOut << "ctype,cspeed_type" << std::endl;
            StreamOut << Courses[i + 1].Checkpoints[j].Type << ",0" << std::endl;
            StreamOut << "cstart" << std::endl;
            StreamOut << Courses[i + 1].Checkpoints[j].Start.z * 32.0f << "," << Courses[i + 1].Checkpoints[j].Start.y * 32.0f << "," << Courses[i + 1].Checkpoints[j].Start.x * 32.0f << std::endl;
            StreamOut << "cend" << std::endl;
            StreamOut << Courses[i + 1].Checkpoints[j].End.z * 32.0f << "," << Courses[i + 1].Checkpoints[j].End.y * 32.0f << "," << Courses[i + 1].Checkpoints[j].End.x * 32.0f << std::endl;
            StreamOut << "cdec_point,cspeed,lastentry" << std::endl;
            StreamOut << "30.000000," << std::to_string(Courses[i + 1].Checkpoints[j].Speed) << ",0" << std::endl;
            StreamOut << "&cSpeedLimit,cTrackWidth" << std::endl;
            StreamOut << "0.000000,0.000000" << std::endl;
        }
    }

    StreamOut << "*** Stadium ***" << std::endl;
    StreamOut << "stadiumFlag,stadiumModelName" << std::endl;
    StreamOut << "0,none" << std::endl;
    StreamOut << "*** Backdrop ***" << std::endl;
    StreamOut << "backdropType,backdropCount" << std::endl;
    StreamOut << "0," << BackdropCount << std::endl;
    StreamOut << "backdropModelName" << std::endl;
    for (int i = 0; i < BackdropCount; i++) {
        StreamOut << Backdrops[i].GetFilename() << std::endl;
    }

    StreamOut.close();

    Filename = "ART\\" + Palette;
    Filename.insert(0, RootDir + ResourceDir);
    StreamOut.open(Filename.c_str(), std::ios::out | std::ios::binary);

    if(StreamOut.fail()) {
        return false;
    }

    StreamOut.write((char*)&TrackPalette[0], sizeof(unsigned char) * 768);
    StreamOut.close();

    Filename = "Fog\\" + FogMap;
    Filename.insert(0, RootDir + ResourceDir);
    StreamOut.open(Filename.c_str(), std::ios::out | std::ios::binary);

    if(StreamOut.fail()) {
        return false;
    }

    StreamOut.write((char*)&Fog[0], sizeof(unsigned char) * 32768);
    StreamOut.close();

    if(Terrain->Save(*Pod, HeightMap, TextureInfo)) {
        Logger->Text("Saved Terrain Heightmap & Texture Info");
    } else {
        Logger->Error("Error Saving Terrain Heightmap & Texture Info");
    }

    if(Boxes->Save(*Pod, LowerBoxes, UpperBoxes, TextureBoxes)) {
        Logger->Text("Saved Ground Boxes & Texture Info");
    } else {
        Logger->Error("Error Saving Ground Boxes & Texture Info");
    }

    if(TexturePool->Save(*Pod, TextureList)) {
        Logger->Text("Saved Textures List");
    } else {
        Logger->Error("Error Saving Textures List");
    }

    Filename = "Data\\Zero.raw";
    Filename.insert(0, RootDir + ResourceDir);
    StreamOut.open(Filename.c_str(), std::ios::out);

    if(StreamOut.fail()) {
        return false;
    }

    StreamOut << "-------- Ground Quake Count -------" << std::endl;
    StreamOut << "0" << std::endl;
    StreamOut << "-------- Box Quake Count --------" << std::endl;
    StreamOut << "0" << std::endl;

    StreamOut.close();

    Filename = "Levels\\" + std::string(Level);
    Filename.insert(0, RootDir + ResourceDir);
    StreamOut.open(Filename.c_str(), std::ios::out);

    if(StreamOut.fail()) {
        return false;
    }

    StreamOut << "0" << std::endl;
    StreamOut << Description << std::endl;
    StreamOut << HeightMap << std::endl;
    StreamOut << TempLevel << ".CLR" << std::endl;
    StreamOut << Palette << std::endl;
    StreamOut << TextureList << std::endl;
    StreamOut << Zero << std::endl;
    StreamOut << TempLevel << ".PUP" << std::endl;
    StreamOut << Animation << std::endl;
    StreamOut << TempLevel << ".TDF" << std::endl;
    StreamOut << WeatherTexture << std::endl;
    StreamOut << WeatherPalette << std::endl;
    StreamOut << TempLevel << ".DEF" << std::endl;
    StreamOut << TempLevel << ".NAV" << std::endl;
    StreamOut << "Break.wav" << std::endl;
    StreamOut << TempLevel << ".FOG" << std::endl;
    StreamOut << LightMap << std::endl;
    StreamOut << int(SunPosition.z * 64) << "," << int(SunPosition.y * 64) << "," << int(SunPosition.x * 64) << std::endl;
    StreamOut << "32768" << std::endl;
    StreamOut << "32000,-46333,0" << std::endl;
    StreamOut << "64000" << std::endl;
    StreamOut << "255" << std::endl;
    if (WaterHeight != 0) {
        StreamOut << "!waterHeight" << std::endl;
        StreamOut << int(WaterHeight * 64) << std::endl;
    }

    StreamOut.close();

    CopyFile(std::string(RootDir + ResourceDir + "UI\\UNTITLEL.BMP").c_str(), std::string(RootDir + ResourceDir + "UI\\" + Logo).c_str(), true);
    CopyFile(std::string(RootDir + ResourceDir + "UI\\UNTITLES.BMP").c_str(), std::string(RootDir + ResourceDir + "UI\\" + Map).c_str(), true);
    CopyFile(std::string(RootDir + ResourceDir + "FOG\\UNTITLED.MAP").c_str(), std::string(RootDir + ResourceDir + "FOG\\" + FogMap).c_str(), true);

//    NewFile("UI\\" + TempLevel + "S.BMP", 0, 0);
//    NewFile("UI\\" + TempLevel + "L.BMP", 0, 0);
//    NewFile("FOG\\" + TempLevel + ".MAP", 255, 32768);
//    NewFile("ART\\" + TempLevel + ".ACT", 255, 768);
    NewFile("DATA\\" + TempLevel + ".PUP", 0, 0);
    NewFile("DATA\\" + TempLevel + ".TTY", '0', 1);
    NewFile("DATA\\" + TempLevel + ".ANI", '0', 1);
    NewFile("DATA\\" + TempLevel + ".TXT", ' ', 0);
    NewFile("DATA\\" + TempLevel + ".CL1", ' ', 262144);
    NewFile("DATA\\" + TempLevel + ".CL2", ' ', 786432);
    NewFile("DATA\\" + TempLevel + ".LTE", 255, 458752);
    NewFile("DATA\\" + TempLevel + ".RA2", ' ', 65536);
    NewFile("DATA\\" + TempLevel + ".RA3", ' ', 65536);
    NewFile("DATA\\" + TempLevel + ".RA4", ' ', 65536);
    NewFile("DATA\\" + TempLevel + ".RA5", ' ', 65536);
    NewFile("LEVELS\\ZERO.RAW", 0, 0);

    Dirty = false;

    return true;
}

bool CTrack::Export(std::string Filename)
{
    SetLevel();

    std::vector<std::string> Filenames;

    Filenames.push_back("WORLD\\" + std::string(Config->Game.Track));
//    Filenames.push_back("UI\\" + Logo);
//    Filenames.push_back("UI\\" + Map);
    Filenames.push_back("DATA\\" + HeightMap);
    Filenames.push_back("DATA\\" + TextureList);
    Filenames.push_back("DATA\\" + TextureInfo);
    Filenames.push_back("DATA\\" + Description);
    Filenames.push_back("DATA\\" + LightMap);
    Filenames.push_back("DATA\\" + LowerBoxes);
    Filenames.push_back("DATA\\" + UpperBoxes);
    Filenames.push_back("DATA\\" + Animation);
    Filenames.push_back("DATA\\" + TextureBoxes);
    Filenames.push_back("ART\\" + WeatherTexture);
    Filenames.push_back("ART\\" + WeatherPalette);
    Filenames.push_back("ART\\" + Palette);
    Filenames.push_back("LEVELS\\" + Level);
    Filenames.push_back("LEVELS\\" + Zero);

    Filenames.push_back("DATA\\" + TempLevel + ".TTY");
    Filenames.push_back("DATA\\" + TempLevel + ".CL1");
    Filenames.push_back("DATA\\" + TempLevel + ".CL2");
    Filenames.push_back("DATA\\" + TempLevel + ".RA2");
    Filenames.push_back("DATA\\" + TempLevel + ".RA3");
    Filenames.push_back("DATA\\" + TempLevel + ".RA4");
    Filenames.push_back("DATA\\" + TempLevel + ".RA5");
//    Filenames.push_back("DATA\\" + TempLevel + ".PUP");
    Filenames.push_back("FOG\\" + TempLevel + ".MAP");
//    Filenames.push_back("ART\\METALCR2.ACT");


    for (unsigned int i = 0; i < Backdrops.size(); i++) {
        Filenames.push_back("MODELS\\" + Backdrops[i].GetFilename());
        for (int j = 0; j < Backdrops[i].TexturePool.NumTextures(); j++) {
            Filenames.push_back("ART\\" + Backdrops[i].TexturePool.GetFilename(j));
        }
    }

    for (int i = 0; i < ModelPool->NumModels(); i++) {
        Filenames.push_back("MODELS\\" + ModelPool->GetFilename(i));
        for (int k = 0; k < ModelPool->Models[i].NumFrames; k++) {
            Filenames.push_back("MODELS\\" + ModelPool->Models[i].Frames[k].Filename);
            for (int j = 0; j < ModelPool->Models[i].TexturePool.NumTextures(); j++) {
                Filenames.push_back("ART\\" + ModelPool->Models[i].Frames[k].TexturePool.GetFilename(j));
                if (ModelPool->Models[i].Frames[k].TexturePool.GetPalette(j)) {
                    Filenames.push_back("ART\\" + ModelPool->Models[i].Frames[k].TexturePool.GetFilename(j).substr(0, ModelPool->Models[i].Frames[k].TexturePool.GetFilename(j).length() - 4) + ".ACT");
                }
            }


        }
        for (int j = 0; j < ModelPool->Models[i].TexturePool.NumTextures(); j++) {
            Filenames.push_back("ART\\" + ModelPool->Models[i].TexturePool.GetFilename(j));
            if (ModelPool->Models[i].TexturePool.GetPalette(j)) {
                Filenames.push_back("ART\\" + ModelPool->Models[i].TexturePool.GetFilename(j).substr(0, ModelPool->Models[i].TexturePool.GetFilename(j).length() - 4) + ".ACT");
            }
        }
    }

    for (int i = 0; i < TexturePool->NumTextures(); i++) {
        Filenames.push_back("ART\\" + TexturePool->GetFilename(i));
        if (TexturePool->GetPalette(i)) {
            Filenames.push_back("ART\\" + TexturePool->GetFilename(i).substr(0, TexturePool->GetFilename(i).length() - 4) + ".ACT");
        }
    }

    Game::CPod *CreatePod = new Game::CPod();
    CreatePod->Create(std::string(RootDir + ResourceDir), Filename.c_str(), Comment, Filenames);
    delete CreatePod;

    return true;
}

void CTrack::Destroy(void)
{
    if(TexturePool) {
        delete TexturePool;
        TexturePool = NULL;
        Logger->Text("Destroyed Texture Pool Object");
    }

    if(ModelPool) {
        delete ModelPool;
        ModelPool = NULL;
        Logger->Text("Destroyed Model Pool Object");
    }

    if(Terrain) {
        delete Terrain;
        Terrain = NULL;
        Logger->Text("Destroyed Terrain Object");
    }

    if(Boxes) {
        delete Boxes;
        Boxes = NULL;
        Logger->Text("Destroyed Ground Box Object");
    }
}

bool CTrack::NewFile(std::string Filename, unsigned char Fill, long Size)
{


//    if (GetFileSize(Filename) != -1) {
//        if(MessageBox(NULL, std::string(Filename + " Already Exists, Overwrite This File?").c_str(), "Save", MB_ICONQUESTION | MB_YESNO) == IDNO) {
//            return false;
//        }
//    }

    if (GetFileSize(Filename) == -1) {

        std::vector<unsigned char>Data(Size, Fill);

        Filename.insert(0, RootDir + ResourceDir);
        std::ofstream StreamOut(Filename.c_str(), std::ios::binary | std::ios::out);

        if(StreamOut.fail()) {
            return false;
        }

        StreamOut.write((char*)&Data[0], Data.size());

        StreamOut.close();

        Data.clear();
    }

    return true;

}

void CTrack::SetLevel(void)
{
    TempLevel = std::string(Level);
    TempLevel = TempLevel.substr(0, TempLevel.find_last_of(".")).c_str();

    FogMap = TempLevel + ".MAP";
    Logo = TempLevel + "S.BMP";
    Map = TempLevel + "L.BMP";
    HeightMap = TempLevel + ".RAW";
    TextureList = TempLevel + ".TEX";
    TextureInfo = TempLevel + ".CLR";
    Description = TempLevel + ".TXT";
    LightMap = TempLevel + ".LTE";
    Palette = TempLevel + ".ACT";
    Animation = TempLevel + ".ANI";
    LowerBoxes = TempLevel + ".RA0";
    UpperBoxes = TempLevel + ".RA1";
    TextureBoxes = TempLevel + ".CL0";
    Zero = "ZERO.RAW";
    WeatherTexture = "CLOUDY2.RAW";
    WeatherPalette = "CLOUDY2.ACT";

}

std::string CTrack::FindMarker(std::ifstream &Stream, std::string Marker1, std::string Marker2)
{

    std::string Line;

    do {
        std::getline(Stream, Line);
    } while((Line != Marker1) && (Line != Marker2) && !Stream.eof());

    std::getline(Stream, Line);

    return Line;

}

void CTrack::AddModelUndo(void)
{
    UndoModels.push_back(Models);

    Dirty = true;
}

void CTrack::RemoveModelUndo(void)
{
    if (UndoModels.size() == 0) {
        return;
    }

    Models = UndoModels[UndoModels.size() - 1];
    UndoModels.resize(UndoModels.size() - 1);

}

void CTrack::AddCourseUndo(void)
{
    UndoCourses.push_back(Courses);

    Dirty = true;
}

void CTrack::RemoveCourseUndo(void)
{
    if (UndoCourses.size() == 0) {
        return;
    }

    Courses = UndoCourses[UndoCourses.size() - 1];
    UndoCourses.resize(UndoCourses.size() - 1);

}

