#include "vtable/any_storage_vt.h"

#include "test_tables.h"

#ifndef COUNT
#define COUNT 100
#endif

using Storage = vt::AnyStorage<sizeof(Foo<0>), alignof(Foo<0>)>;

int main() { return test_storage<Storage, COUNT>(); }
