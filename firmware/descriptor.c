/*
 * descriptor.c -- descriptor implementation for the Focuser project
 *
 * (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
 */
#include <descriptor.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
	.Header			= {
		.Size = sizeof(USB_Descriptor_Device_t),
		.Type = DTYPE_Device
	},

	.USBSpecification	= VERSION_BCD(1,1,0),
	.Class			= USB_CSCP_NoDeviceClass,
	.SubClass		= USB_CSCP_NoDeviceSubclass,
	.Protocol		= USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size		= FIXED_CONTROL_ENDPOINT_SIZE,

	// CONFIGURATION: vendor id and product id
	.VendorID               = 0xF055,
	.ProductID              = 0x1235,
	.ReleaseNumber          = VERSION_BCD(1,0,0),

	.ManufacturerStrIndex   = STRING_ID_Manufacturer,
	.ProductStrIndex        = STRING_ID_Product,
	.SerialNumStrIndex      = STRING_ID_SerialNumber,

	.NumberOfConfigurations = 1
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
        .Config = {
		.Header                 = {
			.Size = sizeof(USB_Descriptor_Configuration_Header_t),
			.Type = DTYPE_Configuration
		},

		.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces        = 1,

		.ConfigurationNumber    = 1,
		.ConfigurationStrIndex  = NO_DESCRIPTOR,

		.ConfigAttributes       = USB_CONFIG_ATTR_RESERVED,

		.MaxPowerConsumption    = USB_CONFIG_POWER_MA(50)
	},
};

const USB_Descriptor_String_t PROGMEM LanguageString
	= USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM ManufacturerString
	= USB_STRING_DESCRIPTOR(L"Andreas Mueller");
const USB_Descriptor_String_t PROGMEM ProductString
	= USB_STRING_DESCRIPTOR(L"Focuser " VERSION BUILDDATE);

USB_Descriptor_String_t	SerialNumberMemoryString
	= USB_STRING_DESCRIPTOR(L".......");

/**
 * \brief Get descriptors
 *
 * This function returns the USB descriptor requested by the wValue and
 * wIndex parameters. Since we have the serial number in EEPROM, we
 * also need the MemoryAddressSpace argument to indicate to the caller
 * in what memory address space it can find the descriptor.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
				    const uint16_t wIndex,
				    const void** const DescriptorAddress,
				    uint8_t* MemoryAddressSpace)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;
	*MemoryAddressSpace = MEMSPACE_FLASH;

	switch (DescriptorType) {
	case DTYPE_Device:
		Address = &DeviceDescriptor;
		Size    = sizeof(USB_Descriptor_Device_t);
		break;
	case DTYPE_Configuration:
		Address = &ConfigurationDescriptor;
		Size    = sizeof(USB_Descriptor_Configuration_t);
		break;
	case DTYPE_String:
		switch (DescriptorNumber) {
		case STRING_ID_Language:
			Address = &LanguageString;
			Size    = pgm_read_byte(&LanguageString.Header.Size);
			break;
		case STRING_ID_Manufacturer:
			Address = &ManufacturerString;
			Size    = pgm_read_byte(&ManufacturerString.Header.Size);
			break;
		case STRING_ID_Product:
			Address = &ProductString;
			Size    = pgm_read_byte(&ProductString.Header.Size);
			break;
		case STRING_ID_SerialNumber:
#if 0
			Address = &SerialNumberString;
			Size    = eeprom_read_byte(&SerialNumberString.Header.Size);
			*MemoryAddressSpace = MEMSPACE_EEPROM;
#else
			Address = &SerialNumberMemoryString;
			Size = SerialNumberString.Header.Size;
			*MemoryAddressSpace = MEMSPACE_RAM;
#endif
			break;
		}

		break;
	}

	*DescriptorAddress = Address;
	return Size;
}

