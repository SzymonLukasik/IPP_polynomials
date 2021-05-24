/** @file
  Implementacja biblioteki umożliwiającej parsowanie wiersza tekstu
  w obiekt typu \a Action, czyli informację o rodzaju czynności,
  którą powinien wykonać program po wczytaniu wiersza oraz jej
  specyfikacji.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/
#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "parsing.h"
#include "calc.h"
#include "mono_stack.h"

/**
 * Sprawdza czy pierwszy znak fragmentu lini jest poprawny w przypadku
 * gdy interpretujemy ten fragment jako liczbę odpowiedniego typu.
 * @param[in] line : fragment lini
 * @param[in] type : typ liczby
 * @return Czy pierwszy znak fragmentu lini jest poprawny w przypadku
 *         gdy interpretujemy ten fragment jako liczbę odpowiedniego typu?
 */

static bool NumberFirstCharCorrect(string line, NumberType type) {
    if (type == EXP || type == VAR_IDX)
        return (line[0] >= '0' && line[0] <= '9');
    else // Wiemy, że type == COEFF
        return ((line[0] >= '0' && line[0] <= '9') || line[0] == '-');
}

/**
 * Dokonuje parsowania fragmentu lini jako liczby określonego typu.
 * W przypadku gdy wskaźnik end_ptr jest różny od NULL oraz parsowanie
 * zakończyło się sukcesem, modyfikuje ten wskaźnik tak aby wskazywał
 * na pierwszy niezinterpretowany znak występujący po liczbie.
 * @param[in] line : fragment lini
 * @param[in] type : typ parsowanej liczby
 * @param[out] number_ptr : wskaźnik na liczbę, której w przypadku powodzenia,
 *                     zostanie przypisana sparsowana wartość, a w przypadku
 *                     nie powodzenia - 0.
 * @param[out] end_ptr : wskaźnik na stringa - gdy parsowanie się uda
 *                       oraz przy wowałaniu end_ptr jest różny od NULL,
 *                       funkcja modyfikuje go tak aby wskazywał na pierwszy
 *                       niezinterpretowany znak występujący po liczbie.
 */
static void NumberParse(string line, NumberType type, void *number_ptr,
                        string *end_ptr) {
    if (type == EXP)
        *((poly_exp_t *) number_ptr) = (int) strtol(line, end_ptr, 10);
    else if (type == VAR_IDX)
        *((size_t *) number_ptr) = strtoul(line, end_ptr, 10);
    else // Wiemy, że type == COEFF
        *((poly_coeff_t *) number_ptr) = strtoll(line, end_ptr, 10);
}

/**
 * Sprawdza czy parsowanie zakończyło się sukcesem i jeśli tak, czy
 * wynik parsowania mieści się w odpowiednim przedziale wartości w
 * zależności od typu.
 * @param[in] type : argument określający typ parsowanej liczby.
 * @param[in] number_ptr : wskaźnik na liczbę.
 * @return Czy parsowanie liczby danego typu zakończyło się powodzeniem?
 */
static bool ParsingSuccessful(NumberType type, void *number_ptr) {
    /* Wartość  parametru polecenia parametru polecenia AT oraz współczynnika
     * wielomianu jest ograniczona przez zakres wartości typu long long,
     * polecenia DEG_BY - przez zakres wartości typu unsigned long.
     * Dlatego w przypadku typu COEFF oraz VAR_IDX wystarczy sprawdzić czy
     * funkcja parsująca nie ustawiła wartości errno na ERANGE lub EINVAL */

    if (errno != 0)
        return false;

    /** W przypadku typu EXP, należy również sprawdzić czy wynik parsowania
     *  mieści się w zadanych w specyfikacji zadania przedziałach. */
    if (type == EXP) {
        poly_exp_t exp = *(poly_exp_t *) number_ptr;
        if (exp < MIN_EXP || exp > MAX_EXP)
            return false;
    }

    return true;
}

