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
# GL, you would use
#
#     AC_FIND_VW([VWGL VWFirewire])
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
AC_ARG_WITH(VW, [  --with-VW=directory  Specify location for VW ],
[opt_vw_dir=$withval])

if test "$opt_vw_dir" = no
then
  AC_MSG_ERROR("VW is not optional. Use --with-VW=dir to specify where it can be
found.")
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
vwdir="Missing"
for dir in  $opt_vw_dir "/usr/include/" "/usr/local/include" \
  "$HOME/usr/local/include" "$HOME/usr/include" "" "$HOME/usr" \
  "$HOME/Src/VW34" "$HOME/code/VW34" "../VW34" "../../VW34" "../VW35" "../../VW35"
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

  AC_COMPILE_IFELSE(
  [
    #include <VW/image.h>
    int main()
    {
      VW::ImageMono<unsigned char> image;
      return 0;
    }
  ], [a=1])
  
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
VW_LIBS="-L$dir/lib"
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


# Since VW doesn't (yet!) do shared libraries, we need to add in some other
# libraries. All of the libraries listed must have passed the test above, so we
# just need to see which ones are listed and add the appropriate additional
# libraries

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
    for dir in /usr/lib64 /usr/X11R6/lib64 /usr/X11R6/lib /usr/lib/X11R6 \
      /usr/X11/lib /usr/lib32 /usr/openwin/lib /usr/lib 
    do
      if test -d "$dir" && ( test -f "$dir/libXext.so" ||
         test -f "$dir/libXext.a" || test -f "$dir/libXext.dll.a" )
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




#####################################################
# Check whether to enable or disable debugging
# Sets CXXFLAGS accordingly
# TODO: This currently assumes g++ and doesn't check whether it is!
#####################################################
# Mofified from Neelakanth Nadgi (http://developers.sun.com/solaris/articles/gnu.html)
# and from the CVD configure script by Ed Rosten and Paul Smith (https://savannah.nongnu.org/projects/libcvd/)

AC_DEFUN([AC_DEBUG_OR_OPTIMISE_CXXFLAGS],
[

AC_LANG_PUSH(C++)

# If AC_CANONICAL_HOST has not been run, we need to run it
# earlier to find the host type
if test "$host" = ""
then
    AC_MSG_WARN([[I don't think autoconf's CANONICAL_HOST been run earlier in the configure script. It should have been.]])
fi


############
#enable-optimisation
#
AC_MSG_CHECKING(whether to enable optimisation)
optimisation_default="yes"
AC_ARG_ENABLE(optimisation, 
      [  --enable-optimisation=[no/yes/full]   Enable/disable optimisation default=yes],,
     enable_optimisation=$optimisation_default)

     
if test x"$enable_optimisation" = "xyes" -o x"$enable_optimisation" = "xfull"
then
    AC_MSG_RESULT($enable_optimisation)

    #What is the CPU type?
    case "$host" in
        *86*linux*)
            cpu="`$AWK '
    /Pentium III/						{print "p3";exit}
    /Intel\(R\) Pentium\(R\) 4/			{print "p4";exit}
    /Intel\(R\) Pentium\(R\) M/			{print "pM";exit}
    /Intel\(R\) Xeon\(TM\)/				{print "xeon";exit}
    /Intel\(R\) XEON\(TM\)/				{print "xeon";exit}
    /AMD Athlon\(tm\) 64 Processor/		{print "athlon64";exit}
    /AMD Athlon\(tm\)/					{print "athlon";exit}
    /AMD Opteron\(tm\)/					{print "opteron";exit}' /proc/cpuinfo`"
        ;;
    esac

    #Also check whether we have a 64-bit processor
    #(since some Pentium 4s have EM64T, and some don't)
    #At least AMD name their processors with x86_64 something different!
    cpu64bit="no"
    AC_MSG_CHECKING([for 64-bit extensions])
    if grep -q lm /proc/cpuinfo
    then
        AC_MSG_RESULT(yes)
        cpu64bit="yes"
    else
        AC_MSG_RESULT(no)
    fi
    
    #OK, now we can think about which -march flags are worth trying
    marchlist="i686 i586 i486 i386"
    case $cpu in
        p3)
        marchlist="pentium3 $marchlist" 
        ;;
        p4)
        marchlist="pentium4 $marchlist"
        if test "x$cpu64bit" = "xyes"
        then
            marchlist="nocona $marchlist"
        fi
        ;;
        xeon)
        marchlist="pentium4 $marchlist"
        ;;
        pM)
        marchlist="pentium-m pentium4 $marchlist"
        ;;
        opteron)
        marchlist="opteron k8 $marchlist"
        ;;
        athlon64)
        marchlist="k8 $marchlist"
        ;;
        athlon)
        marchlist="athlon $marchlist"
        ;;
    esac

    # Now we have a list of -march flags to try, we can try them out 
    save_CXXFLAGS="$CXXFLAGS"
    for i in $marchlist
    do
        i="-march=$i"
        AC_MSG_CHECKING([compiler supports $i]) 
        
        CXXFLAGS="$save_CXXFLAGS $i" 
        AC_COMPILE_IFELSE([int main(){}], [a=1], [a=0])

        if test $a = 1
        then
            AC_MSG_RESULT([yes])
            cpuopt=$i
            break
        else
            AC_MSG_RESULT([no])
        fi
    done
