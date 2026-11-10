#pragma once

#include <tracy/Tracy.hpp>

// Empty unless the project is built with TRACY_ENABLE.
#define D_PROFILE_FRAME() FrameMark
#define D_PROFILE_FUNCTION() ZoneScoped
#define D_PROFILE_SCOPE(name) ZoneScopedN(name)

#ifdef TRACY_ENABLE
	#define D_PROFILE_THREAD(name) tracy::SetThreadName(name)
#else
	#define D_PROFILE_THREAD(name)
#endif
