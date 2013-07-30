# CMake targets for stxxl/containers

stxxl_test(bench_pqueue)
stxxl_test(benchmark_naive_matrix)
stxxl_test(copy_file)
stxxl_test(monotonic_pq)
stxxl_test(pq_benchmark)
stxxl_test(stack_benchmark)
stxxl_test(test_deque)
stxxl_test(test_deque2)
stxxl_test(test_ext_merger)
stxxl_test(test_ext_merger2)
stxxl_test(test_iterators)
stxxl_test(test_many_stacks)
stxxl_test(test_migr_stack)
stxxl_test(test_pqueue)
stxxl_test(test_queue)
stxxl_test(test_sorter)
stxxl_test(test_stack)
stxxl_test(test_vector)
stxxl_test(test_vector_export)
stxxl_test(test_vector_sizes)
stxxl_test(write_vector)
stxxl_test(write_vector2)

add_define(test_many_stacks "STXXL_VERBOSE_LEVEL=1")
add_define(test_ext_merger "STXXL_VERBOSE_LEVEL=1")
add_define(test_ext_merger2 "STXXL_VERBOSE_LEVEL=1")
add_define(bench_pqueue "STXXL_VERBOSE_LEVEL=0")
add_define(benchmark_naive_matrix "STXXL_VERBOSE_LEVEL=0")

add_define(test_deque2 "STXXL_VERBOSE_LEVEL=2")

# TESTS_MAP
stxxl_test(test_map)
stxxl_test(test_map_random)

#TESTS_BDB		 = berkeley_db_benchmark
#TESTS_LEDASM		 = leda_sm_pq_benchmark leda_sm_stack_benchmark
#TESTS_TPIE		 = tpie_stack_benchmark

add_subdirectory(btree)
