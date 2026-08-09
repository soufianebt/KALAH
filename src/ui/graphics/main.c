/**
 * @file main.c
 * @brief KALAH Graphics - Main Entry Point (SDL2)
 * 
 * Entry point for KALAH SDL2 graphics application
 */

#define SDL_MAIN_HANDLED
#include "graphics_interface.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;
    
    printf("Initializing KALAH Graphics Interface...\n");
    
    // Create graphics context with 4 stones per pile (standard rules)
    GraphicsContext *ctx = graphics_create(4);
    
    if (!ctx) {
        fprintf(stderr, "Error: Could not initialize graphics!\n");
        return EXIT_FAILURE;
    }
    
    printf("Starting game loop...\n");
    
    // Run the game
    int result = graphics_run(ctx);
    
    // Cleanup
    graphics_destroy(ctx);
    
    printf("Goodbye!\n");
    return result;
}
