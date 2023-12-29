#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - Patrick Nellessen 2GD10	",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);
	std::cout << "Cycling filter mode also has a mode showing vertex normals" << '\n';
	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					const std::string& val{ pRenderer->CycleFilterMode() };
					std::cout << "Cycled filter mode to " << val << '\n';
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					pRenderer->CycleDoRotation();
					std::cout << "Cycled rotation" << '\n';
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F6)
				{
					pRenderer->CycleNormalMapping();
					std::cout << "Cycled normal mapping" << '\n';
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_F7)
				{
					pRenderer->CycleRenderFireFX();
					std::cout << "Cycled fire FX" << '\n';
				}
				else if (e.key.keysym.scancode == SDL_SCANCODE_LSHIFT && e.key.repeat == 0)
				{
					pRenderer->GetCamera().boost = !pRenderer->GetCamera().boost;
					std::cout << "Cycled boost" << '\n';
				}
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}