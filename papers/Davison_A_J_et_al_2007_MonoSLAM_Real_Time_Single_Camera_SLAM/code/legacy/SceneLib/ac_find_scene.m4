# AC_FIND_SCENE
# 
# Version 1.0
# Paul Smith 2 March 2006
# 
# Find the location of the SCENE libraries
# 
# If it fails to find the Scene libraries, it will exit with a fatal 
# error. On success, it sets SCENE_CFLAGS to the necessary compile 
# flags (i.e. the include path), and SCENE_LIBS to the necessary 
# linker flags (i.e. the library path and the libraries to link with).
#
# You will need these flags in addition to $(VW_CFLAGS) and $(VW_LIBS) 
# These are set by a call to AC_FIND_VW, which you should do before 
# calling AC_FIND_SCENE. If you don't, it will call it for you with
# a minimal set of parameters.
# 
# Thanks to Ed Rosten (this is a modified version of his
# libCVD configure), and to the Mplayer configure file for the libXext 
# hack
# ----------------------------------------------
AC_DEFUN([AC_FIND_SCENE],
[

AC_REQUIRE([AC_FIND_VW])

AC_LANG_PUSH(C++)

#
#Allow the user to specify the location of Scenelib
#
AC_ARG_WITH(Scene, [  --with-Scene=directory  Specify location for Scene ], [opt_scene_dir=$withval])

if test "$opt_scene_dir" = no
then
  AC_MSG_ERROR("Scene is not optional. Use --with-Scene=dir to specify where it can be found.")
fi

#Yes is not a useful answer. We know the answer is yes.
if test "$opt_scene_dir" = yes
then
  opt_scene_dir=
fi

AC_MSG_CHECKING([for Scene location])

#Check for Scene in a bunch of places
save_CPPFLAGS="$CPPFLAGS"
save_LDFLAGS="$LDFLAGS"
scenedir="Missing"
for dir in  $opt_vw_dir "/usr/include/" "/usr/local/include" "$HOME/usr/local/include" "$HOME/usr/include" "" "$HOME/usr" "$HOME/Src/SceneLib" "$HOME/code/SceneLib" "../SceneLib" "../../SceneLib" 
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
    SCENE_CFLAGS="-I$abspath/include"
    CPPFLAGS="$save_CPPFLAGS -I$abspath/include $VW_CFLAGS"
  fi

  AC_COMPILE_IFELSE(
  [
    #include "MonoSLAM/model_creators.h"
    #include "MonoSLAM/monoslaminterface.h"

    int main()
    {
      MonoSLAM_Motion_Model_Creator mm_creator;
      MonoSLAM_Feature_Measurement_Model_Creator fmm_creator;
      MonoSLAMInterface* monoslaminterface = 
        new MonoSLAMInterface("", &mm_creator, &fmm_creator,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      return 0;
    }
  ], [a=1]) 
  
  CPPFLAGS="$save_CPPFLAGS"

  if test x$a = x1
  then
    scenedir=$abspath
    LDFLAGS="$LDFLAGS -L$scenedir/lib"
    break
  fi
done

AC_MSG_RESULT($scenedir)

if test "$scenedir" = Missing
then
  AC_MSG_ERROR(Scene is missing!)
fi

# Check for the libs just in case
SCENE_LIBS="-L$scenedir/lib"
AC_CHECK_LIB(Scene, main, [SCENE_LIBS="$SCENE_LIBS -lScene"], 
  [AC_MSG_ERROR([Can find a SceneLib directory, but not the required Scene library])])
AC_CHECK_LIB(SceneImproc, main, [SCENE_LIBS="$SCENE_LIBS -lSceneImproc"], 
  [AC_MSG_ERROR([Can find a SceneLib directory, but not the required SceneImproc library])])
AC_CHECK_LIB(MonoSLAM, main, [SCENE_LIBS="$SCENE_LIBS -lMonoSLAM"], 
  [AC_MSG_ERROR([Can find a SceneLib directory, but not the required MonoSLAM library])])

LDFLAGS="$save_LDFLAGS"

AC_SUBST(SCENE_CFLAGS)
AC_SUBST(SCENE_LIBS)

AC_LANG_POP

])