#     CXXFLAGS="$save_CXXFLAGS $cpuopt"
    CXXFLAGS="$cpuopt"	

    # Now set the optimisation on or not
    if test "x$enable_optimisation" = "xfull"
    then
        CXXFLAGS="$CXXFLAGS -O3"
    else
        CXXFLAGS="$CXXFLAGS -O2"
    fi    
else
    CXXFLAGS="$CXXFLAGS -O0"
    AC_MSG_RESULT(no)
fi
     

############
#enable-debugging
#
AC_MSG_CHECKING(whether to enable debugging)

#If optimisation is "full", we disable debugging by default, otherwise
#it is turned on
if test "x$enable_optimisation" = "xfull"
then
    debug_default="no"
else
    debug_default="yes"
fi

AC_ARG_ENABLE(debug, 
    [  --enable-debug=[no/yes]  Turn on debugging [default=yes]],, 
    enable_debug=$debug_default)

if test "x$enable_debug" = "xyes"
then
    CXXFLAGS="$CXXFLAGS -g -Wall -W -DDEBUG"
#    CXXFLAGS="$CXXFLAGS -ansi -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wconversion -Woverloaded-virtual -Weffc++ -Wold-style-cast"
    AC_MSG_RESULT(yes)
else
    CXXFLAGS="$CXXFLAGS"
    AC_MSG_RESULT(no)
fi



############
#enable-simd
#
AC_MSG_CHECKING(whether to enable simd instructions)
simd_default="yes"
AC_ARG_ENABLE(simd, 
    [  --enable-simd=[no/yes]   Enable/disable SIMD instructions [default=yes]],, 
    enable_simd=$simd_default)

if test x"$enable_simd" = "xyes"
then
    AC_MSG_RESULT(yes)
	case "$host" in
		*86*linux*)

			#Check for SIMD instructions
			#Currently, we're mostly interested in what AMD call mmxext
			#(which are part of SSE). They include pshufw, movntq and prefetchnta
			AC_MSG_CHECKING([for MMX, SSE])
			if grep -q sse /proc/cpuinfo
			then
				AC_MSG_RESULT(yes)
				CXXFLAGS="$CXXFLAGS -mmmx -msse"
            else
				AC_MSG_RESULT(no)
			fi
			
			AC_MSG_CHECKING([for MMX, MMXEXT])
			if grep -q mmxext /proc/cpuinfo
			then
				AC_MSG_RESULT(yes)
				CXXFLAGS="$CXXFLAGS -mmmx -m3dnow"
			else
				AC_MSG_RESULT(no)
			fi

			AC_MSG_CHECKING([for SSE2])
			if grep -q sse2 /proc/cpuinfo
			then
				AC_MSG_RESULT(yes)
				CXXFLAGS="$CXXFLAGS -msse2"
			else
				AC_MSG_RESULT(no)
			fi
		;;
	esac
else
    AC_MSG_RESULT(no)
fi

AC_LANG_POP(C++)

])
