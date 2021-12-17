#include "stdafx.h"
#include "cpu.h"
#include "memdescr.h"
#include "interrupts.h"
#include "disk.h"
#include "pic_pit.h"

fdd_t fdd[NUM_FDD] = {{0}};

hdd_t hdd[NUM_HDD] = {{0}};

// Template for drive identify command
ide_drive_id_t drive_id =
{
	0x40, 1023, 0, 16, 512 * 63, 512, 63, {0, 0, 0}, "21436587",
	{0, 0, 0}, "1r", "yMH DD", {0, 0}, 0, {0, 0, 0}, 0x0001, 1023, 16, 63,
	(1023 * 16 * 63) & 0xFFFFu, (1023 * 16 * 63) >> 16u, 0,
	(1023 * 16 * 63) & 0xFFFFu, (1023 * 16 * 63) >> 16u
};


void disk_init()
{
	memset(&fdd, 0, sizeof(fdd));
	memset(&hdd, 0, sizeof(hdd));
}

int disk_set_fdd(int drive, int cyls, int heads, int sectors)
{
	if ((drive < 0) || (drive >= NUM_FDD))
		return 0;

	fdd_t *d = &fdd[drive];

	memset(d, 0, sizeof(fdd_t));

	d->cyls = cyls;
	d->heads = heads;
	d->sectors = sectors;

	return 1;
}

int disk_set_hdd(int drive, int cyls, int heads, int sectors)
{
	if ((drive < 0) || (drive >= NUM_HDD))
		return 0;

	hdd_t *d = &hdd[drive];

	memset(d, 0, sizeof(hdd_t));

	d->cyls = cyls;
	d->heads = heads;
	d->sectors = sectors;
	d->drive = drive;

	d->irq_enabled = 1;

	return 1;
}

void disk_deinit()
{
}

void disk_read()
{
	int drive, cyl, head, sector, numheads, numsectors;
	int lba;
	int n;

	void (*hw_read)(int, unsigned char *, unsigned int, unsigned int) = hw_read_floppy;

	hdd_t *hd;
	fdd_t *fd;

	// AL - num sectors
	// CH - cyl
	// CL - sector
	// DH - head
	// DL - disk
	// ES:BX - buffer pointer

	drive = r.dl;

	if (drive < NUM_FDD)
	{
		fd = &fdd[drive];
		numheads = fd->heads;
		numsectors = fd->sectors;
	}

	if ((drive & 0x80) && ((drive & 0x7F) < NUM_HDD))
	{
		hd = &hdd[drive & 0x7F];
		hw_read = hw_read_hdd;
		numheads = hd->heads;
		numsectors = hd->sectors;
	}

	if (0)
	{
		// Error
		r.flags |= F_C;
		r.ax = 0x100 | (r.ax & 0xFF);
		return;
	}

	cyl = (r.ch + ((r.cl & 0xC0) << 2)) & 0x3FF;
	head = r.dh & 0x0F;
	sector = r.cl & 0x3F;

	lba = (cyl * numheads + head) * numsectors + sector - 1;

	n = r.al;

	if (n <= 0)
		n = 1;

	hw_read(drive & 0x7F, &ram[es.value * 16 + r.bx], lba, n);

	r.flags &= ~F_C;
	r.ax = r.ax & 0xFF;
}

void disk_write()
{
	int drive, cyl, head, sector, numheads, numsectors;
	int lba;
	int n;

	void (*hw_write)(int, const unsigned char *, unsigned int, unsigned int) = hw_write_floppy;
	hdd_t *hd;
	fdd_t *fd;

	// AL - num sectors
	// CH - cyl
	// CL - sector
	// DH - head
	// DL - disk
	// ES:BX - buffer pointer

	drive = r.dl;

	if (drive < NUM_FDD)
	{
		fd = &fdd[drive];
		numheads = fd->heads;
		numsectors = fd->sectors;
	}

	if ((drive & 0x80) && ((drive & 0x7F) < NUM_HDD))
	{
		hd = &hdd[drive & 0x7F];
		hw_write = hw_write_hdd;
		numheads = hd->heads;
		numsectors = hd->sectors;
	}

	if (0)//fp == NULL)
	{
		// Error
		r.flags |= F_C;
		r.ax = 0x100 | (r.ax & 0xFF);
		return;
	}

	cyl = (r.ch + ((r.cl & 0xC0) << 2)) & 0x3FF;
	head = r.dh & 0x0F;
	sector = r.cl & 0x3F;

	lba = (cyl * numheads + head) * numsectors + sector - 1;

	n = r.al;

	if (n <= 0)
		n = 1;

	hw_write(drive & 0x7F, &ram[es.value * 16 + r.bx], lba, n);
	// fseek(fp, lba * 512, SEEK_SET);
	// fwrite(&ram[es.value * 16 + r.bx], 512, n, fp);
	
	r.flags &= ~F_C;
	r.ax = r.ax & 0xFF;
}

