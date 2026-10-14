Summary: The VW library - GTK extensions
Name: VWGTK
Version: --VERSION--
Release: --RELEASE--
URL: http://www.robots.ox.ac.uk/ActiveVision/VW
Source0: VW-%{version}.tgz
License: commercial
Group: Development/Libraries
BuildRoot: /tmp/%{name}-buildroot
Requires: VW >= %{version}, VWGL >= %{version}, gtkmm >= 1.2.5, gtkglareamm >= 1.1

%description


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
install -m 755 lib-i686/libVWGTK.a $RPM_BUILD_ROOT/usr/lib/libVWGTK.a
# Install the includes
#   first make the directory
install -d -m 755 include/VW $RPM_BUILD_ROOT/usr/include/VW
#   now the main headers
install -d -m 755 include/VW/DisplayGTK $RPM_BUILD_ROOT/usr/include/VW/DisplayGTK
install -m 755 include/VW/displaygtk.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/DisplayGTK/*.h $RPM_BUILD_ROOT/usr/include/VW/DisplayGTK/

install -d -m 755 include/VW/InterfaceGTK $RPM_BUILD_ROOT/usr/include/VW/InterfaceGTK
install -m 755 include/VW/interfacegtk.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/InterfaceGTK/*.h $RPM_BUILD_ROOT/usr/include/VW/InterfaceGTK/

install -d -m 755 include/VW/Pixmaps $RPM_BUILD_ROOT/usr/include/VW/Pixmaps
install -m 755 include/VW/pixmaps.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/Pixmaps/*.h $RPM_BUILD_ROOT/usr/include/VW/Pixmaps/


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

# The library file
/usr/lib/libVWGTK.a

# Other files are added automatically 
#   (headers into /usr/include/VW)
