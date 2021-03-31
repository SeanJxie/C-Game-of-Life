#include <stdio.h>
#include <memory.h>
#include <SDL.h>

// Define some constants
#define WINWT 1000
#define WINHT 1000
#define WINTT "CGOL"

#define LWINWT 200
#define LWINHT 200

// Store cells in 2d array
static int cellStates[LWINHT][LWINWT];
static const int INTERVAL = 0; // In ms



void render_cells(SDL_Renderer* r) {
	for (int i = 0; i < LWINHT; i++) {
		for (int j = 0; j < LWINWT; j++) {
			if (cellStates[i][j]) {
				SDL_SetRenderDrawColor(r, 0, 255, 0, SDL_ALPHA_OPAQUE);
			}
			else if (i == 0 || j == 0 || i == LWINWT - 1 || j == LWINHT - 1) {
				SDL_SetRenderDrawColor(r, 255, 0, 0, SDL_ALPHA_OPAQUE);

			}
			else {
				SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
			}

			SDL_RenderDrawPoint(r, i, j);
		}
	}
}


void update_cells() {
	// We don't want changes made in the array to affect other changes during the same time period
	static int temp[LWINHT][LWINWT];
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
				cellStates[i][j] = 0;
			}
			
			else if (!temp[i][j] && living == 3) {
				cellStates[i][j] = 1;
			}
		}
	}
}


int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* hWin = SDL_CreateWindow(WINTT, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINWT, WINHT, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* hRend = SDL_CreateRenderer(hWin, -1, SDL_RENDERER_SOFTWARE);

	SDL_RenderSetLogicalSize(hRend, LWINWT, LWINHT);
	SDL_Event event;

	printf("GRID DIMENSIONS: (%d, %d)\n", LWINWT, LWINHT);
	// Init and fill with 0
	printf("Loading array\n");
	for (int i = 0; i < LWINHT; i++) {
		for (int j = 0; j < LWINWT; j++) {
			cellStates[i][j] = 0;
		}
	}

	// r-pentomino
	/*int mid = (int)LWINWT / 2 - 1;
	cellStates[mid][mid] = 1;
	cellStates[mid][mid + 1] = 1;
	cellStates[mid][mid - 1] = 1;
	cellStates[mid - 1][mid] = 1;
	cellStates[mid + 1][mid - 1] = 1;*/

	
	printf("Load complete\n");

	int bRunProg = 1;
	int bRunSim = 0;

	Uint32 tSum = 0;
	Uint32 lastTime = 0;
	Uint32 currTime = 0;

	int mPosX, mPosY;
	int genCount = 0;

	printf("GENERATION: %d | INITIAL\n", genCount);

	while (bRunProg) {
		SDL_GetMouseState(&mPosX, &mPosY);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: bRunProg = 0; break;
			case SDL_MOUSEBUTTONDOWN: 
				// Normalize and scale mouse pos
				cellStates[(int)((float)mPosX / WINWT * LWINWT)][(int)((float)mPosY / WINHT * LWINHT)] ^= 1;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_SPACE: bRunSim = !bRunSim; break;
				case SDLK_RIGHT:
					// Manual iterate
					if (!bRunSim) {
						genCount++;
						printf("GENERATION: %d | MANUAL\n", genCount);
						update_cells(cellStates);
						tSum = 0;
					}
					break;
				case SDLK_ESCAPE: bRunProg = 0; break;
				}
				break;
			}
		}

		SDL_SetRenderDrawColor(hRend, 0, 0, 0, SDL_ALPHA_OPAQUE);
		render_cells(hRend);
		SDL_RenderPresent(hRend);

		if (bRunSim) {
			lastTime = currTime;
			currTime = SDL_GetTicks();

			tSum += currTime - lastTime; // Time taken for one update

			if (tSum >= INTERVAL) { // Check when INTERVAL milliseconds have been reached
				genCount++;
				printf("GENERATION: %d | AUTO | DELAY(MS): %d\n", genCount, tSum);
				update_cells(cellStates);
				tSum = 0;
			}
		}
	}

	SDL_DestroyRenderer(hRend);
	SDL_DestroyWindow(hWin);
	SDL_Quit();

	return 0;
}