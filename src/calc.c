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

static bool StackContains(MinimStackSize size, PolyStack stack, size_t line_nr) {
    if(stack.size >= size)
        return true;
    else {
        fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", line_nr);
        return false;
    }
}

static void ExecuteBinaryOperation(CommandType type, PolyStack *stack, size_t line_nr) {
    if(StackContains(TWO, *stack, line_nr)) {
        Poly res, a, b;
        a = PolyStackPop(stack);
        b = PolyStackPop(stack);
        switch (type) {
            case ADD : {
                res = PolyAdd(&a, &b);
                break;
            }
            case MUL : {
                res = PolyMul(&a, &b);
                break;
            }
            case SUB : {
                res = PolySub(&a, &b);
                break;
            }
        }
        PolyDestroy(&a);
        PolyDestroy(&b);
        PolyStackPush(stack, res);
    }
}

static void PrintZeroOrOne(bool statement) {
    if(statement)
        printf("1\n");
    else
        printf("0\n");
}

static void CommandExecute(CommandType type, CommandParam param,
                           PolyStack *stack, size_t line_nr) {
    switch (type) {
        case ZERO : {
            PolyStackPush(stack, PolyZero());
            break;
        }
        case IS_COEFF : {
            if(StackContains(ONE, *stack, line_nr))
                PrintZeroOrOne(PolyIsCoeff(PolyStackTop(*stack)));
            break;
        }
        case IS_ZERO : {
            if(StackContains(ONE, *stack, line_nr))
                PrintZeroOrOne(PolyIsZero(PolyStackTop(*stack)));
            break;
        }
        case CLONE : {
            if(StackContains(ONE, *stack, line_nr))
                PolyStackPush(stack, PolyClone(PolyStackTop(*stack)));
            break;
        }
        case ADD : {
            ExecuteBinaryOperation(ADD, stack, line_nr);
            break;
        }
        case MUL : {
            ExecuteBinaryOperation(MUL, stack, line_nr);
            break;
        }
        case SUB : {
            ExecuteBinaryOperation(SUB, stack, line_nr);
            break;
        }
        case IS_EQ : {
            if(StackContains(TWO, *stack, line_nr)) {
                PolyPair polys = DoublePolyStackTop(*stack);
                PrintZeroOrOne(PolyIsEq(polys.first, polys.second));
            }
            break;
        }
        case NEG : {
            if(StackContains(ONE, *stack, line_nr)) {
                Poly a = PolyStackPop(stack);
                PolyStackPush(stack, PolyNeg(&a));
                PolyDestroy(&a);
            }
            break;;
        }
        case DEG : {
            if(StackContains(ONE, *stack, line_nr))
                printf("%d\n", PolyDeg(PolyStackTop(*stack)));
            break;
        }
        case DEG_BY : {
            if(StackContains(ONE, *stack, line_nr))
                printf("%d\n", PolyDegBy(PolyStackTop(*stack), param.var_idx));
            break;
        }
        case AT : {
            if(StackContains(ONE, *stack, line_nr)) {
                Poly a = PolyStackPop(stack);
                PolyStackPush(stack, PolyAt(&a, param.x));
                PolyDestroy(&a);
            }
            break;
        }
        case PRINT : {
            if(StackContains(ONE, *stack, line_nr)) {
                Print(*PolyStackTop(*stack));
                printf("\n");
            }
            break;
        }
        case POP : {
            if(StackContains(ONE, *stack, line_nr)) {
                Poly a = PolyStackPop(stack);
                PolyDestroy(&a);
            }
            break;
        }

    }
}

static void ErrorHandle(ErrorType type, size_t line_nr) {
    fprintf(stderr, "ERROR %zu ", line_nr);
    switch (type) {
        case WRONG_COMMAND : {
            fprintf(stderr, "WRONG COMMAND\n");
            break;
        }
        case DEG_BY_WRONG_VAR : {
            fprintf(stderr, "DEG BY WRONG VARIABLE\n");
            break;
        }
        case AT_WRONG_VAL : {
            fprintf(stderr, "AT WRONG VALUE\n");
            break;
        }
        case WRONG_POLY : {
            fprintf(stderr, "WRONG POLY\n");
            break;
        }
    }
}

static void ProcessLine(PolyStack *stack, string line, ssize_t line_length,
                        size_t line_nr) {
    Action action = ParseLine(line, line_length);
    if (action.type == PUSH_POLY)
        PolyStackPush(stack, action.spec.poly);
    else if (action.type == COMMAND)
        CommandExecute(action.spec.command.type, action.spec.command.param,
                       stack, line_nr);
    else if (action.type == ERROR)
        ErrorHandle(action.spec.error_type, line_nr);
}


/**
 * Rozpoczyna działanie kalkulatora.
 * Czyta dane wierszami ze standardowego wejścia, dodaje na stos wielomian,
 * wykonuje odpowiednią instrukcję lub wypisuje stosowny komunikat o błędzie.
 * @param[in] : pusty stos wielomianów używany do
 */
static void RunCalc(FILE *fp) {
    PolyStack stack = NewPolyStack();
    string pntr = NULL;
    size_t line_size = 0;
    size_t line_nr = 1;
    ssize_t line_length;
    errno = 0;

    do {
        line_length = getline(&pntr, &line_size, (fp == NULL ? stdin : fp));
        if (errno == ENOMEM)
            exit(1);
        if (line_length != -1) {
            string line = pntr;
            ProcessLine(&stack, line, line_length, line_nr++);
        }
    } while (line_length != -1);

    free(pntr);
    PolyStackDestroy(stack);
}

int main(int argc, string argv[]) {
    if(argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        RunCalc(fp);
        fclose(fp);
    }
    else {
        RunCalc(NULL);
    }
}