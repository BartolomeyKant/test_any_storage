#include "inheritance/any_storage_inh.h"

#include "test_tables.h"

#ifndef COUNT
#define COUNT 100
#endif

using Storage = it::AnyStorage<sizeof(Foo<0>), alignof(Foo<0>)>;

int main() { return test_storage<Storage, COUNT>(); }
