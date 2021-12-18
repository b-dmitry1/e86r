#ifndef DISK_H
#define DISK_H

#define HDD_DATA			0x1F0
#define HDD_ERROR			0x1F1
#define HDD_NUMSECTORS		0x1F2
#define HDD_SECTOR			0x1F3
#define HDD_LCYL			0x1F4
#define HDD_HCYL			0x1F5
#define HDD_CURRENT			0x1F6	// 101dhhhh
#define HDD_STATUS			0x1F7
#define HDD_CMD				0x3F6

#define HDD_STATUS_ERROR	0x01
#define HDD_STATUS_ECC		0x04
#define HDD_STATUS_DRQ		0x08
#define HDD_STATUS_SEEK		0x10
#define HDD_STATUS_WR_ERROR	0x20
#define HDD_STATUS_READY	0x40
#define HDD_STATUS_BUSY		0x80

#define HDD_CMD_RESTORE			0x10
#define HDD_CMD_READ			0x20
#define HDD_CMD_WRITE			0x30
#define HDD_CMD_VERIFY			0x40
#define HDD_CMD_FORMAT			0x50
#define HDD_CMD_INIT			0x60
#define HDD_CMD_SEEK			0x70
#define HDD_CMD_DIAG			0x90
#define HDD_CMD_SPECIFY			0x91
#define HDD_CMD_IDENTIFY		0xEC
#define HDD_CMD_IDENTIFY_ATAPI	0xEC

typedef struct
{
	int cyls;
	int heads;
	int sectors;
} fdd_t;

typedef struct
{
	int cyls;
	int heads;
	int sectors;

	// Command / state
	int drive;
	int cyl;
	int head;
	int sector;
	int numsectors;
	unsigned int lba;
	int cmd;
	int error;
	int pos;
	int have_data;
	int irq;
	int irq_enabled;
	
	unsigned char buffer[512];
} hdd_t;

typedef struct
{
	// 00  0x40 - fixed disk, 0x80 - removable
	unsigned short flags;
	// 01
	unsigned short cyls;
	// 02
	unsigned short reserved1;
	// 03
	unsigned short heads;
	// 04
	unsigned short unformatted_track_bytes;
	// 05
	unsigned short unformatted_sector_bytes;
	// 06
	unsigned short sectors;
	// 07 - 09
	unsigned short reserved2[3];
	// 10 - 19
	unsigned char serial_number[20];
	// 20 - 22
	unsigned short reserved3[3];
	// 23 - 26
	unsigned char firmware_ver[8];
	// 27 - 46
	unsigned char model_name[40];
	// 47 - 48
	unsigned short reserved4[2];
	// 49  0x200 - LBA supported
	unsigned short capabilites;
	// 50 - 52
	unsigned short reserved5[3];
	// 53  0x0001 - fields 54 - 58 valid
	unsigned short valid_fields;
	// 54
	unsigned short current_cyls;
	// 55
	unsigned short current_heads;
	// 56
	unsigned short current_sectors;
	// 57
	unsigned short current_lba_capacity_low;
	// 58
	unsigned short current_lba_capacity_high;
	// 59
	unsigned short reserved6;
	// 60
	unsigned int default_lba_capacity_low;
	// 61
	unsigned int default_lba_capacity_high;
	// 62 - 255
	unsigned short reserved7[255 - 62 + 1];
} ide_drive_id_t;


void disk_init();
int disk_set_fdd(int drive, int cyls, int heads, int sectors);
int disk_set_hdd(int drive, int cyls, int heads, int sectors);
void disk_deinit();
void bios_disk();

void hw_read_floppy(int disk, unsigned char *buffer, unsigned int lba, unsigned int count);
void hw_write_floppy(int disk, const unsigned char *buffer, unsigned int lba, unsigned int count);
void hw_read_hdd(int disk, unsigned char *buffer, unsigned int lba, unsigned int count);
void hw_write_hdd(int disk, const unsigned char *buffer, unsigned int lba, unsigned int count);

void ide_write(int port, unsigned char value);
unsigned char ide_read(int port);

#endif
