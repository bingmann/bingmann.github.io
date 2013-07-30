# CMake targets for stxxl/stream

stxxl_test(test_loop)
stxxl_test(test_materialize)
stxxl_test(test_naive_transpose)
stxxl_test(test_push_sort)
stxxl_test(test_sorted_runs)
stxxl_test(test_stream)
stxxl_test(test_stream1)

stxxl_build(test_intro1)

add_define(test_stream1 "STXXL_VERBOSE_LEVEL=1")
add_define(test_push_sort "STXXL_VERBOSE_LEVEL=0")
add_define(test_stream "STXXL_VERBOSE_LEVEL=1")
add_define(test_sorted_runs "STXXL_VERBOSE_LEVEL=0")
add_define(test_materialize "STXXL_VERBOSE_LEVEL=0" "STXXL_VERBOSE_MATERIALIZE=STXXL_VERBOSE0")

# ifeq ($(strip $(USE_MCSTL)),yes)
# # ICPC 10.0: internal error: backend signals (issue 466173)
# $(call reduce_optimization,3,1,test_stream,Version_10.0,32-bit,any)
# $(call reduce_optimization,3,1,test_stream1,Version_10.0,32-bit,any)
# $(call reduce_optimization,3,0,test_sorted_runs,Version_10.0,32-bit,any)
# $(call reduce_optimization,3,0,test_push_sort,Version_10.0,32-bit,any)
# $(call reduce_optimization,3,1,test_naive_transpose,Version_10.0,32-bit,any)
# $(call reduce_optimization,3,1,test_loop,Version_10.0,32-bit,any)
# endif
