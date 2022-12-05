#include "TNpch.h"
#include "Chrono.h"

Titan::Timer Titan::Chrono::s_Timer;
float Titan::Chrono::Timestep()
{
	return s_Timer.GetDeltaTime();
}

float Titan::Chrono::TotalTime()
{
	return static_cast<float>(s_Timer.GetTotalTime());
}

void Titan::Chrono::UpdateTimeData()
{
	s_Timer.Update();
}
