#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include "serial.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

void draw_grid(SDL_Renderer *renderer, int grid_size) {
    int width, height;
    SDL_GetRendererOutputSize(renderer, &width, &height);

    // Set the draw color to green for the grid lines
    SDL_SetRenderDrawColor(renderer, 19, 154, 67, 0);

    // Draw vertical lines
    for (int x = 0; x <= width; x += grid_size) {
        SDL_RenderDrawLine(renderer, x, 0, x, height);
    }

    // Draw horizontal lines
    for (int y = 0; y <= height; y += grid_size) {
        SDL_RenderDrawLine(renderer, 0, y, width, y);
    }
}

void draw_trace_line(SDL_Renderer *renderer, int y) {
    int width, height;
    SDL_GetRendererOutputSize(renderer, &width, &height);

    // Set the draw color to red for the trace line
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // Draw horizontal trace line
    SDL_RenderDrawLine(renderer, 0, y, width, y);

}

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    //initialize the random number generator
    srand(time(NULL));

    //arduino port name will change
    //TODO Figure out how to detect that
    const char *portname = "/dev/cu.usbmodem1201"; 

    // Open the serial port
    int fd = open_port(portname);

    // Variables to track time and state
    Uint32 frame_start, frame_time;
    int vertical_shift = 0;

    // Create a window
    SDL_Window *win = SDL_CreateWindow("USVisualizer", 
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Event loop
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        frame_start = SDL_GetTicks();

        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Clear the renderer
        SDL_SetRenderDrawColor(ren, 35, 31, 32, 0);
        SDL_RenderClear(ren);

        // Draw the grid
        draw_grid(ren, 50);

        // Run every half a second(e.g., every second)
        if (frame_start % 100 < FRAME_DELAY) {
            // Read from the serial port
            int distance = read_from_port(fd);
            if (distance > 0)
            {
                printf("Received: %d cm\n", distance);
                //Somehow the distance is not accurate, so we need to adjust it
                if (distance > SCREEN_HEIGHT || distance ==0)
                {
                    // If the distance is greater than the screen height, set the vertical shift to 50 to at least render a line.
                    vertical_shift = 50;
                }
                else
                {
                    vertical_shift = SCREEN_HEIGHT - (distance * 10);
                    printf("Screen Height: %d relative distance %d cm verticlal shift: %d \n", SCREEN_HEIGHT, distance, vertical_shift);
                }
            }
 
        }

        draw_trace_line(ren, vertical_shift);

        // Present the renderer (show everything we have drawn)
        SDL_RenderPresent(ren);

        // Cap the frame rate
        frame_time = SDL_GetTicks() - frame_start;
        if (FRAME_DELAY > frame_time) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    // Clean up
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    clean_serial(fd);

    return 0;
}
