# CMake targets for stxxl/io

stxxl_extra(benchmark_configured_disks)
stxxl_extra(benchmark_disk_and_flash)
stxxl_extra(benchmark_random_block_access)
stxxl_extra(flushbuffers)
stxxl_extra(gen_file)
stxxl_extra(iobench_scatter_in_place)
stxxl_extra(test_cancel)
stxxl_extra(test_io)
stxxl_extra(test_io_sizes)
stxxl_extra(verify_disks)

stxxl_extra(benchmark_disks)

if (NOT MSVC)
  stxxl_extra(sd_test)
endif (NOT MSVC)

if (CPPUNIT_FOUND AND BUILD_TESTS)

  add_executable(unittest_io unittest.cpp)
  target_link_libraries(unittest_io ${STXXL_LIBS} ${CPPUNIT_LIBRARIES})
  add_test(unittest_io unittest_io)

endif (CPPUNIT_FOUND AND BUILD_TESTS)
