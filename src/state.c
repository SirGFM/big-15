#include "state.h"

void push(struct stateHandler **self, struct stateHandler *tmp) {
    tmp->parent = *self;
    tmp->setup(tmp);

    *self = tmp;
}

void pop(struct stateHandler **self) {
    (*self)->release(*self);
    *self = (*self)->parent;
}
