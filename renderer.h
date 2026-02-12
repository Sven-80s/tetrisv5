#ifndef RENDERER_H
#define RENDERER_H

#include "game.h"

/** @file renderer.h
 *  @brief ncurses-basierte Rendering-Funktionen
 */

/** Zeichen für Block */
#define BLOCK_CHAR '#'

/** Zeichen für leere Zelle */
#define EMPTY_CHAR ' '

/** Zeichen für Board-Rahmen */
#define BORDER_CHAR ACS_VLINE

/**
 * @brief Initialisiert ncurses und die Farben
 * @return true bei Erfolg, false bei Fehler
 */
bool renderer_init(void);

/**
 * @brief Beendet ncurses und stellt das Terminal wieder her
 */
void renderer_cleanup(void);

/**
 * @brief Rendert den kompletten Spielbildschirm
 * @param game Pointer auf die Game-Struktur
 */
void renderer_render(const Game *game);

/**
 * @brief Rendert das Spielfeld mit dem aktuellen Tetromino
 * @param game Pointer auf die Game-Struktur
 * @param start_y Y-Startposition für Rendering
 * @param start_x X-Startposition für Rendering
 */
void renderer_render_board(const Game *game, int start_y, int start_x);

/**
 * @brief Rendert das aktuelle Tetromino auf das virtuelle Display
 * @param game Pointer auf die Game-Struktur
 * @param display Das virtuelle Display-Array
 */
void renderer_render_piece_to_display(const Game *game, int display[BOARD_HEIGHT][BOARD_WIDTH]);

/**
 * @brief Rendert die Seitenleiste mit Score, Level, etc.
 * @param game Pointer auf die Game-Struktur
 * @param start_y Y-Startposition für Rendering
 * @param start_x X-Startposition für Rendering
 */
void renderer_render_sidebar(const Game *game, int start_y, int start_x);

/**
 * @brief Rendert das nächste Tetromino in der Vorschau
 * @param next Pointer auf das nächste Tetromino
 * @param start_y Y-Startposition für Rendering
 * @param start_x X-Startposition für Rendering
 */
void renderer_render_next_piece(const Tetromino *next, int start_y, int start_x);

/**
 * @brief Rendert das Pause-Menü
 */
void renderer_render_pause(void);

/**
 * @brief Rendert den Game Over Bildschirm
 * @param final_score Der finale Score
 */
void renderer_render_game_over(int final_score);

/**
 * @brief Rendert die Steuerungshilfe
 * @param start_y Y-Startposition für Rendering
 * @param start_x X-Startposition für Rendering
 */
void renderer_render_help(int start_y, int start_x);

/**
 * @brief Löscht den Bildschirm und zeichnet alles neu
 * @param game Pointer auf die Game-Struktur
 */
void renderer_refresh_screen(const Game *game);

/**
 * @brief Gibt die Farbe für einen Board-Wert zurück
 * @param value Der Wert (0-7)
 * @return Das ncurses-Farbpaar
 */
int renderer_get_color_pair(int value);

/**
 * @brief Zeigt eine Fehlermeldung an und beendet das Programm
 * @param message Die Fehlermeldung
 */
void renderer_show_error(const char *message);

#endif /* RENDERER_H */
