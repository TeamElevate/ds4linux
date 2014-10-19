AM_CFLAGS = -Ibluetooth -Iusb -Icontroller
TOOLS = tools/ds4_conn tools/ds4_sync
bin_PROGRAMS = $(TOOLS)

tools_ds4_conn_SOURCES = tools/ds4_conn.c bluetooth/ds4_bt.c 
tools_ds4_conn_LDADD   = $(LIBS)

tools_ds4_sync_SOURCES = tools/ds4_sync.c bluetooth/ds4_bt.c usb/ds4_usb.c
tools_ds4_sync_LDADD   = $(LIBS)
