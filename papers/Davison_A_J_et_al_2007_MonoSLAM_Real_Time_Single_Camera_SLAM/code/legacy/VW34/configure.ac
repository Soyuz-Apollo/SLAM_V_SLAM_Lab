#                    -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(VW, 3.4)
AC_CONFIG_SRCDIR([VW/Image/imagebase.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])

AC_PREFIX_DEFAULT(`pwd`)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h float.h malloc.h stddef.h stdlib.h string.h sys/file.h sys/ioctl.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([dup2 floor gettimeofday memset munmap pow sqrt strerror tzset])

# Checks for system components

# X
AC_CHECK_HEADERS(X11/Xlib.h X11/Xutil.h,
	        [libx=yes],
                [libx=no; AC_MSG_WARN(**** x libraries not found **** libVWX.a will not be compiled)])
AM_CONDITIONAL(X, test x$libx = xyes)

# XFORMS
AC_CHECK_HEADERS(X11/forms.h,
	        [xforms=yes],
                [xforms=no; AC_MSG_WARN(**** xforms not found **** libVWXForms.a will not be compiled)])
AM_CONDITIONAL(XFORMS, test x$xforms = xyes)

# Firewire
AC_CHECK_LIB(dc1394_control, dc1394_dma_setup_capture,
            [firewire=yes],
	    [firewire=no; AC_MSG_WARN(**** libdc1394 not found **** libVWFirewire.a will not be compiled)], 
	    [-lraw1394])
AM_CONDITIONAL(FIREWIRE, test x$firewire = xyes)

# GL
AC_CHECK_HEADERS(GL/gl.h GL/glu.h,
	         [gl=yes],
                 [gl=no; AC_MSG_WARN(GL not found)])
AM_CONDITIONAL(GL, test x$gl = xyes)

# GTK
#AM_PATH_GTKMM(1.2.10,
#              [gtkmm=yes], 
#	      [gtkmm=no; AC_MSG_WARN([Cannot find correct gtkmm version])])
AM_CONDITIONAL(GTKMM, test x$gtkmm = xyes)
AC_CHECK_FILES(/usr/include/gtkgl--/glarea.h,
	         [gtkglmm=yes],
                 [gtkglmm=no; AC_MSG_WARN(**** gtkgl-- not found **** libVWGTK.a will not have GL support)])
AM_CONDITIONAL(GTKGLMM, test x$gtkglmm = xyes)

# WX
#AM_OPTIONS_WXCONFIG
#AM_PATH_WXCONFIG(2.4.0, [wxwin=yes], [wxwin=no])
AM_CONDITIONAL(WX, test x$wxwin = xyes)

# BJT: Can't get this to work, so force true for now
#AC_CHECK_FILES(wx/glcanvas.h,
#	         [wxgl=yes],
#                 [wxgl=no; AC_MSG_WARN(**** wx/glcanvas.h not found **** libVWWx.a will not have GL support)])
AM_CONDITIONAL(WXGL, test x$wxgl = xyes -a x$wxwin = xyes )
#wxgl=yes
#AM_CONDITIONAL(WXGL, test x$wxwin = xyes)

# GLOW
AC_FIND_GLOW([glow=yes],
             [glow=no; AC_MSG_WARN(GLOW not found)])
AM_CONDITIONAL(GLOW, test x$glow = xyes)

# Qt
BNV_HAVE_QT
AM_CONDITIONAL(QT, test x$have_qt = xyes)

AC_CONFIG_FILES(Makefile
		VNL/Makefile 
		VW/Makefile
		VWFirewire/Makefile
		VWMeteor/Makefile
		VWYorick/Makefile
		VWGL/Makefile
		VWGTK/Makefile
    VWWx/Makefile
    VWGLOW/Makefile
    VWQt/Makefile
		VWX/Makefile
		VWXForms/Makefile)

AC_OUTPUT

echo Components to be compiled:
echo -e "\tCore:"
echo -e "\t  libVNL.a...       \t yes"
echo -e "\t  libVW.a...        \t yes"
echo -e "\tGUI:"
echo -e "\t  libVWX.a...       \t $libx"
echo -e "\t  libVWXForms.a...  \t $xforms"
echo -e "\t  libVWGL.a...      \t $gl"
echo -e "\t  libVWGLOW.a...    \t $glow"
echo -e "\t  libVWGTK.a...     \t $gtkmm (gl support - $gtkglmm)"
echo -e "\t  libVWWx.a...      \t $wxwin (gl support - $wxgl)"
echo -e "\t  libVWQt.a...      \t $have_qt"
echo -e "\tHardware:"
echo -e "\t  libVWMeteor.a...  \t yes"
echo -e "\t  libWVYorick.a...  \t yes"
echo -e "\t  libVWGTI.a...     \t no"
echo -e "\t  libVWFirewire.a...\t $firewire"
