Summary: The VNL (Vision Numerics Library) reduced version
Name: VNL
Version: --VERSION--
Release: --RELEASE--
URL: http://www.robots.ox.ac.uk/ActiveVision/VW
Source0: VW-%{version}.tgz
License: commercial
Group: Development/Libraries
BuildRoot: /tmp/%{name}-buildroot

%description
A reduced (and slightly modded) version of the VXL project numerics library, which is required by the core of VW.

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
install -m 755 lib-i686/libVNL.a $RPM_BUILD_ROOT/usr/lib/libVNL.a
# Install the includes
#   first make the directories
install -d -m 755 include/VNL $RPM_BUILD_ROOT/usr/include/VNL
install -d -m 755 include/VNL/Algo $RPM_BUILD_ROOT/usr/include/VNL/Algo
#   now the main headers
install -m 755 include/netlib.h $RPM_BUILD_ROOT/usr/include/
install -m 755 include/VNL/*.h $RPM_BUILD_ROOT/usr/include/VNL/
install -m 755 include/VNL/*.tpp $RPM_BUILD_ROOT/usr/include/VNL/
install -m 755 include/VNL/Algo/*.h $RPM_BUILD_ROOT/usr/include/VNL/Algo/
install -m 755 include/VNL/Algo/*.tpp $RPM_BUILD_ROOT/usr/include/VNL/Algo/


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

# The library file
/usr/lib/libVNL.a
/usr/include/netlib.h

# Other files are added automatically 
#   (headers into /usr/include/vnl)
