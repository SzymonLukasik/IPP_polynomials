/** @file
  Implementacja biblioteki umożliwiającej parsowanie wiersza tekstu
  w obiekt typu \a Action, czyli informację o rodzaju czynności,
  którą powinien wykonać program po wczytaniu wiersza oraz jej
  argumentach.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/
#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "calc.h"
#include "parsing.h"
#include "mono_stack.h"

static bool IsToBeIgnored(string line, ssize_t line_length, Action *action) {
    if ((line_length == 1 &&  *line == '\n') || *line == '#') {
        action->type = IGNORE;
        return true;
    }
    return false;
}

static bool CoeffTryToParseShift(string *line, poly_coeff_t *coeff, char end_char) {
    if(*line == NULL || **line == '\0')
        return false;
    string endptr;
    errno = 0;
    *coeff = strtoll(*line, &endptr, 10);
    /* Wartość współczynnika jednomianu jest
     * ograniczona przez zakres wartości typu long long */

    /* Sprawdzamy czy parsowanie się powiodło i czy nie został
     * przekroczony zakres wartości typu */
    if (errno != 0 || *endptr != end_char)
        return false;
    *line = endptr;
    return true;
}

static bool ExpTryToParseShift(string *line, poly_exp_t *exp, char end_char) {
    if(*line == NULL || **line == '\0' || **line < '0' || **line > '9')
        return false;
    string endptr;
    errno = 0;
    *exp = (int) strtol(*line, &endptr, 10);
    if (errno != 0  || *endptr != end_char
        || *exp < MIN_EXP || *exp > MAX_EXP)
        return false;
    *line = endptr;
    return true;
}

static bool CoeffTryToParse(string line, poly_coeff_t *coeff) {
    if(line == NULL || *line == '\0'
       || strchr(COEFF_FIRST_CHARS, *line) == NULL)
        return false;
    string endptr;
    errno = 0;
    *coeff = strtoll(line, &endptr, 10);
    /* Wartość  parametru polecenia AT jest ograniczona
     * przez zakres wartości typu long long */
    if (errno != 0 || *endptr != '\n')
        return false;
    return true;
}

static bool VarIdxTryToParse(string line, size_t *var_idx) {
    if(line == NULL || *line < '0' || *line > '9')
        return false;
    string endptr;
    errno = 0;
    *var_idx = strtoul(line, &endptr, 10);
    /* Wartość  parametru polecenia DEG_BY jest ograniczona
     * przez zakres wartości typu unsigned long */
    if (errno != 0 || *endptr != '\n' || *var_idx < 0)
        return false;
    return true;
}

static bool MonoStackTryToParse(string *line, MonoStack *stack, char end_char);

static bool PolyTryToParse(string *line, Poly *poly, char end_char) {

    if (strchr(COEFF_FIRST_CHARS, **line) != NULL) {
        poly_coeff_t coeff;
        if (!CoeffTryToParseShift(line, &coeff, end_char) || *line[0] != end_char)
            return false;
        *poly = PolyFromCoeff(coeff);
        return true;
    } else if (**line == '(') {
        MonoStack stack = NewMonoStack();
        if (!MonoStackTryToParse(line, &stack, end_char) || **line != end_char) {
            MonoStackDestroy(stack);
            return false;
        }
        *poly = PolyAddMonos(stack.size, stack.data);
        MonoStackDestroy(stack);
        return true;
    }
    return false;
}

static bool MonoStackTryToParse(string *line, MonoStack *stack, char end_char) {
    while (**line != end_char) {
        if (**line == '(' && (stack->size == 0 || *(*line - 1) == '+')) {
            (*line)++;
            Poly poly = PolyZero();
            poly_exp_t exp;
            if (!PolyTryToParse(line, &poly, ',')) {
                PolyDestroy(&poly);
                return false;
            }
            (*line)++;
            if (!ExpTryToParseShift(line, &exp, ')')) {
                PolyDestroy(&poly);
                return false;
            }
            if (!((*line)[0] == ')')) {
                PolyDestroy(&poly);
                return false;
            }
            (*line)++;
            Mono mono = MonoFromPoly(&poly, exp);
            MonoStackPush(stack, mono);
        } else if (**line == '+') {
            if ((*line)[1] != '(')
                return false;
            (*line)++;
        } else
            return false;
    }
    return true;
}


static void PolyParse(string line, Action *action) {
    Poly poly = PolyZero();

    if (PolyTryToParse(&line, &poly, '\n')) {
        action->type = PUSH_POLY;
        action->spec.poly = poly;
    } else {
        action->type = ERROR;
        action->spec.error_type = WRONG_POLY;
        PolyDestroy(&poly);
    }
}

static string GetParameter(string line, size_t line_length) {
    int i = 0;
    while(*(line + i) != ' ' && *(line + i) != '\0')
        i++;
    if(*(line + i) == '\0')
        return NULL;
    string res = malloc(line_length - i + 1);
    strcpy(res, line + i + 1);
    return res;
}

static bool IsDegBy(string line, Action *action, size_t line_length) {
    if (strncmp(line, "DEG_BY", 6) == 0) {
        string param = GetParameter(line, line_length);
        if (param != NULL &&
            VarIdxTryToParse(param, &action->spec.command.param.var_idx)) {
            action->type = COMMAND;
            action->spec.command.type = DEG_BY;
        }
        else {
            action->type = ERROR;
            action->spec.error_type = DEG_BY_WRONG_VAR;
        }
        free(param);
        return true;
    }
    return false;
}

static bool IsAt(string line, Action *action, size_t line_length) {
    if (strncmp(line, "AT", 2) == 0) {
        string param = GetParameter(line, line_length);
        if (param != NULL &&
            CoeffTryToParse(param, &action->spec.command.param.x)) {
            action->type = COMMAND;
            action->spec.command.type = AT;
        }
        else {
            action->type = ERROR;
            action->spec.error_type = AT_WRONG_VAL;
        }
        free(param);
        return true;
    }
    return false;
}

static bool IsNonParamCommand(string line, Action *action) {
    string commands[12] = {"ZERO\n", "IS_COEFF\n", "IS_ZERO\n", "CLONE\n",
                           "ADD\n", "MUL\n", "NEG\n", "SUB\n", "IS_EQ\n",
                           "DEG\n", "PRINT\n", "POP\n"};
    /* Kolejność napisów w tablicy commands odpowiada kolejności w jakiej
     * zostały wymienione bezargumentowe polecenia w deklaracji typu
     * wyliczeniowego CommandType w pliku calc.h */
    for(int i = 0; i < 12; i++) {
        if(strcmp(line, commands[i]) == 0) {
            action->spec.command.type = (CommandType) i;
            return true;
        }
    }
    return false;
}

static bool IsCommand(string line, Action *action, size_t line_length) {
    if (('A' <= line[0] && line[0] <= 'Z') ||
        ('a' <= line[0] && line[0] <= 'z')) {

        if (IsDegBy(line, action, line_length)) {}
        else if (IsAt(line, action, line_length)) {}
        else if (IsNonParamCommand(line, action)) {
            action->type = COMMAND;
        }
        else {
            action->type = ERROR;
            action->spec.error_type = WRONG_COMMAND;
        }
        return true;
    }
    return false;
}

Action ParseLine(string line, ssize_t line_length) {
    Action action;

    if (IsToBeIgnored(line, line_length, &action))
        return action;

    if (line[line_length - 1] != '\n')
        strncat(line, "\n", 1);

    if (IsCommand(line, &action, line_length))
        return action;

    PolyParse(line, &action);
    return action;
}

