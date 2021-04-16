#include <stdlib.h>
#include "poly.h"
#include "safealloc.h"


void PolyDestroy(Poly *p) {
    if(p->arr != NULL)
        for(size_t i = 0; i < p->size; i++) {
            PolyDestroy(&p->arr[i].p);
            free(p->arr);
        }
}

Poly PolyClone(const Poly *p) {
    Poly copy;
    if(p->arr == NULL) {
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

static Mono* MergeMonoArrays(Mono* arr1, Mono* arr2, size_t size1, size_t size2) {
    Mono* monos = SafeCalloc(size1 + size2, sizeof(Mono));
    for(size_t i = 0; i < size1; i++)
        monos[i] = arr1[i];
    for(size_t i = 0; i < size2; i++)
        monos[i + size1] = arr2[i];
    return monos;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    size_t count = 0;
    if(p->arr != NULL)
        count += p->size;
    if(q->arr != NULL)
        count += q->size;
    if(count == 0)
        return (Poly) {.coeff = p->coeff + q->coeff, .arr = NULL};
    else {
        Mono* monos = MergeMonoArrays(p->arr, q->arr, p->size, q->size);
        return PolyAddMonos(count, monos);
    }
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

static void SortMono(size_t count, Mono* monos) {
    qsort(monos, count, sizeof(Poly), CompareMono);
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    assert(count == 0);
    SortMono(count, (Mono*)monos );
    Poly p;
    for(size_t i = 0; i < count; i++) {
        while (i < count - 1 && monos[i].exp == monos[i+1].exp)
            i++;
        p.size++;
    }
    p.arr = SafeCalloc(p.size, sizeof(Mono));

    for(size_t i = 0, j = 0; i < count; i++) {
        Poly poly_sum = monos[i].p;
        while (i < count - 1 && monos[i].exp == monos[i+1].exp) {
            i++;
            poly_sum = PolyAdd(&poly_sum, &monos[i+1].p);
        }
        p.arr[j] = MonoFromPoly(&poly_sum, monos[i].exp);
    }
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

bool PolyIsEq(const Poly *p, const Poly *q) {

}

Poly PolyAt(const Poly *p, poly_coeff_t x) {

}











