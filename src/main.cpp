#include <SDL2/SDL.h>
#include "engine.h"

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO);
	const int result = CEngine::Run();
	SDL_Quit();
	return result;
}