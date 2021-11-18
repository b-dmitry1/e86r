#pragma once

#pragma pack(push, 1)
typedef struct
{
	unsigned char second;
	unsigned char alm_second;
	unsigned char min;
	unsigned char alm_min;
	unsigned char hour;
	unsigned char alm_hour;
	unsigned char dow;
	unsigned char day;
	unsigned char month;
	unsigned char yearmod100;
	unsigned char rate : 4;
	unsigned char div : 3;
	unsigned char updateA : 1;
	unsigned char daylight : 1;
	unsigned char hr24 : 1;
	unsigned char bindate : 1;
	unsigned char sqwave : 1;
	unsigned char update_int_enable : 1;
	unsigned char alarm_int_enable : 1;
	unsigned char periodic_int_enable : 1;
	unsigned char updateB : 1;
	unsigned char int_status;
	unsigned char power_control;
	unsigned char reserved1 : 2;
	unsigned char time_error : 1;
	unsigned char cant_boot_hdd : 1;
	unsigned char ram_size_error : 1;
	unsigned char config_changed : 1;
	unsigned char checksum_error : 1;
	unsigned char power_lost : 1;
	unsigned char shutdown; // 0x0F: 0x05 / 0x0A - pmode exit
	unsigned char second_diskette : 4; // 0 - no, 1 - 360, 2 - 1.2, 3 - 720, 4 - 1.44
	unsigned char first_diskette : 4;
	unsigned char reserved2;
	unsigned char second_hdd : 4; // 0 - no, F - use 0x19/0x1A
	unsigned char first_hdd : 4;
	unsigned char reserved3;
	unsigned char diskette_drives_installed : 1;
	unsigned char math287_installed : 1;
	unsigned char reserved4 : 2;
	unsigned char primary_display : 2; // 2 - 80 col CGA/EGA/VGA
	unsigned char diskette_drives : 2; // 0 - 1
	unsigned char base_mem_size_low;
	unsigned char base_mem_size_high;
	unsigned char ext_mem_size_low;
	unsigned char ext_mem_size_high;
	unsigned char hdd0_type;
	unsigned char hdd1_type;
	unsigned char reserved5[19];
	unsigned char checksum1020_high;
	unsigned char checksum1020_low;
	unsigned char ext_mem_size2_low;
	unsigned char ext_mem_size2_high;
	unsigned char checksum1031_high;
	unsigned char checksum1031_low;
	unsigned char reserved6[12];
} cmos_t;
#pragma pack(pop)

unsigned char cmos_read(int port);
void cmos_write(int port, unsigned char value);