void disk_info()
{
	int drive;

	hdd_t *hd;
	fdd_t *fd;

	drive = r.dl;

	if (drive < NUM_FDD)
	{
		fd = &fdd[drive];

		r.bx = 4; // 1.44 MB
		r.cl = fd->sectors;
		r.ch = fd->cyls - 1;
		r.dl = NUM_HDD;
		r.dh = fd->heads - 1;
		r.flags &= ~F_C;
		r.flags |= F_I;
		return;
	}

	if ((drive & 0x80) && ((drive & 0x7F) < NUM_HDD))
	{
		hd = &hdd[drive & 0x7F];

		r.bx = 41;
		r.cl = hd->sectors;
		r.cl |= (hd->cyls >> 2) & 0xC0;
		r.ch = hd->cyls & 0xFF;
		r.dl = NUM_HDD;
		r.dh = hd->heads - 1;
		r.flags &= ~F_C;
		r.flags |= F_I;
		return;
	}

	// Error
	r.flags |= F_C;
	r.cx = 0;
	r.dx = 0;
	r.ax = 0x100 | (r.ax & 0xFF);
}

void bios_disk()
{
	int f = r.ah;

	switch (f)
	{
		case 0:
			r.flags &= ~F_C;
			r.ax = r.ax & 0xFF;
			return;
		case 2:
			disk_read();
			return;
		case 3:
			disk_write();
			return;
		case 8:
			disk_info();
			return;
		case 0x95:
			if (r.dl == 0x0)
			{
				r.ah = 0;
				r.al = 1;
				r.flags &= ~F_C;
				return;
			}
			if (r.dl == 0x80)
			{
				r.ah = 0;
				r.al = 3;
				r.flags &= ~F_C;
				return;
			}
			break;
	}

	r.ax = 0x0100 | (r.ax & 0xFF);
	r.flags |= F_C;
}


// IDE HDD emulation

int get_drive_number_by_port(int port)
{
	int result = 0;

	if ((port & 0x80) == 0)
		result += 2;

	if (result >= NUM_HDD)
		return result;

	result += hdd[result].drive;

	return result;
}

void ide_irq(int drive)
{
	if (drive >= NUM_HDD)
		return;

	if (hdd[drive & ~1].irq_enabled)
		irq(drive >= 2 ? 15 : 14);
}

