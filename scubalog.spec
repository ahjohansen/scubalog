#
# RPM specification file for ScubaLog
#

Name:      scubalog
Version:   0.2.0
Release:   1
Copyright: GPL
Group:     Applications
Prefix:    /usr
Source:    http://www.stud.ifi.uio.no/~andrej/scubalog/scubalog-0.2.0.tar.gz
URL:       http://www.stud.ifi.uio.no/~andrej/scubalog/
Packager:  André Johansen <andrej@ifi.uio.no>
Summary:   Dive logging application
BuildRoot: /var/tmp/%{name}-root

%description
ScubaLog is a dive logging program for scuba divers.
It uses KDE and Qt, and integrates nicely into the K Desktop Environment.

%prep
%setup -q

%build
./configure --prefix=$RPM_BUILD_ROOT/usr
make

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/bin/scubalog
/usr/share/applnk/Applications/scubalog.desktop
/usr/share/doc/HTML/en/scubalog
/usr/share/icons/locolor/16x16/apps/scubalog.png
/usr/share/icons/locolor/32x32/apps/scubalog.png
/usr/share/locale/es/LC_MESSAGES/scubalog.mo
/usr/share/locale/no/LC_MESSAGES/scubalog.mo
%doc AUTHORS
%doc ChangeLog
%doc COPYING
%doc INSTALL
%doc NEWS
%doc README
%doc TODO
%doc VERSION


%changelog
* Sat Nov 24 2001 André Johansen <andrej@world-online.no>
- Updated to work with KDE v2 in Red Hat 7.2.
- Removed code documentation package.

* Thu Feb 17 2000 André Johansen <andrej@ifi.uio.no>
- Changed default prefix to /usr to comply with Red Hat 6.1.
