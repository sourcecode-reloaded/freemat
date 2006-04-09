AC_DEFUN([MP_WITH_CURSES],
  [AC_ARG_WITH(ncurses, [  --with-ncurses          Force the use of ncurses over curses],,)
   mp_save_LIBS="$LIBS"
   CURSES_LIB=""
   if test "$with_ncurses" != yes
   then
     AC_CACHE_CHECK([for working curses], mp_cv_curses,
       [LIBS="$LIBS -lcurses"
        AC_TRY_LINK(
          [#include <curses.h>],
          [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
          mp_cv_curses=yes, mp_cv_curses=no)])
     if test "$mp_cv_curses" = yes
     then
       AC_DEFINE(HAVE_CURSES_H)
       CURSES_LIB="-lcurses"
     fi
   fi
   if test ! "$CURSES_LIB"
   then
     AC_CACHE_CHECK([for working ncurses], mp_cv_ncurses,
       [LIBS="$mp_save_LIBS -lncurses"
        AC_TRY_LINK(
          [#include <ncurses.h>],
          [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
          mp_cv_ncurses=yes, mp_cv_ncurses=no)])
     if test "$mp_cv_ncurses" = yes
     then
       AC_DEFINE(HAVE_NCURSES_H)
       CURSES_LIB="-lncurses"
     fi
   fi
   LIBS="$mp_save_LIBS"
])dnl

dnl for Qt (KSW was here)
AC_DEFUN([BNV_HAVE_QT],
[
  dnl THANKS! This code includes bug fixes by:
  dnl Tim McClarren.

  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

  AC_MSG_CHECKING(for Qt)

  QT_XLIBS="$X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
  if test x"$no_x" = xyes; then
     QT_XLIBS=""
  fi
  dnl KSW hackish... requires another macro to set is_osx
  if test x"$is_osx" = xyes; then
     QT_XLIBS=""
  fi

  AC_ARG_WITH([Qt-dir],
    [  --with-Qt-dir=DIR       DIR is equal to \$QTDIR if you have followed the
                          installation instructions of Trolltech. Header
                          files are in DIR/include, binary utilities are
                          in DIR/bin and the library is in DIR/lib])
  AC_ARG_WITH([Qt-include-dir],
    [  --with-Qt-include-dir=DIR
                          Qt header files are in DIR])
  AC_ARG_WITH([Qt-bin-dir],
    [  --with-Qt-bin-dir=DIR   Qt utilities such as moc and uic are in DIR])
  AC_ARG_WITH([Qt-lib-dir],
    [  --with-Qt-lib-dir=DIR   The Qt library is in DIR])
  AC_ARG_WITH([Qt-lib],
    [  --with-Qt-lib=LIB       Use -lLIB to link with the Qt library])
  bnv_is_qt4=no
  if test x"$with_Qt_dir" = x"no" ||
     test x"$with_Qt_include_dir" = x"no" ||
     test x"$with_Qt_bin_dir" = x"no" ||
     test x"$with_Qt_lib_dir" = x"no" ||
     test x"$with_Qt_lib" = x"no"; then
    # user disabled Qt. Leave cache alone.
    have_qt="User disabled Qt."
  else
    # "yes" is a bogus option
    if test x"$with_Qt_dir" = xyes; then
      with_Qt_dir=
    fi
    if test x"$with_Qt_include_dir" = xyes; then
      with_Qt_include_dir=
    fi
    if test x"$with_Qt_bin_dir" = xyes; then
      with_Qt_bin_dir=
    fi
    if test x"$with_Qt_lib_dir" = xyes; then
      with_Qt_lib_dir=
    fi
    if test x"$with_Qt_lib" = xyes; then
      with_Qt_lib=
    fi
    # No Qt unless we discover otherwise
    have_qt=no
    # Check whether we are requested to link with a specific version
    if test x"$with_Qt_lib" != x; then
      bnv_qt_lib="$with_Qt_lib"
    fi
    # Check whether we were supplied with an answer already
    if test x"$with_Qt_dir" != x; then
      have_qt=yes
      bnv_qt_dir="$with_Qt_dir"
      bnv_qt_include_dir="$with_Qt_dir/include"
      bnv_qt_bin_dir="$with_Qt_dir/bin"
      bnv_qt_lib_dir="$with_Qt_dir/lib"
      # Only search for the lib if the user did not define one already
      if test x"$bnv_qt_lib" = x; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* 2> /dev/null | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
        if test x"$bnv_qt_lib" = x; then
           bnv_qt_lib="`ls $bnv_qt_lib_dir/libQtCore.* 2> /dev/null | sed -n 1p |
                        sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
           if test x"$bnv_qt_lib" != x; then
              bnv_is_qt4=yes
           fi
        fi
      fi
      if test x"$bnv_is_qt4" = xyes; then
         bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib -lQtGui -lQtOpenGL -lQtNetwork -lQt3Support $QT_XLIBS "
      else
         bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $QT_XLIBS"
      fi
    else
      # Use cached value or do search, starting with suggestions from
      # the command line
      AC_CACHE_VAL(bnv_cv_have_qt,
      [
        # We are not given a solution and there is no cached value.
        bnv_qt_dir=NO
        bnv_qt_include_dir=NO
        bnv_qt_lib_dir=NO
        if test x"$bnv_qt_lib" = x; then
          bnv_qt_lib=NO
        fi
        BNV_PATH_QT_DIRECT
        if test "$bnv_qt_dir" = NO ||
           test "$bnv_qt_include_dir" = NO ||
           test "$bnv_qt_lib_dir" = NO ||
           test "$bnv_qt_lib" = NO; then
          # Problem with finding complete Qt.  Cache the known absence of Qt.
          bnv_cv_have_qt="have_qt=no"
        else
          # Record where we found Qt for the cache.
          bnv_cv_have_qt="have_qt=yes                  \
                       bnv_qt_dir=$bnv_qt_dir          \
               bnv_qt_include_dir=$bnv_qt_include_dir  \
                   bnv_qt_bin_dir=$bnv_qt_bin_dir      \
                   bnv_is_qt4=$bnv_is_qt4              \
                      bnv_qt_LIBS=\"$bnv_qt_LIBS\""
        fi
      ])dnl
      eval "$bnv_cv_have_qt"
    fi # all $bnv_qt_* are set
  fi   # $have_qt reflects the system status
  if test x"$have_qt" = xyes; then
    if test x"$bnv_is_qt4" = xyes; then
      QT_CXXFLAGS="-DQT3_SUPPORT -I$bnv_qt_include_dir -I$bnv_qt_include_dir/QtCore -I$bnv_qt_include_dir/QtGui -I$bnv_qt_include_dir/QtOpenGL -I$bnv_qt_include_dir/QtNetwork -I$bnv_qt_include_dir/Qt3Support "
    else
      QT_CXXFLAGS="-I$bnv_qt_include_dir"
    fi
    QT_DIR="$bnv_qt_dir"
    QT_LIBS="$bnv_qt_LIBS"
    # If bnv_qt_dir is defined, utilities are expected to be in the
    # bin subdirectory
    if test x"$bnv_qt_dir" != x; then
        if test -x "$bnv_qt_dir/bin/uic"; then
          QT_UIC="$bnv_qt_dir/bin/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
      QT_MOC="$bnv_qt_dir/bin/moc"
    else
      # Or maybe we are told where to look for the utilities
      if test x"$bnv_qt_bin_dir" != x; then
        if test -x "$bnv_qt_bin_dir/uic"; then
          QT_UIC="$bnv_qt_bin_dir/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
        QT_MOC="$bnv_qt_bin_dir/moc"
      else
      # Last possibility is that they are in $PATH
        QT_UIC="`which uic`"
        QT_MOC="`which moc`"
      fi
    fi
    # All variables are defined, report the result
    if test x"$bnv_is_qt4" = xyes; then
      QT_UIC="$QT_UIC"3
    fi
    AC_MSG_RESULT([$have_qt:
    QT_CXXFLAGS=$QT_CXXFLAGS
    QT_DIR=$QT_DIR
    QT_LIBS=$QT_LIBS
    QT_UIC=$QT_UIC
    QT_MOC=$QT_MOC])
  else
    # Qt was not found
    QT_CXXFLAGS=
    QT_DIR=
    QT_LIBS=
    QT_UIC=
    QT_MOC=
    AC_MSG_RESULT($have_qt)
  fi
  if test x"$bnv_is_qt4" = xyes; then
    HAVE_QT4=1
    AC_SUBST(HAVE_QT4)
    AC_DEFINE( HAVE_QT4 )
  fi
  AC_SUBST(QT_CXXFLAGS)
  AC_SUBST(QT_DIR)
  AC_SUBST(QT_LIBS)
  AC_SUBST(QT_UIC)
  AC_SUBST(QT_MOC)


  #### Being paranoid:
  if test x"$have_qt" = xyes; then
    AC_MSG_CHECKING(correct functioning of Qt installation)
    AC_CACHE_VAL(bnv_cv_qt_test_result,
    [
      cat > bnv_qt_test.h << EOF
#include <qobject.h>
class Test : public QObject
{
Q_OBJECT
public:
  Test() {}
  ~Test() {}
public slots:
  void receive() {}
signals:
  void send();
};
EOF

      cat > bnv_qt_main.$ac_ext << EOF
#include "bnv_qt_test.h"
#include <qapplication.h>
int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  Test t;
  QObject::connect( &t, SIGNAL(send()), &t, SLOT(receive()) );
}
EOF

      bnv_cv_qt_test_result="failure"
      bnv_try_1="$QT_MOC bnv_qt_test.h -o moc_bnv_qt_test.$ac_ext >/dev/null 2>bnv_qt_test_1.out"
      AC_TRY_EVAL(bnv_try_1)
      bnv_err_1=`grep -v '^ *+' bnv_qt_test_1.out | grep -v "^bnv_qt_test.h\$"`
      if test x"$bnv_err_1" != x; then
        echo "$bnv_err_1" >&AC_FD_CC
        echo "configure: could not run $QT_MOC on:" >&AC_FD_CC
        cat bnv_qt_test.h >&AC_FD_CC
      else
        bnv_try_2="$CXX $QT_CXXFLAGS -c $CXXFLAGS -Wno-non-virtual-dtor -o moc_bnv_qt_test.o moc_bnv_qt_test.$ac_ext >/dev/null 2>bnv_qt_test_2.out"
        AC_TRY_EVAL(bnv_try_2)
        bnv_err_2=`grep -v '^ *+' bnv_qt_test_2.out | grep -v "^bnv_qt_test.{$ac_ext}\$"`
        if test x"$bnv_err_2" != x; then
          echo "$bnv_err_2" >&AC_FD_CC
          echo "configure: could not compile:" >&AC_FD_CC
          cat bnv_qt_test.$ac_ext >&AC_FD_CC
        else
          bnv_try_3="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o bnv_qt_main.o bnv_qt_main.$ac_ext >/dev/null 2>bnv_qt_test_3.out"
          AC_TRY_EVAL(bnv_try_3)
          bnv_err_3=`grep -v '^ *+' bnv_qt_test_3.out | grep -v "^bnv_qt_main.{$ac_ext}\$"`
          if test x"$bnv_err_3" != x; then
            echo "$bnv_err_3" >&AC_FD_CC
            echo "configure: could not compile:" >&AC_FD_CC
            cat bnv_qt_main.$ac_ext >&AC_FD_CC
          else
            bnv_try_4="$CXX $QT_LIBS $LIBS -o bnv_qt_main bnv_qt_main.o moc_bnv_qt_test.o >/dev/null 2>bnv_qt_test_4.out"
            AC_TRY_EVAL(bnv_try_4)
            bnv_err_4=`grep -v '^ *+' bnv_qt_test_4.out`
            if test x"$bnv_err_4" != x; then
              echo "$bnv_err_4" >&AC_FD_CC
            else
              bnv_cv_qt_test_result="success"
            fi
          fi
        fi
      fi
    ])dnl AC_CACHE_VAL bnv_cv_qt_test_result
    AC_MSG_RESULT([$bnv_cv_qt_test_result]);
    if test x"$bnv_cv_qt_test_result" = "xfailure"; then
      AC_MSG_ERROR([Failed to find matching components of a complete
                  Qt installation. Try using more options,
                  see ./configure --help.])
    fi

    rm -f bnv_qt_test.h moc_bnv_qt_test.$ac_ext moc_bnv_qt_test.o \
          bnv_qt_main.$ac_ext bnv_qt_main.o bnv_qt_main \
          bnv_qt_test_1.out bnv_qt_test_2.out bnv_qt_test_3.out bnv_qt_test_4.out
  fi

    AC_LANG_RESTORE
])

dnl Internal subroutine of BNV_HAVE_QT
dnl Set bnv_qt_dir bnv_qt_include_dir bnv_qt_bin_dir bnv_qt_lib_dir bnv_qt_lib
dnl Copyright 2001 Bastiaan N. Veelo <Bastiaan.N.Veelo@immtek.ntnu.no>
AC_DEFUN(BNV_PATH_QT_DIRECT,
[
  ## Binary utilities ##
  if test x"$with_Qt_bin_dir" != x; then
    bnv_qt_bin_dir=$with_Qt_bin_dir
  fi
  ## Look for header files ##
  if test x"$with_Qt_include_dir" != x; then
    bnv_qt_include_dir="$with_Qt_include_dir"
    dnl FIXME this does not work for UIC or all Qt4 includes
    if test -f $bnv_qt_include_dir/Qt/$qt_direct_test_header ; then
      bnv_is_qt4=yes
    fi
  else
    # The following header file is expected to define QT_VERSION.
    qt_direct_test_header=qglobal.h
    # Look for the header file in a standard set of common directories.
    bnv_include_path_list="
      /usr/include
      `ls -dr /usr/include/qt* 2>/dev/null`
      `ls -dr /usr/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/local/qt*/include 2>/dev/null`
      `ls -dr /opt/sfw/include 2>/dev/null`
      `ls -dr /opt/qt*/include 2>/dev/null`
      `ls -dr /usr/qt*/include 2>/dev/null`
      `ls -dr /usr/qt/*/include 2>/dev/null`
    "
    for bnv_dir in $bnv_include_path_list; do
      if test -r "$bnv_dir/$qt_direct_test_header"; then
        bnv_dirs="$bnv_dirs $bnv_dir"
      fi
dnl FIXME here's the Qt4 detection
dnl      if test -r "$bnv_dir/Qt/$qt_direct_test_header"; then
dnl        bnv_dirs="$bnv_dirs $bnv_dir"
dnl      fi
    done
    # Now look for the newest in this list
    bnv_prev_ver=0
    for bnv_dir in $bnv_dirs; do
      if test -r "$bnv_dir/$qt_direct_test_header"; then
         bnv_this_ver=`egrep -w '#define QT_VERSION' $bnv_dir/$qt_direct_test_header | sed s/'#define QT_VERSION'//`
         if expr $bnv_this_ver '>' $bnv_prev_ver > /dev/null; then
           bnv_qt_include_dir=$bnv_dir
           bnv_prev_ver=$bnv_this_ver
         fi
      else
         bnv_this_ver=`egrep -w '#define QT_VERSION' $bnv_dir/Qt/$qt_direct_test_header | sed s/'#define QT_VERSION'//`
         if expr $bnv_this_ver '>' $bnv_prev_ver > /dev/null; then
           bnv_is_qt4=yes
           bnv_qt_include_dir=$bnv_dir
           bnv_prev_ver=$bnv_this_ver
         fi
      fi
    done
  fi dnl Found header files.

  # Are these headers located in a traditional Trolltech installation?
  # That would be $bnv_qt_include_dir stripped from its last element:
  bnv_found_traditional=no
  bnv_possible_qt_dir=`dirname $bnv_qt_include_dir`
  if test -x $bnv_possible_qt_dir/bin/moc &&
     ls $bnv_possible_qt_dir/lib/libqt* 1> /dev/null 2> /dev/null; then
    # Then the rest is a piece of cake
    bnv_qt_dir=$bnv_possible_qt_dir
    bnv_qt_bin_dir="$bnv_qt_dir/bin"
    bnv_qt_lib_dir="$bnv_qt_dir/lib"
    # Only look for lib if the user did not supply it already
    if test x"$bnv_qt_lib" = xNO; then
      bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* 2> /dev/null | sed -n 1p |
                   sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
    fi
    bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $QT_XLIBS"
    bnv_found_traditional=yes
  fi
  if test -x $bnv_possible_qt_dir/bin/moc &&
     ls $bnv_possible_qt_dir/lib/libQtCore.* 1> /dev/null 2> /dev/null; then
    # Then the rest is a piece of cake
    bnv_qt_dir=$bnv_possible_qt_dir
    bnv_qt_bin_dir="$bnv_qt_dir/bin"
    bnv_qt_lib_dir="$bnv_qt_dir/lib"
    # Only look for lib if the user did not supply it already
    if test x"$bnv_qt_lib" = xNO; then
      bnv_qt_lib="`ls $bnv_qt_lib_dir/libQtCore.* 2> /dev/null | sed -n 1p |
                   sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
    fi
    bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib -lQtGui -lQtOpenGL -lQtNetwork -lQt3Support $QT_XLIBS"
    bnv_found_traditional=yes
  fi
  if test $bnv_found_traditional = no; then
    # There is no valid definition for $QTDIR as Trolltech likes to see it
    bnv_qt_dir=
    ## Look for Qt library ##
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
      # Only look for lib if the user did not supply it already
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* $bnv_qt_lib_dir/libQtCore.* 2> /dev/null | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      if test x"$bnv_is_qt4" = xyes; then
         bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib -lQtGui -lQtOpenGL -lQtNetwork -lQt3Support $QT_XLIBS"
      else
         bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $QT_XLIBS"
      fi
    else
      # Normally, when there is no traditional Trolltech installation,
      # the library is installed in a place where the linker finds it
      # automatically.
      # If the user did not define the library name, try with qt
      if test x"$bnv_qt_lib" = xNO; then
        if test x"$bnv_is_qt4" = xyes; then
           bnv_qt_lib=QtCore
        else
           bnv_qt_lib=qt
        fi
      fi
      qt_direct_test_header=qapplication.h
      qt_direct_test_main="
        int argc;
        char ** argv;
        QApplication app(argc,argv);
      "
      # See if we find the library without any special options.
      # Don't add top $LIBS permanently yet
      bnv_save_LIBS="$LIBS"
      bnv_save_CXXFLAGS="$CXXFLAGS"
      if test x"$bnv_is_qt4" = xyes; then
        CXXFLAGS="-I$bnv_qt_include_dir -I$bnv_qt_include_dir/QtCore -I$bnv_qt_include_dir/QtGui -I$bnv_qt_include_dir/QtOpenGL -I$bnv_qt_include_dir/QtNetwork -I$bnv_qt_include_dir/Qt3Support "
        LIBS="-l$bnv_qt_lib -lQtGui -lQtOpenGL -lQtNetwork -lQt3Support $QT_XLIBS"
      else
        CXXFLAGS="-I$bnv_qt_include_dir"
        LIBS="-l$bnv_qt_lib $QT_XLIBS"
      fi
      bnv_qt_LIBS="$LIBS"
      AC_TRY_LINK([#include <$qt_direct_test_header>],
        $qt_direct_test_main,
      [
        # Succes.
        # We can link with no special library directory.
        bnv_qt_lib_dir=
      ], [
        # That did not work. Try the multi-threaded version
        echo "Non-critical error, please neglect the above." >&AC_FD_CC
        bnv_qt_lib=qt-mt
        LIBS="-l$bnv_qt_lib $QT_XLIBS"
        AC_TRY_LINK([#include <$qt_direct_test_header>],
          $qt_direct_test_main,
        [
          # Succes.
          # We can link with no special library directory.
          bnv_qt_lib_dir=
        ], [
          # That did not work. Try the OpenGL version
          echo "Non-critical error, please neglect the above." >&AC_FD_CC
          bnv_qt_lib=qt-gl
          LIBS="-l$bnv_qt_lib $QT_XLIBS"
          AC_TRY_LINK([#include <$qt_direct_test_header>],
            $qt_direct_test_main,
          [
            # Succes.
            # We can link with no special library directory.
            bnv_qt_lib_dir=
          ], [
            # That did not work. Maybe a library version I don't know about?
            echo "Non-critical error, please neglect the above." >&AC_FD_CC
            # Look for some Qt lib in a standard set of common directories.
            bnv_dir_list="
              `echo $bnv_qt_includes | sed ss/includess`
              /lib
              /usr/lib
              /usr/local/lib
              /opt/lib
              `ls -dr /usr/lib/qt* 2>/dev/null`
              `ls -dr /usr/local/qt* 2>/dev/null`
              `ls -dr /opt/qt* 2>/dev/null`
            "
            for bnv_dir in $bnv_dir_list; do
              if ls $bnv_dir/libqt* > /dev/null 2> /dev/null ; then
                # Gamble that it's the first one...
                bnv_qt_lib="`ls $bnv_dir/libqt* 2> /dev/null | sed -n 1p |
                             sed s@$bnv_dir/lib@@ | [sed s@[.].*@@]`"
                bnv_qt_lib_dir="$bnv_dir"
                break
              fi
            done
            # Try with that one
            LIBS="-l$bnv_qt_lib $QT_XLIBS"
          ])
        ])
      ])
      if test x"$bnv_qt_lib_dir" != x; then
        bnv_qt_LIBS="-L$bnv_qt_lib_dir $LIBS"
      else
        bnv_qt_LIBS="$LIBS"
      fi
      LIBS="$bnv_save_LIBS"
      CXXFLAGS="$bnv_save_CXXFLAGS"

    fi dnl $with_Qt_lib_dir was not given

  fi dnl Done setting up for non-traditional Trolltech installation

])

dnl for -lqglviewer (KSW was here)
AC_DEFUN([KSW_HAVE_QGL],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([BNV_HAVE_QT])

  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS

  AC_MSG_CHECKING(Qt OpenGL)

    AC_CACHE_VAL(ksw_cv_qgl_test_result,
    [
      cat > ksw_qgl_test.${ac_ext} << EOF
#include <qapplication.h>
#include <qwidget.h>
#include <qgl.h>

int main( int argc, char ** argv )
{
   QApplication app( argc, argv );
   QGLWidget w;
   w.show();
   return app.exec();
}
EOF

      ksw_cv_qgl_test_result="failure"
        ksw_try_1="$CXX $GL_CFLAGS $GL_LIBS $QT_CXXFLAGS $QT_LIBS -o ksw_qgl_test ksw_qgl_test.${ac_ext} >/dev/null 2>ksw_qgl_test_1.out"
        AC_TRY_EVAL(ksw_try_1)
        ksw_err_1=`grep -v '^ *+' ksw_qgl_test_1.out | grep -v "^ksw_qgl_test.{$ac_ext}\$"`
        if test x"$ksw_err_1" != x; then
           ksw_try_2="$CXX $GL_CFLAGS $GL_LIBS $QT_CXXFLAGS $QT_LIBS -lqglviewer -o ksw_qgl_test ksw_qgl_test.${ac_ext} >/dev/null 2>ksw_qgl_test_2.out"
           AC_TRY_EVAL(ksw_try_2)
           ksw_err_2=`grep -v '^ *+' ksw_qgl_test_2.out | grep -v "^ksw_qgl_test.{$ac_ext}\$"`
           if test x"$ksw_err_2" != x; then
             echo "$ksw_err_2" >&AC_FD_CC
             echo "configure: could not compile:" >&AC_FD_CC
             cat ksw_qgl_test.${ac_ext} >&AC_FD_CC
           else
             ksw_cv_qgl_test_result="success"
             QGL_LIBS=-lqglviewer
           fi
        else
         ksw_cv_qgl_test_result="success"
         QGL_LIBS=
        fi
    ])dnl AC_CACHE_VAL ksw_cv_qgl_test_result
    AC_MSG_RESULT([$ksw_cv_qgl_test_result]);
    if test x"$ksw_cv_qgl_test_result" = "xfailure"; then
      AC_MSG_ERROR([Failed to link Qt with OpenGL support.])
    fi

    rm -f ksw_qgl_test.h \
          ksw_qgl_test.${ac_ext} ksw_qgl_test.o ksw_qgl_test \
          ksw_qgl_test_1.out ksw_qgl_test_2.out

    AC_SUBST(QGL_LIBS)

    AC_LANG_RESTORE
])

AC_DEFUN([KSW_IS_OSX],
[
  AC_REQUIRE([AC_PROG_CC])

  AC_CACHE_CHECK([for OS X], ksw_cv_is_osx,
  [
    if test -e /System/Library/Frameworks/Carbon.framework; then
       is_osx=yes
    else
       is_osx=no
    fi

   rm -f ksw_is_osx*

    ksw_cv_is_osx="$is_osx"
  ])
  is_osx="$ksw_cv_is_osx"
  if test x"$is_osx" = "xyes"; then
    AC_DEFINE( IS_OSX )
  fi
])
