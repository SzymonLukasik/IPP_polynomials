/** @file
  Implementacja kalkulatora wielomianów rzadkich wielu zmiennych.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/
#define _GNU_SOURCE


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "calc.h"
#include "poly_stack.h"
#include "parsing.h"

/**
 * Sprawdza czy stos wielomianóœ zawiera odpowiednią ilość wielomianów.
 * Gdy zawiera, zwraca prawdę, a w przeciwnym przypadku obsługuje błąd
 * STACK UNDERFLOW.
 * @param[in] size : argument typu MinimalStackSize opisujący wymaganą
 *                   wielkość stosu
 * @param[in] stack : stos wielomianów
 * @param[in] line_nr : numer lini
 * @return Czy stos zawiera wymaganą ilość wielomianów?
 */
static bool
StackContains(MinimalStackSize size, PolyStack stack, size_t line_nr) {
    if (stack.size >= size)
        return true;
    else {
        fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", line_nr);
        return false;
    }
}

/**
 * Sprawdza czy podane polecenie jest typu PushCommand
 * @param[in] command : polecenie
 * @return Czy polecenie jest typu PushCommand?
 */
static bool IsPushCommand(Command command) {
    for (size_t i = 0; i < PUSH_COMM_NUM; i++)
        if (PUSH_COMMANDS[i] == command.name)
            return true;
    return false;
}

/**
 * Jeśli stos zawiera co najmniej dwa wielomiany, wykonuje na dwóch
 * wielomianach z weierzchu stosu operację binarną, wcześniej ściągając
 * z wierzchu stosu dwa wielomiany i przypisuje zmiennej res jej wynik.
 * W przeciwnym przypadku zleca obsłużenie błędu STACK UNDERFLOW.
 * @param[in] command polecenie
 * @param[in, out] stack : wskaźnik na stos - gdy stos zawiera co
 *                        najmniej dwa wielomiany, zdejmujemy z
 *                        jego wierzchu dwa wielomiany
 * @param[in] line_nr : numer lini
 * @param[out] res : wskaźnik na wielomian, któremu w przypadku powodzenia
 *                   przypisujemy wynik operacji.
 * @return Czy stos zawierał co najmniej dwa wielomiany i wykonanie operacji
 *         się powiodło?
 */
static bool
PolyStackBinaryOperation(Command command, PolyStack *stack, size_t line_nr,
                         Poly *res) {
    if (StackContains(TWO, *stack, line_nr)) {
        Poly a = PolyStackPop(stack);
        Poly b = PolyStackPop(stack);

        if (command.name == ADD)
            *res = PolyAdd(&a, &b);
        else if (command.name == MUL)
            *res = PolyMul(&a, &b);
        else if (command.name == SUB)
            *res = PolySub(&a, &b);

        PolyDestroy(&a);
        PolyDestroy(&b);
        return true;
    }
    return false;
}

/**
 * Jeśli stos zawiera co najmniej jeden wielomian, wykonuje na wielomianie
 * z wierzchołka stosu operację unarną (w przypadku gdy polecenie to
 * NEG lub AT, wcześniej ściąga wielomian z wierzchołka stosu) i
 * przypisuje zmiennej res jej wynik.
 * W przeciwnym przypadku zleca obsłużenie błędu STACK UNDERFLOW.
 * @param[in] command polecenie
 * @param[in, out] stack : wskaźnik na stos - w przypadku wykonywania
 *                        polecenia NEG lub AT, gdy stos zawiera co
 *                        najmniej jeden wielomian, zdejmujemy wielomian
 *                        z jego wierzchołka.
 * @param[in] line_nr : numer lini
 * @param[out] res : wskaźnik na wielomian, któremu w przypadku powodzenia
 *                   przypisujemy wynik operacji.
 * @return Czy stos zawierał co najmniej jeden wielomian i wykonanie operacji
 *         się powiodło?
 */
static bool
PolyStackUnaryOperation(Command command, PolyStack *stack, size_t line_nr,
                        Poly *res) {
    if (StackContains(ONE, *stack, line_nr)) {
        if (command.name == CLONE)
            *res = PolyClone(PolyStackTop(*stack));
        else {
            Poly a = PolyStackPop(stack);

            if (command.name == NEG)
                *res = PolyNeg(&a);
            else if (command.name == AT)
                *res = PolyAt(&a, command.param.x);

            PolyDestroy(&a);
        }
        return true;
    }
    return false;
}

