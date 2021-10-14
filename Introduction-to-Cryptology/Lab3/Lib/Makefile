all: tools crypto

tools:
	cd Tools; make

crypto:
	cd Crypto; make

##### install the doxygen package, probably along with graphviz
##### it will produce html and latex output which are quite nice
Doxyfile:
	doxygen -g
##	sed 's/OPTIMIZE_OUTPUT_FOR_C  = NO/OPTIMIZE_OUTPUT_FOR_C  = YES/' Doxyfile > /tmp/foo

doc: doxygen
	mv latex/refman.pdf inf558_lib.pdf

doxygen: Doxyfile.inf558
	doxygen Doxyfile.inf558
	cd latex; pdflatex refman; makeindex refman; pdflatex refman

clean:
	cd Tools; make clean
	cd Crypto; make clean

