# $Id: Makefile.am 269 2008-07-14 11:10:43Z tb $

pdfs: baseplots sidebysideplots

baseplots:
	PLOTNAME=p2-300-debian-etch ./speedtest.gnuplot
	PLOTNAME=p3-1000-debian-etch ./speedtest.gnuplot
	PLOTNAME=ath-2000-debian-etch ./speedtest.gnuplot
	PLOTNAME=cel-2660-debian-etch ./speedtest.gnuplot
	PLOTNAME=p4-3200-debian-etch ./speedtest.gnuplot
	PLOTNAME=p4-3200-debian-lenny ./speedtest.gnuplot
	PLOTNAME=p4-3200-gentoo ./speedtest.gnuplot
	PLOTNAME=p4-3200-ubuntu-gutsy ./speedtest.gnuplot
	PLOTNAME=p4-3200-ubuntu-hardy ./speedtest.gnuplot
	PLOTNAME=p4-3200-fedora8 ./speedtest.gnuplot
	./distrospeed.gnuplot

PAGELIST6=`for p in \`seq 1 23\`; do echo A$$p B$$p C$$p D$$p E$$p F$$p; done`

PAGELIST9=`for p in \`seq 1 23\`; do echo A$$p B$$p C$$p D$$p E$$p F$$p G$$p H$$p I$$p; done`

sidebysideplots:
# Make other plots which will fill an A4 paper

	PLOTNAME=p2-300-debian-etch FILENAME=p2-300-debian-etch-a4 PAPER=a4 ./speedtest.gnuplot
	PLOTNAME=p3-1000-debian-etch FILENAME=p3-1000-debian-etch-a4 PAPER=a4 ./speedtest.gnuplot
	PLOTNAME=ath-2000-debian-etch FILENAME=ath-2000-debian-etch-a4 PAPER=a4 ./speedtest.gnuplot
	PLOTNAME=cel-2660-debian-etch FILENAME=cel-2660-debian-etch-a4 PAPER=a4 ./speedtest.gnuplot
	PLOTNAME=p4-3200-debian-etch FILENAME=p4-3200-debian-etch-a4 PAPER=a4 ./speedtest.gnuplot
	PLOTNAME=p4-3200-debian-lenny FILENAME=p4-3200-debian-lenny-a4 PAPER=a4 ./speedtest.gnuplot
	PLOTNAME=p4-3200-gentoo FILENAME=p4-3200-gentoo-a4 PAPER=a4 ./speedtest.gnuplot
	PLOTNAME=p4-3200-ubuntu-gutsy FILENAME=p4-3200-ubuntu-gutsy-a4 PAPER=a4 ./speedtest.gnuplot
	PLOTNAME=p4-3200-fedora8 FILENAME=p4-3200-fedora8-a4 PAPER=a4 ./speedtest.gnuplot

# Extract pages in order from 9 different pdf
	pdftk \
		A=p2-300-debian-etch-a4.pdf B=p3-1000-debian-etch-a4.pdf C=ath-2000-debian-etch-a4.pdf \
		D=cel-2660-debian-etch-a4.pdf E=p4-3200-debian-etch-a4.pdf F=p4-3200-debian-lenny-a4.pdf \
		G=p4-3200-ubuntu-gutsy-a4.pdf H=p4-3200-gentoo-a4.pdf I=p4-3200-fedora8-a4.pdf \
		cat $(PAGELIST9) output sidebyside-comparison.pdf

	pdfnup --nup 3x3 --paper a4paper --orient landscape --frame false sidebyside-comparison.pdf
	rm sidebyside-comparison.pdf

# Extract pages in order from 6 different pdf
	pdftk \
		A=p2-300-debian-etch-a4.pdf B=p3-1000-debian-etch-a4.pdf C=ath-2000-debian-etch-a4.pdf \
		D=cel-2660-debian-etch-a4.pdf E=p4-3200-debian-etch-a4.pdf F=p4-3200-gentoo-a4.pdf \
		cat $(PAGELIST6) output cpu-sidebyside-comparison.pdf

	pdflatex -halt-on-error cpu-sidebyside-comparison-3x2.tex
	rm cpu-sidebyside-comparison.pdf
	rm cpu-sidebyside-comparison-3x2.aux cpu-sidebyside-comparison-3x2.log

	rm p2-300-debian-etch-a4.pdf p3-1000-debian-etch-a4.pdf ath-2000-debian-etch-a4.pdf
	rm cel-2660-debian-etch-a4.pdf p4-3200-debian-etch-a4.pdf p4-3200-debian-lenny-a4.pdf
	rm p4-3200-ubuntu-gutsy-a4.pdf p4-3200-gentoo-a4.pdf p4-3200-fedora8-a4.pdf

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
	README \
	speedtest.gnuplot \
	distrospeed.gnuplot \
	*.pdf \
	p2-300-debian-etch/*.txt \
	p3-1000-debian-etch/*.txt \
	ath-2000-debian-etch/*.txt \
	cel-2660-debian-etch/*.txt \
	p4-3200-debian-etch/*.txt \
	p4-3200-debian-lenny/*.txt \
	p4-3200-gentoo/*.txt \
	p4-3200-ubuntu-gutsy/*.txt \
	p4-3200-ubuntu-hardy/*.txt \
	p4-3200-fedora8/*.txt \
	cpu-sidebyside-comparison-3x2.tex \
	maxtable.pl \
	*.csv
