Summary: The VW library
Name: VW
Version: --VERSION--
Release: --RELEASE--
URL: http://www.robots.ox.ac.uk/ActiveVision/VW
Source0: %{name}-%{version}.tgz
License: LGPL
Group: Development/Libraries
BuildRoot: /tmp/%{name}-buildroot
Requires: VNL >= %{version}

%description
VW consists of a library and a set of header files. This core library provides some image, image processing  and image-feature classes, basic computer vision algoirthms and simple file IO. There are also plug-in libraries available to provide hardware interfacing and a GUI.


%prep
%setup -q -n VW-%{version}

%build
make make &> /dev/null
make headers &> /dev/null
cd VWCore
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
install -m 755 lib-i686/libVW.a $RPM_BUILD_ROOT/usr/lib/libVW.a
# Install the includes
#   first make the directory
install -d -m 755 include/VW $RPM_BUILD_ROOT/usr/include/VW
#   now the main headers
install -d -m 755 include/VW/Math $RPM_BUILD_ROOT/usr/include/VW/Math
install -m 755 include/VW/math.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/Math/*.h $RPM_BUILD_ROOT/usr/include/VW/Math/
install -m 755 include/VW/Math/*.tpp $RPM_BUILD_ROOT/usr/include/VW/Math/

install -d -m 755 include/VW/Utils/ $RPM_BUILD_ROOT/usr/include/VW/Utils
install -m 755 include/VW/utils.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/Utils/*.h $RPM_BUILD_ROOT/usr/include/VW/Utils/

install -d -m 755 include/VW/Image $RPM_BUILD_ROOT/usr/include/VW/Image
install -m 755 include/VW/image.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/Image/*.h $RPM_BUILD_ROOT/usr/include/VW/Image/

install -d -m 755 include/VW/GeomObjects $RPM_BUILD_ROOT/usr/include/VW/GeomObjects
install -m 755 include/VW/geomobjects.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/GeomObjects/*.h $RPM_BUILD_ROOT/usr/include/VW/GeomObjects/

install -d -m 755 include/VW/GeomCompute $RPM_BUILD_ROOT/usr/include/VW/GeomCompute
install -m 755 include/VW/geomcompute.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/GeomCompute/*.h $RPM_BUILD_ROOT/usr/include/VW/GeomCompute/
install -m 755 include/VW/GeomCompute/*.tpp $RPM_BUILD_ROOT/usr/include/VW/GeomCompute/

install -d -m 755 include/VW/IOBasics $RPM_BUILD_ROOT/usr/include/VW/IOBasics
install -m 755 include/VW/iobasics.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/IOBasics/*.h $RPM_BUILD_ROOT/usr/include/VW/IOBasics/

install -d -m 755 include/VW/Events $RPM_BUILD_ROOT/usr/include/VW/Events
install -m 755 include/VW/events.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/Events/*.h $RPM_BUILD_ROOT/usr/include/VW/Events/

install -d -m 755 include/VW/Sequencers $RPM_BUILD_ROOT/usr/include/VW/Sequencers
install -m 755 include/VW/sequencers.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/Sequencers/*.h $RPM_BUILD_ROOT/usr/include/VW/Sequencers/

install -d -m 755 include/VW/Improc $RPM_BUILD_ROOT/usr/include/VW/Improc
install -m 755 include/VW/improc.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/Improc/*.h $RPM_BUILD_ROOT/usr/include/VW/Improc/

install -d -m 755 include/VW/Graphics $RPM_BUILD_ROOT/usr/include/VW/Graphics
install -m 755 include/VW/graphics.h $RPM_BUILD_ROOT/usr/include/VW/
install -m 755 include/VW/Graphics/*.h $RPM_BUILD_ROOT/usr/include/VW/Graphics/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

# The library file
/usr/lib/libVW.a

# Other files are added automatically 
#   (headers into /usr/include/VW)
