
#ifndef WEATHER_HPP
#define WEATHER_HPP

#include "Models.hpp"

namespace Game
{

class CWeather
{

private:

    glm::vec4 GeneralTint;
    glm::vec4 BackdropTint;
    glm::vec4 WaterTint;
    glm::vec4 FogColor;

    int DomeIndex;
    Game::CModel SkyDomes[4];

public:

    CWeather(void);
    ~CWeather(void);

    void Create(CPod &Pod);
    void Destroy(void);
    void SetWeather(int Index);

    void ApplyTint(void);
    void ApplyBackdropTint(void);
    void ApplyWaterTint(void);

    void DrawSkyDome(void)
    {
        if (DomeIndex > -1) {
            glCallList(SkyDomes[DomeIndex].GetModel());
        }
    }

};

}

#endif