/**
 * Wykonuje odpowiednie polecenie rodzaju PushCommand.
 * Jeśli polecenie to ZERO, wykonuje je, a w przeciwnym przypadku
 * zleca wykonanie odpowiedniej operacji binarnej bądź odpowiedniej
 * operacji unarnej na wielomianach bądź wielomianie z wierzchu stosu
 * i w razie gdy wykonanie tej operacji się powiodło, dodaje na
 * wierzchołek stosu wynik tej operacji.
 * @param[in] command : polecenie
 * @param[in, out] stack : wskaźnik stos - w przypadku powodzenia, wielomian
 *                        jest dodawany na jego wierzchołek
 * @param[in] line_nr : numer lini
 */
static void
ExecutePushCommand(Command command, PolyStack *stack, size_t line_nr) {
    Poly poly = PolyZero();
    bool success = true;

    if (command.name == ZERO)
        poly = PolyZero();
    else if (command.name == ADD || command.name == MUL || command.name == SUB)
        success = PolyStackBinaryOperation(command, stack, line_nr, &poly);
    else // Wiemy, że polecenie to CLONE, NEG lub AT
        success = PolyStackUnaryOperation(command, stack, line_nr, &poly);

    if (success)
        PolyStackPush(stack, poly);
}

/**
 * Wykonuje polecenie PRINT lub zleca obsłużenie błędu STACK UNDERFLOW
 * w przypadku gdy stos nie zawiera co najmniej jednego wielomianu.
 * @param stack
 * @param line_nr
 */
static void PrintTopPoly(PolyStack stack, size_t line_nr) {
    if (StackContains(ONE, stack, line_nr)) {
        Print(*PolyStackTop(stack));
        printf("\n");
    }
}

/**
 * Wykonuje odpowiednie polecenie rodzaju DegCommandlub zleca obsłużenie
 * błędu STACK UNDERFLOW w przypadku gdy stos nie zawiera co najmniej jednego
 * wielomianu.
 * @param[in] command : polecenie
 * @param[in] stack : stos
 * @param[in] line_nr : numer lini
 */
static void
ExecuteDegCommand(Command command, PolyStack stack, size_t line_nr) {
    if (StackContains(ONE, stack, line_nr)) {
        poly_exp_t deg;

        if (command.name == DEG)
            deg = PolyDeg(PolyStackTop(stack));
        else if (command.name == DEG_BY)
            deg = PolyDegBy(PolyStackTop(stack), command.param.var_idx);

        printf("%d\n", deg);
    }
}

/**
 * Wypisuje na standardowe wyjście 1 jeśli wartość argumentu statement
 * to prawda i 0 w przeciwnym przypadku.
 * @param[in] statement : zmienna logiczna
 */
static void PrintZeroOrOne(bool statement) {
    if (statement)
        printf("1\n");
    else
        printf("0\n");
}

/**
 * Wykonuje polecenie IS_EQ na dwóch wielomanach z wierzchu stosu.
 * Zakłada, że stos zawiera co najmniej dwa wielomiany.
 * @param[in] stack : stos zawierający co najmniej dwa wielomiany
 * @return Czy dwa wielomiany z wierzchu stosu są równe?
 */
static bool CompareTopPolynomials(PolyStack stack) {
    assert(stack.size >= 2);
    PolyPair polys = DoublePolyStackTop(stack);
    return PolyIsEq(polys.first, polys.second);
}

/**
 * Wykonuje odpowiednie polecenie rodzaju BooleanCommand lub zleca
 * obsłużenie błędu STACK UNDERFLOW w przypadku gdy stos nie
 * zawiera co najmniej dwóch wielomianów jeśli polecenie to IS_EQ
 * lub co najmniej jednego wielomianu jeśli polecenie to IS_COEFF
 * bądź IS_ZERO.
 * @param[in] command : polecenie
 * @param[in] stack : stos wielomianów
 * @param[in] line_nr : numer lini
 */
static void
ExecuteBooleanCommand(Command command, PolyStack stack, size_t line_nr) {
    if (command.name == IS_EQ) {
        if (StackContains(TWO, stack, line_nr))
            PrintZeroOrOne(CompareTopPolynomials(stack));
    } else if (StackContains(ONE, stack, line_nr)) {
        if (command.name == IS_COEFF)
            PrintZeroOrOne(PolyIsCoeff(PolyStackTop(stack)));
        else if (command.name == IS_ZERO)
            PrintZeroOrOne(PolyIsZero(PolyStackTop(stack)));
    }
}

/**
 * Wykonuje polecenie rodzaju PrintCommand. Jeśli polecenie to PRINT
 * wykonuje je, a w przeciwnym przypadku zleca wykonanie odpowiedniego
 * polecenia rodzaju DegCommand lub odpowiedniego polecenia rodzaju
 * BooleanCommand.
 * @param[in] command : polecenie
 * @param[in] stack : stos wielomianów
 * @param[in] line_nr : numer lini
 */
