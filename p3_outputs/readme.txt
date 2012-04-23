debug runs
./smp_cache 8192 8 64 canneal.04t.debug > debug_8192_64.out
./smp_cache 32768 8 32 canneal.04t.debug > debug_32768_32.out

validation runs
./smp_cache 8192 8 64 canneal.04t.longTrace > val_8192_64.out
./smp_cache 32768 8 32 canneal.04t.longTrace > val_32768_32.out

Extra runs with longTrace
./smp_cache 524288 8 64 canneal.04t.longTrace > val_524288_64.out
./smp_cache 1048576 8 128 canneal.04t.longTrace > val_1048576_128.out
