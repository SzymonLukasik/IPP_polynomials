/** @file
  Interfejs kalkulatora wielomianów rzadkich wielu zmiennych.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#ifndef __CALC_H__
#define __CALC_H__

#include "poly.h"

// Enum określający dostepne polecenia
typedef enum CommandType {
    /* Polecenia DEG_BY oraz AT - jedyne, które wymagają parametrów
     * zostały wymienione jako ostatnie - dzięki temu możemy skorzystać
     * z wartości stałych opisujących polecenia bezparametrowe podczas
     * parsowania w pliku parsing.c w funkcji IsNonParamCommand */
    ZERO, IS_COEFF, IS_ZERO, CLONE, ADD, MUL,
    NEG, SUB, IS_EQ, DEG, PRINT, POP,
    DEG_BY, AT
} CommandType;

// Unia określająca parametr poleceń AT lub DEG_BY
typedef union CommandParam {
    poly_coeff_t x;
    size_t var_idx;
} CommandParam;

/* Struktura określająca polecenie wraz z argumentem
 * (jeśli polecenie go wymaga) */
typedef struct Command {
    CommandType type;
    union CommandParam param;
} Command;

// Enum określający obsługiwane błędy wejścia
typedef enum ErrorType {
    WRONG_COMMAND, DEG_BY_WRONG_VAR,
    AT_WRONG_VAL, WRONG_POLY
} ErrorType;

/* Enum określający rodzaj czynności,
 * jaką powinien wykonać program po wczytaniu wiersza */
typedef enum ActionType {
    COMMAND, // Wykonanie polecenia
    ERROR, // Obsłużenie błędu
    PUSH_POLY,  // Dodanie wielomianu na wierzchołek stosu
    IGNORE //Zignorowanie wiersza
} ActionType;

/* Unia zawierająca specyfikację danej czynności,
 * w zależności od jej rodzaju. */
typedef union ActionSpec {
    Command command;
    ErrorType error_type;
    Poly poly;
} ActionSpec;


/* Struktura przechowująca informację o rodzaju czynności,
 * jaką powinien wykonać program i jej argumencie */
typedef struct Action {
    ActionType type;
    ActionSpec spec;
} Action;

typedef enum MinimStackSize {
    ONE = 1, TWO = 2
} MinimStackSize;

typedef char* string;

#endif // __CALC_H__
