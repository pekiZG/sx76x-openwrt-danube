## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign 1.6 dist-zip nostdinc

SUBDIRS = src

# use the configure options
DISTCHECK_CONFIGURE_FLAGS=@CONFIGURE_OPTIONS@

drv_vinboard_docdir = ${prefix}/doc/drv_vinboard

drv_vinboard_doc_DATA = \
	README\
	AUTHORS\
	NEWS\
	TODO

EXTRA_DIST = $(drv_vinboard_doc_DATA) \
	autogen.sh \
   scripts/genDrv.sh\
   drv_vinboard.wpj\
	doc/doxyconfig\
	doc/footer.html\
	doc/header.html\
	doc/logo.gif\
	doc/stylesheet.css\
	doc/html/logo.gif

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

