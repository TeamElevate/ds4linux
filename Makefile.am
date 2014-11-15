AM_CFLAGS = -Ibluetooth -Iusb -Icontroller -Ishm
if MRAA
AM_CFLAGS += -Iuavtalk
endif

TOOLS = tools/ds4_conn tools/ds4_sync tools/ds4_daemon tools/ds4_client
if MRAA
TOOLS += tools/control
endif

bin_PROGRAMS = $(TOOLS)

tools_ds4_conn_SOURCES = tools/ds4_conn.c bluetooth/ds4_bt.c controller/ds4.c
tools_ds4_conn_LDADD   = $(BLUEZ_LIBS)

tools_ds4_sync_SOURCES = tools/ds4_sync.c bluetooth/ds4_bt.c usb/ds4_usb.c
tools_ds4_sync_LDADD   = $(BLUEZ_LIBS) $(LIBUSB_LIBS)

tools_ds4_daemon_SOURCES   = tools/ds4_daemon.c bluetooth/ds4_bt.c controller/ds4.c shm/shm.c
tools_ds4_daemon_LDADD   = $(BLUEZ_LIBS)

tools_ds4_client_SOURCES = tools/ds4_client.c controller/ds4.c bluetooth/ds4_bt.c shm/shm.c
 
if MRAA
tools_control_SOURCES  = tools/control.c bluetooth/ds4_bt.c uavtalk/manualcontrol.c controller/ds4.c uavtalk/uavtalkheader.c
tools_control_LDADD    = $(BLUEZ_LIBS) $(MRAA_LIBS)
endif
