AM_CFLAGS = -Ibluetooth -Iusb -Icontroller
if MRAA
AM_CFLAGS += -Iuavtalk
endif

TOOLS = tools/ds4_conn tools/ds4_sync
if MRAA
TOOLS += tools/control tools/fakecontrol
endif

bin_PROGRAMS = $(TOOLS)

tools_ds4_conn_SOURCES = tools/ds4_conn.c bluetooth/ds4_bt.c controller/ds4.c
tools_ds4_conn_LDADD   = $(BLUEZ_LIBS)

tools_ds4_sync_SOURCES = tools/ds4_sync.c bluetooth/ds4_bt.c usb/ds4_usb.c
tools_ds4_sync_LDADD   = $(BLUEZ_LIBS) $(LIBUSB_LIBS)

if MRAA
tools_control_SOURCES  = tools/control.c bluetooth/ds4_bt.c uavtalk/manualcontrol.c controller/ds4.c
tools_control_LDADD    = $(BLUEZ_LIBS) $(MRAA_LIBS)

tools_fakecontrol_SOURCES  = tools/fakecontrol.c uavtalk/manualcontrol.c controller/ds4.c
tools_fakecontrol_LDADD    = $(MRAA_LIBS)
endif
