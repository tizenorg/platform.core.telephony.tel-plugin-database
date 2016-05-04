%define major 0
%define minor 1
%define patchlevel 26

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
BuildRequires:  pkgconfig(libtzplatform-config)
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

mkdir -p %TZ_SYS_DB

rm -f %TZ_SYS_DB/.mcc_mnc_oper_list.db
sqlite3 %TZ_SYS_DB/.mcc_mnc_oper_list.db < /tmp/mcc_mnc_oper_list.sql
rm -f /tmp/mcc_mnc_oper_list.sql

chmod 600 %TZ_SYS_DB/.mcc_mnc_oper_list.db
chown telephony:telephony %TZ_SYS_DB/.mcc_mnc_oper_list.db
chsmack -a System %TZ_SYS_DB/.mcc_mnc_oper_list.db
chmod 644 %TZ_SYS_DB/.mcc_mnc_oper_list.db-journal
chown telephony:telephony %TZ_SYS_DB/.mcc_mnc_oper_list.db-journal
chsmack -a System %TZ_SYS_DB/.mcc_mnc_oper_list.db-journal

%if 0%{?sec_product_feature_telephony_cdma}
	rm -f %TZ_SYS_DB/.mcc_sid_list.db
	sqlite3 %TZ_SYS_DB/.mcc_sid_list.db < /tmp/mcc_sid_list.sql
	rm -f /tmp/mcc_sid_list.sql
	chmod 600 %TZ_SYS_DB/.mcc_sid_list.db
	chown telephony:telephony %TZ_SYS_DB/.mcc_mnc_oper_list.db
	chsmack -a System %TZ_SYS_DB/.mcc_mnc_oper_list.db
	chmod 644 %TZ_SYS_DB/.mcc_sid_list.db-journal
	chown telephony:telephony %TZ_SYS_DB/.mcc_mnc_oper_list.db-journal
	chsmack -a System %TZ_SYS_DB/.mcc_mnc_oper_list.db-journal
%endif

%postun -p /sbin/ldconfig

%install
%make_install
mkdir -p %{buildroot}%{_datadir}/license

%files
%defattr(644,root,root,-)
/tmp/mcc_mnc_oper_list.sql
%if 0%{?sec_product_feature_telephony_cdma}
	/tmp/mcc_sid_list.sql
%endif
#%doc COPYING
%{_libdir}/telephony/plugins/db-plugin*
%{_datadir}/license/tel-plugin-database
