/** @file
  Implementacja interfejsu klasy wielomianów rzadkich wielu zmiennych.

  @authors Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#include <stdlib.h>
#include "poly.h"
#include "safealloc.h"

void PolyDestroy(Poly *p) {
    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < p->size; i++)
            MonoDestroy(&p->arr[i]);
        free(p->arr);
        p->arr = NULL;
    }
}

Poly PolyClone(const Poly *p) {
    Poly copy;
    if (PolyIsCoeff(p)) {
        copy.arr = NULL;
        copy.coeff = p->coeff;
    } else {
        copy.size = p->size;
        copy.arr = SafeCalloc(copy.size, sizeof(Mono));
        for (size_t i = 0; i < copy.size; i++)
            copy.arr[i] = MonoClone(&p->arr[i]);
    }
    return copy;
}

/**
 * Redukuje wielomian. Jeśli wielomian @f$p@f$ jest postaci @f$c\cdot x^0@f$,
 * gdzie @f$c@f$ to wielomian będący współczynnikiem, upraszcza @f$p@f$ do
 * postaci @f$c@f$.
 * @param[in] p : wielomian @f$p@f$
 */
static void PolyReduce(Poly *p) {
    if (!PolyIsCoeff(p) && p->size == 1 && p->arr[0].exp == 0
        && PolyIsCoeff(&p->arr[0].p)) {
        Poly p_temp = p->arr[0].p;
        free(p->arr);
        *p = p_temp;
    }
}

/**
 * Zwraca tablicę jednomianów zawierającą elementy tablicy @p arr1 i @p arr2.
 * @param[in] arr1 : tablica jednomianów
 * @param[in] arr2 : tablica jednomianów
 * @param[in] size1 : rozmiar tablicy @p arr1
 * @param[in] size2 : rozmiar tablicy @p arr2
 * @return tablicę jednomianów będąca złączeniem tablic @p arr1 i @p arr2
 */
static Mono *MergeMonoArrays(Mono arr1[], Mono arr2[],
                             size_t size1, size_t size2) {
    Mono *monos = SafeCalloc(size1 + size2, sizeof(Mono));
    for (size_t i = 0; i < size1; i++)
        monos[i] = arr1[i];
    for (size_t i = 0; i < size2; i++)
        monos[i + size1] = arr2[i];
    return monos;
}

/**
 * Porównuje jednomiany ze względu na wykładnik.
 * @param[in] a : jednomian @f$a@f$
 * @param[in] b : jednomian @f$b@f$
 * @return -1 gdy @p a.exp < @p b.exp, 1 gdy @p a.exp > @p b.exp, 0
 * w przeciwnym przypadku
 */
static int CompareMono(const void *a, const void *b) {
    Mono m1 = *(Mono *) a;
    Mono m2 = *(Mono *) b;

    if (m1.exp < m2.exp)
        return -1;
    else if (m1.exp > m2.exp)
        return 1;
    return 0;
}

/**
 * Sortuje tablicę jednomianów rosnąco względem wykładnika.
 * @param[in] count : rozmiar tablicy @p monos
 * @param[in] monos : tablica jednomianów
 */
static void SortMono(size_t count, Mono monos[]) {
    qsort(monos, count, sizeof(Mono), CompareMono);
}

/**
 * Zwraca liczbę jednomianów tożsamościowo równych zeru, zawartych w @f$p@f$.
 * @param[in] p : wielomian niebędący współczynnikiem @f$p@f$
 * @return liczba jednomianów zerowych zawartych w @f$p@f$
 */
static size_t NumberOfZeros(Poly *p) {
    size_t n = 0;
    for (size_t i = 0; i < p->size; i++) {
        if (PolyIsZero(&p->arr[i].p))
            n++;
    }
    return n;
}

/**
 * Usuwa zerowe jednomiany zawarte w wielomianie @f$p@f$.
 * @param[in] p : wielomian niebędący współczynnikiem @f$p@f$
 */
