#pragma once
#include "Timer.h"

namespace Titan
{
	class Chrono
	{
	public:
		static float Timestep();
		static float TotalTime();

		static void UpdateTimeData();

	private:
		static Titan::Timer s_Timer;
	};
}


