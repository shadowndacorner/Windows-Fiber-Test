#include "frame_data.h"
#include "fiber_scheduler.h"
#include <new>

flib::FrameParams::FrameParams():
	global_allocator(0),m_frameNum(0)
{

}

flib::FrameParams::FrameParams(const uint32_t& fnum)
	:global_allocator(fnum), m_frameNum(fnum)
{
	
}

flib::FrameParams::~FrameParams()
{
	
}

namespace flib
{
	static thread_local uint32_t cur_frame = 0;
	FrameParams params[sizeof(FrameParams) * FRAMES_TO_TRACK];

	bool HasFrameCompleted(uint32_t frame)
	{
		return GetFrameData(frame)->m_frameNum < frame || GetFrameData(frame)->m_hasRendered;
	}

	uint32_t GetCurrentFrameNumber()
	{
		if (flib::fiber_util::flIsFiber())
		{
			auto ptr = flib::fiber::get_active_data();
			return ptr->frame_number;
		}
		return cur_frame;
	}

	FrameParams* GetFrameData(uint32_t frame)
	{
		return &params[frame % FRAMES_TO_TRACK];
	}

	FrameParams* GetActiveFrameData()
	{
		return GetFrameData(GetCurrentFrameNumber());
	}

	void InitializeFrame(const uint32_t& frame_num, const double& dt)
	{
		cur_frame = frame_num;
		auto frame = GetFrameData(frame_num);
		if (flib::fiber_util::flIsFiber())
			flib::fiber::get_active_data()->frame_number = frame_num;

		frame->~FrameParams();
		new(frame) FrameParams(frame_num);
		frame->m_frameNum = frame_num;
		frame->m_stage = FrameStage::Game;
		frame->startTimes[FrameStage::Game] = 0;	// TODO: get time
		frame->deltaTime = dt;
	}

	void TransitionToStage(const FrameStage& stage)
	{
		auto frame = GetActiveFrameData();
		frame->endTimes[frame->m_stage] = 0;	// TODO: get time
		frame->m_stage = stage;
		frame->startTimes[stage] = 0;			// TODO: get time
	}
}