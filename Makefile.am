if LIBC
  __enable_libc=yes
else
  __enable_libc=no
endif

if LIBK
  __enable_libk=yes
else
  __enable_libk=no
endif

if JKERNEL
  __enable_libk=yes
endif

if DEBUG
  __enable_debug=yes
else
  __enable_debug=no
endif

lib:
if LIB
	cd libc; ./configure \
		--host=$(HOST) \
		--enable-libc=$(__enable_libc) \
		--enable-libk=$(__enable_libk) \
		--enable-debug=$(__enable_debug) \
		--includedir=$(abs_srcdir)/include \
		--libdir=$(abs_srcdir)/lib \
		--prefix=$(abs_srcdir)/libc-prefix \
		CC=$(CC)
	$(MAKE) -C $(srcdir)/libc
	$(MAKE) -C $(srcdir)/libc install

endif 


jkernel: lib
if JKERNEL
	cd jkernel; ./configure \
		--enable-debug=$(__enable_debug)
	$(MAKE) -C $(srcdir)/jkernel
endif 

all: lib jkernel

clean-local:
if LIB
	$(MAKE) -C $(srcdir)/libc
	-rm -rf $(abs_srcdir)/lib
	-rm -rf $(abs_srcdir)/include
	-rm -rf $(abs_srcdir)/libc-prefix
endif

distclean-local:
if LIB
	$(MAKE) -C $(srcdir)/libc distclean
endif 
