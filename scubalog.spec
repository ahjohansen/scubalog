#
# RPM specification file for ScubaLog
# $Id$
#

Name:      scubalog
Version:   0.1.0
Release:   1
Copyright: GPL
Group:     Applications
Prefix:    /opt/kde
Source:    http://www.stud.ifi.uio.no/~andrej/scubalog/scubalog-0.1.0.tar.gz
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
/opt/kde/bin/scubalog
/opt/kde/share/applnk/Applications/scubalog.kdelnk
/opt/kde/share/icons/scubalog.xpm
/opt/kde/share/icons/mini/scubalog.xpm
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
