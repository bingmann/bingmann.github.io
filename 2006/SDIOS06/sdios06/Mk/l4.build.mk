######################################################################
##                
## Copyright (C) 2003-2005,  Karlsruhe University
##                
## File path:     l4.build.mk
## Description:   Rules for compilation
##                
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
## OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
## LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
## OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.
##                
## $Id: l4.build.mk,v 1.9.2.3 2004/04/28 18:02:52 skoglund Exp $
##                
######################################################################

VPATH=		$(srcdir)
MAKEFILES=	.depend

MKFILE_DEPS=	Makefile \
		$(top_srcdir)/Mk/l4.base.mk \
		$(top_srcdir)/Mk/l4.build.mk \
		$(top_srcdir)/Mk/l4.lib.mk \
		$(top_srcdir)/Mk/l4.prog.mk \
		$(top_srcdir)/Mk/sdi.idl.mk \
		$(top_builddir)/config.mk


# Portable way of converting SRCS to OBJS

_CC_OBJS=	${filter-out %.c %.S, ${SRCS}}
_C_OBJS=	${filter-out %.S %cc %cpp, ${SRCS}}
_S_OBJS=	${filter-out %.c %cc %cpp, ${SRCS}}

_IDL_STUBS=     ${filter %.idl, ${IDLS}}

_CC_OBJS_O= $(addsuffix .o, $(basename $(_CC_OBJS)))

OBJS+=		$(_CC_OBJS_O) $(_C_OBJS:.c=.o) $(_S_OBJS:.S=.o) \
		${SRCS:C/.(cc|c|S)$/.o/g}

SERV_STUBS+=  $(subst .idl,-server.h,$(_IDL_STUBS))
CLIE_STUBS+=  $(subst .idl,-client.h,$(_IDL_STUBS))
TMPL_STUBS+=  $(subst .idl,-template.cc,$(_IDL_STUBS))

.cc.o:	$(MKFILE_DEPS)
	@$(ECHO_MSG) `echo $< | sed s,^$(top_srcdir)/,,`
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.cpp.o:	$(MKFILE_DEPS)
	@$(ECHO_MSG) `echo $< | sed s,^$(top_srcdir)/,,`
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.c.o:	$(MKFILE_DEPS)
	@$(ECHO_MSG) `echo $< | sed s,^$(top_srcdir)/,,`
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.S.o:	$(MKFILE_DEPS)
	@$(ECHO_MSG) `echo $< | sed s,^$(top_srcdir)/,,`
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# IDL client stub code generation ...

$(SERV_STUBS):	$(MKFILE_DEPS)
		@$(ECHO_MSG) server-stub : $@
		$(IDL) $(IDLFLAGS) -s -h $@ $(srcdir)/$(subst -server.h,.idl,$@)

$(CLIE_STUBS):	$(MKFILE_DEPS)
		@$(ECHO_MSG) client-stub : $@ 
		$(IDL) $(IDLFLAGS) -c -h $@ $(srcdir)/$(subst -client.h,.idl,$@)

$(TMPL_STUBS):	$(MKFILE_DEPS)
		@$(ECHO_MSG) template-stub : $@ 
		$(IDL) $(IDLFLAGS) -t -o $@ $(srcdir)/$(subst -template.cc,.idl,$@)

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@$(ECHO_MSG) Rebuilding `echo $(srcdir)/ | sed s,^$(top_srcdir)/*,,`$@
	@(cd $(top_builddir) && $(SHELL) ./config.status)

$(top_builddir)/config.mk: $(top_srcdir)/config.mk.in $(top_builddir)/config.status
	@$(ECHO_MSG) Rebuilding config.mk
	@(cd $(top_builddir) && $(SHELL) ./config.status)

$(top_builddir)/config.status: $(top_srcdir)/configure
	(cd $(top_builddir) && $(SHELL) ./config.status --recheck)

$(top_srcdir)/configure: $(top_srcdir)/configure.in
	@$(ECHO_MSG) Rebuilding configure
	@(cd $(top_srcdir) && $(AUTOCONF))

.depend: $(SRCS)
	@if test ! -z "$(SRCS)"; then \
	  $(ECHO_MSG) Making dependencies in \
	    `echo $(srcdir) | sed s,^$(top_srcdir)/,,`; \
	  $(CC) $(CPPFLAGS) $(CFLAGS) -M -MG \
	    `echo $(SRCS) | sed -e 's, , $(srcdir)/,g' -e 's,^,$(srcdir)/,'` \
	    > .depend; \
	else \
	  touch $@; \
	fi
