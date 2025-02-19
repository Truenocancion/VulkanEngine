#pragma once
#include "SDL2/SDL.h"

class CViewport{
	friend class CEngine;
	friend class VulkanRender;

	CViewport();
	
	~CViewport();
	
	void Update();

	SDL_Window* m_Window;
};