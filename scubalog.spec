#
# RPM specification file for ScubaLog
#

Name:		scubalog
Version:	0.2.3
Release:	1
Copyright:	GPL
Group:		Applications/Productivity
Prefix:		/usr
Source:		http://home.tiscali.no/andrej/scubalog/%{name}-%{version}.tar.bz2
URL:		http://home.tiscali.no/andrej/scubalog/
Packager:	André Johansen <andrejoh@c2i.net>
Vendor:		André Johansen
Summary:	Dive logging application
BuildRoot:	/var/tmp/%{name}-root
Requires:	kdelibs >= 3
BuildPrereq:	kdelibs-devel >= 3, autoconf, automake
BuildRequires:	desktop-file-utils >= 0.2.93

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
mkdir -p ${RPM_BUILD_ROOT}%{_datadir}/icons/hicolor/16x16/apps
mkdir -p ${RPM_BUILD_ROOT}%{_datadir}/icons/hicolor/32x32/apps
mkdir -p ${RPM_BUILD_ROOT}%{_datadir}/icons/hicolor/48x48/apps
cp ${RPM_BUILD_ROOT}%{_datadir}/icons/locolor/16x16/apps/scubalog.png \
   ${RPM_BUILD_ROOT}%{_datadir}/icons/hicolor/16x16/apps/scubalog.png
cp ${RPM_BUILD_ROOT}%{_datadir}/icons/locolor/32x32/apps/scubalog.png \
   ${RPM_BUILD_ROOT}%{_datadir}/icons/hicolor/32x32/apps/scubalog.png
cp ${RPM_BUILD_ROOT}%{_datadir}/icons/locolor/48x48/apps/scubalog.png \
   ${RPM_BUILD_ROOT}%{_datadir}/icons/hicolor/48x48/apps/scubalog.png

desktop-file-install \
   --vendor kde \
   --dir $RPM_BUILD_ROOT%{_datadir}/applications \
   --add-category "Application;Utility;X-Red-Hat-Base;" \
   --delete-original \
   $RPM_BUILD_ROOT%{_datadir}/applnk/Applications/scubalog.desktop

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{_bindir}/scubalog
%{_datadir}/applications/kde-scubalog.desktop
%{_datadir}/icons/locolor/16x16/apps/scubalog.png
%{_datadir}/icons/locolor/32x32/apps/scubalog.png
%{_datadir}/icons/locolor/48x48/apps/scubalog.png
%{_datadir}/icons/hicolor/16x16/apps/scubalog.png
%{_datadir}/icons/hicolor/32x32/apps/scubalog.png
%{_datadir}/icons/hicolor/48x48/apps/scubalog.png
%{_datadir}/locale/es/LC_MESSAGES/scubalog.mo
%{_datadir}/locale/no/LC_MESSAGES/scubalog.mo
%doc AUTHORS
%doc ChangeLog
%doc COPYING
%doc INSTALL
%doc NEWS
%doc README
%doc TODO
%doc VERSION


%changelog
* Sat Apr 26 2003 André Johansen <andrejoh@c2i.net> 0.2.3-1
- ported to Red Hat Linux 9.

* Tue Jun 04 2002 André Johansen <andrej@tiscali.no>
- Ported to KDE v3 on Red Hat 7.3.

* Sat Nov 24 2001 André Johansen <andrej@world-online.no>
- Updated to work with KDE v2 in Red Hat 7.2.
- Removed code documentation package.

* Thu Feb 17 2000 André Johansen <andrej@ifi.uio.no>
- Changed default prefix to /usr to comply with Red Hat 6.1.
