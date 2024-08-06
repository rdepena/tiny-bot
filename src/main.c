#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

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

///Serial related: TODO extract to a separate file
int set_interface_attribs(int fd, int speed) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // 8-bit chars
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; 
    // disable break processing
    tty.c_iflag &= ~IGNBRK;
    // no signaling chars, no echo,
    // no canonical processing
    tty.c_lflag = 0;
    // no remapping, no delays
    tty.c_oflag = 0;
    // read doesn't block
    tty.c_cc[VMIN]  = 0;
    // 0.5 seconds read timeout
    tty.c_cc[VTIME] = 5;
    // shut off xon/xoff ctrl
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    // ignore modem controls,
    // enable reading
    tty.c_cflag |= (CLOCAL | CREAD);
    // shut off parity
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag |= 0;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

// Set the blocking mode of the serial port
// TODO: extract to a separate file
void set_blocking(int fd, int should_block) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
        perror("tcsetattr");
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
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    set_interface_attribs(fd, B9600);  // set speed to 9600 bps, 8n1 (no parity)
    set_blocking(fd, 0);                // set no blocking

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

        char buf[100];
        // Run every half a second(e.g., every second)
        if (frame_start % 100 < FRAME_DELAY) {
            int n = read(fd, buf, sizeof(buf) - 1); // read up to 100 characters if ready to read
            if (n > 0)
            {
                buf[n] = '\0'; // Null-terminate the string

                // Parse the received data as a float
                int distance = atoi(buf);
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
            //vertical_shift = rand() % (SCREEN_HEIGHT - 100) + 50;
 
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
    close(fd);

    return 0;
}
