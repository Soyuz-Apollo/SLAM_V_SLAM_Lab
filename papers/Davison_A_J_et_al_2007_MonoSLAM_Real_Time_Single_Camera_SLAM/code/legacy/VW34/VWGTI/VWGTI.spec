Summary: The VW library - GTI extensions
Name: VWGTI
Version: --VERSION--
Release: --RELEASE--
URL: http://www.robots.ox.ac.uk/ActiveVision/VW
Source0: VW-%{version}.tgz
License: commercial
Group: Development/Libraries
BuildRoot: /tmp/%{name}-buildroot
Requires: VW >= %{version}

%description
Support for the gti mobile robot.

%prep
%setup -q -n VW-%{version}

%build
make make &> /dev/null
make headers &> /dev/null
cd %{name}
make all &> make.log
echo "Errors" > ../%{name}-%{version}-%{release}.report
grep -i error make.log | wc -l >> ../%{name}-%{version}-%{release}.report
echo "Warnings" >> ../%{name}-%{version}-%{release}.report
grep -i warning make.log | wc -l >> ../%{name}-%{version}-%{release}.report
cp make.log ../%{name}-%{version}-%{release}.log

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/lib
mkdir -p $RPM_BUILD_ROOT/usr/include

# Install the library
install -m 755 lib-i686/libVWGTI.a $RPM_BUILD_ROOT/usr/lib/libVWGTI.a
# Install the includes
#   first make the directory
install -d -m 755 include/VW $RPM_BUILD_ROOT/usr/include/VW
#   now the main headers
install -d -m 755 include/VW/GTI $RPM_BUILD_ROOT/usr/include/VW/GTI
install -m 755 include/VW/gti.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/GTI/*.h $RPM_BUILD_ROOT/usr/include/VW/GTI/

install -d -m 755 include/VW/GTITransputer $RPM_BUILD_ROOT/usr/include/VW/GTITransputer
install -m 755 include/VW/gtitransputer.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/GTITransputer/*.h $RPM_BUILD_ROOT/usr/include/VW/GTITransputer/


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

# The library file
/usr/lib/libVWGTI.a

# Other files are added automatically 
#   (headers into /usr/include/VW)
