# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
#SUBDIRS =

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = scubalog

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# the library search path. 
scubalog_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
scubalog_LDADD   = $(LIB_KFILE)

# which sources should be compiled for scubalog
scubalog_SOURCES = \
	dateitem.cpp \
	divelog.cpp \
	divelogitem.cpp \
	equipmentlog.cpp \
	equipmentview.cpp \
	htmlexporter.cpp \
	integerdialog.cpp \
	kcelledit.cpp \
	kcelleditview.cpp \
	kdateedit.cpp \
	kdatevalidator.cpp \
	kintegeredit.cpp \
	kintegervalidator.cpp \
	ktabcontrol.cpp \
	ktimeedit.cpp \
	ktimevalidator.cpp \
	listbox.cpp \
	locationlog.cpp \
	locationview.cpp \
	logbook.cpp \
	loglistview.cpp \
	logview.cpp \
	main.cpp \
	personalinfoview.cpp \
	scubalog.cpp \
	scubalogproject.cpp \
	udcfexporter.cpp

# these are the headers for your project that won't be installed
noinst_HEADERS = \
	chunkio.h \
	dateitem.h \
	debug.h \
	divelist.h \
	divelog.h \
	divelogitem.h \
	equipmentlog.h \
	equipmentview.h \
	integerdialog.h \
	kcelledit.h \
	kcelleditview.h \
	kdateedit.h \
	kdatevalidator.h \
	kintegeredit.h \
	kintegervalidator.h \
	ktabcontrol.h \
	ktimeedit.h \
	ktimevalidator.h \
	listbox.h \
	locationlog.h \
	locationview.h \
	logbook.h \
	loglistview.h \
	logview.h \
	personalinfoview.h \
	scubalog.h \
	importer.h \
	exporter.h \
	scubalogproject.h \
	htmlexporter.h \
	udcfexporter.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) -ki18n -o ../po/scubalog.pot $(scubalog_SOURCES)

# this is where the desktop file will go 
desktopdir   = $(kde_appsdir)/Applications
desktop_DATA = scubalog.desktop

install-data-local:
	$(mkinstalldirs) $(kde_icondir)/locolor/48x48/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-scubalog.png $(kde_icondir)/locolor/48x48/apps/scubalog.png
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-scubalog.png $(kde_icondir)/locolor/32x32/apps/scubalog.png
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/lo16-app-scubalog.png $(kde_icondir)/locolor/16x16/apps/scubalog.png

uninstall-local:
	-rm -f $(kde_icondir)/locolor/48x48/apps/scubalog.png
	-rm -f $(kde_icondir)/locolor/32x32/apps/scubalog.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/scubalog.png
