# $Id: Makefile.am 99 2008-08-13 11:15:13Z tb $

SUBDIRS = . include testsuite speedtest wxbtreedemo

AM_CFLAGS = -W -Wall -I$(srcdir)/include
AM_CXXFLAGS = -W -Wall -Wold-style-cast -DBTREE_DEBUG -I$(srcdir)/include

EXTRA_DIST = Doxyfile doxygen.css \
	doxygen-html/*

if GCOV

clean-local:
	find -name "*.da" -o -name "*.gcov" -o -name "*.gcda" -o -name "*.gcno" | xargs rm || true

run-gcov: clean-gcov
	mkdir -p coverage/
	lcov --directory . --zerocounters
	-$(MAKE) check
	lcov --directory . --capture --output-file coverage/all.info
	lcov -e coverage/all.info "/tdata/*" -o coverage/testsuite.info
	genhtml -o coverage --num-spaces 8 coverage/testsuite.info --title "STX B+ Tree Testsuite" --prefix `pwd` --legend

clean-gcov:
	find -name "*.gcda" | xargs rm || true

endif
