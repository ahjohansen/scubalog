#
# RPM specification file for ScubaLog
#

Name:      scubalog
Version:   0.2.2
Release:   1
Copyright: GPL
Group:     Applications
Prefix:    /usr
Source:    http://home.tiscali.no/andrej/scubalog/scubalog-0.2.2.tar.bz2
URL:       http://home.tiscali.no/andrej/scubalog/
Packager:  André Johansen <andrej@tiscali.no>
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
mkdir -p ${RPM_BUILD_ROOT}%{prefix}/share/icons/hicolor/16x16/apps
mkdir -p ${RPM_BUILD_ROOT}%{prefix}/share/icons/hicolor/32x32/apps
mkdir -p ${RPM_BUILD_ROOT}%{prefix}/share/icons/hicolor/48x48/apps
cp ${RPM_BUILD_ROOT}%{prefix}/share/icons/locolor/16x16/apps/scubalog.png \
   ${RPM_BUILD_ROOT}%{prefix}/share/icons/hicolor/16x16/apps/scubalog.png
cp ${RPM_BUILD_ROOT}%{prefix}/share/icons/locolor/32x32/apps/scubalog.png \
   ${RPM_BUILD_ROOT}%{prefix}/share/icons/hicolor/32x32/apps/scubalog.png
cp ${RPM_BUILD_ROOT}%{prefix}/share/icons/locolor/48x48/apps/scubalog.png \
   ${RPM_BUILD_ROOT}%{prefix}/share/icons/hicolor/48x48/apps/scubalog.png

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{prefix}/bin/scubalog
%{prefix}/share/applnk/Applications/scubalog.desktop
%{prefix}/share/doc/HTML/en/scubalog
%{prefix}/share/icons/locolor/16x16/apps/scubalog.png
%{prefix}/share/icons/locolor/32x32/apps/scubalog.png
%{prefix}/share/icons/locolor/48x48/apps/scubalog.png
%{prefix}/share/icons/hicolor/16x16/apps/scubalog.png
%{prefix}/share/icons/hicolor/32x32/apps/scubalog.png
%{prefix}/share/icons/hicolor/48x48/apps/scubalog.png
%{prefix}/share/locale/es/LC_MESSAGES/scubalog.mo
%{prefix}/share/locale/no/LC_MESSAGES/scubalog.mo
%doc AUTHORS
%doc ChangeLog
%doc COPYING
%doc INSTALL
%doc NEWS
%doc README
%doc TODO
%doc VERSION


%changelog
* Tue Jun 04 2002 André Johansen <andrej@tiscali.no>
- Ported to KDE v3 on Red Hat 7.3.

* Sat Nov 24 2001 André Johansen <andrej@world-online.no>
- Updated to work with KDE v2 in Red Hat 7.2.
- Removed code documentation package.

* Thu Feb 17 2000 André Johansen <andrej@ifi.uio.no>
- Changed default prefix to /usr to comply with Red Hat 6.1.