static void PolyDeleteZeros(Poly *p) {
    size_t size = p->size - NumberOfZeros(p);

    //Wielomian jest wielomianem zerowym
    if (size == 0) {
        free(p->arr);
        *p = (Poly) {.coeff = 0, .arr = NULL};
    }
        //Wielomian nie jest zerowy, ale zawiera zerowe jednomiany
    else if (size < p->size) {
        Mono *arr = SafeCalloc(size, sizeof(Mono));
        for (size_t i = 0, j = 0; i < p->size; i++)
            if (!PolyIsZero(&p->arr[i].p))
                arr[j++] = p->arr[i];
        free(p->arr);
        p->size = size;
        p->arr = arr;
    }
}

/**
 * Zwraca liczbę różnych wykładników jednomianów, które zawarte sa w tablicy @p monos.
 * @param[in] count : rozmiar tablicy @p monos
 * @param[in] monos : tablica jednomianów
 * @return liczba różnych wykładników jednomianów z tablicy @p monos
 */
static size_t NumberOfDiffExponents(size_t count, const Mono *monos) {
    size_t size = 0;
    for (size_t i = 0; i < count; i++) {
        while (i < count - 1 && monos[i].exp == monos[i + 1].exp)
            i++;
        size++;
    }
    return size;
}

/**
 * Zwraca wielomian będący sumą wszystkich wielomianów przy
 * jednomianach z tablicy @p monos o rozważanym wykładniku.
 * Ustawia indeks @p i na indeks wskazujący na ostatni jednomian
 * z tablicy @p monos o rozważanym wykładniku.
 * @param[in] count : rozmiar tablicy monos
 * @param[in] monos : tablica jednomianów posortowanych rosnąco względem wykładnika
 * @param[in] i : indeks pierwszego jednomianu w tablicy @p monos o rozważanym
 *                wykładniku
 * @return
 */

static Poly PolyAddMonosWithSameExponent(size_t count, const Mono monos[], size_t *i) {
    if (*i < count - 1 && monos[*i].exp == monos[*i + 1].exp) {
        Poly p_sum = PolyAdd(&monos[*i].p, &monos[*i + 1].p);
        (*i)++;

        while (*i < count - 1 && monos[*i].exp == monos[*i + 1].exp) {
            Poly p_temp = p_sum;
            p_sum = PolyAdd(&p_sum, &monos[*i + 1].p);
            PolyDestroy(&p_temp);
            (*i)++;
        }
        return p_sum;
    } else //W tablicy monos jest tylko jeden jednomian o rozważanym wykładniku
        return PolyClone(&monos[*i].p);
}

/**
* Zwraca wielomian powstały poprzez zsumowanie jednomianów
  zawartych w tablicy @p monos. Wynikowy wielomian
  nie jest zredukowany do pożądanej postaci.
* @param[in] count : liczba jednomianów
* @param[in] monos : tablica jednomianów
* @return wielomian będący sumą jednomianów, niezredukowany
*/
static Poly PolyAddMonosBrute(size_t count, const Mono *monos) {
    Poly p = PolyFromCoeff(0);
    p.size = NumberOfDiffExponents(count, monos);
    p.arr = SafeCalloc(p.size, sizeof(Mono));

    for (size_t i = 0, j = 0; i < count; i++) {
        Poly p_sum = PolyAddMonosWithSameExponent(count, monos, &i);
        p.arr[j] = MonoFromPoly(&p_sum, monos[i].exp);
        j++;
    }
    return p;
}

