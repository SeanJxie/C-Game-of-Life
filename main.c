#include <stdio.h>
#include <memory.h>
#include <SDL.h>

// Define some constants
#define WINWT 800
#define WINHT 800
#define WINTT "CGOL"

#define LWINWT 100
#define LWINHT 100

// Store cells in 2d array
static int cellStates[LWINWT][LWINHT];
static const int INTERVAL = 1;


void flip_cell(int i, int j) {
	// Set cell value to not itself
	cellStates[i][j] = !cellStates[i][j];
}


void render_cells(SDL_Renderer* r) {
	for (int i = 0; i < LWINHT; i++) {
		for (int j = 0; j < LWINWT; j++) {
			if (cellStates[i][j]) {
				SDL_SetRenderDrawColor(r, 255, 255, 255, SDL_ALPHA_OPAQUE);
			}
			else {
				SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
			}

			if (i == 0 || j == 0 || i == LWINWT - 1 || j == LWINHT - 1)
			{
				SDL_SetRenderDrawColor(r, 0, 255, 255, SDL_ALPHA_OPAQUE);
			}

			SDL_RenderDrawPoint(r, i, j);
		}
	}
}


void update_cells() {
	// We don't want changes made in the array to affect other changes during the same time period
	static int temp[LWINWT][LWINHT];
	memcpy(temp, cellStates, LWINWT * LWINHT * sizeof(int));

	// There is a border of permanently dead cells
	for (int i = 1; i < LWINHT - 1; i++) {
		for (int j = 1; j < LWINWT - 1; j++) {

			int living = 0;
			
			if (temp[i - 1][j]) living++;
			if (temp[i + 1][j]) living++;
			if (temp[i - 1][j - 1]) living++;
			if (temp[i][j - 1]) living++; 
			if (temp[i + 1][j - 1]) living++;
			if (temp[i - 1][j + 1]) living++;
			if (temp[i][j + 1]) living++;
			if (temp[i + 1][j + 1]) living++;
				

			if (temp[i][j] && !(living == 2 || living == 3)) {
				flip_cell(i, j);
			}
			
			else if (!temp[i][j] && living == 3) {
				flip_cell(i, j);
			}
		}
	}
}


int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* hWin = SDL_CreateWindow(WINTT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINWT, WINHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* hRend = SDL_CreateRenderer(hWin, -1, SDL_RENDERER_SOFTWARE);

	SDL_RenderSetLogicalSize(hRend, LWINWT, LWINHT);
	SDL_Event event;

	// Init and fill with 0
	printf("Loading array\n");
	for (int i = 0; i < LWINHT; i++) {
		for (int j = 0; j < LWINWT; j++) {
			cellStates[i][j] = rand() % 2 == 0;
		}
	}
	printf("Load complete\n");

	int bRunProg = 1;
	int bRunSim = 0;
	int nProgTicks = 0;

	int mPosX, mPosY;
	int bPressed = 0;

	while (bRunProg) {
		SDL_GetMouseState(&mPosX, &mPosY);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: bRunProg = 0; break;

			case SDL_MOUSEBUTTONDOWN:
				// Normalize and scale mouse pos
				bPressed = 1;
				break;

			case SDL_MOUSEBUTTONUP:
				bPressed = 0;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_SPACE:
					bRunSim = !bRunSim;
					break;
				}
				break;
			}
		}

		if (bPressed) {
			flip_cell((int)((float)mPosX / WINWT * LWINWT), (int)((float)mPosY / WINHT * LWINHT));
		}

		SDL_SetRenderDrawColor(hRend, 0, 0, 0, SDL_ALPHA_OPAQUE);
		render_cells(hRend);
		SDL_RenderPresent(hRend);

		if (bRunSim) {
			nProgTicks++;
			if (nProgTicks == INTERVAL) {
				update_cells(cellStates);
				nProgTicks = 0;
			}
		}
	}

	SDL_DestroyRenderer(hRend);
	SDL_DestroyWindow(hWin);
	SDL_Quit();

	return 0;
}
