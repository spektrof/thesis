#include <GL/glew.h> //GLEW = OpenGL Extinsion Wrangler Library

#include <SDL.h>	//SDL
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

#include "Visualization/vis.h"

int main(int argc, char* args[])
{
	//SDL initialization, say if we got an error
	if ( SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		std::cout << "[SDL indítás] Hiba: " << SDL_GetError() << "\n";
		return 1;
	}

	// Sets the number of bits stored colors, etc.
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // depth puffer

	//Window Create
	SDL_Window *win = 0;
	win = SDL_CreateWindow("MeshApprox!",
		100,									
		50,									
		800,									
		600,									
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (win == 0)
	{
		std::cout << "[Ablak létrehozása] Hiba: " << SDL_GetError() << "\n";
		return 1;
	}
	/////////////////////////////////
	//Create OpenGL context -> we can draw now
	SDL_GLContext context = SDL_GL_CreateContext(win);

	if (context == 0)
	{
		std::cout << "[OGL context létrehozása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
		return 1;
	}

	// megjelenítés: várjuk be a vsync-et
	SDL_GL_SetSwapInterval(1);

	// indítsuk el a GLEW-t
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "[GLEW] Hiba az inicializálás során!" << std::endl;
		return 1;
	}
	////////////////
	int glVersion[2] = { -1, -1 };
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

	if (glVersion[0] == -1 && glVersion[1] == -1)
	{
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(win);
		
		std::cout << "[OGL context create] False\n";
	}

	std::stringstream window_title;
	window_title << "MeshApprox ";
	SDL_SetWindowTitle(win, window_title.str().c_str());

	bool quit = false; //relates to running of our program
	SDL_Event ev; //msg what we should use

	QApplication ui(argc, args);
	Visualization app;

	if (!app.Init())
	{
			SDL_DestroyWindow(win);
			std::cout << "[app.Init] False!\n";
			return 1;
	}

	while (!quit)
	{
		
		while (SDL_PollEvent(&ev))
		{
				switch (ev.type)
				{
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					quit = ev.key.keysym.sym == SDLK_ESCAPE;
					app.KeyboardDown(ev.key);
					break;
				case SDL_MOUSEBUTTONDOWN:
					app.MouseDown(ev.button);
					break;
				case SDL_MOUSEBUTTONUP:
					app.MouseUp(ev.button);
					break;
				case SDL_MOUSEMOTION:
					app.MouseMove(ev.motion);
					break;
				case SDL_MOUSEWHEEL:
					app.MouseWheel(ev.wheel);
					break;
				case SDL_WINDOWEVENT:
					if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						app.Resize(ev.window.data1, ev.window.data2);
					}
					break;
				}
		}

		SDL_GL_SwapWindow(win);
		app.Update();
		app.Render();

	}

	ui.exit(0);
	SDL_Quit();


	return 0;
}