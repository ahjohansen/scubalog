#
# RPM specification file for ScubaLog
#

Name:      scubalog
Version:   0.1.3
Release:   1
Copyright: GPL
Group:     Applications
Prefix:    /usr
Source:    http://www.stud.ifi.uio.no/~andrej/scubalog/scubalog-0.1.3.tar.gz
URL:       http://www.stud.ifi.uio.no/~andrej/scubalog/
Packager:  André Johansen <andrej@ifi.uio.no>
Summary:   Dive logging application
%description
ScubaLog is a dive logging program for scuba divers.
It uses KDE and Qt, and integrates nicely into the K Desktop Environment.

%package codedoc
Group:     Documentation/Applications
Summary:   Code documentation for ScubaLog
%description codedoc
This package contains code documentation for ScubaLog.
This is only needed if you want to develop ScubaLog or want
to see how ScubaLog is created.

The documentation is created with doxygen, a program used to extract
documentation from the source files (this is known as literate programming).


%prep
%setup

%build
./configure
make

%install
make install

%files
/usr/bin/scubalog
/usr/share/applnk/Applications/scubalog.kdelnk
/usr/share/mimelnk/application/x-scubalog-logbook.kdelnk
/usr/share/icons/scubalog.xpm
/usr/share/icons/mini/scubalog.xpm
%doc AUTHORS
%doc COPYING
%doc INSTALL
%doc README
%doc NEWS
%doc TODO
%doc VERSION
%doc ChangeLog

%files codedoc
%doc documentation


%changelog
* Thu Feb 17 2000 André Johansen <andrej@ifi.uio.no>

- Changed default prefix to /usr to comply with Red Hat 6.1.
