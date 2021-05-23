/** @file
  Biblioteka umożliwiająca parsowanie wiersza tekstu
  w obiekt typu \a Action, czyli informację o rodzaju czynności,
  którą powinien wykonać program po wczytaniu wiersza oraz jej
  argumentach.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/
#define _GNU_SOURCE


#ifndef __PARSING_H__
#define __PARSING_H__

#include <sys/types.h>
#include "calc.h"

// Definicje stałych opisujących dozwolone wartości wykładnika jednomianu
#define MIN_EXP 0
#define MAX_EXP 2147483647

#define COEFF_FIRST_CHARS "-0123456789"

/**
 * Analizuje wiersz tekstu i zwraca odpowiedni obiekt typu \a Action
 * @param[in] line : string opisujący wiersz
 * @param[in] line_length : liczba znaków w wierszu
 * @return obiekt typu \a Action
 */
Action ParseLine(string line, ssize_t line_length);

#endif //__PARSING_H__
