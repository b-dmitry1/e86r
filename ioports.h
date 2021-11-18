#ifndef IOPORTS_H
#define IOPORTS_H

class SmallBuffer
{
private:
	unsigned char m_data[16];
	int m_rp, m_wp, m_count;
	mutex m_mtx;
public:
	SmallBuffer()
	{
		m_rp = m_wp = m_count = 0;
		memset(m_data, 0, sizeof(m_data));
	}

	int count() const
	{
		return m_count;
	}

	int peek()
	{
		return m_data[m_rp];
	}

	int get()
	{
		int res;
		if (m_count <= 0)
			return -1;
		m_mtx.lock();
		res = m_data[m_rp++];
		m_rp %= sizeof(m_data);
		m_count--;
		m_mtx.unlock();
		return res;
	}

	void put(unsigned char value)
	{
		while (m_count > sizeof(m_data))
		{
			m_wp++;
			m_wp %= sizeof(m_data);
			m_count--;
		}
		m_mtx.lock();
		m_data[m_wp++] = value;
		m_wp %= sizeof(m_data);
		m_count++;
		m_mtx.unlock();
	}
};

extern unsigned char ports[65536];

extern SmallBuffer keybuf;
extern SmallBuffer mousebuf;

void mouseevent(int x, int y, int buttons);

unsigned char portread8(unsigned short port);
unsigned short portread16(unsigned short port);
unsigned int portread32(unsigned short port);
void portwrite8(unsigned short port, unsigned char v);
void portwrite16(unsigned short port, unsigned short v);
void portwrite32(unsigned short port, unsigned int v);

#endif
