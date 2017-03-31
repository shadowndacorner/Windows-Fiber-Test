#pragma once
#include <stdint.h>
#define FRAMES_TO_TRACK 16

namespace flib
{
	enum FrameStage
	{
		Game = 1 << 0,
		Render = 1 << 1,
		GPU = 1 << 2,
		Flip = 1 << 3,
		NumStages = 4
	};

	struct FrameParams
	{
		double deltaTime;
		// float3 camPos
		// float* skinningMatrices
		// std::vector<Mesh> renderers;
		double startTimes[FrameStage::NumStages];
		double endTimes[FrameStage::NumStages];
		
		bool hasRendered;
		uint32_t frameNumber;
	};

	FrameParams params[FRAMES_TO_TRACK];

	static thread_local FrameParams* current;
	static bool HasFrameCompleted(uint32_t frame)
	{
		return params[frame % FRAMES_TO_TRACK].hasRendered;
	}
}