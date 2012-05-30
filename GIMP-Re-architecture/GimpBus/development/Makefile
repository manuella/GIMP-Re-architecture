# File:
#   Makefile
# Summary:
#   A Makefile for GimpBu.

# +---------+---------------------------------------------------------
# | Globals |
# +---------+

# The utilities that these programs use.  These should be the names that
# pkg-config expects.  Note that the libgimp stuff is covered by gimptool.
UTILS=glib-2.0 gio-unix-2.0

# The applications we build
PLUGINS=gimpbus

# Instructions for building a plugin
BUILD_PLUGIN = \
	export CFLAGS="$(CFLAGS)"; \
	export LDFLAGS="$(LDFLAGS)"; \
	gimptool-2.0 --build

LIBGIMPBUS_SOURCES = slog.o


# +----------+--------------------------------------------------------
# | Settings |
# +----------+

CFLAGS=-g -Wall -O0 -DDEBUG -DOBSERVE $(shell pkg-config --cflags $(UTILS))
#-DSLOG

LDFLAGS=$(shell pkg-config --libs $(UTILS)) -L. -lgimpbus

LIBRARIES=libgimpbus.a


# +------------------+------------------------------------------------
# | Standard Targets |
# +------------------+

default: $(PLUGINS)

clean:
	rm -f *.o 

distclean: clean
	rm -rf $(APPS) $(PLUGINS)

install: gimpbus-install


# +--------------+----------------------------------------------------
# | Gimp Plugins |
# +--------------+

gimpbus: gimpbus.c libgimpbus.a
	$(BUILD_PLUGIN) gimpbus.c

gimpbus-install: gimpbus
	gimptool-2.0 --install-admin-bin $<

gimpbus-uninstall:
	gimptool-2.0 --uninstall-admin-bin gimpbus


# +-----------+-----------------------------------------------------------------
# | Libraries |
# +-----------+

libgimpbus.a: $(LIBGIMPBUS_SOURCES)
	ar -r $@ $^
	ranlib $@

