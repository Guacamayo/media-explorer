SUBDIRS = build mex applets shell plugins tools

if ENABLE_TESTS
SUBDIRS += tests
endif

if ENABLE_EXTERNAL_PLAYER
SUBDIRS += player
endif

SUBDIRS += thumbnailer

SUBDIRS += data doc po

ACLOCAL_AMFLAGS = -I build ${ACLOCAL_FLAGS}

DISTCLEANFILES = aclocal.m4 config.h.in gtk-doc.make
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-tests

stats:
	@total=`find . -name "*.[ch]" | xargs wc -l | tail -n 1 | \
	        tr  -d [:space:][:alpha:]` && \
	 real=`sloccount mex/ examples/ | grep -e "^ansic:" | \
	        awk '{print $$2}'` && \
	 echo "Lines of code: $$total ($$real real)"

release: distcheck
	git tag $(VERSION) -a -m "$(VERSION)"
	$(MAKE) dist
	@echo "**************************"
	@echo "Tag & tarball created: $(VERSION)"
	@echo "**************************"

include $(top_srcdir)/build/Makefile.am.release
