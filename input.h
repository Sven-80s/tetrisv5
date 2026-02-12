#ifndef INPUT_H
#define INPUT_H

#include "game.h"

/** @file input.h
 *  @brief Tastatureingabe-Verarbeitung
 */

/**
 * @brief Initialisiert die Eingabe (ncurses Einstellungen)
 */
void input_init(void);

/**
 * @brief Verarbeitet Tastatureingaben
 * @param game Pointer auf die Game-Struktur
 * @return true wenn das Spiel weiterläuft, false zum Beenden
 */
bool input_process(Game *game);

/**
 * @brief Prüft ob eine Taste gedrückt wurde (non-blocking)
 * @return true wenn Taste gedrückt
 */
bool input_has_key(void);

/**
 * @brief Liest eine Taste (non-blocking)
 * @return Der Tastencode oder ERR
 */
int input_get_key(void);

/**
 * @brief Handhabt die Pause-Taste
 * @param game Pointer auf die Game-Struktur
 */
void input_handle_pause(Game *game);

/**
 * @brief Handhabt die Bewegung nach links
 * @param game Pointer auf die Game-Struktur
 */
void input_handle_left(Game *game);

/**
 * @brief Handhabt die Bewegung nach rechts
 * @param game Pointer auf die Game-Struktur
 */
void input_handle_right(Game *game);

/**
 * @brief Handhabt die Bewegung nach unten
 * @param game Pointer auf die Game-Struktur
 */
void input_handle_down(Game *game);

/**
 * @brief Handhabt die Rotation
 * @param game Pointer auf die Game-Struktur
 * @param clockwise true für im Uhrzeigersinn
 */
void input_handle_rotate(Game *game, bool clockwise);

/**
 * @brief Handhabt den Hard Drop (sofortiges Fallenlassen)
 * @param game Pointer auf die Game-Struktur
 */
void input_handle_hard_drop(Game *game);

/**
 * @brief Handhabt das Beenden des Spiels
 * @param game Pointer auf die Game-Struktur
 */
void input_handle_quit(Game *game);

#endif /* INPUT_H */