/**
 * Dokonuje próby parsowania fragmentu lini jako liczby typu opisanego przez
 * argument type.
 * Jeśli się ona udała oraz pierwszy niezinterpretowany znak jest
 * równy argumentowi end_char, zwraca prawdę oraz przypisuje zmiennej
 * number_ptr odpowiednią wartość.
 * W przeciwnym przypadku zwraca fałsz.
 * Funkcja modyfikuje argument end_ptr jeśli jest różny od NULL -
 * gdy zwracana przez funkcję wartość to prawda oraz end_ptr != NULL
 * - zmienna end_ptr wskazuje na pierwszy niezinterpretowany znak
 * występujący po liczbie.
 * @param[in] line : fragment lini
 * @param[in] type : typ parsowanej liczby.
 * @param[out] number_ptr : wskaźnik na liczbę, której jeśli funkcja zwróci
 *                          prawdę, zostanie przypisana sparsowana wartość.
 * @param[out] end_ptr : wskaźnik na stringa - jeśli funkcja zwraca prawdę,
 *                       oraz przy wowałaniu end_ptr jest różny od NULL,
 *                       funkcja modyfikuje go tak aby wskazywał na pierwszy
 *                       niezinterpretowany znak występujący po liczbie.
 * @param[in] end_char : oczekiwana wartość pierwszego niezinterpretowanego znaku.
 * @return Czy fragment lini jest możliwy do zinterpretowania jako liczba danego typu
 *         oraz pierwszy niezinterpretowany znak jest równy
 *         argumentowi end_char?
 */
static bool NumberTryToParse(string line, NumberType type, void *number_ptr,
                             string *end_ptr, char end_char) {
    // Sprawdzamy poprawność pierwszego znaku fragmentu lini
    if (line == NULL || !NumberFirstCharCorrect(line, type))
        return false;

    errno = 0;
    /* Jeśli end_ptr == NULL zastępujemy go wskaźnikiem na line
     * aby móc później zweryfikować poprawność pierwszego niezinterpretowanego
     * znaku. Gdy end_ptr != NULL, funkcja parsująca go modyfikuje. */
    if (end_ptr == NULL)
        end_ptr = &line;

    /* Dokonujemy parsowania i zwracamy informację o jego powodzeniu
     * i poprawności ostatniego niezinterpretowanego znaku. */
    NumberParse(line, type, number_ptr, end_ptr);
    return (ParsingSuccessful(type, number_ptr) && (*end_ptr)[0] == end_char);
}

static bool MonoSumTryToParse(string *line, MonoStack *stack, char end_char);

/**
* Dokonuje próby parsowania fragmentu lini jako wielomianu.
* Jeśli się ona udała oraz pierwszy niezinterpretowany znak jest
* równy argumentowi end_char, zwraca prawdę oraz przypisuje zmiennej
* poly odpowiedni wielomian.
* W przeciwnym przypadku zwraca fałsz.
* Funkcja modyfikuje argument line - jeśli zwracana przez funkcję wartość
* to prawda - zmienna line wskazuje na pierwszy niezinterpretowany znak
* występujący po wielomianie.
* @param[in, out] line : wskaźnik na parsowany fragment lini - jeśli funkcja
*                       zwraca prawdę, wskazuje na pierwszy niezinterpretowany
*                       znak występujący po wielomianie.
* @param[out] poly : wskaźnik na zmienną typu \a Poly.
* @param[in] end_char : oczekiwana wartość pierwszego niezinterpretowanego znaku.
* @return Czy fragment lini jest możliwy do zinterpretowania jako wielomian
*         oraz pierwszy niezinterpretowany znak jest równy argumentowi end_char?
*/
static bool PolyTryToParse(string *line, Poly *poly, char end_char) {

    if ((*line)[0] == '(') { // Parsujemy sumę jednomianów bądź jednomian
        MonoStack stack = NewMonoStack();
        if (MonoSumTryToParse(line, &stack, end_char)) {
            *poly = PolyAddMonos(stack.size, stack.data);
            MonoStackDestroy(stack);
            return true;
        }
        MonoStackDestroy(stack);
    } else { // Parsujemy stały wielomian
        poly_coeff_t coeff;
        if (NumberTryToParse(*line, COEFF, &coeff, line, end_char)) {
            *poly = PolyFromCoeff(coeff);
            return true;
        }
    }
    return false;
}

