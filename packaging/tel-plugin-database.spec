%define major 3
%define minor 0
%define patchlevel 1

Name: tel-plugin-database
Summary: Telephony DataBase storage plugin
Version:    %{major}.%{minor}.%{patchlevel}
Release:    1
Group:      System/Libraries
License:    Apache-2.0
Source0:    tel-plugin-database-%{version}.tar.gz
Source1001: 	tel-plugin-database.manifest
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(tcore)
BuildRequires:  pkgconfig(db-util)

%description
Telephony DataBase storage plugin

%prep
%setup -q
cp %{SOURCE1001} .

%build
%cmake .
make %{?jobs:-j%jobs}

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%install
%make_install
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/telephony/plugins/db-plugin*
/usr/share/license/%{name}
