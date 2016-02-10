/*
 * fclient.c -- client program for the focuser
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

/*
 * we have to find out whether this is a sufficiently modern libusb.
 * unfortunately, older libusb implementations have no way to tell 
 * what version they are. But maybe the presence of the LIBUSB_ERROR_COUNT
 * preprocessor symbol allows us to recognize sufficiently modern 
 * libusb for a full implementation
 */

#ifndef LIBUSB_ERROR_COUNT
#define LIBUSB_LOG_LEVEL_INFO	3
#define LIBUSB_LOG_LEVEL_DEBUG	4
char	*libusb_strerror(int rc) {
	switch (rc) {
	case  0:
		return "Success";
	case -1:
		return "IO error";
	case -2:
		return "invalid parameter";
	case -3:
		return "access denied";
	case -4:
		return "no device";
	case -5:
		return "not found";
	case -6:
		return "busy";
	case -7:
		return "timeout";
	case -8:
		return "overflow";
	case -9:
		return "pipe error";
	case -10:
		return "system call interrupted";
	case -11:
		return "insufficient memory";
	case -12:
		return "not supported";
	}
	return "other error";
}
#endif /* LIBUSB_ERROR_COUNT */

/*
 * commands understood by the Focuser device
 */
#define FOCUSER_RESET	0
#define FOCUSER_GET	1
#define FOCUSER_SET	2
#define FOCUSER_LOCK	3
#define FOCUSER_RCVR	4
#define FOCUSER_STOP	5
#define FOCUSER_SAVED	6

/*
 * display the descriptors, for tesing
 */
int	show_descriptors(libusb_device_handle *handle) {
	libusb_device	*device = libusb_get_device(handle);

	// get the descriptors
	struct libusb_device_descriptor	device_descriptor;
	libusb_get_device_descriptor(device, &device_descriptor);
	printf("bLength:            %d\n", device_descriptor.bLength);
	printf("bDescriptorType:    %d\n", device_descriptor.bDescriptorType);
	printf("bcdUSB:             %x\n", device_descriptor.bcdUSB);
	printf("bDeviceClass:       %x\n", device_descriptor.bDeviceClass);
	printf("bDeviceSubClass:    %x\n", device_descriptor.bDeviceSubClass);
	printf("bDeviceProtocol:    %x\n", device_descriptor.bDeviceProtocol);
	printf("bMaxPacketSize0:    %d\n", device_descriptor.bMaxPacketSize0);
	printf("idVendor:           0x%04x\n", device_descriptor.idVendor);
	printf("idProduct:          0x%04x\n", device_descriptor.idProduct);
	unsigned char	s[128];
	if (device_descriptor.iManufacturer) {
		libusb_get_string_descriptor_ascii(handle,
			device_descriptor.iManufacturer, s, sizeof(s));
		printf("Manufacturer:       %s\n", s);
	}
	if (device_descriptor.iProduct) {
		libusb_get_string_descriptor_ascii(handle,
			device_descriptor.iProduct, s, sizeof(s));
		printf("Product:            %s\n", s);
	}
	if (device_descriptor.iSerialNumber) {
		libusb_get_string_descriptor_ascii(handle,
			device_descriptor.iSerialNumber, s, sizeof(s));
		printf("Serial Number:      %s\n", s);
	}
	printf("bNumConfigurations: %d\n", device_descriptor.bNumConfigurations);

	struct libusb_config_descriptor	*config_descriptor;
	if (libusb_get_config_descriptor(device, 0, &config_descriptor)) {
		fprintf(stderr, "cannot get config descriptor\n");
		return EXIT_FAILURE;
	}
	printf("    bLength:              %d\n",
		config_descriptor->bLength);
	printf("    bDescriptorType:      %d\n",
		config_descriptor->bDescriptorType);
	printf("    wTotalLength:         %d\n",
		config_descriptor->wTotalLength);
	printf("    bNumInterfaces:       %d\n",
		config_descriptor->bNumInterfaces);
	printf("    bConfigurationValue:  %d\n",
		config_descriptor->bConfigurationValue);
	if (config_descriptor->iConfiguration) {
		libusb_get_string_descriptor_ascii(handle,
			config_descriptor->iConfiguration, s, sizeof(s));
		printf("Serial Number:      %s\n", s);
	}
	printf("    bmAttributes:         %d\n",
		config_descriptor->bmAttributes);
	printf("    MaxPower:             %d\n",
		2 * config_descriptor->MaxPower);
	return EXIT_SUCCESS;
}

void	show_version() {
	const struct libusb_version* version;
	version = libusb_get_version();
	printf("libusb version: %d.%d.%d.%d\n",
		version->major, version->minor, version->micro, version->nano);
}

/*
 * Show usage message
 */
void	usage(const char *progname) {
	printf("usage:\n");
	printf("  %s [ options ] descriptors\n", progname);
	printf("  %s [ options ] set <value>\n", progname);
	printf("  %s [ options ] get\n", progname);
	printf("  %s [ options ] stop\n", progname);
	printf("  %s [ options ] receiver\n", progname);
	printf("  %s [ options ] [ lock | unlock ]\n", progname),
	printf("  %s [ options ] reset\n", progname);
	printf("options:\n");
	printf(" -d            enablue USB debugging\n");
	printf(" -h,-?         display this help and exit\n");
	printf(" -V            show version of USB library and exit\n");
	printf(" -v <vid>      use this vendor id to connect\n");
	printf(" -p <pid>      use this product id to connect\n");
	printf(" -f            fast movement (set command only)\n");
}

/*
 * Main function of the client program
 */
