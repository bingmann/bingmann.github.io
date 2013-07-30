# CMake targets for stxxl/common

stxxl_test(test_random)
stxxl_test(test_manyunits2)
stxxl_test(test_globals)

stxxl_build(stxxl_info)

# Left-overs from GNUmakefile

# ifneq (,$(wildcard .svn))
# version.$(lo): STXXL_COMPILER_OPTIONS += -DHAVE_VERSION_SVN_DEFS
# endif

# ifneq (,$(wildcard version.defs))
# version.$(lo): STXXL_COMPILER_OPTIONS += -DHAVE_VERSION_DEFS
# endif
