#include "viewport.h"
#include "engine.h"

CViewport::CViewport() {
	m_Window = SDL_CreateWindow(
		"Vulkan engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1920,
		1080,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
}

CViewport::~CViewport() {
	SDL_DestroyWindow(m_Window);
}

void CViewport::Update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_CLOSE:
				gEngine->Exit();
				break;
			case SDL_WINDOWEVENT_RESIZED:
				gEngine->OnViewportResize(event.window.data1, event.window.data2);
				break;
			}
			break;
		}
	}
}