# AC_FIND_VW([lib1 lib2 ...])
# 
# Version 1.0
# Paul Smith 2 March 2006
# 
# Find the location of the VW libraries, checks for the existence
# of the appropriate component libraries. This macro always checks 
# for the VW and VNL libraries, and also for any of the component 
# libraries listed in the (space-separated) list in the parameter.
# For example, for an application that uses VW with a firewire camera,
# GL and Qt, you would use
#
#     AC_FIND_VW([VWGL VWFirewire VWQt])
# 
# If it fails to find VW or any of the required libraries, it will 
# exit with a fatal error. On success, it sets VW_CFLAGS to the
# necessary compile flags (i.e. the include path), and VW_LIBS to
# the necessary linker flags (i.e. the library path and the libraries
# to link with).
# 
# Thanks to Ed Rosten (this is a modified version of his
# libCVD configure), and to the Mplayer configure file for the libXext 
# hack
# ----------------------------------------------
AC_DEFUN([AC_FIND_VW],
[

AC_LANG_PUSH(C++)

#
#Allow the user to specify the location of VW
#
AC_ARG_WITH(VW, [  --with-VW=directory  Specify location for VW ], [opt_vw_dir=$withval])

if test "$opt_vw_dir" = no
then
  AC_MSG_ERROR("VW is not optional. Use --with-VW=dir to specify where it can be found.")
fi

#Yes is not a useful answer. We know the answer is yes.
if test "$opt_vw_dir" = yes
then
  opt_vw_dir=
fi

#Check for the basic VW functionality
AC_MSG_CHECKING([for VW location])

#Check for VW in a bunch of places
save_CPPFLAGS="$CPPFLAGS"
save_LDFLAGS="$LDFLAGS"
vwdir="Missing"
for dir in  $opt_vw_dir "/usr/include/" "/usr/local/include" "$HOME/usr/local/include" "$HOME/usr/include" "" "$HOME/usr" "$HOME/Src/VW34" "$HOME/code/VW34" "../VW34" "../../VW34" 
do
  a=
  if test "$dir" = ""
  then
    CPPFLAGS="$save_CPPFLAGS"
  else
    # dir might be a relative path, so convert it to absolute
    # for safety (if, for example, our project has subdirectories)
    D=`dirname "$dir"`
    B=`basename "$dir"`
    abspath="`cd \"$D\" 2>/dev/null && pwd || echo \"$D\"`/$B"
    VW_CFLAGS="-I$abspath/include"
    CPPFLAGS="$save_CPPFLAGS -I$abspath/include"
  fi

  AC_COMPILE_IFELSE([#include <VW/image.h> int main(){VW::ImageMono<unsigned char> image;return 0;}], [a=1]) 
  
  CPPFLAGS="$save_CPPFLAGS"

  if test x$a = x1
  then
    vwdir=$abspath
    LDFLAGS="$LDFLAGS -L$abspath/lib"
    break
  fi
done

AC_MSG_RESULT($vwdir)

if test "$vwdir" = Missing
then
  AC_MSG_ERROR(VW is missing!)
fi

# We shall add the path and VW and VNL to the list since they're always needed.
# We'll check for the libs just in case
VW_LIBS="-L$vwdir/lib"
AC_CHECK_LIB(VW, main, [VW_LIBS="$VW_LIBS -lVW"], 
  [AC_MSG_ERROR([Can find VW, but not the required VW library])])
AC_CHECK_LIB(VNL, main, [VW_LIBS="$VW_LIBS -lVNL"], 
  [AC_MSG_ERROR([Can find VW, but not the required VNL library])])


# Now see if any sub-libraries have been listed

for sub_library in $1; do
  # Already tried VW and VNL, do don't do those again
  if test x$sub_library != xVW && test x$sub_library != xVNL
  then
    AC_CHECK_LIB($sub_library, main, 
      [
        # Add it at the beginning so that VNL and VW are linked first
        VW_LIBS="-l$sub_library $VW_LIBS"
      ],
      [AC_MSG_ERROR([Can find VW, but not the required $sub_library library])])
  fi
done

LDFLAGS="$save_LDFLAGS"

#
# Since VW doesn't (yet!) do shared libraries, we need to add in some other libraries
# All of the libraries listed must have passed the test above, so we just need to see
# which ones are listed and add the appropriate additional libraries
for sub_library in $1; do
  if test "$sub_library" = "VW"
  then
    VW_LIBS="$VW_LIBS -lpthread"
  fi

  if test "$sub_library" = "VWFirewire"
  then
    VW_LIBS="$VW_LIBS -lraw1394 -ldc1394_control"
  fi

  if test "$sub_library" = "VWGL"
  then
    VW_LIBS="$VW_LIBS -lglut -lGLU -lGL"
    # Hack to set the right path to X11R6 for lib64 machines
    # (this wouldn't be needed if VW had shared libraries!
    xext_dir="Missing"
    AC_MSG_CHECKING([for libXext location])
    for dir in /usr/X11R6/lib64 /usr/X11R6/lib /usr/lib/X11R6 /usr/X11/lib /usr/lib32 /usr/openwin/lib /usr/lib 
    do
      if test -d "$dir" && ( test -f "$dir/libXext.so" || test -f "$dir/libXext.a" || test -f "$dir/libXext.dll.a" )
      then
          VW_LIBS="-L$dir $VW_LIBS"
          xext_dir=$dir
          break
      fi
    done

    AC_MSG_RESULT($xext_dir)
    
    if test "$xext_dir" = Missing
    then
      AC_MSG_ERROR(Can't find libXext (needed for libGL))
    fi

  fi
done

########################

AC_SUBST(VW_CFLAGS)
AC_SUBST(VW_LIBS)

AC_LANG_POP

])

