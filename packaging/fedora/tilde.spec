Name:           tilde
Version:        0.1.1
Release:        1%{?dist}
Summary:        A native C++ Markdown and LaTeX editor built on Qt 6 and KDE KatePart
License:        GPL-3.0-only
URL:            https://github.com/funnym0th/Tilde
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  cmake
BuildRequires:  ninja-build
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qttools-devel
BuildRequires:  qt6-qtpdf-devel
BuildRequires:  kf6-ktexteditor-devel
BuildRequires:  desktop-file-utils

Requires:       qt6-qtbase
Requires:       qt6-qtpdf
Requires:       kf6-ktexteditor
Requires:       texlive-scheme-basic

%description
Tilde is a native C++ document editor for Markdown and LaTeX built with Qt 6
and the KDE KTextEditor engine. It features real-time live preview,
synchronized scrolling, and direct PDF compilation without Electron bloat.

%prep
%autosetup

%build
%cmake -G Ninja
%cmake_build

%install
%cmake_install
desktop-file-validate %{buildroot}%{_datadir}/applications/tilde.desktop

%files
%license LICENSE
%doc README.md
%{_bindir}/tilde
%{_datadir}/applications/tilde.desktop
%{_datadir}/icons/hicolor/scalable/apps/tilde.svg

%changelog
* Tue Jul 22 2026 funnym0th <funnym0th@users.noreply.github.com> - 0.1.1-1
- Add help menu with about dialog
- Add close file action to file menu
- Update README

* Tue Jul 21 2026 funnym0th <funnym0th@users.noreply.github.com> - 0.1.0-1
- Initial release v0.1.0
