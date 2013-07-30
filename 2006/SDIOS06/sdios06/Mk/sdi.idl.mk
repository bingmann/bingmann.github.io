
# This is used to create interface stubs from the specified IDL files
# Files are specified through IDLS and SRCS. Both ignoring anything else than .idl files!

include $(top_srcdir)/Mk/l4.build.mk

#Portable way of converting SRCS/IDLS to IDLS

IDLINTERF=	${filter %.idl, ${SRCS}}
IDLINTERF+=	${filter %.idl, ${IDLS}}

IDLSTUBS = $(subst .idl,.h,$(IDLINTERF))

do-all:		$(IDLSTUBS)

$(IDLSTUBS): $(IDLINTERF)
	@$(ECHO_MSG) client-stub : `echo $@ | sed s,^$(top_srcdir)/,,`
	$(IDL) $(IDLFLAGS) -c -h $@ $(srcdir)/$(subst .h,.idl,$@)

do-clean:	idl-clean

idl-clean:
	rm -f *~ \#* $(IDLSTUBS)

do-install:


