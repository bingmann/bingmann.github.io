# CMake targets for stxxl/utils

stxxl_build(pq_param)
stxxl_build(log2)

if (NOT MSVC)
  stxxl_build(malloc)
endif (NOT MSVC)

stxxl_build(createdisks)

if (NOT MSVC)
  stxxl_build(mlock)
endif (NOT MSVC)