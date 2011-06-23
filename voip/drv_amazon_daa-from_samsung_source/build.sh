#!/bin/sh
TOPDIR=`pwd`/../
. ${TOPDIR}/build/tools/Path.sh
APPS_NAME="vinetic"

echo "----------------------------------------------------------------------"
echo "---------------------------     build amazon_daa ---------------------"
echo "----------------------------------------------------------------------"

parse_args $@

if [ $BUILD_CLEAN -eq 1 ]; then
    make distclean
	[ ! $BUILD_CONFIGURE -eq 1 ] && exit 0
fi

if [ "$1" = "config_only" -a ! -f .config_ok -o $BUILD_CONFIGURE -eq 1 ]; then
    ./configure --build=i686-pc-linux --host=mips-linux --enable-kernelincl="${KERNEL_SOURCE_DIR}/include" --enable-duslicincl=`pwd`/../drv_vinetic/src 
fi

if [ "$1" = "config_only" ]; then
	exit 0
fi


make
ifx_error_check $?


#install -d ${BUILD_ROOTFS_DIR}lib/
#cp -f src/amazon_daa ${BUILD_ROOTFS_DIR}lib/.
#ifx_error_check $?

