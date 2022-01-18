#ifndef CRC_H
#define CRC_H

unsigned short crc16_1189_update(unsigned char data, unsigned short crc);
unsigned short crc16_1189(const void *data, int len);
unsigned short crc16_update(unsigned char data, unsigned short crc);
unsigned short crc16(const void *data, int len);

#endif
