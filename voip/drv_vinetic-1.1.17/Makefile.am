## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign 1.7 nostdinc

SUBDIRS = 2cpe vip
#SUBDIRS = 2cpe

# define configure options for distcheck
#DISTCHECK_CONFIGURE_FLAGS=@CONFIGURE_OPTIONS@
DISTCHECK_CONFIGURE_FLAGS= --build=i686-pc-linux \
	--host=powerpc-linux-uclibc --enable-warnings \
	--enable-kernelincl=${HOME}/linux/include \
	--prefix=/exports/${USER}_ppc/opt/ifx \
	--enable-trace --enable-2cpe \
	--with-access-mode=INTEL_DEMUX

drv_vinetic_docdir = ${prefix}/doc/drv_vinetic

drv_vinetic_doc_DATA = \
	README\
	AUTHORS\
	NEWS\
	TODO \
   FEATURES

EXTRA_DIST = $(drv_vinetic_doc_DATA) \
	doc/doxyconfig\
	doc/footer.html\
	doc/header.html\
	doc/logo.gif\
	doc/DataChSwitching.emf\
	doc/stylesheet.css\
	doc/html/logo.gif

# Copy all the spec files. Of cource, only one is actually used.
#dist-hook:
#	rm -rf $(distdir)/src/evaluation;
#	for specfile in *.spec; do \
#		if test -f $$specfile; then \
#			cp -p $$specfile $(distdir); \
#		fi \
#	done

