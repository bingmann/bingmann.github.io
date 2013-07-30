# kurze Makefile

all: skript

.PHONY: skript web clean

skript: Algebra1.tex
	pdflatex Algebra1.tex
	makeindex Algebra1.idx

clean:
	rm -f Algebra1.aux Algebra1.log Algebra1.idx Algebra1.ilg Algebra1.ind Algebra1.thm Algebra1.out Algebra1.toc

web: index.html clean

Algebra1.zip: Makefile Algebra1.tex extarrows.sty a1.css webindex.xsl pics/*
	rm -f $@
	zip -9 -r $@ $^

index.html: Algebra1.pdf Algebra1.zip webindex.xsl
	( PDFSIZE=`du Algebra1.pdf | cut -f 1`; \
	  ZIPSIZE=`du Algebra1.zip | cut -f 1`; \
	  svn log --verbose --xml Algebra1.tex | \
	  xsltproc -o index.html --stringparam PDFSIZE $$PDFSIZE --stringparam ZIPSIZE $$ZIPSIZE webindex.xsl - )
