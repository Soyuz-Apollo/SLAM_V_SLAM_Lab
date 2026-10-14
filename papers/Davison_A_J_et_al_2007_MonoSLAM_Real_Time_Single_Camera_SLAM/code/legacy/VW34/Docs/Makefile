all: UserGuide SourceRef

UserGuide: UserGuideHTML UserGuideInfo UserGuidePDF

SourceRef:
	cd ..; \
	doxygen Docs/Bits/doxygen.config; \
	cp Docs/Bits/search.php Docs/Bits/search.html Docs/HTML/SourceDocs/;

UserGuideHTML:
	mkdir -p HTML/UserGuide
	- cp UserGuide/*.png HTML/UserGuide
	cd UserGuide; \
	texi2html -expandinfo -menu -number -split_chapter -nosec_nav -frames main.texi -prefix userguide
	mv UserGuide/userguide/*.html HTML/UserGuide
	rmdir UserGuide/userguide

UserGuideInfo:
	cd UserGuide; \
	makeinfo main.texi -o ../Info/userguide.info;

UserGuidePDF:
	cd UserGuide; \
	texi2pdf main.texi; \
	mv main.pdf ../PDF/userguide.pdf; \
	rm -f main.aux main.cp main.fn main.ky main.log main.pg main.toc main.tc  main.tp main.vr

clean:
	rm -rf HTML/UserGuide HTML/SourceDocs; \
	rm -rf Info/* PDF/*;
