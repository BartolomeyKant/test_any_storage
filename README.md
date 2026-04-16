# Compare different approach to type erasure management

## Compare sizes

The simple test in [src/test_tables.h](src/test_tables.h) creates `Count` of different `Foo` types and stores them in `vector` of `Storage`.
The `vector` then is copied and moved and return the value which is printed at the end.
To be sure everything is working as expected count of `Foo` destruction, copying and moving also is printed.

The `Storage` is defined in test_manage_table.cpp, test_vtable.cpp and test_inheritance_table.cpp.

Compare binary sizes of different N for clang and gcc compilers.

### Clang

```sh
clang --version
clang version 22.1.1
```
Comparing test_manage_table vs test_vtable vs test_inherit_table sizes in bytes:

| Count | test_manage_table | test_vtable | test_inherit_table |
| :--- | :--- | :--- | :--- |
|    10 |            133752 |      135024 |            135352 |
|    50 |            234272 |      241216 |            250536 |
|   100 |            351624 |      377960 |            384184 |
|   256 |            728112 |      785264 |            811152 |
|   512 |           1340376 |     1463128 |           1506680 |
|  1024 |           2569288 |     2819048 |           2906136 |
|  2048 |           5033128 |     5530896 |           5706056 |

### GCC

```sh
gcc --version
gcc (GCC) 15.2.1 20260209
```
Comparing test_manage_table vs test_vtable vs test_inherit_table sizes in bytes:

| Count | test_manage_table | test_vtable | test_inherit_table |
| :--- | :--- | :--- | :--- |
|    10 |            147344 |      152728 |            150112 |
|    50 |            224016 |      238784 |            259632 |
|   100 |            324336 |      349800 |            390984 |
|   256 |            605272 |      663928 |            765048 |
|   512 |           1039880 |     1161512 |           1351704 |
|  1024 |           1913392 |     2152832 |           2533384 |
|  2048 |           3663664 |     4148864 |           4901064 |

### Conclusion

Using more complicated manage table approach is more effective speaking about binary size.
