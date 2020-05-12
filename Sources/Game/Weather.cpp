
#include "Weather.hpp"

using namespace Game;

CWeather::CWeather(void)
{
    GeneralTint[0] = 1.0f;
    GeneralTint[1] = 1.0f;
    GeneralTint[2] = 1.0f;
    GeneralTint[3] = 1.0f;
    BackdropTint[0] = GeneralTint[0];
    BackdropTint[1] = GeneralTint[1];
    BackdropTint[2] = GeneralTint[2];
    BackdropTint[3] = GeneralTint[3];

    DomeIndex = -1;
}

CWeather::~CWeather(void)
{
    for (int i = 0; i < 4; i++) {
        SkyDomes[i].Destroy();
    }
}

void CWeather::SetWeather(int Index)
{
    glm::vec4 FogColor(0.0f);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, &FogColor[0]);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogf(GL_FOG_END, Config->Graphics.FarClip);
    glDisable(GL_FOG);

    switch(Index) {

    case -1:

        DomeIndex = -1;
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        GeneralTint[3] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        BackdropTint[3] = GeneralTint[3];

    case 0:	// Clear SkyDome
        DomeIndex = 0;
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 1:	// Cloudy SkyDome
        DomeIndex = 0;
        GeneralTint[0] = 0.8f;
        GeneralTint[1] = 0.8f;
        GeneralTint[2] = 0.8f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 2:	// Fog Skydome
        DomeIndex = 1;
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        glFogf(GL_FOG_DENSITY, 0.125f);
        glEnable(GL_FOG);
        break;

    case 3:	// Dense Fog Skydome
        DomeIndex = 1;
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        glFogf(GL_FOG_DENSITY, 0.25f);
        glEnable(GL_FOG);
        break;

    case 4:	// Rainy SkyDome
        DomeIndex = 2;
        GeneralTint[0] = 0.5f;
        GeneralTint[1] = 0.5f;
        GeneralTint[2] = 0.5f;
        BackdropTint[0] = 0.3f;
        BackdropTint[1] = 0.3f;
        BackdropTint[2] = 0.3f;
        break;

    case 5:	// Dusk SkyDome
        DomeIndex = 2;
        GeneralTint[0] = 0.8f;
        GeneralTint[1] = 0.7f;
        GeneralTint[2] = 0.6f;
        BackdropTint[0] = 0.3f;
        BackdropTint[1] = 0.3f;
        BackdropTint[2] = 0.3f;
        break;

    case 6:	// Night SkyDome
        DomeIndex = 3;
        GeneralTint[0] = 0.3f;
        GeneralTint[1] = 0.3f;
        GeneralTint[2] = 0.3f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 7:	// Night SkyDome (Pitch Black)
        DomeIndex = 3;
        GeneralTint[0] = 0.1f;
        GeneralTint[1] = 0.1f;
        GeneralTint[2] = 0.1f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;
    }

    WaterTint[0] = 0.6f;
    WaterTint[1] = 0.8f;
    WaterTint[2] = 1.0f;
    WaterTint[3] = 0.75f;

    GeneralTint[3] = BackdropTint[3] = 1.0f;
}

void CWeather::Create(CPod &Pod)
{
    if(SkyDomes[0].Load(Pod, "Cloudy2.bin")) {
        Logger->Text("Loaded Weather SkyDome Model - Cloudy2.bin");
    } else {
        Logger->Error("Error Loading Weather SkyDome Model - Cloudy2.bin");
    }

    if(SkyDomes[1].Load(Pod, "CClouds.bin")) {
        Logger->Text("Loaded Weather SkyDome Model - CClouds.bin");
    } else {
        Logger->Error("Error Loading Weather SkyDome Model - CClouds.bin");
    }

    if(SkyDomes[2].Load(Pod, "DuskSky.bin")) {
        Logger->Text("Loaded Weather SkyDome Model - DuskSky.bin");
    } else {
        Logger->Error("Error Loading Weather SkyDome Model - DuskSky.bin");
    }

    if(SkyDomes[3].Load(Pod, "NiteSky.bin")) {
        Logger->Text("Loaded Weather SkyDome Model - NiteSky.bin");
    } else {
        Logger->Error("Error Loading Weather SkyDome Model - NiteSky.bin");
    }

}

void CWeather::ApplyTint(void)
{
    glLightfv(GL_LIGHT0, GL_AMBIENT, &GeneralTint[0]);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, &GeneralTint[0]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, &glm::vec4(1.0f)[0]);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    if (Config->Render.Lighted) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }

}

void CWeather::ApplyBackdropTint(void)
{
    glDisable(GL_LIGHTING);
    glColor4f(BackdropTint.r, BackdropTint.g, BackdropTint.b, BackdropTint.a);

}

void CWeather::ApplyWaterTint(void)
{
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glColor4f(WaterTint.r, WaterTint.g, WaterTint.b, WaterTint.a);

}