/**
* Sumuje listę jednomianów i tworzy z nich wielomian.
* Nie przejmuje na własność zawartość tablicy @p monos.
* @param[in] count : liczba jednomianów
* @param[in] monos : tablica jednomianów
* @return wielomian będący sumą jednomianów
*/
static Poly MyPolyAddMonos(size_t count, const Mono monos[]) {
    SortMono(count, (Mono *) monos);
    Poly p = PolyAddMonosBrute(count, monos);
    PolyDeleteZeros(&p);
    PolyReduce(&p);
    return p;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return (Poly) {.coeff = p->coeff + q->coeff, .arr = NULL};

    if (PolyIsCoeff(q))
        return PolyAdd(q, p);

    size_t count;
    Mono *monos;
    if (PolyIsCoeff(p)) {
        Mono m = MonoFromPoly(p, 0);
        monos = MergeMonoArrays(&m, q->arr, 1, q->size);
        count = q->size + 1;
    }
        //Wielomiany p i q nie są współczynnikami
    else {
        monos = MergeMonoArrays(p->arr, q->arr, p->size, q->size);
        count = p->size + q->size;
    }

    Poly p_sum = MyPolyAddMonos(count, monos);
    free(monos);
    return p_sum;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0)
        return PolyFromCoeff(0);
    Mono *monos_copy = MergeMonoArrays(NULL, (Mono *) monos, 0, count);
    Poly p = MyPolyAddMonos(count, monos_copy);
    free(monos_copy);
    for (size_t i = 0; i < count; i++)
        MonoDestroy((Mono *) &monos[i]);
    return p;
}

/**
 * Zwraca wielomian będący sumą iloczynów jednomianów
 * zawartych w tablicach @p p_monos i @p q_monos. Mnoży każdy
 * jednomian z tablicy @p p_monos z każdym jednomianem z tablicy
 * @p q_monos, i zwraca sumę tych iloczynów.
 * @param[in] p_count : rozmiar tablicy @p p_monos
 * @param[in] p_monos : tablica jednomianów
 * @param[in] q_count : rozmiar tablicy @p q_monos
 * @param[in] q_monos : tablica jednomianów
 * @return wielomian będący sumą iloczynów jednomianów
 * zawartych w tablicach @p p_monos i @p q_monos
 */

static Poly PolyMulMonos(size_t p_count, Mono *p_monos,
                         size_t q_count, Mono *q_monos) {

    Mono *monos = SafeCalloc(p_count * q_count, sizeof(Mono));

    for (size_t i = 0, z = 0; i < p_count; i++) {
        for (size_t j = 0; j < q_count; j++) {
            Poly p_mul = PolyMul(&p_monos[i].p, &q_monos[j].p);
            monos[z++] = MonoFromPoly(&p_mul, p_monos[i].exp + q_monos[j].exp);
        }
    }

    Poly res = PolyAddMonos(p_count * q_count, monos);
    free(monos);
    return res;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return (Poly) {.coeff = p->coeff * q->coeff, .arr = NULL};

    if (PolyIsCoeff(q))
        return PolyMul(q, p);
    if (PolyIsCoeff(p)) {
        Mono m = MonoFromPoly(p, 0);
        return PolyMulMonos(1, &m, q->size, q->arr);
    }
    //Wielomiany p i q nie są współczynnikami
    return PolyMulMonos(p->size, p->arr, q->size, q->arr);
}

Poly PolyNeg(const Poly *p) {
    Poly q = PolyFromCoeff(-1);
    return PolyMul(p, &q);
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly q_neg = PolyNeg(q);
    Poly p_sum = PolyAdd(p, &q_neg);
    PolyDestroy(&q_neg);
    return p_sum;
}

/** Zwraca największy z dwóch wykładników.
 * @param[in] e1 : wykładnik
 * @param[in] e2 : wykładnik
 * @return @f$\max(e_1, e_2)@f$
 */
static poly_exp_t max(poly_exp_t e1, poly_exp_t e2) {
    if (e1 > e2)
        return e1;
    return e2;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (var_idx == 0) {
        if (PolyIsZero(p))
            return -1;
        if (PolyIsCoeff(p))
            return 0;
        return p->arr[p->size - 1].exp;
    }

    /* Przypadek gdy sprawdzamy stopień względem zmiennej,
     * której nie reprezentuje dany wielomian. */
    if (PolyIsCoeff(p))
        return -1;

    poly_exp_t res = -1;
    for (size_t i = 0; i < p->size; i++) {
        res = max(res, PolyDegBy(&p->arr[i].p, var_idx - 1));
    }
    return res;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p))
        return -1;
    if (PolyIsCoeff(p))
        return 0;

    poly_exp_t res = -1;
    for (size_t i = 0; i < p->size; i++) {
        res = max(res, PolyDeg(&p->arr[i].p) + p->arr[i].exp);
    }
    return res;
}

