#include <Windows.h>

// Default to High Performance Mode on machines with dual graphics
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;	// AMD
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;		// NVIDIA
