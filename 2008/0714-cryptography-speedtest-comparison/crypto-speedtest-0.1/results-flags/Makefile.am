# $Id: Makefile.am 269 2008-07-14 11:10:43Z tb $

pdfs: flagsplots.pl
	./flagsplots.pl
	./flags-graph.gnuplot

pngplots:
	for pdf in *.pdf; do \
	    echo Exploding $$pdf; \
	    pdftoppm -r 200 $$pdf $${pdf%%.pdf}; \
	    for p in $${pdf%%.pdf}-*.ppm; do \
	        echo Converting $$p to PNG; \
	        convert $$p $${p%%.ppm}.png; \
	        rm -f $$p; \
	    done; \
	done

EXTRA_DIST = \
	build-linux.sh run-linux.sh \
	build-mingw.sh run-win32.bat \
	*.vcproj *.sln \
	flagsplots.pl \
	flags-graph.gnuplot \
	*.pdf \
	gcc34-*/*.txt \
	gcc41-*/*.txt \
	icc-*/*.txt \
	mingw-*/*.txt \
	msvc8-*/*.txt \
	maxtable.pl maxtable.csv
