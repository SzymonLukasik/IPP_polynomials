# IPP polynomials

## Multivariable sparse polynomial calculator

The library providing operations on multivariable sparse polynomials and a calculator operating on such polynomials using reverse polish notation.

## Definition of polynomial 
We say that $x_0, x_1, x_2 \dots$ are polynomial's variables.\
Polynomial has a recursive definition. \
A polynomial is a sum of monomials of form $px_i^n$ where $n$ is an exponent of this monomial being a nonnegative integer and $p$ is the coefficient which is a polynomial.\
Coefficient in a monomial of variable $x_i$ is a sum of monomials of variable $x_{i+1}$.\
Recursion ends when the coefficient is a number (constant polynomial).\
Exponents of monomials in each of considered sums are pairwise different.\
Polynomials are sparse i.e. degree of a polynomial might be much larger than a number of componential monomials. 

## Library overview

Provided functions enable creating multivariable polynomials and performing basic operations on them:

- adding
- subtracting
- multiplying 
- returning the opposite polynomial
- returning the polynomial's degree (globally or with respect to particular variable)
- returning the polynomial created by substituting given value for a particular variable
- checking if two polynomials are equal
- composition of polynomials

## Implementation of the library

We are using the safe_alloc.h library - provading functions enabling safe allocation of memory.

Polynomials created with library's function that are not constant polynomials are represented as an array of monomials sorted ascending by their exponent.

Function that adds polynomials works in time proportional to sum of polynomials' width multiplied by square of polynomials' depth.

Function that multiplies polynomials works in time proportional to product of width polynomials multiplied by square of polynomials' depth.

## Calculator's interface

Calculator's program reads the data one line at a time from the standard input.\
A line consists of a polynomial or a calculator's command.

### Graphical representation of polynomials
A polynomial is represented as a constant, monomial or sum of monomials.\
Constant is an integer. Monomial is a pair (**coeff**, **exp**) where coefficient **coeff** is a polynomial and an exponent **exp** is a nonnegative integer.\
The **+** character express the sum of monomials.

For example **((3, 0)+((5,4),7),12)+((1,0)+(2,1)+(1,2),14)+(5,28)** represents following polynomial:
$(3 + 5x_2^4 \, x_1^7) \, x_0^{12} + (1 + 2x_1 + x_1^2) \, x_0^{14} + 5x_0^{28}$

### Calculator's commands

- ZERO - puts on stack a zero polynomial
- IS_COEFF - checks whether the polynomial on the top of the stack is a coefficient (constant) - writes 1 or 0 to standard output
- IS_ZERO - checks whether the polynomial on the top of the stack is a zero polynomial - writes 1 or 0 to standard output
- CLONE - puts on the stack a copy of polynomial from the top of the stack
- ADD - adds two polynomials on the top of the stack, takes them off the stack and puts their sum on stack
- MUL multiplies two polynomials on the top of the stack, takes them off the stack and puts their product on the stack
- NEG - negates the polynomial on the top of the stack
- SUB - substracts the second polynomial from the top of the stack from the polynomial on top of the stack, takes them off the stack and puts their difference on the top of the stack
- IS_EQ - checks whether two polynomials on top of the stack are equal
- DEG - writes the degree of the polynomial on the top of the stack (-1 for zero polynomial) to the standatd output
- DEG_BY *idx* - writes the degree of the polynomial on the top of the stack with respect to a variable with a number *idx* (-1 for zero polynomial)
- AD *x* - computes the value of a polynomial on the top of the stack in point *x*, takes it off the stack and puts on the stack the result of the operation
- PRINT - writes the polynomial on the top of the stack to the standard output
- POP - takes the polynomial from the top off the stack
- COMPOSE $k$ - takes the polynomial from the top and *k* consecutive polynomials off the stack and puts on the stack the result of composing them.\
Let the $l$ denotes the number of variables of the polynomial on the top of the stack.\
Consecutive $k$ polynomials from under the top of the stack are substituted for variables $x_0, x_1, \dots, x_{\mathrm{min}(k, l) \, - \, 1}$ of the polynomial on the top of the stack.\
If $k < l$ zeros are substituted for variables $x_k, x_{k+1}, \dots, x_{l-1}$.

### Errors
The program handles 5 kinds of errors. That is STACK_UNDERFLOW error - raised when there's too few polynomials on the stack to perform given operation, and 4 input errors:

- WRONG COMMAND - improper command name
- DEG BY WRONG VARIABLE - improper DEG_BY parameter or lack of it
- AT WRONG VALUE - improper AT parameter or lack of it
- COMPOSE WRONG PARAMETER - improper COMPOSE parameter or lack of it
- WRONG POLY - improper polynomial

## Usage

To run the calculator, in the repository root do:\
<code>
mkdir release\
cd release\
cmake ..\
chmod u+x ./poly\
./poly
</code>