int	main(int argc, char *argv[]) {
	// parse command line
	int	c;
	int	debug = 0;
	uint16_t	vid = 0xf055;
	uint16_t	pid = 0x1235;
	int	fast = 0;
	while (EOF != (c = getopt(argc, argv, "dfh?v:p:V")))
		switch (c) {	
		case 'd':
			debug = 1;
			break;
		case 'f':
			fast = 1;
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			return EXIT_SUCCESS;
		case 'v':
			vid = atoi(optarg);
			break;
		case 'p':
			pid = atoi(optarg);
			break;
		case 'V':
			show_version();
			return EXIT_SUCCESS;
		}

	// get the command
	if (optind >= argc) {
		fprintf(stderr, "command argument missing\n");
		return EXIT_FAILURE;
	}
	char	*command = argv[optind++];

	// initialize libusb library
	libusb_context	*context;
	libusb_init(&context);
	libusb_set_debug(context,
		(debug) ? LIBUSB_LOG_LEVEL_DEBUG : LIBUSB_LOG_LEVEL_INFO);

	// connect to the device
	libusb_device_handle *handle;
	handle = libusb_open_device_with_vid_pid(context, vid, pid);
	if (NULL == handle) {
		fprintf(stderr, "cannot open device\n");
		return EXIT_FAILURE;
	}

	// process the descriptors command
	if (0 == strcmp(command, "descriptors")) {
		return show_descriptors(handle);
	}

	// commands below may need to evaluate the USB return code
	int		rc;

	// all commands need value and index, so we allocate these variables
	// only once
	uint16_t	value;
	uint16_t	index;

	// set command implementation
	if (0 == strcmp(command, "set")) {
		if (optind >= argc) {
			fprintf(stderr, "no argument to set given\n");
			return EXIT_SUCCESS;
		}
		value = atoi(argv[optind++]);
		index = (fast) ? 1 : 0;
		rc = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_OUT, FOCUSER_SET, 
			value, index, NULL, 0, 1000);
		if (rc) {
			fprintf(stderr, "cannot send SET: %s\n", 
				libusb_strerror(rc));
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	// set command implementation
	if (0 == strcmp(command, "get")) {
		index = 0xf;
		unsigned short	result[3];
		rc = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_IN, FOCUSER_GET, 
			0, index, (unsigned char *)result, 6, 1000);
		if (rc < 0) {
			fprintf(stderr, "cannot send GET: %s\n", 
				libusb_strerror(rc));
			return EXIT_FAILURE;
		}
		if (rc != 6) {
			fprintf(stderr, "focuser position not received\n");
			return EXIT_FAILURE;
		}
		if (result[0] == result[1]) {
			printf("current: %hu, target: %hu\n",
				result[0], result[1]);
		} else {
			printf("current: %hu, target: %hu, speed: %s\n",
				result[0], result[1],
				(result[2]) ? "fast" : "slow");
		}
		return EXIT_SUCCESS;
	}

	// set command implementation
	if (0 == strcmp(command, "saved")) {
		unsigned short	result;
		rc = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_IN, FOCUSER_GET, 
			0, 0, (unsigned char *)&result, 2, 1000);
		if (rc < 0) {
			fprintf(stderr, "cannot send GET: %s\n", 
				libusb_strerror(rc));
			return EXIT_FAILURE;
		}
		if (rc != 2) {
			fprintf(stderr, "focuser position not received\n");
			return EXIT_FAILURE;
		}
		printf("saved: %hu\n", result);
		return EXIT_SUCCESS;
	}

	// stop command implementation
	if (0 == strcmp(command, "stop")) {
		rc = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_OUT, FOCUSER_STOP, 
			0, 0, NULL, 0, 1000);
		if (rc < 0) {
			fprintf(stderr, "cannot send STOP: %s\n", 
				libusb_strerror(rc));
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	// set command implementation
	if (0 == strcmp(command, "receiver")) {
		unsigned char	result;
		rc = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_IN, FOCUSER_RCVR, 
			0, 0, (unsigned char *)&result, 1, 1000);
		if (rc < 0) {
			fprintf(stderr, "cannot send GET: %s\n", 
				libusb_strerror(rc));
			return EXIT_FAILURE;
		}
		if (rc != 1) {
			fprintf(stderr, "button status not received\n");
			return EXIT_FAILURE;
		}
		printf("receiver status: %c%c%c%c%s\n",
			(result & 0x01) ? 'A' : '_',
			(result & 0x02) ? 'B' : '_',
			(result & 0x03) ? 'C' : '_',
			(result & 0x04) ? 'D' : '_',
			(result & 0x80) ? " (locked)" : ""
		);
		return EXIT_SUCCESS;
	}

	// lock command
	if (0 == strcmp(command, "lock")) {
		rc = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_OUT, FOCUSER_LOCK, 
			0, 1, NULL, 0, 1000);
		if (rc) {
			fprintf(stderr, "cannot send SET: %s\n", 
				libusb_strerror(rc));
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	// unlock command
	if (0 == strcmp(command, "unlock")) {
		rc = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_OUT, FOCUSER_LOCK, 
			0, 0, NULL, 0, 1000);
		if (rc) {
			fprintf(stderr, "cannot send SET: %s\n", 
				libusb_strerror(rc));
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	// reset command
	if (0 == strcmp(command, "reset")) {
		rc = libusb_control_transfer(handle,
			LIBUSB_REQUEST_TYPE_VENDOR |
			LIBUSB_RECIPIENT_DEVICE |
			LIBUSB_ENDPOINT_OUT, FOCUSER_RESET, 
			0, 0, NULL, 0, 1000);
		if (rc) {
			fprintf(stderr, "cannot send RESET: %s (%d)\n", 
				libusb_strerror(rc), rc);
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	// command was not interpreted
	fprintf(stderr, "unknown command '%s'\n", command);
	return EXIT_FAILURE;
}