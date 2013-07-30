# CMake targets for stxxl/mng

stxxl_test(test_aligned)
stxxl_test(test_block_alloc_strategy)
stxxl_test(test_buf_streams)
stxxl_test(test_mng)
stxxl_test(test_mng1)
stxxl_test(test_mng_recursive_alloc)
stxxl_test(test_pool_pair)
stxxl_test(test_prefetch_pool)
stxxl_test(test_read_write_pool)
stxxl_test(test_write_pool)

if (CPPUNIT_FOUND AND BUILD_TESTS)

  add_executable(unittest_mng unittest.cpp)
  target_link_libraries(unittest_mng ${STXXL_LIBS} ${CPPUNIT_LIBRARIES})
  add_test(unittest_mng unittest_mng)

endif (CPPUNIT_FOUND AND BUILD_TESTS)

add_define(test_mng "STXXL_VERBOSE_LEVEL=2")
add_define(test_mng1 "STXXL_VERBOSE_LEVEL=2")
add_define(test_write_pool "STXXL_VERBOSE_LEVEL=1")
add_define(test_prefetch_pool "STXXL_VERBOSE_LEVEL=2")
add_define(test_mng_recursive_alloc "STXXL_VERBOSE_LEVEL=3")

#ifeq ($(strip $(USE_MCSTL)),yes)
## ICPC 11.0: internal error: 0_1374 (issue 539748)
#$(call reduce_optimization,3,0,test_block_alloc_strategy,Version_11.0,any,any)

## ICPC 10.0: internal error: backend signals (issue 466173)
#$(call reduce_optimization,3,1,test_buf_streams,Version_10.0,32-bit,any)
#$(call reduce_optimization,3,1,unittest,Version_10.0,32-bit,any)
#endif
