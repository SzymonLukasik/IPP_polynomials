/** @file
  Definicje typów służących do reprezentowania czynności, które
  program ma wykonywać oraz paru innych, przydatnych w implementacji.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#ifndef __CALC_H__
#define __CALC_H__

#include "poly.h"

/** Definicja typu string */
typedef char *string;

/** Enum określający dostepne polecenia. */
typedef enum CommandName {
    /* Polecenia AT, DEG_BY oraz COMPOSE- jedyne, które wymagają parametrów
     * zostały wymienione jako ostatnie - dzięki temu możemy skorzystać
     * z wartości stałych opisujących polecenia bezparametrowe podczas
     * parsowania w pliku parsing.c w funkcji IsNoParamCommand. */
    ZERO, IS_COEFF, IS_ZERO, CLONE, ADD, MUL,
    NEG, SUB, IS_EQ, DEG, PRINT, POP,
    AT, DEG_BY, COMPOSE
} CommandName;

/** Liczba poleceń bezparametrowych. */
static const size_t NO_PARAM_COMM_NUM = 12;

/** Tablica stringów zawierająca nazwy poleceń bezparametrowych
 *  z dodanym na końcu znakiem nowej lini.
 *  Kolejność nazw tych poleceń w tablicy odpowiada kolejności ich
 *  występowania w definicji typu CommandName.
 *  */
static const string NO_PARAM_COMM_NAMES[12] =
        {"ZERO\n", "IS_COEFF\n", "IS_ZERO\n", "CLONE\n",
         "ADD\n", "MUL\n", "NEG\n", "SUB\n", "IS_EQ\n",
         "DEG\n", "PRINT\n", "POP\n"};

/** Liczba poleceń rodzaju PushCommand - skutkujących dodaniem
 * wielomianu na stos . */
static const size_t PUSH_COMM_NUM = 8;

/** Tablica zawierająca te wartości typu CommandName, które opisują
 *  polecenia rodzaju PushCommand */
static const CommandName PUSH_COMMANDS[8] =
        {ZERO, CLONE, ADD, MUL, NEG, SUB, AT, COMPOSE};

/** Unia określająca parametr poleceń AT lub DEG_BY. */
typedef union CommandParam {
    poly_coeff_t x; ///< Parametr polecenia AT
    size_t var_idx; ///< Parametr polecenia DEG_BY
    size_t k; ///< Parametr polecenia COMPOSE
} CommandParam;

/** Struktura określająca nazwę polecenia wraz z parametrem
 * (jeśli polecenie go wymaga). */
typedef struct Command {
    CommandName name; ///< Nazwa
    union CommandParam param; ///< Parametr
} Command;

/** Enum określający obsługiwane błędy wejścia. */
typedef enum InputError {
    WRONG_COMMAND, DEG_BY_WRONG_VARIABLE,
    AT_WRONG_VALUE, COMPOSE_WRONG_PARAMATER, WRONG_POLY
} InputError;

/** Liczba obsługiwanych błędów wejścia. */
static const size_t INPUT_ERR_NUM = 5;
/** Tablica stringów zawierająca nazwy błędów wyjścia.
 *  Kolejność nazw tych błędów w tablicy odpowiada kolejności ich
 *  występowania w definicji typu InputError. */
static const string INPUT_ERR_NAMES[5] =
        {"WRONG COMMAND", "DEG BY WRONG VARIABLE",
         "AT WRONG VALUE", "COMPOSE_WRONG_PARAMATER", "WRONG POLY"};

/** Enum określający rodzaj czynności,
 * jaką powinien wykonać program po wczytaniu wiersza. */
typedef enum ActionType {
    COMMAND, // Wykonanie polecenia
    INPUT_ERROR, // Obsłużenie błędu wejścia
    PUSH_POLY,  // Dodanie wielomianu na wierzchołek stosu
    IGNORE // Zignorowanie wiersza
} ActionType;

/** Unia zawierająca specyfikację danej czynności,
 * w zależności od jej rodzaju. */
typedef union ActionSpec {
    Command command; ///< Polecenie
    InputError error; ///< Błąd wejścia
    Poly poly; ///< Wielomian
} ActionSpec;

/** Struktura przechowująca informację o rodzaju czynności,
 * jaką powinien wykonać program i jej specyfikacji. */
typedef struct Action {
    ActionType type; ///< Rodzaj czynności
    ActionSpec spec; ///< Specyfikacja czynności
} Action;

#endif // __CALC_H__
