#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include "serial.h"
#include "find_arduino.h"
#include <SDL2/SDL_ttf.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;
const int GRID_SIZE = 50;
const int GRID_WIDTH_PADDING = 200; //Get some space for the buttons
const int LINE_THICKNESS = 3;

SDL_Color vibrant_green = {50, 200, 100, 255};

typedef struct {
    SDL_Rect rect;   // Position and size
    SDL_Color color; // Button color
    char label[20];  // Button text
} Button;

void draw_button(SDL_Renderer *renderer, Button button, TTF_Font *font) {
    // Set border color
    SDL_SetRenderDrawColor(renderer, button.color.r, button.color.g, button.color.b, button.color.a);

    // Draw multiple rectangles to create a thick border
    for (int i = 0; i < LINE_THICKNESS; i++) {
        SDL_Rect border_rect = {
            button.rect.x + i, 
            button.rect.y + i, 
            button.rect.w - 2 * i, 
            button.rect.h - 2 * i
        };
        SDL_RenderDrawRect(renderer, &border_rect);
    }

    // Draw button text
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, button.label, vibrant_green);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Rect textRect = {
        button.rect.x + (button.rect.w - text_surface->w) / 2,
        button.rect.y + (button.rect.h - text_surface->h) / 2,
        text_surface->w,
        text_surface->h
    };

    SDL_RenderCopy(renderer, text_texture, NULL, &textRect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}


void draw_grid(SDL_Renderer *renderer, int grid_size) {
    int width, height;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    width = width - GRID_WIDTH_PADDING; //Get some space for the buttons
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
    width = width - GRID_WIDTH_PADDING; //Get some space for the buttons

    // Set the draw color to red for the trace line
    SDL_SetRenderDrawColor(renderer, 50, 200, 100, 255);

    // Draw horizontal trace line
    // Draw multiple rectangles to create a thick border
    for (int i = 0; i < LINE_THICKNESS; i++) {
        SDL_RenderDrawLine(renderer, 0, y+i, width, y+i);
    }

}

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    //initialize the random number generator
    srand(time(NULL));

    //check for font support
    if (TTF_Init() == -1)
    {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("assets/fonts/joystix-monospace.otf", 24);
    if (!font)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    //arduino port name will change
    char *arduino_port = find_arduino();
    if (arduino_port == NULL) {
        printf("Arduino not found\n");
        return 1;
    }

    // Open the serial port
    int fd = open_port(arduino_port);

    // Variables to track time and state
    Uint32 frame_start, frame_time;
    int vertical_shift = 0;

    // Create a window
    SDL_Window *win = SDL_CreateWindow("TinyBot Remote", 
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

    // Command buttons
    Button btn_sit = {{630, 50, 150, 50}, {50, 200, 100, 255}, "Sit"};
    Button btn_stand = {{630, 150, 150, 50}, {50, 200, 100, 255}, "Stand"};
    Button btn_wave = {{630, 250, 150, 50}, {50, 200, 100, 255}, "Wave"};
    Button btn_lay_down = {{630, 350, 150, 50}, {50, 200, 100, 255}, "Lay"};
    Button btn_stretch = {{630, 450, 150, 50}, {50, 200, 100, 255}, "Stretch"};

    while (!quit) {
        frame_start = SDL_GetTicks();

        // Handle events
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouse_x = e.button.x, mouse_y = e.button.y;

                //TOOD: Refactor this to a function
                if (SDL_PointInRect(&(SDL_Point){mouse_x, mouse_y}, &btn_sit.rect))
                {
                    printf("You clicked sit!\n");
                    write_to_port(fd, "sit\n");
                }
                else if (SDL_PointInRect(&(SDL_Point){mouse_x, mouse_y}, &btn_stand.rect))
                {
                    printf("You clicked stand!\n");
                    write_to_port(fd, "stand\n");
                }
                else if (SDL_PointInRect(&(SDL_Point){mouse_x, mouse_y}, &btn_wave.rect))
                {
                    printf("You clicked wave!\n");
                    write_to_port(fd, "wave\n");
                }
                else if (SDL_PointInRect(&(SDL_Point){mouse_x, mouse_y}, &btn_lay_down.rect))
                {
                    printf("You clicked lay down!\n");
                    write_to_port(fd, "lay-down\n");
                }
                else if (SDL_PointInRect(&(SDL_Point){mouse_x, mouse_y}, &btn_stretch.rect))
                {
                    printf("You clicked walk!\n");
                    write_to_port(fd, "stretch\n");
                }
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
                //TODO Apply scaling factor to distance
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

        draw_button(ren, btn_sit, font);
        draw_button(ren, btn_stand, font);
        draw_button(ren, btn_wave, font);
        draw_button(ren, btn_lay_down, font);
        draw_button(ren, btn_stretch, font);

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
