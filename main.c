#define _DEFAULT_SOURCE  /* Für usleep() */
#define _POSIX_C_SOURCE 200809L  /* Für sigaction */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#include "game.h"
#include "renderer.h"
#include "input.h"

/**
 * @file main.c
 * @brief Einstiegspunkt für das Tetris-Spiel
 */

/**
 * @brief Globale Variable für sauberes Beenden bei Signalen
 */
static volatile sig_atomic_t g_running = 1;

/**
 * @brief Signal-Handler für SIGINT und SIGTERM
 * @param sig Das empfangene Signal
 */
static void signal_handler(int sig) {
    (void)sig;
    g_running = 0;
}

/**
 * @brief Setup der Signal-Handler
 * @return true bei Erfolg, false bei Fehler
 */
static bool setup_signals(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        fprintf(stderr, "Error: Failed to setup SIGINT handler\n");
        return false;
    }
    
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        fprintf(stderr, "Error: Failed to setup SIGTERM handler\n");
        return false;
    }
    
    return true;
}

/**
 * @brief Hauptspiel-Schleife
 * @param game Pointer auf die Game-Struktur
 */
static void game_loop(Game *game) {
    if (game == NULL) return;
    
    while (g_running && game->running) {
        /* Eingabe verarbeiten */
        if (!input_process(game)) {
            break;
        }
        
        /* Spielzustand rendern */
        renderer_render(game);
        
        /* Nur updaten wenn nicht pausiert */
        if (game->state == STATE_PLAYING) {
            /* Verzögerung basierend auf Geschwindigkeit */
            usleep((useconds_t)(game->speed_ms * 1000));
            
            /* Spielzustand aktualisieren */
            game_update(game);
        } else {
            /* Kürzere Verzögerung im Pause- oder Game Over-Modus */
            usleep(50000);  /* 50ms */
        }
    }
}

/**
 * @brief Hauptfunktion
 * @param argc Anzahl der Argumente
 * @param argv Argumentvektor
 * @return EXIT_SUCCESS bei Erfolg, EXIT_FAILURE bei Fehler
 */
int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    Game game;
    
    /* Zufallsgenerator initialisieren */
    srand((unsigned int)time(NULL));
    
    /* Signal-Handler setup */
    if (!setup_signals()) {
        return EXIT_FAILURE;
    }
    
    /* ncurses initialisieren */
    if (!renderer_init()) {
        fprintf(stderr, "Error: Failed to initialize renderer\n");
        return EXIT_FAILURE;
    }
    
    /* Spiel initialisieren */
    if (!game_init(&game)) {
        renderer_cleanup();
        fprintf(stderr, "Error: Failed to initialize game\n");
        return EXIT_FAILURE;
    }
    
    /* Erstes Piece spawnen */
    if (!game_spawn_piece(&game)) {
        renderer_cleanup();
        fprintf(stderr, "Error: Failed to spawn first piece\n");
        return EXIT_FAILURE;
    }
    
    /* Eingabe initialisieren */
    input_init();
    
    /* Hauptspiel-Schleife */
    game_loop(&game);
    
    /* Aufräumen */
    game_cleanup(&game);
    renderer_cleanup();
    
    printf("Thanks for playing Tetris!\n");
    printf("Final Score: %d\n", game.score);
    
    return EXIT_SUCCESS;
}
