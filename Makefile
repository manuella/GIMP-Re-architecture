#******************************************************************
# Make file                                                       *
# default: compiles GimpDBusServer.c, installs with gimptools-2.0 *
# clean: removes obsolete executable                              *
#******************************************************************

GimpDBusServer: GimpDBusServer.c
	gimptool-2.0 --build GimpDBusServer.c
clean:
	rm -f GimpDBusServer