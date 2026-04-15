#include "manage_table/any_storage_mt.h"

#include "test_tables.h"

#ifndef COUNT
#define COUNT 100
#endif

using Storage = mt::AnyStorage<sizeof(Foo<0>), alignof(Foo<0>)>;

int main() { return test_storage<Storage, COUNT>(); }