/**
 * Dokonuje próby parsowania fragmentu lini jako jednomianu.
 * Jeśli się ona udała, zwraca prawdę oraz przypisuje zmiennej mono
 * odpowiedni jednomian.
 * W przeciwnym przypadku zwraca fałsz.
 * Funkcja modyfikuje argument line - jeśli zwracana przez funkcję wartość
 * to prawda - zmienna line wskazuje na pierwszy niezinterpretowny znak
 * występujący po jednomianie.
 * @param[in, out] line : wskaźnik na parsowany fragment lini - jeśli funkcja
 *                       zwraca prawdę, wskazuje na pierwszy niezinterpretowany
 *                       znak występujący po jednomianie.
 * @param[out] mono : wskaźnik na zmienną typu \a Mono.
 * @return Czy fragment lini jest możliwy do zinterpretowania jako jednomian?
 */
static bool MonoTryToParse(string *line, Mono *mono) {
    Poly poly = PolyZero();
    poly_exp_t exp;
    string my_line = *line;

    if (my_line[0] == '(') {
        my_line++;
        /* Parsujemy fragment lini jako wielomian - spodziewamy się, że
         * pierwszym niezinterpretowanym znakiem jest ','. */
        if (PolyTryToParse(&my_line, &poly, ',')) {
            my_line++;
            /* Parsujemy fragment lini jako wykładnik - spodziewamy się, że
             * pierwszym niezinterpretowanym znakiem jest ')'. */
            if (NumberTryToParse(my_line, EXP, &exp, &my_line, ')')) {
                my_line++;
                *mono = MonoFromPoly(&poly, exp);
                *line = my_line;
                return true;
            }
        }
    }

    PolyDestroy(&poly);
    return false;
}

/**
 * Dokonuje próby parsowania fragmentu lini jako jednomianu.
 * Jeśli się ona udała, zwraca prawdę oraz dodaje sparsowany jednomian
 * na wierzchołek stosu jednomianów.
 * W przeciwnym przypadku zwraca fałsz.
 * Funkcja modyfikuje argument line - jeśli zwracana przez funkcję wartość
 * to prawda - zmienna line wskazuje na pierwszy niezinterpretowny znak
 * występujący po jednomianie.
 * @param[in, out] line : wskaźnik na parsowany fragment lini - jeśli funkcja
 *                       zwraca prawdę, wskazuje na pierwszy niezinterpretowany
 *                       znak występujący po jednomianie.
 * @param[out] stack : wskaźnik na zmienną typu \a MonoStack.
 * @return Czy fragment lini jest możliwy do zinterpretowania jako jednomian?
 */
static bool MonoParseAndPush(string *line, MonoStack *stack) {
    Mono mono;
    if (MonoTryToParse(line, &mono)) {
        MonoStackPush(stack, mono);
        return true;
    }
    return false;
}

/**
 * Dokonuje próby parsowania fragmentu lini jako sumy jednomianów
 * lub jednomianu.
 * Jeśli się ona udała oraz pierwszy niezinterpretowany znak jest równy
 * argumentowi end_char, zwraca prawdę, a stos jednomianów, na który wskazuje
 * argument stack, zawiera wszystkie sparsowane jednomiany.
 * W przeciwnym przypadku zwraca fałsz.
 * Funkcja modyfikuje argument line - jeśli zwracana przez funkcję wartość
 * to prawda - zmienna line wskazuje na pierwszy niezinterpretowny znak
 * występujący po jednomianie bądź ich sumie.
 * @param[in, out] line : wskaźnik na parsowany fragment lini - jeśli funkcja
 *                       zwraca prawdę, wskazuje na pierwszy niezinterpretowany
 *                       znak występujący po jednomianie bądź ich sumie.
 * @param[out] stack : wskaźnik na zmienną typu \a MonoStack.
 * @param[in] end_char : oczekiwana wartość pierwszego niezinterpretowanego znaku.
 * @return Czy fragment lini jest możliwy do zinterpretowania jako suma jednomianów
 *         bądź jednomian oraz pierwszy niezinterpretowany znak jest równy
 *         argumentowi end_char?
 */
static bool MonoSumTryToParse(string *line, MonoStack *stack, char end_char) {
    string my_line = *line;
    bool plus_sign_occured = false;

    while (my_line[0] != end_char) {
        if (plus_sign_occured || stack->size == 0) {
            if (MonoParseAndPush(&my_line, stack))
                plus_sign_occured = false;
            else
                return false;
        } else {
            if (my_line[0] == '+' && my_line[1] == '(') {
                plus_sign_occured = true;
                my_line++;
            } else
                return false;
        }
    }

    *line = my_line;
    return true;
}

