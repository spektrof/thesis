/*Keszitette: Lukacs Peter

  Megjelenito felulet elokeszitese, majd a program hasznalata.
*/

#include <GL/glew.h> //GLEW = OpenGL Extinsion Wrangler Library

#include<iostream>
#include<sstream>

#include "Visualization/vis.h"

int main(int argc, char* args[])
{
	QDir dir("Logger");
	if (!dir.exists()) 	dir.mkdir(".");

	//SDL incializalas
	if ( SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		LOG("[SDL ind�t�s] Hiba: " << SDL_GetError() << "\n");
		return 1;
	}

	// A tarolt szin bittulajdonsagainak a beallitasa
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // depth puffer

	//ablak letrehozasa
	SDL_Window *win = 0;
	win = SDL_CreateWindow("MeshApprox!",
		100, 50,			//x,y						
		800, 600,			//width, height						
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (win == 0)
	{
		LOG("[Ablak l�trehoz�sa] Hiba: " << SDL_GetError() << "\n");
		return 1;
	}
	/////////////////////////////////
	//Context keszites, �gy mar tudunk rajzolni
	SDL_GLContext context = SDL_GL_CreateContext(win);

	if (context == 0)
	{
		LOG("[OGL context l�trehoz�sa]Hiba az SDL inicializ�l�sa k�zben: " << SDL_GetError() << std::endl);
		return 1;
	}

	// megjelen�t�s: v�rjuk be a vsync-et
	SDL_GL_SetSwapInterval(1);

	// ind�tsuk el a GLEW-t
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		LOG("[GLEW] Hiba az inicializ�l�s sor�n!" << std::endl);
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
		LOG("[OGL context create] False\n");
	}

	std::stringstream window_title;
	window_title << "MeshApprox ";
	SDL_SetWindowTitle(win, window_title.str().c_str());

	bool quit = false;
	SDL_Event ev;

	QApplication ui(argc, args);
	Visualization app;

	if (!app.Init())
	{
			SDL_DestroyWindow(win);
			LOG("[app.Init] False!\n");
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
		
		SDL_SetWindowTitle(win, (window_title.str() + " distance: " + std::to_string(app.GetDistance())).c_str() );
	}

	ui.exit(0);
	SDL_Quit();

	return 0;
}