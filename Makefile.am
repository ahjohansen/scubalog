# These are the subdirectories that have Makefiles
SUBDIRS = scubalog po

EXTRA_DIST = scubalog.spec automoc


dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/automoc

codedoc:
	doxygen scubalog.dox
