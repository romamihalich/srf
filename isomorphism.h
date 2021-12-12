#ifndef ISOMORPHISM_H
#define ISOMORPHISM_H

#include "list.h"
#include "semiring.h"

void generate_arrays(List* arrays);
bool areisomorphic(Semiring r1, Semiring r2, List arrays);

#endif // !ISOMORPHISM_H
