/**
 * @file main.c
 * @brief KALAH Game - Main Entry Point
 * 
 * Entry point for KALAH CLI application
 */

#include "cli_interface.h"

int main(int argc, char *argv[]) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;
    
    // Create CLI context with 4 stones per pile (standard rules)
    CLIContext *ctx = cli_create(4);
    
    if (!ctx) {
        fprintf(stderr, "Error: Could not initialize game!\n");
        return EXIT_FAILURE;
    }
    
    // Run the game
    cli_run(ctx);
    
    // Cleanup
    cli_destroy(ctx);
    
    return EXIT_SUCCESS;
}
