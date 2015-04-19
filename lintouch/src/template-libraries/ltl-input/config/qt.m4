dnl $Id: qt.m4 1243 2006-01-10 09:44:00Z modesto $

dnl
dnl Try to find given DIRECTORY in a set of DIRS. The first one from DIRS
dnl that contains subdirectory DIRECTORY will be set to VARIABLE
dnl
dnl LNT_FIND_DIRECTORY(DIRECTORY,VARIABLE,[DIRS])
dnl
AC_DEFUN([LNT_FIND_DIRECTORY],
[
$2=""
for dir in $3 ; do
        if test -x $dir/$1 && test -d $dir/$1 ; then
                $2="$dir"
                break
        fi
done
])

dnl
dnl Try to find Qt in predefined set of directories and set QTDIR accordingly
dnl additionally --with-qt-dir argument can be used to specify where to look
dnl for Qt
dnl
dnl LNT_LIB_QT
dnl
AC_DEFUN([LNT_LIB_QT],
[

AC_ARG_WITH(qt-dir,[    --with-qt-dir=DIR            Qt in DIR])

dnl
dnl try to guess QTDIR
dnl
LNT_FIND_DIRECTORY(mkspecs,QQQ,[$with_qt_dir $QTDIR /usr/share/qt3 /usr/share/qte3])
AC_MSG_CHECKING([for Qt Library])
if test "X${QQQ}" = "X"
then
        AC_MSG_RESULT([not found])
        AC_MSG_WARN([])
        AC_MSG_WARN([Qt Library not found])
        AC_MSG_WARN([Please install Qt from http://trolltech.com/qt])
        AC_MSG_WARN([If you have installed Qt in an unusual place,])
        AC_MSG_WARN([use the \"--with-qt-dir=\" option])
        AC_MSG_WARN([])
        AC_MSG_WARN([The Qt dependent parts will not be built])
        AC_MSG_WARN([])
else
        QTDIR=${QQQ}
        AC_SUBST(QTDIR)
        AC_MSG_RESULT([found in $QTDIR])
fi

])

AC_DEFUN([CHECK_QT_MISC],[

AC_ARG_ENABLE(debug,       [  --enable-debug            enable debugging])
AC_ARG_ENABLE(profile,     [  --enable-profile          enable profiling])


dnl
dnl debug option
dnl
QT_MISC_DEBUG="release"

if test "$enable_debug" = "yes"
then
        echo ""
        echo "Debugging support enabled"
        echo ""
        QT_MISC_DEBUG="debug"
fi
AC_SUBST(QT_MISC_DEBUG)

dnl
dnl profile option
dnl
if test "$enable_profile" = "yes"
then
        echo ""
        echo "Profiling support enabled"
        echo ""
        QT_MISC_PROFILE="profile"
else
        QT_MISC_PROFILE=""
fi
AC_SUBST(QT_MISC_PROFILE)

])

dnl vim: set et ts=8 sw=8 tw=0:
dnl $Id: qt.m4 1243 2006-01-10 09:44:00Z modesto $