/**
 * Dokonuje parsowania lini jako wielomianu.
 * Ustawia odpowiedni rodzaj i specyfikację czynności argumentowi action.
 * Jeśli parsowanie się nie powiodło jest to opis błędu wejścia -
 * niepoprawny wielomian, w przeciwnym przypadku - jest to opis
 * czynności dodania wielomaianu na stos.
 * @param[in] line : rozważana linia
 * @param[out] action : wskaźnik na zmienną typu \a Action.
 */
static void PolyParse(string line, Action *action) {
    Poly poly = PolyZero();
    /* Parsujemy całą linię jako wielomian - spodziewamy się, że
     * pierwszym niezinterpretowanym znakiem jest '\n'. */
    if (PolyTryToParse(&line, &poly, '\n')) {
        action->type = PUSH_POLY;
        action->spec.poly = poly;
    } else {
        action->type = INPUT_ERROR;
        action->spec.error = WRONG_POLY;
        PolyDestroy(&poly);
    }
}

/**
 * Zwraca kopię fragmentu lini występującego po pierwszym od lewej znaku ' ',
 * czyli przy założeniu że linia opisuje polecenie AT lub DEG_BY -
 * zwraca kopię jej fragmentu, który reprezentuje parametr polecenia.
 * W przypadku gdy linia nie zawiera znaku ' ', zwraca NULL.
 * @param[in] line : rozważana linia.
 * @param[in] line_length : długość lini
 * @return Kopia fragmentu lini występującego po pierwszym od lewej znaku ' '
 *         lub NULL jeśli linia nie zawiera znaku ' '.
 */
static string GetParameter(string line, size_t line_length) {
    int i = 0;
    while (line[i] != ' ' && line[i] != '\0')
        i++;

    if (line[i] == '\0')
        return NULL;

    string res = malloc(line_length - i + 1);
    strcpy(res, line + i + 1);
    return res;
}

/**
 * Sprawdza czy linia powinna być interpretowana jako polecenie DEG_BY.
 * Jeśli powinna, próbuje dokonać parsowania
 * parametru polecenia oraz ustawia odpowiedni rodzaj i specyfikację
 * czynności argumentowi action (jeśli parsowanie się nie powiedzie
 * ustawia jest to opis błędu wejścia - niepoprawny parametr bądź jego brak).
 * Jeśli nie należy, zwraca fałsz.
 * @param[in] line : rozważana linia
 * @param[in] line_length : długość lini
 * @param[out] action : wskaźnik na zmienną typu \a Action
 * @return Czy linię należy interpretować jako polecenie DEG_BY?
 */
static bool IsDegBy(string line, Action *action, size_t line_length) {
    if (strncmp(line, "DEG_BY", 6) == 0) {
        string param = GetParameter(line, line_length);
        if (NumberTryToParse(param, VAR_IDX,
                             &action->spec.command.param.var_idx,
                             NULL, '\n')) {
            action->type = COMMAND;
            action->spec.command.name = DEG_BY;
        } else {
            action->type = INPUT_ERROR;
            action->spec.error = DEG_BY_WRONG_VARIABLE;
        }
        free(param);
        return true;
    }
    return false;
}

/**
 * Sprawdza czy linia powinna być interpretowana jako polecenie AT.
 * Jeśli powinna, próbuje dokonać parsowania
 * parametru polecenia oraz ustawia odpowiedni rodzaj i specyfikację
 * czynności argumentowi action (jeśli parsowanie się nie powiedzie
 * ustawia jest to opis błędu wejścia - niepoprawny parametr bądź jego brak).
 * @param[in] line : rozważana linia
 * @param[in] line_length : długość lini
 * @param[out] action : wskaźnik na zmienną typu \a Action
 * @return Czy linię należy interpretować jako polecenie AT?
 */
static bool IsAt(string line, size_t line_length, Action *action) {
    if (strncmp(line, "AT", 2) == 0) {
        string param = GetParameter(line, line_length);
        // Dokonujemy próby parsowania parametru
        if (NumberTryToParse(param, COEFF, &action->spec.command.param.x,
                             NULL, '\n')) {
            action->type = COMMAND;
            action->spec.command.name = AT;
        } else {
            action->type = INPUT_ERROR;
            action->spec.error = AT_WRONG_VALUE;
        }
        // Zwalniamy pamięć zaalokowaną przez funkcję GetParameter.
        free(param);
        return true;
    }
    return false;
}