void ide_write(int port, unsigned char value)
{
	unsigned int lba;
	hdd_t *d;
	hdd_t *ch;

	int drive = get_drive_number_by_port(port);

	if (drive >= NUM_HDD)
		return;

	d = &hdd[drive & ~1];
	ch = &hdd[drive];

	switch (port | 0x80)
	{
		case 0x1F0:
			if (ch->pos < sizeof(ch->buffer))
			{
				ch->buffer[ch->pos] = value;
			}
			ch->pos++;
			if ((ch->cmd == HDD_CMD_WRITE) && (ch->numsectors > 0))
			{
				if (ch->pos >= 512 * ch->numsectors)
					ch->pos = 0;
			}
			break;
		case 0x1F1:
			if ((ch->cmd == HDD_CMD_WRITE) && (ch->numsectors > 0))
			{
				ch->pos++;

				if (ch->pos % 512 == 0)
				{
					d->lba++;
					hw_write_hdd(drive, d->buffer, d->lba, 1);
				}

				if ((ch->pos % 512 == 0) && (ch->pos / 512 < ch->numsectors))
					ide_irq(drive);

				if (ch->pos >= 512 * ch->numsectors)
					ch->pos = 0;
			}
			break;
		case 0x1F2:
			d->numsectors = value;
			ch->have_data = 0;
			break;
		case 0x1F3:
			d->sector = value;
			ch->have_data = 0;
			break;
		case 0x1F4:
			d->cyl = (d->cyl & 0xFF00) | value;
			ch->have_data = 0;
			break;
		case 0x1F5:
			d->cyl = (d->cyl & 0xFF) | (value << 8);
			ch->have_data = 0;
			break;
		case 0x1F6:
			// d->drive = (value & 0x10) ? 1 : 0;
			d->head = value & 0x0F;
			// d->lba =  (value & 0x40) ? 1 : 0;
			ch->have_data = 0;
			break;
		case 0x1F7:
			d->have_data = 0;
			ch->error = 0;
			lba = (d->cyl * ch->heads + d->head) * ch->sectors + d->sector - 1;
			d->cmd = value;
			switch (value)
			{
				case HDD_CMD_RESTORE:
				case HDD_CMD_SEEK:
					ide_irq(drive);
					break;
				case HDD_CMD_INIT:
					break;
				case HDD_CMD_READ:
					ch->lba = lba;
					hw_read_hdd(drive, ch->buffer, ch->lba, 1);
					ch->pos = 0;
					ch->have_data = d->numsectors * 512;
					ide_irq(drive);
					break;
				case HDD_CMD_WRITE:
					ch->lba = lba;
					ch->pos = 0;
					ch->have_data = d->numsectors * 512;
					break;
				case HDD_CMD_SPECIFY:
					ide_irq(drive);
					break;
				case HDD_CMD_IDENTIFY:
					memset(ch->buffer, 0, 512);
					drive_id.current_cyls = ch->cyls;
					drive_id.current_heads = ch->heads;
					drive_id.current_sectors = ch->sectors;
					drive_id.cyls = ch->cyls;
					drive_id.heads = ch->heads;
					drive_id.sectors = ch->sectors;
					drive_id.current_lba_capacity_low = (ch->cyls * ch->heads * ch->sectors) & 0xFFFFu;
					drive_id.current_lba_capacity_high = (ch->cyls * ch->heads * ch->sectors) >> 16u;
					drive_id.default_lba_capacity_low = (ch->cyls * ch->heads * ch->sectors) & 0xFFFFu;
					drive_id.default_lba_capacity_high = (ch->cyls * ch->heads * ch->sectors) >> 16u;
					drive_id.serial_number[0] += drive;

					memcpy(ch->buffer, &drive_id, sizeof(drive_id));

					ch->pos = 0;
					d->numsectors = 1;
					ch->have_data = d->numsectors * 512;

					ide_irq(drive);
					break;
				default:
					break;
			}
			break;
		case 0x3F6:
			// Interrupt / reset register
			hdd[drive & ~1].irq_enabled = ~value & 0x02 ? 1 : 0;
			break;
	}
}

unsigned char ide_read(int port)
{
	hdd_t *d;
	int r = 0;

	// We read 16-bit ports in 2 transactions so we need this flag
	// to know when OS reads data (1x1F0) or an error (0x1F1) register
	static int last_0 = 0;

	int drive = get_drive_number_by_port(port);

	if (drive >= NUM_HDD)
		return 0;

	d = &hdd[drive];

	switch (port | 0x80)
	{
		case 0x1F0:
			r = d->buffer[d->pos++ % sizeof(d->buffer)];

			if (d->pos % 512 == 0)
			{
				d->lba++;
				hw_read_hdd(drive, d->buffer, d->lba, 1);
			}

			if ((d->numsectors > 0) && (d->cmd == HDD_CMD_READ) && (d->pos % 512 == 0) && (d->pos / 512 < d->numsectors))
			{
				ide_irq(drive);
			}

			if (d->have_data)
			{
				d->have_data--;
			}
			break;
		case 0x1F1:
			if (last_0)
			{
				r = d->buffer[d->pos++ % sizeof(d->buffer)];
			
				if (d->pos % 512 == 0)
				{
					d->lba++;
					hw_read_hdd(drive, d->buffer, d->lba, 1);
				}

				if (d->have_data)
				{
					d->have_data--;
				}

				if ((d->numsectors > 0) && (d->cmd == HDD_CMD_READ) && (d->pos % 512 == 0) && (d->pos / 512 < d->numsectors))
				{
					if (d->have_data)
						ide_irq(drive);
				}
			}
			else
			{
				r = d->error;
			}
			break;
		case 0x1F2:
			r = d->numsectors;
			break;
		case 0x1F3:
			r = d->sector & 0xFF;
			break;
		case 0x1F4:
			r = d->sector >> 8;
			break;
		case 0x1F5:
			r = d->cyl;
			break;
		case 0x1F6:
			r = d->head;
			if (hdd[drive & ~1].drive)
				r |= 0x10;
			break;
		case 0x1F7:
			irqs &= ~((1 << 15) | (1 << 14));
			d->irq = 0;
			r = ((d->have_data) ? 0x08 : 0) | 0x50;
			break;
		case 0x3F6:
			r = ((d->have_data) ? 0x08 : 0) | 0x50;
			break;
		case 0x3F7:
			r = d->head | (d->drive ? 0x10 : 0);
			break;
	}

	last_0 = (port & 7) == 0;
	return r;
}
