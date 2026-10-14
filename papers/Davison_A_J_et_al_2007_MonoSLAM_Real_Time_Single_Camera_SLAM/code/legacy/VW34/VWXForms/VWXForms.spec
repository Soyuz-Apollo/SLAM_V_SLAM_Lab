Summary: The VW library - XForms extensions
Name: VWXForms
Version: --VERSION--
Release: --RELEASE--
URL: http://www.robots.ox.ac.uk/ActiveVision/VW
Source0: VW-%{version}.tgz
License: commercial
Group: Development/Libraries
BuildRoot: /tmp/%{name}-buildroot
Requires: VW >= %{version}, xforms >= 0.88, xforms-devel >= 0.88

%description
Although seldom used now, XForms was once a popular method fro creating GUI's, and this library provides a VW display class for XForms.

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
install -m 755 lib-i686/libVWXForms.a $RPM_BUILD_ROOT/usr/lib/libVWXForms.a
# Install the includes
#   first make the directory
install -d -m 755 include/VW $RPM_BUILD_ROOT/usr/include/VW
#   now the main headers
install -d -m 755 include/VW/DisplayXForms $RPM_BUILD_ROOT/usr/include/VW/DisplayXForms
install -m 755 include/VW/displayxforms.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/DisplayXForms/*.h $RPM_BUILD_ROOT/usr/include/VW/DisplayXForms/

install -d -m 755 include/VW/InterfaceXForms $RPM_BUILD_ROOT/usr/include/VW/InterfaceXForms
install -m 755 include/VW/interfacexforms.h $RPM_BUILD_ROOT/usr/include/VW/
#install -m 755 include/VW/InterfaceXForms/*.h $RPM_BUILD_ROOT/usr/include/VW/InterfaceXForms/


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

# The library file
/usr/lib/libVWXForms.a

# Other files are added automatically 
#   (headers into /usr/include/VW)
