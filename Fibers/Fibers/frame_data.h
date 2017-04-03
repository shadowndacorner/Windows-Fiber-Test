#pragma once
#include <stdint.h>
#include <new>
#include <utility>
#include "tagged_linear_allocator.h"
#include "atomic_counter.h"
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
	public:
		FrameParams();
		FrameParams(const uint32_t& fnum);
		~FrameParams();

	public:
		template <typename T>
		T* allocate(const size_t& size);

		template<typename T, class... _Types>
		void construct(T* ptr, _Types&&... _Args);

		template<typename T>
		void destroy(T* ptr, ...);
		
		// Allocators
		tagged_linear_allocator<char> global_allocator;

		double deltaTime;
		// float3 camPos
		// float* skinningMatrices
		// std::vector<Mesh*> renderers;
		double startTimes[FrameStage::NumStages];
		double endTimes[FrameStage::NumStages];

		uint32_t m_frameNum;
		bool m_hasRendered;
		FrameStage m_stage;
	};

	bool HasFrameCompleted(uint32_t frame);
	uint32_t GetCurrentFrameNumber();
	FrameParams* GetFrameData(uint32_t frame);
	FrameParams* GetActiveFrameData();
	void InitializeFrame(const uint32_t& frame_num, const double& dt);
	void TransitionToStage(const FrameStage& stage);
	
	template<typename T>
	inline T * FrameParams::allocate(const size_t& size)
	{
		return reinterpret_cast<T*>(global_allocator.allocate(size * sizeof(T)));
	}

	template<typename T, class... _Types>
	inline void FrameParams::construct(T* ptr, _Types&&... _Args)
	{
		new(ptr) T(std::forward<_Types>(_Args)...);
	}

	template<typename T>
	inline void FrameParams::destroy(T* ptr, ...)
	{
		ptr->~T();
	}
}