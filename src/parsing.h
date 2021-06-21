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

/** Definicja typu string */
typedef char *string;

/**
 * Analizuje wiersz tekstu i zwraca odpowiedni obiekt typu \a Action
 * @param[in] line : string opisujący wiersz
 * @param[in] line_length : liczba znaków w wierszu
 * @return obiekt typu \a Action
 */
Action ParseLine(string line, ssize_t line_length);

#endif //__PARSING_H__