static void
ExecutePrintCommand(Command command, PolyStack stack, size_t line_nr) {
    if (command.name == PRINT)
        PrintTopPoly(stack, line_nr);
    else if (command.name == DEG || command.name == DEG_BY)
        ExecuteDegCommand(command, stack, line_nr);
    else
        ExecuteBooleanCommand(command, stack, line_nr);
}

/**
 * Wykonuje polecenie POP lub zleca obsłużenie błędu STACK UNDERFLOW
 * w przypadku gdy stos nie zawiera co najmniej jednego wielomianu.
 * @param[in] stack : wskaźnik na stos
 * @param[in] line_nr : numer lini
 */
static void ExecutePopCommand(PolyStack *stack, size_t line_nr) {
    if (StackContains(ONE, *stack, line_nr)) {
        Poly a = PolyStackPop(stack);
        PolyDestroy(&a);
    }
}

/**
 * Wykonuje polecenie. Jeśli polecenie to POP, wykonuje je, a
 * w przeciwnym przypadku zleca wykonanie odpowiedniego
 * polecenia rodzaju PushCommand lub odpowiedniego polecenia
 * rodzaju PrintCommand.
 * @param[in] command : polecenie
 * @param[in, out] stack : wskaźnik na stos wielomianów, który jest modyfikowany
 *                        w przypadku wykonania polecenia POP lub odpowiedniego
 *                        polecenia rodzaju PushCommand.
 * @param[in] line_nr : numer lini
 */
static void CommandExecute(Command command, PolyStack *stack, size_t line_nr) {

    if (command.name == POP)
        ExecutePopCommand(stack, line_nr);
    else if (IsPushCommand(command))
        ExecutePushCommand(command, stack, line_nr);
    else // Wiemy, że polecenie jest rodzaju PrintCommand
        ExecutePrintCommand(command, *stack, line_nr);
}

/**
 * Obsługuje odpowiedni błąd wejścia.
 * @param[in] error : błąd wejścia
 * @param[in] line_nr : numer lini
 */
static void HandleInputError(InputError error, size_t line_nr) {
    fprintf(stderr, "ERROR %zu ", line_nr);

    for (size_t i = 0; i < INPUT_ERR_NUM; i++)
        if ((InputError) i == error)
            fprintf(stderr, "%s\n", INPUT_ERR_NAMES[i]);
}

/**
 * Parsuje linię, a następnie wykonuje odpowiednią czynność -
 * dodaje na wierzchołek stosu sparsowany wielomian lub
 * wykonuje polecenie lub obsługuje błąd wejścia.
 * @param[in] line : rozważana linia
 * @param[in] line_length : długość lini
 * @param[in] line_nr : numer lini
 * @param[out] stack : stos wielomianów, który funkcja modyfikuje
 *                     w przypadku dodawania wielomianu na stos
 *                     lub wykonywania niektórych poleceń
 */
static void ProcessLine(string line, ssize_t line_length, size_t line_nr,
                        PolyStack *stack) {
    Action action = ParseLine(line, line_length);

    if (action.type == PUSH_POLY)
        PolyStackPush(stack, action.spec.poly);
    else if (action.type == COMMAND)
        CommandExecute(action.spec.command, stack, line_nr);
    else if (action.type == INPUT_ERROR)
        HandleInputError(action.spec.error, line_nr);
}

/**
 * Rozpoczyna działanie kalkulatora.
 * Czyta dane wierszami ze standardowego wejścia, dodaje na stos wielomian,
 * wykonuje odpowiednią instrukcję lub wypisuje stosowny komunikat o błędzie.
 */
static void RunCalc() {
    PolyStack stack = NewPolyStack();
    string pntr = NULL;
    size_t line_size = 0;
    size_t line_nr = 1;
    ssize_t line_length;
    errno = 0;

    do {
        line_length = getline(&pntr, &line_size, stdin);
        if (errno == ENOMEM)
            exit(1);
        if (line_length != -1) {
            string line = pntr;
            ProcessLine(line, line_length, line_nr++, &stack);
        }
    } while (line_length != -1);

    free(pntr);
    PolyStackDestroy(stack);
}

/**
 * Funkcja main wykonuje funkcję RunCalc(), rozpoczynając
 * działanie kalkulatora.
 * @return 0 w przypadku, gdy uda się wczytać wszystkie wiersze ze
 *         standardowego wejścia oraz nie dojdzie do błędów
 *         skutkujących awaryjnym zakończeniem programu
 *         (na przykład nieudana alokacja pamięci).
 *         W przeciwnym przypadku 1.
 */
int main() {
    RunCalc();
}