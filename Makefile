CC = gcc 

CFLAGS = -Wall -c -g
LDFLAGS =

BLUETOOTH_DIR = bluetooth
USB_DIR       = usb
TOOLS_DIR     = tools

all: bluetooth usb tools

bluetooth: $(BLUETOOTH_DIR)/ds4_bt.o

$(BLUETOOTH_DIR)/ds4_bt.o: $(BLUETOOTH_DIR)/ds4_bt.c
	$(CC) $(CFLAGS) -o $@ $^

usb: $(USB_DIR)/ds4_usb.o

$(USB_DIR)/ds4_usb.o: $(USB_DIR)/ds4_usb.c
	$(CC) $(CFLAGS) $(shell pkg-config libusb-1.0 --cflags) -o $@ $^

tools: $(TOOLS_DIR)/ds4_sync.o $(BLUETOOTH_DIR)/ds4_bt.o $(USB_DIR)/ds4_usb.o
	$(CC) $(LDFLAGS) -o $(TOOLS_DIR)/ds4_sync $^ -lusb-1.0 -lbluetooth

$(TOOLS_DIR)/ds4_sync.o: $(TOOLS_DIR)/ds4_sync.c
	$(CC) $(CFLAGS) $(shell pkg-config libusb-1.0 --cflags) -I$(BLUETOOTH_DIR)/ -I$(USB_DIR)/ -o $@ $^ 

clean:
	rm -f $(TOOLS_DIR)/ds4_sync
	rm -f $(TOOLS_DIR)/*.o
	rm -f $(BLUETOOTH_DIR)/*.o
	rm -f $(USB_DIR)/*.o
