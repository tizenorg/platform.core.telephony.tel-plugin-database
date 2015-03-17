%define major 0
%define minor 1
%define patchlevel 12

Name:           tel-plugin-database
Version:        %{major}.%{minor}.%{patchlevel}
Release:        1
License:        Apache
Summary:        Telephony DataBase storage plugin
Group:          System/Libraries
Source0:        tel-plugin-database-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(tcore)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Telephony DataBase storage plugin

%prep
%setup -q

%build
versionint=$[%{major} * 1000000 + %{minor} * 1000 + %{patchlevel}]
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DVERSION=$versionint
make %{?_smp_mflags}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%install
%make_install
mkdir -p %{buildroot}%{_datadir}/license

%files
%defattr(-,root,root,-)
#%doc COPYING
%{_libdir}/telephony/plugins/db-plugin*
%{_datadir}/license/tel-plugin-database
