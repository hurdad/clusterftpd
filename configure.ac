dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(clusterftpd, 0.1)

AC_LANG([C++])

AC_CHECK_LIB([pthread], [main], [],[
         echo "pthread library is required for this program"
         exit -1])
         
AC_CHECK_LIB([hiredis], [main], [],[
         echo "hiredis library is required for this program"
         exit -1])

AC_CHECK_LIB([z], [main], [],[
         echo "zlib library is required for this program"
         exit -1])

AC_CHECK_LIB([thrift], [main], [],[
         echo "thrift library is required for this program"
         exit -1])

AC_CHECK_LIB([config++], [main], [],[
         echo "libconfig library is required for this program"
         exit -1])
         
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_PROG_CXX

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

