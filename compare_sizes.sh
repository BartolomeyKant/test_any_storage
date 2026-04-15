#!/usr/bin/bash

COUNTS=(10 50 100 256 512 1024 2048)

manage_table_sizes=()
vtable_sizes=()

for count in "${COUNTS[@]}"; do
    echo "test Count: $count"
    cmake -DCMAKE_CXX_FLAGS="-DCOUNT=$count" . > /dev/null
    cmake --build . --parallel > /dev/null

    if [[ $? -ne 0 ]]; then
        echo "Build failed for count $count"
        exit 1
    fi

    manage_table_sizes+=($(stat -c "%s" "./test_manage_table"))
    vtable_sizes+=($(stat -c "%s" "./test_vtable"))
done

# print results
printf "Comparing test_manage_table vs test_vtable sizes in bytes:\n"
printf "%5s | %17s | %11s \n" "Count" "test_manage_table" "test_vtable"
for i in "${!COUNTS[@]}"; do
    printf "%5s |" ${COUNTS[$i]}
    printf " %17s |" ${manage_table_sizes[$i]}
    printf " %11s \n" ${vtable_sizes[$i]}
done
