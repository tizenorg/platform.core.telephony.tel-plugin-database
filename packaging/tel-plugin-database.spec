%define major 0
%define minor 1
%define patchlevel 19

Name:           tel-plugin-database
Version:        %{major}.%{minor}.%{patchlevel}
Release:        1
License:        Apache-2.0
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
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DLIB_INSTALL_DIR=%{_libdir} \
	-DVERSION=$versionint \
%if 0%{?sec_product_feature_telephony_cdma}
    -DOPERATOR_CDMA=1 \
%endif

make %{?_smp_mflags}

%post
/sbin/ldconfig

mkdir -p /opt/dbspace

rm -f /opt/dbspace/.mcc_mnc_oper_list.db
sqlite3 /opt/dbspace/.mcc_mnc_oper_list.db < /tmp/mcc_mnc_oper_list.sql
rm -f /tmp/mcc_mnc_oper_list.sql

chmod 600 /opt/dbspace/.mcc_mnc_oper_list.db
chown system:system /opt/dbspace/.mcc_mnc_oper_list.db
chmod 644 /opt/dbspace/.mcc_mnc_oper_list.db-journal
chown system:system /opt/dbspace/.mcc_mnc_oper_list.db-journal
chsmack -a 'telephony_framework::db' /opt/dbspace/.mcc_mnc_oper_list.db*

%if 0%{?sec_product_feature_telephony_cdma}
	rm -f /opt/dbspace/.mcc_sid_list.db
	sqlite3 /opt/dbspace/.mcc_sid_list.db < /tmp/mcc_sid_list.sql
	rm -f /tmp/mcc_sid_list.sql
	chmod 600 /opt/dbspace/.mcc_sid_list.db
	chown system:system /opt/dbspace/.mcc_mnc_oper_list.db
	chmod 644 /opt/dbspace/.mcc_sid_list.db-journal
	chown system:system /opt/dbspace/.mcc_mnc_oper_list.db-journal
	chsmack -a 'telephony_framework::db' /opt/dbspace/.mcc_sid_list.db*
%endif

%postun -p /sbin/ldconfig

%install
%make_install
mkdir -p %{buildroot}%{_datadir}/license

%files
%defattr(644,system,system,-)
/tmp/mcc_mnc_oper_list.sql
%if 0%{?sec_product_feature_telephony_cdma}
	/tmp/mcc_sid_list.sql
%endif
#%doc COPYING
%{_libdir}/telephony/plugins/db-plugin*
%{_datadir}/license/tel-plugin-database
