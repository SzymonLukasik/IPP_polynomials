#include <stdlib.h>
#include "poly.h"
#include "safealloc.h"


void    PolyDestroy(Poly *p) {
    if(!PolyIsCoeff(p)) {
        for(size_t i = 0; i < p->size; i++)
            MonoDestroy(&p->arr[i]);
        free(p->arr);
        p->arr = NULL;
    }
}

Poly PolyClone(const Poly *p) {
    Poly copy;
    if(PolyIsCoeff(p)) {
        copy.arr = NULL;
        copy.coeff = p->coeff;
    }
    else {
        copy.size = p->size;
        copy.arr = SafeCalloc(copy.size, sizeof(Mono));
        for(size_t i = 0; i < copy.size; i++)
            copy.arr[i] = MonoClone(&p->arr[i]);
    }
    return copy;
}

static Mono* MergeMonoArrays(Mono arr1[], Mono arr2[], size_t size1, size_t size2) {
    Mono* monos = SafeCalloc(size1 + size2, sizeof(Mono));
    for(size_t i = 0; i < size1; i++)
        monos[i] = arr1[i];
    for(size_t i = 0; i < size2; i++)
        monos[i + size1] = arr2[i];
    return monos;
}

static int CompareMono(const void* a, const void* b) {
    Mono m1 = *(Mono*) a;
    Mono m2 = *(Mono*) b;

    if(m1.exp < m2.exp)
        return -1;
    else if(m1.exp > m2.exp)
        return 1;
    return 0;
}

static void SortMono(size_t count, Mono monos[]) {
    qsort(monos, count, sizeof(Mono), CompareMono);
}

static void PolyDeleteZeros(Poly* p) {
    size_t size = 0;
    for(size_t i = 0; i < p->size; i++)
        if(!PolyIsZero(&p->arr[i].p))
            size++;
    if(size == 0) {
        free(p->arr);
        *p = (Poly) {.coeff = 0, .arr = NULL};
    }
    else if(size < p->size) {
        Mono *arr = SafeCalloc(size, sizeof(Mono));
        for (size_t i = 0, j = 0; i < p->size; i++)
            if (!PolyIsZero(&p->arr[i].p))
                arr[j++] = p->arr[i];
        free(p->arr);
        p->size = size;
        p->arr = arr;
    }
 }

Poly MyPolyAddMonos(size_t count, const Mono monos[]) {
    assert(count != 0);

    SortMono(count, (Mono*)monos );

    Poly p = PolyFromCoeff(0);

    for(size_t i = 0; i < count; i++) {
        while (i < count - 1 && monos[i].exp == monos[i+1].exp)
            i++;
        p.size++;
    }
    p.arr = SafeCalloc(p.size, sizeof(Mono));

    for(size_t i = 0, j = 0; i < count; i++) {
        Poly poly_sum;
        if(i < count - 1 && monos[i].exp == monos[i+1].exp)
            poly_sum = monos[i].p;
        else
            poly_sum = PolyClone(&monos[i].p);

        while (i < count - 1 && monos[i].exp == monos[i+1].exp) {
            poly_sum = PolyAdd(&poly_sum, &monos[i+1].p);
            i++;
        }
        p.arr[j] = MonoFromPoly(&poly_sum, monos[i].exp);
        j++;
    }

    PolyDeleteZeros(&p);

    if(p.arr != NULL && p.size == 1 && p.arr[0].exp == 0
        && PolyIsCoeff(&p.arr[0].p)) {
        Poly p_temp = p.arr[0].p;
        free(p.arr);
        p = p_temp;
    }

    return p;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if(PolyIsCoeff(p) && PolyIsCoeff(q))
        return (Poly) {.coeff = p->coeff + q->coeff, .arr = NULL};

    size_t count; Mono* monos;
    if(PolyIsCoeff(p)) {
        Mono m = MonoFromPoly(p, 0);
        monos = MergeMonoArrays(&m, q->arr, 1, q->size);
        count = q->size + 1;
    }
    else if(PolyIsCoeff(q)) {
        Mono m = MonoFromPoly(q, 0);
        monos = MergeMonoArrays(&m, p->arr, 1, p->size);
        count = p->size + 1;
    }
    else {
        monos = MergeMonoArrays(p->arr, q->arr, p->size, q->size);
        count = p->size + q->size;
    }
    Poly p_sum = MyPolyAddMonos(count, monos);
    free(monos);
    return p_sum;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    Poly p = MyPolyAddMonos(count, monos);
    for(size_t i = 0; i < count; i++)
        MonoDestroy(&monos[i]);
    return p;
}

Poly PolyMul(const Poly *p, const Poly *q) {

}

Poly PolyNeg(const Poly *p) {

}

Poly PolySub(const Poly *p, const Poly *q) {

}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {

}

poly_exp_t PolyDeg(const Poly *p) {

}

bool MonoIsEq(const Mono *m, const Mono *n) {
    if(m->exp == n->exp)
        return PolyIsEq(&m->p, &n->p);
    return false;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if(PolyIsCoeff(p) && PolyIsCoeff(q))
        return p->coeff == q->coeff;
    else if(!PolyIsCoeff(p) && !PolyIsCoeff(q) && p->size == q->size) {
        for(size_t i = 0; i < p->size; i++)
            if(!MonoIsEq(&p->arr[i], &q->arr[i]))
                return false;
        return true;
    }
    return false;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {

}