/**
 * Sprawdza czy linia jest poleceniem bezparametrowym.
 * Jeśli tak, ustawia odpowiedni rodzaj i specyfikację czynności
 * argumentowi action.
 * @param[in] line : rozważana linia
 * @param[out] action : wskaźnik na zmienną typu \a Action
 * @return Czy linia jest poleceniem bezparametrowym?
 */
static bool IsNoParamCommand(string line, Action *action) {
    /* Kolejność napisów w tablicy NO_PARAM_COMM_NAMES odpowiada
     * kolejności w jakiejzostały wymienione bezargumentowe
     * polecenia w deklaracji typu wyliczeniowego CommandName w pliku calc.h */

    for (size_t i = 0; i < NO_PARAM_COMM_NUM; i++) {
        if (strcmp(line, NO_PARAM_COMM_NAMES[i]) == 0) {
            action->type = COMMAND;
            action->spec.command.name = (CommandName) i;
            return true;
        }
    }
    return false;
}

/**
 * Sprawdza czy pierwszy znak lini jest małą lub wielką literą
 * alfabetu angielskiego.
 * @param[in] line : rozważana linia
 * @return Czy pierwszy znak lini jest małą lub wielką lieteą alfabetu
 *         angielskiego?
 */
static bool IsFirstCharLatinLetter(string line) {
    return (('A' <= line[0] && line[0] <= 'Z') ||
            ('a' <= line[0] && line[0] <= 'z'));
}

/**
 * Dokonuje próby parsowania lini jako polecenia.
 * Zwraca prawdę jeśli jest się ona udała i fałsz w przeciwnym przypadku.
 * Ustawia odpowiedni rodzaj i specyfikację czynności argumentowi action.
 * @param[in] line : rozważana linia
 * @param[in] line_length : długość lini
 * @param[out] action : wskaźnik na zmienną typu \a Action
 * @return Czy udało się sparsować linię jako polecenie?
 */
static bool CommandTryToParse(string line, size_t line_length, Action *action) {
    if (IsNoParamCommand(line, action)
        || IsDegBy(line, action, line_length)
        || IsAt(line, line_length, action))
        return true;
    return false;
}

/**
 * Sprawdza czy linia powinna zostać intperpretowana jako polecenie.
 * Jeśli powinna, próbuje dokonać parsowania lini jako polecenie oraz
 * ustawia odpowiedni rodzaj i specyfikację czynności argumentowi action
 * (jeśli parsowanie się nie powiedzie jest to opis błędu wejścia -
 *  niepoprawna nazwa polecenia)
 * @param[in] line : rozważana linia
 * @param[in] line_length : długość lini
 * @param[out] action : wskaźnik na zmienną typu \a Action
 * @return Czy program traktuję linie jako polecenie?
 */
static bool IsCommand(string line, size_t line_length, Action *action) {
    // Linia jest poleceniem jeśli jej pierwszy znak to litera alfabetu ang.
    if (IsFirstCharLatinLetter(line)) {

        if (!CommandTryToParse(line, line_length, action)) {
            action->type = INPUT_ERROR;
            action->spec.error = WRONG_COMMAND;
        }
        return true;
    }
    return false;
}

/**
 * Sprawdza czy linia powinna zostać zignorowana.
 * Jeśli powinna, ustawia odpowiedni rodzaj czynności argumentowi action.
 * @param[in] line : rozważana linia
 * @param[in] line_length : długość lini
 * @param[out] action : wskaźnik na zmienną typu \a Action
 * @return Czy linia powinna zostać zignorowana?
 */
static bool IsToBeIgnored(string line, ssize_t line_length, Action *action) {
    if ((line_length == 1 && *line == '\n') || *line == '#') {
        action->type = IGNORE;
        return true;
    }
    return false;
}

Action ParseLine(string line, ssize_t line_length) {
    Action action;

    // Sprawdzamy czy linię należy zignorować.
    if (IsToBeIgnored(line, line_length, &action))
        return action;

    // Dodajemy na koniec lini znak '\n' jeśli się nim nie kończy.
    if (line[line_length - 1] != '\n')
        strcat(line, "\n");

    // Jeśli linia jest poleceniem, parsujemy ją.
    if (IsCommand(line, line_length, &action))
        return action;

    // Parsujemy linię jako wielomian.
    PolyParse(line, &action);
    return action;
}