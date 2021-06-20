/** @file
  Biblioteka umożliwiająca parsowanie wiersza tekstu
  w wartość typu \a Action, czyli informację o rodzaju czynności,
  którą powinien wykonać program po wczytaniu wiersza oraz jej
  specyfikacji.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#ifndef __PARSING_H__
#define __PARSING_H__

#include <sys/types.h>
#include "calc.h"

/** Enum określający parsowane liczby. */
typedef enum NumberType {
    EXP, ULL, COEFF
} NumberType;

/** Definicje makra opisującego minimalną wartość wykładnika jednomianu. */
#define MIN_EXP 0

/** Definicje makra opisującego maksymalną wartość wykładnika jednomianu. */
#define MAX_EXP 2147483647

/**
 * Analizuje wiersz tekstu i zwraca odpowiedni obiekt typu \a Action
 * @param[in] line : string opisujący wiersz
 * @param[in] line_length : liczba znaków w wierszu
 * @return obiekt typu \a Action
 */
Action ParseLine(string line, ssize_t line_length);

#endif //__PARSING_H__
