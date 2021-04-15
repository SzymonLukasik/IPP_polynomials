#include <stdlib.h>
#include "poly.h"
#include "safealloc.h"


void PolyDestroy(Poly *p) {
    if(p->arr != NULL)
        for(size_t i = 0; i < p->size; ++i) {
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
        for(size_t i = 0; i < copy.size; ++i)
            copy.arr[i] = MonoClone(&p->arr[i]);
    }
    return copy;
}

Poly PolyAdd(const Poly *p, const Poly *q) {

}

Poly PolyAddMonos(size_t count, const Mono monos[]) {

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











