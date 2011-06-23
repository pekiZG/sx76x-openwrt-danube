#!/bin/sh
TOPDIR=`pwd`/../../../../
. ${TOPDIR}tools/build_tools/Path.sh
APPS_NAME="vinetic"

echo "----------------------------------------------------------------------"
echo "---------------------------     build drv_vinetic --------------------"
echo "----------------------------------------------------------------------"

parse_args $@

if [ $BUILD_CLEAN -eq 1 ]; then
#	make distclean
#	rm -f config.cache
#	rm -f aclocal.m4
#	rm -f config.h.in
#	rm -f configure
#	find . -name Makefile.in | xargs rm -rf
#	rm -rf autom4te.cache
#	[ ! $BUILD_CONFIGURE -eq 1 ] && exit 0
	echo "------------------------------------"
	echo "---- build clean not supported  ----"
	echo "---- because it requires newer  ----"
	echo "---- automake/autoconf versions ----"
	echo "------------------------------------"
fi

if [ "$1" = "config_only" -a ! -f .config_ok -o $BUILD_CONFIGURE -eq 1 ]; then
#        aclocal
#        autoheader
#        autoconf
#        automake --foreign --add-missing
#	./autogen.sh --build=i686-pc-linux --host=mips-linux --enable-warnings --enable-kernelincl=${KERNEL_SOURCE_DIR}include --enable-boardname=AMAZON --enable-incl-bin --enable-v1 --enable-voice --enable-spi --enable-trace 
#	ifx_error_check $?
#	echo -n > .config_ok
	echo "------------------------------------"
	echo "---- build config not supported ----"
	echo "---- because it requires newer  ----"
	echo "---- automake/autoconf versions ----"
	echo "------------------------------------"
fi

if [ "$1" = "config_only" ]; then
	exit 0
fi


make
ifx_error_check $?

install -d ${BUILD_ROOTFS_DIR}lib/
cp -f src/drv_vinetic ${BUILD_ROOTFS_DIR}lib/.
ifx_error_check $?

