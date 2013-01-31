Name:       tel-plugin-database
Summary:    Telephony DataBase storage plugin
Version:    0.1.7
Release:    1
Group:      System/Libraries
License:    Apache
Source0:    tel-plugin-database-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(tcore)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(db-util)

%description
Telephony DataBase storage plugin

%prep
%setup -q

%build
%cmake .
make %{?jobs:-j%jobs}

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%install
%make_install

%files
%defattr(-,root,root,-)
%{_libdir}/telephony/plugins/db-plugin*
/usr/share/license/tel-plugin-database