/**
 * Sprawdza równość dwóch jednomianów
 * @param[in] m : jednomian
 * @param[in] n : jednomian
 * @return prawda gdy @f$ m = n @f$, fałsz w przeciwnym przypadku
 */
static bool MonoIsEq(const Mono *m, const Mono *n) {
    if (m->exp == n->exp)
        return PolyIsEq(&m->p, &n->p);
    return false;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return p->coeff == q->coeff;
    else if (!PolyIsCoeff(p) && !PolyIsCoeff(q) && p->size == q->size) {
        for (size_t i = 0; i < p->size; i++)
            if (!MonoIsEq(&p->arr[i], &q->arr[i]))
                return false;
        return true;
    }
    return false;
}

/**
 * Zwraca wartość liczby @p x podniesionej do potęgi @p exp.
 * Wykorzystuje algorytm szybkiego potęgowania.
 * @param[in] x : potęgowana liczba
 * @param[in] exp : wykładnik potęgi
 * @return @f$ x^{exp} @f$
 */
static poly_coeff_t Exponantiate(poly_coeff_t x, poly_exp_t exp) {
    if (exp == 0)
        return 1;
    poly_coeff_t temp = Exponantiate(x, exp / 2);
    if (exp % 2 == 0)
        return temp * temp;
    return temp * temp * x;
}

/**
 * Tworzy tablicę @p monos jednomianów powstałych poprzez
 * podstawienie za zmienną @f$x_0@f$ w jednomianach zawartych w
 * wielomianie @p p, wartości @p x i "opakowanie" ich w sztuczną
 * zmienną o wykładniku 0. Iloczyn wielomianu stojącego
 * przy jednomianie z wartością @p x podniesioną do odpowiedniej potęgi
 * zamieniamy w jednomian nowej zmiennej o wykładniku 0 i zapisujemy
 * w tablicy @p monos. Umożliwi nam to później wywołanie funkcji
 * PolyAddMonos w celu obliczenia wyniku funkcji PolyAt.
 * @param[in] p : wielomian niebędący współczynnikiem
 * @param[in] x : wartość, którą podstawiamy za zmienną
 * @return tablica jednomianów powstałych w opisany wyżej sposób
 */
static Mono *MonosAtFromPoly(const Poly *p, poly_coeff_t x) {
    Mono *monos = SafeCalloc(p->size, sizeof(Mono));
    Poly p_coeff, p_mul;

    for (size_t i = 0; i < p->size; i++) {
        p_coeff = PolyFromCoeff(Exponantiate(x, p->arr[i].exp));
        p_mul = PolyMul(&p->arr[i].p, &p_coeff);
        monos[i] = MonoFromPoly(&p_mul, 0);
    }
    return monos;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p))
        return *p;

    Mono *monos = MonosAtFromPoly(p, x);
    Poly res = PolyAddMonos(p->size, monos);
    free(monos);

    /* W funkcji MonosAtFromPoly sztucznie dodaliśmy zmienną o wykładniku 0,
     * dlatego musimy teraz zredukować wielomian jeżeli PolyAddMonos nie
     * zrobiło tego wcześniej.
     * (PolyAddMonos zredukuje wielomian tylko gdy wynikiem jest
     * współczynnik. - My chcemy zredukować także w przeciwnym
     * przypadku.) */
    if (!PolyIsCoeff(&res)) {
        Poly p_temp = res.arr[0].p;
        free(res.arr);
        res = p_temp;
    }
    return res;
}
