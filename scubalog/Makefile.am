# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_sounddir      Where system sounds should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# this is where the kdelnk file will go 
APPSDIR = $(kde_appsdir)/Applications

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
SUBDIRS = doc pics

# this is where the source documentation will be installed
SRCDOC_DEST=$(kde_htmldir)/en/scubalog

EXTRA_DIST = scubalog.kdelnk

####### This part is very scubalog specific
# this is the program that gets installed
bin_PROGRAMS = 	scubalog

# Which sources should be compiled for scubalog.
scubalog_SOURCES = divelog.cpp divelogitem.cpp equipmentlog.cpp \
		   equipmentview.cpp exporter.cpp htmlexporter.cpp \
		   integerdialog.cpp kcelledit.cpp kcelleditview.cpp \
		   kdateedit.cpp kdatevalidator.cpp kintegeredit.cpp \
		   kintegervalidator.cpp ktabcontrol.cpp ktimeedit.cpp \
		   ktimevalidator.cpp locationlog.cpp locationview.cpp \
		   logbook.cpp loglistview.cpp logview.cpp main.cpp \
		   personalinfoview.cpp scubalog.cpp

# let automoc handle all of the meta source files (moc)
scubalog_METASOURCES = USE_AUTOMOC

# the library search path. 
scubalog_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
scubalog_LDADD = $(LIB_KFILE)

# these are the headers for your project
noinst_HEADERS = chunkio.h debug.h divelist.h divelog.h divelogitem.h \
		 equipmentlog.h equipmentview.h exporter.h htmlexporter.h \
		 integerdialog.h kcelledit.h kcelleditview.h kdateedit.h \
		 kdatevalidator.h kintegeredit.h kintegervalidator.h \
		 ktabcontrol.h ktimeedit.h ktimevalidator.h locationlog.h \
		 locationview.h logbook.h loglistview.h logview.h \
		 personalinfoview.h scubalog.h

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(scubalog_METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(scubalog_SOURCES) && mv messages.po ../po/scubalog.pot

# generate source documentation
srcdoc:
	$(mkinstalldirs) $(SRCDOC_DEST)
	kdoc -H -d $(SRCDOC_DEST) scubalog 	     $(noinst_HEADERS) -lqt

# just install datas here. Use install-exec-local for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) scubalog.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) pics/scubalog.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) pics/scubalog-mini.xpm $(kde_minidir)/scubalog.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(APPSDIR)/scubalog.kdelnk
	-rm -f $(kde_icondir)/scubalog.xpm
	-rm -f $(kde_minidir)/scubalog.xpm
