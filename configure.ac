# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ds4linux], [0.1], [schultetwin1@gmail.com])

PKG_PROG_PKG_CONFIG

AM_INIT_AUTOMAKE([subdir-objects])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([LIBUSB], [libusb-1.0], [have_libusb=yes], [have_libusb=no])
AM_CONDITIONAL([LIBUSB], [test "$have_libusb" = "yes"])

PKG_CHECK_MODULES([BLUEZ], [bluez], [have_bluez=yes], [have_bluez=no])
AM_CONDITIONAL([BLUEZ], [test "$have_bluez" = "yes"])

AC_CHECK_LIB([bluetooth], [hci_open_dev])

PKG_CHECK_MODULES([MRAA], [mraa], [have_mraa=yes], [have_mraa=no])
AM_CONDITIONAL([MRAA], [test "$have_mraa" = "yes"])

# Checks for header files.
AC_HEADER_ASSERT
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h sys/socket.h unistd.h])
AC_CHECK_HEADERS([assert.h stdio.h  stdlib.h])
AC_CHECK_HEADERS([bluetooth/bluetooth.h libusb-1.0/libusb.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
