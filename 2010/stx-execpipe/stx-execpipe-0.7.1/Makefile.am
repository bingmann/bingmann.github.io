# $Id: Makefile.am 16 2010-07-30 15:04:11Z tb $

SUBDIRS = src testsuite examples

if GCOV

clean-local:
	find -name "*.da" -o -name "*.gcov" -o -name "*.gcda" -o -name "*.gcno" | xargs rm || true

run-gcov: clean-gcov
	mkdir -p coverage/
	lcov --directory . --zerocounters
	-$(MAKE) check
	lcov --directory . --capture --output-file coverage/all.info
	lcov -e coverage/all.info "/tdata/*" -o coverage/testsuite.info
	genhtml -o coverage --num-spaces 8 coverage/testsuite.info --title "STX ExecPipe Testsuite" --prefix `pwd`

clean-gcov:
	find -name "*.gcda" | xargs rm || true

endif

EXTRA_DIST = Doxyfile doxygen-html
