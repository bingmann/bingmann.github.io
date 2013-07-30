# $Id: Makefile.am 59 2007-07-17 14:43:23Z tb $

SUBDIRS = libstx-exparser testsuite examples wxparserdemo perl-binding

EXTRA_DIST = Doxyfile doxygen.css

if GCOV

clean-local:
	find -name "*.da" -o -name "*.gcov" -o -name "*.gcda" -o -name "*.gcno" | xargs rm || true

run-gcov: clean-gcov
	mkdir -p coverage/
	lcov --directory . --zerocounters --path `pwd`
	-$(MAKE) check
	lcov --directory . --capture --output-file coverage/all.info
	lcov -r coverage/all.info "/usr/*" -o coverage/testsuite.info
	genhtml -o coverage --num-spaces 8 coverage/testsuite.info --title "STX ExpressionParser Testsuite" --prefix `pwd`

clean-gcov:
	find -name "*.gcda" | xargs rm || true

endif
