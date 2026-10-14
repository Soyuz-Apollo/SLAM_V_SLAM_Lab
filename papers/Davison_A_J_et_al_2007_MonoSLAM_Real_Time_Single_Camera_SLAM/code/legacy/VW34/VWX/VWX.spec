Summary: The VW library - X extensions
Name: VWX
Version: --VERSION--
Release: --RELEASE--
URL: http://www.robots.ox.ac.uk/ActiveVision/VW
Source0: VW-%{version}.tgz
License: commercial
Group: Development/Libraries
BuildRoot: /tmp/%{name}-buildroot
Requires: VW >= %{version}

%description
This library provides some GUI stuff using raw X. This is the simplest method (ie. fewest dependencies) for getting an image onto the screen.

%prep
%setup -q -n VW-%{version}

%build
make make    &> /dev/null
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
install -m 755 lib-i686/libVWX.a $RPM_BUILD_ROOT/usr/lib/libVWX.a
# Install the includes
#   first make the directory
install -d -m 755 include/VW $RPM_BUILD_ROOT/usr/include/VW
#   now the main headers
install -d -m 755 include/VW/DisplayX $RPM_BUILD_ROOT/usr/include/VW/DisplayX
install -m 755 include/VW/displayx.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/DisplayX/*.h $RPM_BUILD_ROOT/usr/include/VW/DisplayX/

install -d -m 755 include/VW/InterfaceX $RPM_BUILD_ROOT/usr/include/VW/InterfaceX
install -m 755 include/VW/interfacex.h $RPM_BUILD_ROOT/usr/include/VW/
#install -m 755 include/VW/InterfaceX/*.h $RPM_BUILD_ROOT/usr/include/VW/InterfaceX/


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

# The library file
/usr/lib/libVWX.a

# Other files are added automatically 
#   (headers into /usr/include/VW)
