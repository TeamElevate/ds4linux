CC = gcc 

CFLAGS = -Wall -c -g
LDFLAGS =

BLUETOOTH_DIR  = bluetooth
USB_DIR        = usb
CONTROLLER_DIR = controller
TOOLS_DIR      = tools
TOOLS          = $(TOOLS_DIR)/ds4_sync.c $(TOOLS_DIR)/ds4_conn.c
TOOL_OBJS      = $(TOOLS:.c=.o)
TOOL_EXES      = $(basename $(TOOLS))

all: bluetooth usb tools

bluetooth: $(BLUETOOTH_DIR)/ds4_bt.o

$(BLUETOOTH_DIR)/ds4_bt.o: $(BLUETOOTH_DIR)/ds4_bt.c
	$(CC) $(CFLAGS) -o $@ $^

usb: $(USB_DIR)/ds4_usb.o

$(USB_DIR)/ds4_usb.o: $(USB_DIR)/ds4_usb.c
	$(CC) $(CFLAGS) $(shell pkg-config libusb-1.0 --cflags) -o $@ $^

tools: $(TOOL_EXES)

$(TOOL_EXES): %: %.o $(USB_DIR)/ds4_usb.o $(BLUETOOTH_DIR)/ds4_bt.o
	$(CC) $(LDFLAGS) -o $@ $^ $(shell pkg-config libusb-1.0 --libs) $(shell pkg-config bluez --libs)

$(TOOL_OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(shell pkg-config libusb-1.0 --cflags) -I$(CONTROLLER_DIR)/ -I$(BLUETOOTH_DIR)/ -I$(USB_DIR)/ -o $@ $<

clean:
	rm -f $(TOOL_EXES)
	rm -f $(TOOL_OBJS)
	rm -f $(BLUETOOTH_DIR)/*.o
	rm -f $(USB_DIR)/*.o
