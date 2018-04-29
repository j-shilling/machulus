LIBS=$(top_srcdir)/src/libc/libk.a -nostdlib -lgcc

KERNEL_ARCH_OBJS=\
  $(ARCHDIR)/boot.o \
  $(ARCHDIR)/tty.o

KERNEL_OBJS=\
  $(KERNEL_ARCH_OBJS) \
  kernel.o

OBJS=\
  $(ARCHDIR)/crti.o \
  $(CRTBEGIN) \
  $(KERNEL_OBJS) \
  $(CRTEND) \
  $(ARCHDIR)/crtn.o

LINK_LIST=\
  $(LDFLGS) \
  $(ARCHDIR)/crti.o \
  $(CRTBEGIN) \
  $(KERNEL_OBJS) \
  $(LIBS) \
  $(CRTEND) \
  $(ARCHDIR)/crtn.o

.PHONY: all clean
SUFFIXES=.o .c .s

all: jkernel

jkernel: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)

.c.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.s.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean-local:
	rm -f jkernel
	rm -f *.o
	rm -f $(ARCHDIR)/*.o
