####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = scubalog
scubalog_SOURCES = udcfexporter.cpp personalinfoview.cpp logview.cpp loglistview.cpp logbook.cpp locationview.cpp locationlog.cpp listbox.cpp ktimevalidator.cpp ktimeedit.cpp ktabcontrol.cpp kintegervalidator.cpp kintegeredit.cpp kdatevalidator.cpp kdateedit.cpp kcelleditview.cpp kcelledit.cpp integerdialog.cpp htmlexporter.cpp exporter.cpp equipmentview.cpp equipmentlog.cpp divelogitem.cpp divelog.cpp scubalog.cpp main.cpp 
scubalog_LDADD   = -lkio -lkdeui -lkdecore $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = scubalog.desktop lo32-app-scubalog.png lo16-app-scubalog.png main.cpp scubalog.cpp chunkio.h debug.h divelist.h divelog.cpp divelog.h divelogitem.cpp divelogitem.h equipmentlog.cpp equipmentlog.h equipmentview.cpp equipmentview.h exporter.cpp exporter.h htmlexporter.cpp htmlexporter.h integerdialog.cpp integerdialog.h kcelledit.cpp kcelledit.h kcelleditview.cpp kcelleditview.h kdateedit.cpp kdateedit.h kdatevalidator.cpp kdatevalidator.h kintegeredit.cpp kintegeredit.h kintegervalidator.cpp kintegervalidator.h ktabcontrol.cpp ktabcontrol.h ktimeedit.cpp ktimeedit.h ktimevalidator.cpp ktimevalidator.h listbox.cpp listbox.h locationlog.cpp locationlog.h locationview.cpp locationview.h logbook.cpp logbook.h loglistview.cpp loglistview.h logview.cpp logview.h scubalog.h personalinfoview.h personalinfoview.cpp udcfexporter.cpp udcfexporter.h 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/scubalog.desktop $(kde_appsdir)/Applications/scubalog.desktop
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-scubalog.png $(kde_icondir)/locolor/32x32/apps/scubalog.png
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/lo16-app-scubalog.png $(kde_icondir)/locolor/16x16/apps/scubalog.png

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/scubalog.desktop
	-rm -f $(kde_icondir)/locolor/32x32/apps/scubalog.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/scubalog.png

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
scubalog_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDE
#rcdir = $(kde_datadir)/scubalog
#rc_DATA = scubalogui.rc

#WARNING: if you use a ui.rc file above, use:

# messages: rc.cpp

# instead of 

# messages:

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/scubalog.pot; \
	fi

messages2:
	$(XGETTEXT) -C -ki18n $(scubalog_SOURCES) && mv messages.po ../po/scubalog.pot
