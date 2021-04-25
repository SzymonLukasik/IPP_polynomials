#include <stdlib.h>
#include "poly.h"
#include "safealloc.h"


void  PolyDestroy(Poly *p) {
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

void PolyReduce(Poly *p) {
    if(p->arr != NULL && p->size == 1 && p->arr[0].exp == 0) {
        Poly p_temp = p->arr[0].p;
        free(p->arr);
        *p = p_temp;
    }
}

static Mono* MergeMonoArrays(Mono arr1[], Mono arr2[],
                             size_t size1, size_t size2) {
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
        Poly p_sum, p_temp;
        p_sum = PolyClone(&monos[i].p);

        while (i < count - 1 && monos[i].exp == monos[i+1].exp) {
            p_temp = p_sum;
            p_sum = PolyAdd(&p_temp, &monos[i+1].p);
            PolyDestroy(&p_temp);
            i++;
        }
        p.arr[j] = MonoFromPoly(&p_sum, monos[i].exp);
        j++;
    }

    PolyDeleteZeros(&p);
    PolyReduce(&p);
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
        MonoDestroy((Mono*) &monos[i]);
    return p;
}

Poly PolyMulMonos(size_t p_count, Mono* p_monos,
                  size_t q_count, Mono* q_monos) {
    Mono* monos = SafeCalloc(p_count * q_count, sizeof(Mono));

    for(size_t i = 0, z = 0; i < p_count; i++) {
        for(size_t j = 0; j < q_count; j++) {
            Poly p_mul = PolyMul(&p_monos[i].p, &q_monos[j].p);
            monos[z++] = MonoFromPoly(&p_mul, p_monos[i].exp + q_monos[j].exp);
        }
    }

    Poly res = PolyAddMonos(p_count * q_count, monos);
    free(monos);
    return res;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if(PolyIsCoeff(p) && PolyIsCoeff(q))
        return (Poly) {.coeff = p->coeff * q->coeff, .arr = NULL};

    size_t p_count, q_count;
    Mono* p_monos; Mono* q_monos;
    if(PolyIsCoeff(p)) {
        Mono m = MonoFromPoly(p, 0);
        p_monos = &m;
        p_count = 1;
        q_monos = q->arr;
        q_count = q->size;
    }
    else if(PolyIsCoeff(q)) {
        Mono m = MonoFromPoly(q, 0);
        q_monos = &m;
        q_count = 1;
        p_monos = p->arr;
        p_count = p->size;
    }
    else {
        p_monos = p->arr;
        p_count = p->size;
        q_monos = q->arr;
        q_count = q->size;
    }
    return PolyMulMonos(p_count, p_monos, q_count, q_monos);

}

Poly PolyNeg(const Poly *p) {
    Poly q = PolyFromCoeff(-1);
    return PolyMul(p, &q);
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly q_neg = PolyNeg(q);
    Poly p_sum = PolyAdd(p ,&q_neg);
    PolyDestroy(&q_neg);
    return p_sum;
}


poly_exp_t max(poly_exp_t e1, poly_exp_t e2) {
    if(e1 > e2)
        return e1;
    return e2;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if(var_idx == 0) {
        if(PolyIsZero(p))
            return -1;
        if(PolyIsCoeff(p))
            return 0;
        return p->arr[p->size - 1].exp;
    }
    if(PolyIsCoeff(p))
        return -1;
    poly_exp_t res = -1;
    for(size_t i = 0; i < p->size; i++) {
        res = max(res, PolyDegBy(&p->arr[i].p, var_idx - 1));
    }
    return res;
}

poly_exp_t PolyDeg(const Poly *p) {
    if(PolyIsZero(p))
        return -1;
    if(PolyIsCoeff(p))
        return 0;
    poly_exp_t res = -1;
    for(size_t i = 0; i < p->size; i++) {
        res = max(res, PolyDeg(&p->arr[i].p) + p->arr[i].exp);
    }
    return res;
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

poly_coeff_t Exponantiate(poly_coeff_t x, poly_exp_t exp) {
    if(exp == 0)
        return 1;
    poly_coeff_t temp = Exponantiate(x, exp / 2);
    if(exp % 2 == 0)
        return temp * temp;
    return temp * temp * x;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if(PolyIsCoeff(p))
        return *p;

    Mono* monos = SafeCalloc(p->size, sizeof(Mono));
    Poly p_coeff, p_mul;
    for(size_t i = 0; i < p->size; i++) {
        p_coeff = PolyFromCoeff(Exponantiate(x, p->arr[i].exp));
        p_mul = PolyMul(&p->arr[i].p, &p_coeff);
        monos[i] = MonoFromPoly(&p_mul, 0);
    }
    Poly res = PolyAddMonos(p->size, monos);
    free(monos);
    return res;
}











