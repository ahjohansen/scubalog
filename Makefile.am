# These are the subdirectories that have Makefiles
SUBDIRS = scubalog po

EXTRA_DIST = scubalog.spec automoc


dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/automoc
	DOCUDIR=$$PWD/documentation && cd $(top_distdir) && cp -R $$DOCUDIR .

codedoc:
	doxygen scubalog.dox
