#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "descs.h"
#include "stopwatch.hpp"
using namespace avrlib;

#define LEDBITS *((uint32_t volatile *)0xC0000000)
#define TMR *((uint32_t volatile *)0xC0000004)

#define USB_CTRL *((uint8_t volatile *)0xC0000008)
#define USB_CTRL_ATTACH (1<<0)
#define USB_CTRL_RST (1<<1)
#define USB_CTRL_RST_CLR (1<<2)

#define USB_ADDRESS *((uint8_t volatile *)0xC000000C)

#define USB_EP0_OUT_CTRL *((uint8_t volatile *)0xC0000010)
#define USB_EP0_OUT_STATUS *((uint8_t const volatile *)0xC0000010)
#define USB_EP0_OUT_CNT *((uint8_t volatile *)0xC0000011)
#define USB_EP0_IN_CTRL *((uint8_t volatile *)0xC0000014)
#define USB_EP0_IN_STATUS *((uint8_t const volatile *)0xC0000014)
#define USB_EP0_IN_CNT *((uint8_t volatile *)0xC0000015)
#define USB_EP0_OUT ((uint8_t volatile *)0xC0001000)
#define USB_EP0_IN ((uint8_t volatile *)0xC0001040)

#define USB_EP1_OUT_CTRL *((uint8_t volatile *)0xC0000018)
#define USB_EP1_OUT_STATUS *((uint8_t const volatile *)0xC0000018)
#define USB_EP1_OUT_CNT *((uint8_t volatile *)0xC0000019)
#define USB_EP1_IN_CTRL *((uint8_t volatile *)0xC000001C)
#define USB_EP1_IN_STATUS *((uint8_t const volatile *)0xC000001C)
#define USB_EP1_IN_CNT (*((uint8_t volatile *)0xC000001D))
#define USB_EP1_OUT ((uint8_t volatile *)0xC0001080)
#define USB_EP1_IN ((uint8_t volatile *)0xC00010C0)

#define USB_EP2_OUT_CTRL *((uint8_t volatile *)0xC0000048)
#define USB_EP2_OUT_STATUS *((uint8_t const volatile *)0xC0000048)
#define USB_EP2_OUT_CNT *((uint8_t volatile *)0xC0000049)
#define USB_EP2_IN_CTRL *((uint8_t volatile *)0xC000004C)
#define USB_EP2_IN_STATUS *((uint8_t const volatile *)0xC000004C)
#define USB_EP2_IN_CNT (*((uint8_t volatile *)0xC000004D))
#define USB_EP2_OUT ((uint8_t volatile *)0xC0001100)
#define USB_EP2_IN ((uint8_t volatile *)0xC0001140)

#define USB_EP3_OUT_CTRL *((uint8_t volatile *)0xC0000040)
#define USB_EP3_OUT_STATUS *((uint8_t const volatile *)0xC0000040)
#define USB_EP3_IN_CTRL *((uint8_t volatile *)0xC0000044)
#define USB_EP3_IN_STATUS *((uint8_t const volatile *)0xC0000044)

#define USB_EP_PAUSE_CLR (1<<7)
#define USB_EP_PAUSE_SET (1<<6)
#define USB_EP_TOGGLE_CLR (1<<5)
#define USB_EP_TOGGLE_SET (1<<4)
#define USB_EP_STALL_SET  (1<<3)
#define USB_EP_SETUP_CLR  (1<<2)
#define USB_EP_PULL       (1<<1)
#define USB_EP_PUSH       (1<<0)

#define USB_EP_PAUSE_bm   (1<<7)
#define USB_EP_TRANSIT_bm (1<<6)
#define USB_EP_TOGGLE     (1<<4)
#define USB_EP_STALL      (1<<3)
#define USB_EP_SETUP      (1<<2)
#define USB_EP_EMPTY      (1<<1)
#define USB_EP_FULL       (1<<0)

#define SPI_CTRL *((uint8_t volatile *)0xC0000020)
#define SPI_DATA *((uint8_t volatile *)0xC0000024)

#define ICAP_CTRL *((uint32_t volatile *)0xC0000028)
#define ICAP_CS_bm (1<<0)
#define ICAP_BUSY_bm (1<<1)
#define ICAP_RUN_bm (1<<2)
#define ICAP_DATA *((uint32_t volatile *)0xC000002C)

#define DNA *((uint64_t volatile *)0xC0000030)
#define DNA_READY_bm (1ull<<63)

#define TEST100 *((uint32_t volatile *)0xD0000000)
#define SDRAM_CTRL *((uint32_t volatile *)0xD0000010)
#define SDRAM_ENABLE_bm (1<<0)

#define SDRAM_DMA_CTRL *((uint32_t volatile *)0xD0000014)
#define SDRAM_DMA_CHOKED_bm (1<<1)
#define SDRAM_DMA_CHOKE_ENABLE_bm (1<<2)
#define SDRAM_DMA_UNCHOKE_bm (1<<3)
#define SDRAM_DMA_CHOKE_bm (1<<4)
#define SDRAM_DMA_CLEAR_MARKER_STATE_bm (1<<5)
#define SDRAM_DMA_CHOKE_ADDR_bm 0xff00
#define SDRAM_DMA_CHOKE_ADDR_gp 8

#define SDRAM_DMA_RDADDR *((uint32_t volatile *)0xD0000020)
#define SDRAM_DMA_RDSTATUS *((uint32_t volatile *)0xD0000024)
#define SDRAM_DMA_WRADDR *((uint32_t volatile *)0xD0000028)
#define SDRAM_DMA_WRSTATUS *((uint32_t volatile *)0xD000002C)
#define SDRAM_DMA_SFADDR *((uint32_t volatile *)0xD0000030)
#define SDRAM_DMA_CURRENT_MARKER *((uint64_t volatile *)0xD0000038)
#define SDRAM_DMA_START_MARKER *((uint64_t volatile *)0xD0000040)
#define SDRAM_DMA_STOP_MARKER *((uint64_t volatile *)0xD0000048)
#define SDRAM_DMA_MARKER_IDX_bm 0x3FFFFFFFFFFF

#define SDRAM_DMA_ENABLED_bm (1<<0)
#define SDRAM_DMA_BUF_EMPTY_bm (1<<1)
#define SDRAM_DMA_BUF_BUSY_bm (1<<2)

#define SDRAM_DMA_SFADDR_PTR_bm 0xffffff
#define SDRAM_DMA_SFADDR_BUSY_bm (1<<24)

#define SDRAM ((uint32_t volatile *)0xD1000000)

#define SAMPLER_CTRL *((uint32_t volatile *)0xE0000000)
#define SAMPLER_STATUS *((uint32_t const volatile *)0xE0000000)
#define SAMPLER_TMR_PER *((uint32_t volatile *)0xE0000004)
#define SAMPLER_MUX1 *((uint32_t volatile *)0xE0000010)
#define SAMPLER_MUX2 *((uint32_t volatile *)0xE0000014)
#define SAMPLER_MUX3 *((uint32_t volatile *)0xE0000018)
#define SAMPLER_COMPRESSOR_STATE *((uint32_t volatile *)0xE0000020)
#define SAMPLER_SERIALIZER_STATE *((uint32_t volatile *)0xE0000024)
#define SAMPLER_SRC_SAMPLE_INDEX_LO *((uint32_t volatile *)0xE0000028)
#define SAMPLER_SRC_SAMPLE_INDEX_HI *((uint32_t volatile *)0xE000002C)

#define SAMPLER_ENABLE_bm (1<<0)
#define SAMPLER_CLEAR_PIPELINE_bm (1<<1)
#define SAMPLER_SET_MONITOR_bm (1<<2)
#define SAMPLER_COMPRESSOR_MONITOR_bm (1<<3)
#define SAMPLER_OVERFLOW_bm (1<<4)
#define SAMPLER_PIPELINE_BUSY_bm (1<<5)
#define SAMPLER_LOG_CHANNELS_gp 8
#define SAMPLER_LOG_CHANNELS_bm (0x700)
#define SAMPLER_EDGE_CTRL_gp 16
#define SAMPLER_EDGE_CTRL_gm 0x30000
#define SAMPLER_RISING_EDGE_bm (1<<16)
#define SAMPLER_FALLING_EDGE_bm (1<<17)

#define PIC_STATUS *((uint32_t const volatile *)0xC0000050)
#define PIC_CTRL *((uint8_t volatile *)0xC0000050)
#define PIC_MCLR_bm (1<<0)
#define PIC_OUT_bm (1<<1)
#define PIC_BUSY_bm (1<<2)
#define PIC_CNT *((uint8_t volatile *)0xC0000051)
#define PIC_SHIFT *((uint32_t volatile *)0xC0000054)

#define AD0 *((uint16_t volatile *)0xC0000058)

#define VOUT0 *((uint16_t volatile *)0xC000005C)
#define VOUT_VALUE_bm 0x03
#define VOUT_VALID_bm 0x100
#define VOUT0_MIN_THRES *((uint8_t volatile *)0xC000005E)
#define VOUT0_MAX_THRES *((uint8_t volatile *)0xC000005F)

struct timer_t
{
	typedef uint32_t value_type;
	typedef uint32_t time_type;
	static uint8_t const value_bits = 32;

	static value_type value()
	{
		return TMR;
	}
} timer;

template <typename T>
T load_le(uint8_t const * p, uint8_t size = sizeof(T))
{
	T res = 0;
	p += size;
	while (size--)
		res = (res << 8) | *--p;
	return res;
}

template <typename T>
uint8_t * store_le(uint8_t * p, T v, uint8_t size = sizeof(T))
{
	while (size--)
	{
		*p++ = (uint8_t)v;
		v = v >> 8;
	}

	return p;
}

bool usb_dbg_enabled = false;

static bool rxready()
{
	return usb_dbg_enabled && (USB_EP1_OUT_STATUS & USB_EP_EMPTY) == 0;
}

uint8_t usb_dbg_pos = 0;

static uint8_t recv()
{
	while (!rxready())
	{
	}

	uint8_t ch = USB_EP1_OUT[usb_dbg_pos++];
	if (usb_dbg_pos == USB_EP1_OUT_CNT)
	{
		USB_EP1_OUT_CTRL = USB_EP_PULL | USB_EP_PUSH;
		usb_dbg_pos = 0;
	}

	return ch;
}

uint8_t usb_dbg_tx_pos = 0;

static void send_range(char const * first, char const * last)
{
	if (!usb_dbg_enabled)
		return;

	while (first != last)
	{
		while (USB_EP1_IN_STATUS & USB_EP_FULL)
		{
		}

		while (first != last && usb_dbg_tx_pos < 64)
			USB_EP1_IN[usb_dbg_tx_pos++] = *first++;

		if ((USB_EP1_IN_STATUS & USB_EP_EMPTY) || usb_dbg_tx_pos == 64)
		{
			USB_EP1_IN_CNT = usb_dbg_tx_pos;
			USB_EP1_IN_CTRL = USB_EP_PUSH;
			usb_dbg_tx_pos = 0;
		}
	}
}

static void sendch(char ch)
{
	send_range(&ch, &ch + 1);
}

static void send(char const * str)
{
	char const * end = str;
	while (*end != 0)
		++end;
	send_range(str, end);
}

template <typename T>
static void sendhex(T n)
{
	uint8_t const * p = (uint8_t const *)&n;
	uint8_t const * pend = (uint8_t const *)&n + sizeof n;
	while (p != pend)
	{
		--pend;

		static char const digits[] = "0123456789abcdef";
		sendch(digits[*pend >> 4]);
		sendch(digits[*pend & 0xf]);
	}
}

template <typename T>
static void sendhex(char *& res, T n)
{
	uint8_t const * p = (uint8_t const *)&n;
	uint8_t const * pend = (uint8_t const *)&n + sizeof n;
	while (p != pend)
	{
		--pend;

		static char const digits[] = "0123456789abcdef";
		*res++ = digits[*pend >> 4];
		*res++ = digits[*pend & 0xf];
	}
}

#define assert(x) do { if (!(x)) assertion_failed(__LINE__); } while(0)
int g_assert_lineno = 0;
static void assertion_failed(int lineno)
{
	g_assert_lineno = lineno;
	LEDBITS |= 1;
}

static void spi_begin()
{
	SPI_CTRL = 0;
}

static void spi_end()
{
	SPI_CTRL = (1<<0);
}

static uint8_t spi(uint8_t v = 0)
{
	SPI_DATA = v;
	while (SPI_CTRL & (1<<1))
	{
	}
	return SPI_DATA;
}

struct usb_ctrl_req_t
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;

	char ctx_buf[8];

	template <typename T>
	T & ctx()
	{
		return *reinterpret_cast<T *>(ctx_buf);
	}

	uint16_t cmd() const
	{
		return (bmRequestType << 8) | bRequest;
	}

	bool is_write() const
	{
		return (bmRequestType & 0x80) == 0;
	}

	bool is_read() const
	{
		return (bmRequestType & 0x80) != 0;
	}
};

class usb_writer_t
{
public:
	usb_writer_t()
	{
	}

	uint8_t * alloc_packet();
	void commit_packet(uint8_t size);
};

class usb_control_handler
{
public:
	virtual bool on_data_out(usb_ctrl_req_t & req, uint8_t const * p, uint8_t len) = 0;
	virtual bool on_data_out_done(usb_ctrl_req_t & req) = 0;
	virtual uint8_t on_data_in(usb_ctrl_req_t & req, uint8_t * p) = 0;
	virtual void commit_write(usb_ctrl_req_t & req) = 0;
};


struct pic_flash_handler
{
	pic_flash_handler()
		: m_status(st_app), m_time_base(0), m_current_address(0)
	{
	}

	bool is_busy() const
	{
		return m_status == st_init || m_status == st_busy_flash || m_status == st_busy_config;
	}

	uint32_t busy_time() const
	{
		uint32_t res;

		switch (m_status)
		{
		case st_init:
			res = 250;
			break;
		case st_busy_flash:
			res = 2500;
			break;
		case st_busy_config:
			res = 5000;
			break;
		default:
			res = 0;
		}

		uint32_t tmr = TMR;
		if (tmr - m_time_base < res)
			res = res - (tmr - m_time_base);
		else
			res = 0;
		return res;
	}

	void process()
	{
		switch (m_status)
		{
		case st_app:
		case st_idle:
			break;
		case st_init:
			if (TMR - m_time_base > 250)
			{
				this->write(0x4D434850, 33);
				m_current_address = 0;
				m_status = st_idle;
			}
			break;
		case st_busy_flash:
			if (TMR - m_time_base > 2500)
				m_status = st_idle;
			break;
		case st_busy_config:
			if (TMR - m_time_base > 5000)
				m_status = st_idle;
			break;
		}
	}

	void enter()
	{
		if (m_status == st_app)
		{
			PIC_CTRL = PIC_MCLR_bm;
			m_status = st_init;
			m_time_base = TMR;
		}
	}

	void program_block(uint16_t addr, uint16_t const * data, uint8_t size)
	{
		if (size == 0)
			return;

		assert((addr & 0x2000) == 0);
		assert((addr & 0xfff0) == ((addr + size - 1) & 0xfff0));

		this->seek(addr);

		uint16_t const * last = data + size;
		while (data != last)
		{
			this->write(0x02, 6);
			this->write(*data++ << 1, 16);

			if (data != last)
				this->increment_address();
		}

		this->write(0x08, 6);
		m_time_base = TMR;
		m_status = st_busy_flash;
	}

	void program_config(uint16_t addr, uint16_t value)
	{
		this->seek(addr);
		this->write(0x02, 6);
		this->write(value << 1, 16);
		this->write(0x08, 6);
		m_time_base = TMR;
		m_status = st_busy_config;
	}

	void increment_address()
	{
		this->write(0x06, 6);
		++m_current_address;
	}

	void bulk_erase()
	{
		this->write(0x09, 6);
		m_time_base = TMR;
		m_status = st_busy_config;
	}

	void seek(uint16_t addr)
	{
		addr &= 0x3fff;

		if ((m_current_address & 0x2000) != (addr & 0x2000) || m_current_address > addr)
		{
			if (addr & 0x2000)
			{
				this->write(0x00, 6);
				this->write(0x3fff << 1, 16);
				m_current_address = 0x2000;
			}
			else
			{
				this->write(0x16, 6);
				m_current_address = 0x0000;
			}
		}

		while (m_current_address < addr)
			this->increment_address();
	}

	void read(uint8_t * buf, uint8_t size)
	{
		uint8_t * end = buf + size;
		while (buf != end)
		{
			this->write(0x04, 6);
			uint16_t data = (this->read(16) >> 1) & 0x3fff;
			*buf++ = data;
			if (buf != end)
				*buf++ = data >> 8;
			if (buf != end)
				this->increment_address();
		}
	}

	void read(uint16_t addr, uint8_t * buf, uint8_t size)
	{
		this->seek(addr);
		this->read(buf, size);
	}

	void exit()
	{
		PIC_CTRL = 0;
		m_status = st_app;
	}

private:
	uint16_t read(uint8_t len)
	{
		PIC_SHIFT = 0xFFFFFFFF;
		PIC_CNT = len;
		while (PIC_STATUS & PIC_BUSY_bm)
		{
		}
		wait(timer, 1);
		return PIC_SHIFT >> (32 - len);
	}

	void write(uint32_t shift, uint8_t len)
	{
		PIC_SHIFT = shift;
		PIC_CTRL = PIC_MCLR_bm | PIC_OUT_bm;
		PIC_CNT = len;
		while (PIC_STATUS & PIC_BUSY_bm)
		{
		}
		wait(timer, 1);
		PIC_CTRL = PIC_MCLR_bm;
	}

	enum state_t { st_app, st_init, st_idle, st_busy_flash, st_busy_config } m_status;

	uint32_t m_time_base;
	uint16_t m_current_address;
};

class dfu_handler
	: public usb_control_handler
{
public:
	explicit dfu_handler(pic_flash_handler & pfh)
		: m_state(app_idle), m_status(err_ok),
		  m_needs_manifest(nm_no),
		  m_detach_time_base(0), m_detach_time_size(0), m_getstatus_poll(0), m_pfh(pfh)
	{
		spi_begin();
		spi(0x9F);
		spi(0x00);
		spi(0x00);
		m_flash_size = spi(0x00);
		spi_end();

		if (m_flash_size < 31)
			m_flash_size = (1<<m_flash_size);
	}

	void on_usb_reset()
	{
		if (TMR - m_detach_time_base < m_detach_time_size)
		{
			m_state = dfu_idle;
			m_pfh.enter();
		}
		else
		{
			if (m_needs_manifest == nm_flash_committed)
				reconfigure();

			m_state = app_idle;
			m_pfh.exit();
		}
		m_status = err_ok;
	}

	void commit_write(usb_ctrl_req_t & req)
	{
	}

	bool on_control_transfer(usb_ctrl_req_t & req)
	{
		switch (req.cmd())
		{
		case cmd_getstate:
			if (this->is_dfu_mode())
				return true;
			break;
		case cmd_upload:
			if (m_state == dfu_idle || m_state == dfu_upload_idle)
			{
				if (m_state == dfu_idle)
				{
					m_upload_state = us_yb_header;
					m_block_offset = 0;
					m_state = dfu_upload_idle;
				}
				return true;
			}
			break;
		case cmd_detach:
			if (req.wLength == 0 && m_state == app_idle)
			{
				m_detach_time_base = TMR;
				m_detach_time_size = req.wValue * 1000;
				return true;
			}
			break;
		case cmd_dnload:
			if (req.wLength <= 256
				&& (m_state == dfu_idle || m_state == dfu_dnload_idle))
			{
				m_detach_time_size = 0;

				if (m_state == dfu_idle)
					this->dnload_init();

				m_state = dfu_dnload_sync;
				if (req.wLength == 0)
				{
					status_t status = this->dnload_finalize();
					if (status != err_ok)
					{
						m_state = dfu_error;
						m_status = status;
					}
					else
					{
						m_state = dfu_manifest_sync;
					}
				}

				return true;
			}
			break;
		case cmd_clrstatus:
			if (this->is_dfu_mode() && req.wLength == 0 && m_state == dfu_error)
			{
				m_status = err_ok;
				m_state = dfu_idle;
				return true;
			}
			break;
		case cmd_abort:
			if (req.wLength == 0
				&& (m_state == dfu_idle || m_state == dfu_dnload_sync || m_state == dfu_dnload_idle || m_state == dfu_manifest_sync || m_state == dfu_upload_idle))
			{
				m_status = err_ok;
				m_state = dfu_idle;
				return true;
			}
			break;
		case cmd_getstatus:
			if (this->is_dfu_mode() && req.wLength >= 6)
			{
				if (m_state == dfu_dnload_sync)
				{
					if (m_download_state != ds_flash_erase_wait && m_download_state != ds_flash_wait
							&& m_download_state != ds_pic_wait)
						m_state = dfu_dnload_idle;
				}
				else if (m_state == dfu_manifest_sync)
				{
					m_state = dfu_idle;
					if (m_needs_manifest == nm_flash_modified)
						m_needs_manifest = nm_flash_committed;
				}

				return true;
			}
			break;
		}

		if (m_state >= dfu_idle)
			m_state = dfu_error;
		m_status = err_stalledpkt;
		return false;
	}

	bool on_data_out_done(usb_ctrl_req_t & req)
	{
		return true;
	}

	bool on_data_out(usb_ctrl_req_t & req, uint8_t const * p, uint8_t len)
	{
		switch (req.cmd())
		{
		case cmd_dnload:
			m_chbuf.push(p, len);
			this->process();
			break;
		}

		return true;
	}

	void process()
	{
		if (TMR - m_getstatus_pollbase >= m_getstatus_poll)
			m_getstatus_poll = 0;

		if (m_state == dfu_dnload_sync || m_state == dfu_dnload_idle)
		{
			status_t status = this->dnload_process();
			if (status != err_notdone && status != err_ok)
			{
				m_state = dfu_error;
				m_status = status;
			}
		}
	}

	uint8_t on_data_in(usb_ctrl_req_t & req, uint8_t * p)
	{
		switch (req.cmd())
		{
		case cmd_getstate:
			p[0] = m_state;
			return 1;
		case cmd_getstatus:
		{
			p[0] = m_status;
			uint32_t poll_timeout = 0;
			uint32_t tmr = TMR;
			if (tmr - m_getstatus_pollbase < m_getstatus_poll)
			{
				tmr = m_getstatus_poll - (tmr - m_getstatus_pollbase);
				poll_timeout = tmr / 1000;
			}
			p[1] = (uint8_t)poll_timeout;
			p[2] = (uint8_t)(poll_timeout >> 8);
			p[3] = (uint8_t)(poll_timeout >> 16);
			p[4] = (m_state == dfu_dnload_sync? dfu_dnbusy: m_state);
			p[5] = 0;
			return 6;
		}
		case cmd_upload:
			{
				static uint16_t const pic_pgm_size = 512;

				switch (m_upload_state)
				{
				case us_yb_header:
					memcpy(p, usb_descriptor_data + version_info_offset, version_info_length);
					memset(p + version_info_length, 0, 64 - version_info_length);
					m_upload_state = us_pic_config_header;
					return 64;
				case us_pic_config_header:
					p = store_le<uint32_t>(p, 2);
					p = store_le<uint32_t>(p, 0x14);
					p = store_le<uint32_t>(p, 0x4000);
					p = store_le<uint32_t>(p, 0x01);
					memset(p, 0, 64-16);
					m_upload_state = us_pic_config;
					return 64;
				case us_pic_config:
					m_pfh.read(0x2000, p, 0x14);
					memset(p + 0x14, 0, 64-0x14);
					m_upload_state = us_pic_pgm_header;
					return 64;
				case us_pic_pgm_header:
					p = store_le<uint32_t>(p, 2);
					p = store_le<uint32_t>(p, pic_pgm_size);
					p = store_le<uint32_t>(p, 0);
					memset(p, 0, 64-12);
					m_pfh.seek(0x0000);
					m_upload_state = us_pic_pgm_data;
					m_block_offset = 0;
					return 64;
				case us_pic_pgm_data:
					m_pfh.read(p, 64);
					m_block_offset += 64;
					if (m_block_offset == pic_pgm_size)
						m_upload_state = us_flash_header;
					return 64;
				case us_flash_header:
					p = store_le<uint32_t>(p, 1);
					p = store_le<uint32_t>(p, m_flash_size);
					memset(p, 0, 64-8);
					m_upload_state = us_flash;
					m_block_offset = 0;
					return 64;
				case us_flash:
					{
						uint8_t chunk = req.wLength > 64? 64: req.wLength;

						uint32_t rem = m_flash_size - m_block_offset;
						if (chunk > rem)
							chunk = rem;

						spi_begin();
						spi(0x0B);
						spi(m_block_offset >> 16);
						spi(m_block_offset >> 8);
						spi(m_block_offset);
						spi(0x00);
						for (uint32_t i = 0; i < chunk; ++i)
							*p++ = spi(0x00);
						spi_end();

						m_block_offset += chunk;
						if (chunk < 64)
							m_state = dfu_idle;
						return chunk;
					}
				}

				return 0;
			}
		}

		return 0;
	}

	bool is_dfu_mode() const
	{
		return m_state >= dfu_idle;
	}

	void reconfigure()
	{
		static uint16_t const reset_seq[] = {
			0xFFFF, 0xAA99, 0x5566,
			0x3261, 0x0000,
			0x3281, 0x0000,
			0x32A1, 0x0000,
			0x32C1, 0x0000,
			0x30A1, 0x000E,
			0x2000,
		};

		ICAP_CTRL = ICAP_CS_bm;
		for (size_t i = 0; i < sizeof reset_seq / sizeof reset_seq[0]; ++i)
		{
			ICAP_DATA = reset_seq[i];
			while (ICAP_DATA & ICAP_BUSY_bm)
			{
			}
		}
		ICAP_CTRL = ICAP_RUN_bm;
	}

private:
	enum status_t
	{
		err_ok = 0x00,
		err_target = 0x01,
		err_file = 0x02,
		err_write = 0x03,
		err_erase = 0x04,
		err_check_erased = 0x05,
		err_prog = 0x06,
		err_verify = 0x07,
		err_address = 0x08,
		err_notdone = 0x09,
		err_firmware = 0x0a,
		err_vendor = 0x0b,
		err_usbr = 0x0c,
		err_por = 0x0d,
		err_unknown = 0x0e,
		err_stalledpkt = 0x0f,
	};

	void flash_write_enable()
	{
		spi_begin();
		spi(0x06);
		spi_end();
	}

	void flash_erase(uint8_t sector_addr)
	{
		this->flash_write_enable();

		spi_begin();
		spi(0xD8); // ERASE_SECTOR
		spi(sector_addr);
		spi(0);
		spi(0);
		spi_end();

		m_getstatus_pollbase = TMR;
		m_getstatus_poll = 600000;
	}

	void dnload_init()
	{
		m_chbuf.clear();
		m_download_state = ds_yb_header;
	}

	status_t dnload_process()
	{
		for (;;)
		{
			switch (m_download_state)
			{
			case ds_yb_header:
				if (uint8_t const * p = m_chbuf.pop64())
				{
					if (memcmp(p, usb_descriptor_data + version_info_offset, 17) != 0)
						return err_firmware;

					m_download_state = ds_block_header;
					break;
				}

				return err_notdone;
			case ds_block_header:
				if (uint8_t const * p = m_chbuf.pop64())
				{
					uint32_t block_type = load_le<uint32_t>(p);
					m_block_size = load_le<uint32_t>(p + 4);
					m_block_offset = load_le<uint32_t>(p + 8);
					uint32_t flags = load_le<uint32_t>(p + 12);

					switch (block_type)
					{
					case 0:
						m_download_state = ds_pad;
						if (m_block_offset != 0 || m_block_size == 0 || m_block_size % 64 != 0 || flags != 0)
							return err_firmware;
						break;
					case 1:
						if (m_block_offset % 0x10000 != 0 || m_block_size == 0 || flags != 0)
							return err_firmware;
						this->flash_erase(m_block_offset >> 16);
						m_download_state = ds_flash_erase_wait;
						m_needs_manifest = nm_flash_modified;
						return err_notdone;
					case 2:
						if (m_block_offset % 2 != 0 || m_block_offset > 0x8000 || (flags & ~(uint32_t)0x01))
							return err_firmware;
						if (m_block_offset >= 0x4000 && (m_block_size > 0x14 || m_block_offset + m_block_size > 0x4014))
							return err_firmware;
						if (m_block_offset < 0x4000 && (m_block_size > 0x200 || m_block_offset + m_block_size > 0x200 || m_block_offset % 32 != 0))
							return err_firmware;

						m_pfh.seek(m_block_offset / 2);
						if (flags & 0x01)
							m_pfh.bulk_erase();
						m_subblock_offset = 0;
						m_download_state = ds_pic_wait;
						return err_notdone;
					default:
						return err_firmware;
					}

					break;
				}
				return err_notdone;
			case ds_pad:
				if (m_chbuf.pop64())
				{
					if (m_block_size > 64)
						m_block_size -= 64;
					else
						m_download_state = ds_block_header;
					break;
				}
				return err_notdone;
			case ds_flash_erase_wait:
				{
					spi_begin();
					spi(0x05); // GET_STATUS
					uint8_t status = spi(0x00);
					spi_end();

					if ((status & 1) == 0)
					{
						m_download_state = ds_flash;
						break;
					}
				}
				return err_notdone;
			case ds_flash:
				{
					size_t chunk = m_block_size > 256? 256: m_block_size;
					if (m_chbuf.size64() >= chunk)
					{
						this->flash_write_enable();

						spi_begin();

						spi(0x02); // WRITE_MEMORY
						spi(m_block_offset >> 16);
						spi(m_block_offset >> 8);
						spi(0);

						m_block_offset += chunk;
						m_block_size -= chunk;

						while (chunk)
						{
							uint8_t const * p = m_chbuf.pop64();
							assert(p != 0);

							uint8_t subchunk = chunk > 64? 64: chunk;
							uint8_t const * last = p + subchunk;
							chunk -= subchunk;
							while (p != last)
								spi(*p++);
						}
						spi_end();

						m_download_state = ds_flash_wait;
						break;
					}
				}
				return err_notdone;
			case ds_flash_wait:
				{
					spi_begin();
					spi(0x05); // GET_STATUS
					uint8_t status = spi(0x00);
					spi_end();

					if ((status & 1) == 0)
					{
						if (m_block_size == 0)
						{
							m_download_state = ds_block_header;
						}
						else if (m_block_offset % 0x10000 == 0)
						{
							this->flash_erase(m_block_offset >> 16);
							m_download_state = ds_flash_erase_wait;
						}
						else
						{
							m_download_state = ds_flash;
						}
						break;
					}
				}
				return err_notdone;
			case ds_pic:
				if (m_block_size == 0)
				{
					m_download_state = ds_block_header;
					break;
				}

				if (uint8_t const * p = m_chbuf.data64())
				{
					if (m_block_offset >= 0x4000)
					{
						if (m_block_offset == 0x4000 || m_block_offset == 0x4002
							|| m_block_offset == 0x4004 || m_block_offset == 0x4006
							|| m_block_offset == 0x400e)
						{
							m_pfh.program_config(m_block_offset / 2, load_le<uint16_t>(p + m_subblock_offset));
							m_download_state = ds_pic_wait;
						}

						m_subblock_offset += 2;
						m_block_offset += 2;
						m_block_size -= 2;
						if (m_block_size == 0)
							m_chbuf.pop64();
					}
					else
					{
						assert(m_block_offset % 32 == 0);
						uint8_t chunk = m_block_size > 32? 32: m_block_size;
						m_pfh.program_block(m_block_offset / 2, (uint16_t const *)(p + m_subblock_offset), chunk / 2);
						m_download_state = ds_pic_wait;

						m_subblock_offset += chunk;
						m_block_offset += chunk;
						m_block_size -= chunk;

						if (m_subblock_offset == 64 || m_block_size == 0)
						{
							m_subblock_offset = 0;
							m_chbuf.pop64();
						}
					}

					break;
				}
				return err_notdone;
			case ds_pic_wait:
				if (!m_pfh.is_busy())
				{
					m_download_state = ds_pic;
					break;
				}
				return err_notdone;
			default:
				return err_unknown;
			}
		}
	}

	status_t dnload_finalize()
	{
		if (m_download_state != ds_block_header)
			return err_notdone;

		if (!m_chbuf.empty())
			return err_firmware;

		return err_ok;
	}

	enum
	{
		cmd_detach = 0x2100,
		cmd_dnload = 0x2101,
		cmd_upload = 0xa102,
		cmd_getstatus = 0xa103,
		cmd_clrstatus = 0x2104,
		cmd_getstate = 0xa105,
		cmd_abort = 0x2106,
	};

	enum state_t
	{
		app_idle,
		//app_detach,
		dfu_idle = 2,
		dfu_dnload_sync,
		dfu_dnbusy,
		dfu_dnload_idle,
		dfu_manifest_sync,
		dfu_manifest,
		dfu_manifest_wait_reset,
		dfu_upload_idle,
		dfu_error
	};

	state_t m_state;
	status_t m_status;
	enum { nm_no, nm_flash_modified, nm_flash_committed } m_needs_manifest;
	uint32_t m_detach_time_base;
	uint32_t m_detach_time_size;

	enum {
		us_yb_header,
		us_pic_config_header, us_pic_config, us_pic_pgm_header, us_pic_pgm_data,
		us_flash_header, us_flash
	} m_upload_state;

	enum {
		ds_yb_header,
		ds_block_header,
		ds_pad,
		ds_flash_erase_wait,
		ds_flash,
		ds_flash_wait,
		ds_pic,
		ds_pic_wait,
	} m_download_state;

	uint32_t m_block_offset;
	uint32_t m_block_size;
	uint32_t m_subblock_offset;

	class chunked_buffer
	{
	public:
		static const size_t capacity = 256;

		chunked_buffer()
			: m_rd(0), m_wr(0), m_size(0)
		{
		}

		void push(uint8_t const * p, size_t len)
		{
			assert(m_size + len <= capacity);

			if (m_wr + len > capacity)
			{
				size_t chunk = capacity - m_wr;
				memcpy(m_buffer + m_wr, p, chunk);
				m_wr = len - chunk;
				memcpy(m_buffer, p + chunk, m_wr);
			}
			else
			{
				memcpy(m_buffer + m_wr, p, len);
				m_wr += len;
			}

			m_size += len;
		}

		uint8_t const * pop64()
		{
			if (m_size >= 64)
			{
				uint8_t const * res = m_buffer + m_rd;

				m_rd += 64;
				if (m_rd >= capacity)
					m_rd -= capacity;
				m_size -= 64;

				return res;
			}
			else
			{
				return 0;
			}
		}

		uint8_t const * data64() const
		{
			if (m_size >= 64)
				return m_buffer + m_rd;
			else
				return 0;
		}

		size_t const size64() const
		{
			return m_size & ~(size_t)63;
		}

		bool empty() const
		{
			return m_size == 0;
		}

		void clear()
		{
			m_size = 0;
			m_rd = 0;
			m_wr = 0;
		}

	private:
		uint8_t m_buffer[capacity];
		size_t m_rd;
		size_t m_wr;
		size_t m_size;
	};

	chunked_buffer m_chbuf;

	uint32_t m_getstatus_pollbase;
	uint32_t m_getstatus_poll;

	uint32_t m_flash_size;

	pic_flash_handler & m_pfh;
};

class usb_core_handler
	: public usb_control_handler
{
public:
	usb_core_handler()
		: m_config(0), m_descriptor_set(0)
	{
		while ((DNA & DNA_READY_bm) == 0)
		{
		}

		uint64_t dna = DNA;
		uint8_t const * p = (uint8_t const *)&dna;

		uint8_t * sn = m_sn;
		*sn++ = 30;
		*sn++ = 3;

		for (uint8_t i = 7; i != 0; --i)
		{
			static char const digits[] = "0123456789abcdef";
			uint8_t d = p[i-1];
			*sn++ = digits[d >> 4];
			*sn++ = 0;
			*sn++ = digits[d & 0xf];
			*sn++ = 0;
		}
	}

	void on_usb_reset()
	{
		m_config = 0;
		usb_dbg_enabled = false;
	}

	void commit_write(usb_ctrl_req_t & req)
	{
		switch (req.cmd())
		{
		case cmd_set_address:
			USB_ADDRESS = req.wValue;
			break;
		}
	}

	bool on_control_transfer(usb_ctrl_req_t & req)
	{
		switch (req.cmd())
		{
		case cmd_get_descriptor: // get_descriptor
			if (req.wValue != 0x302)
			{
				uint8_t entry_first = usb_descriptor_set[m_descriptor_set].first;
				uint8_t entry_last = usb_descriptor_set[m_descriptor_set].last;

				usb_descriptor_entry_t const * selected = 0;
				for (size_t i = entry_first; !selected && i < entry_last; ++i)
				{
					if (usb_descriptor_map[i].index == req.wValue)
						selected = &usb_descriptor_map[i];
				}

				if (!selected)
					return false;

				uint16_t size = selected->size;
				if (size > req.wLength)
					size = req.wLength;

				get_desc_ctx_t & ctx = req.ctx<get_desc_ctx_t>();
				ctx.first = usb_descriptor_data + selected->offset;
				ctx.last = ctx.first + size;
			}
			else
			{
				uint16_t size = sizeof m_sn;
				if (size > req.wLength)
					size = req.wLength;

				get_desc_ctx_t & ctx = req.ctx<get_desc_ctx_t>();
				ctx.first = m_sn;
				ctx.last = m_sn + size;
			}
			return true;
		case cmd_set_address:
			return true;
		case cmd_get_config:
			return true;
		case cmd_set_config:
			if (req.wValue < 2)
			{
				m_config = (uint8_t)req.wValue;
				usb_dbg_enabled = (m_config == 1);
				return true;
			}
			break;
		}

		return false;
	}

	bool on_data_out(usb_ctrl_req_t & req, uint8_t const * p, uint8_t len)
	{
		return false;
	}

	bool on_data_out_done(usb_ctrl_req_t & req)
	{
		return false;
	}

	uint8_t on_data_in(usb_ctrl_req_t & req, uint8_t * p)
	{
		switch (req.cmd())
		{
		case cmd_get_descriptor:
			{
				get_desc_ctx_t & ctx = req.ctx<get_desc_ctx_t>();
				uint16_t chunk = ctx.last - ctx.first;
				if (chunk > 64)
					chunk = 64;
				memcpy(p, ctx.first, chunk);
				ctx.first += chunk;
				return chunk;
			}

		case cmd_get_config:
			p[0] = m_config;
			return 1;
		}

		return 0;
	}

	void set_desc_set(uint8_t ds)
	{
		m_descriptor_set = ds;
	}

private:
	enum
	{
		cmd_set_address = 0x0005,
		cmd_get_descriptor = 0x8006,
		cmd_get_config = 0x8008,
		cmd_set_config = 0x0009,
	};

	struct get_desc_ctx_t
	{
		uint8_t const * first;
		uint8_t const * last;
	};

	uint8_t m_config;
	uint8_t m_descriptor_set;
	uint8_t m_sn[30];
};

static uint8_t const g_min_voltage_thres[3] = { 0x00, 0x8A, 0xD7 };
static uint8_t const g_max_voltage_thres[3] = { 0x0E, 0xA5, 0xFF };

class usb_omicron_handler
	: public usb_control_handler
{
public:
	usb_omicron_handler()
		: m_running(false), m_start_src_index(0), m_start_recv_index(0)
	{
		m_voltage_timeout.init_stopped(timer, 100000);
	}

	void on_usb_reset()
	{
	}

	void commit_write(usb_ctrl_req_t & req)
	{
	}

	bool on_control_transfer(usb_ctrl_req_t & req)
	{
		switch (req.cmd())
		{
		case cmd_set_wraddr:
		case cmd_set_rdaddr:
			if (req.wLength == 4)
				return true;
			break;
		case cmd_start:
			if (req.wLength == 18)
			{
				if (m_running)
					this->stop();
				return true;
			}
			break;
		case cmd_stop:
			if (req.wLength == 0)
			{
				this->stop();
				return true;
			}
			break;
		case cmd_get_sample_index:
			if (req.wLength >= 12)
				return true;
			break;
		case cmd_get_config:
			return true;
		case cmd_read_voltage:
			return true;
		case cmd_unchoke:
			{
				uint32_t ctrl = SDRAM_DMA_CTRL;
				if (!(SDRAM_DMA_CTRL & SDRAM_DMA_CHOKED_bm))
					return false;

				uint8_t choke_addr = (ctrl & SDRAM_DMA_CHOKE_ADDR_bm) >> SDRAM_DMA_CHOKE_ADDR_gp;
				uint32_t sfaddr = SDRAM_DMA_SFADDR & SDRAM_DMA_SFADDR_PTR_bm;

				if ((sfaddr >> 16) == choke_addr)
					return false;

				return true;
			}
		case cmd_move_choke:
			if (req.wLength == 4)
				return true;
			break;
		case cmd_set_voltage:
			if (req.wLength == 1)
				return true;
			break;
		}

		return false;
	}

	bool on_data_out_done(usb_ctrl_req_t & req)
	{
		return true;
	}

	bool on_data_out(usb_ctrl_req_t & req, uint8_t const * p, uint8_t len)
	{
		switch (req.cmd())
		{
		case cmd_set_wraddr:
			USB_EP3_OUT_CTRL = USB_EP_PAUSE_SET;

			while (!(SDRAM_DMA_WRSTATUS & SDRAM_DMA_BUF_EMPTY_bm))
			{
			}

			SDRAM_DMA_WRADDR = load_le<uint32_t>(p);
			USB_EP3_OUT_CTRL = USB_EP_PAUSE_CLR;
			return true;
		case cmd_set_rdaddr:
			USB_EP3_IN_CTRL = USB_EP_PAUSE_SET;
			SDRAM_DMA_RDSTATUS = 0;

			while (USB_EP3_IN_STATUS & USB_EP_TRANSIT_bm)
			{
			}

			while (SDRAM_DMA_RDSTATUS & SDRAM_DMA_BUF_BUSY_bm)
			{
			}

			while (!(USB_EP3_IN_STATUS & USB_EP_EMPTY))
			{
				USB_EP3_IN_CTRL = USB_EP_PULL;
			}

			SDRAM_DMA_RDADDR = load_le<uint32_t>(p);

			SDRAM_DMA_RDSTATUS = SDRAM_DMA_ENABLED_bm;
			USB_EP3_IN_CTRL = USB_EP_PAUSE_CLR;
			return true;
		case cmd_start:
			assert(!m_running);
			if (*p > 4)
				return false;

			SAMPLER_MUX1 = load_le<uint32_t>(p + 6);
			SAMPLER_MUX2 = load_le<uint32_t>(p + 10);
			SAMPLER_MUX3 = load_le<uint32_t>(p + 14);

			this->start(
				p[0],
				load_le<uint32_t>(p + 2),
				p[1]);
			return true;
		case cmd_move_choke:
			{
				uint32_t addr = load_le<uint32_t>(p);
				uint8_t new_choke = (addr + 0xff0000) >> 16;
				SDRAM_DMA_CTRL = (new_choke << SDRAM_DMA_CHOKE_ADDR_gp) | SDRAM_DMA_CHOKE_ENABLE_bm;
			}
			return true;
		case cmd_set_voltage:
			if (p[0] >= 3)
				return false;

			this->set_voltage(static_cast<voltage_t>(p[0]));
			return true;
		}

		return false;
	}

	void start(uint8_t shift, uint32_t tmr, uint8_t edge_ctrl)
	{
		assert(!m_running);

		m_serializer_shift = shift;
		m_start_src_index = SAMPLER_SRC_SAMPLE_INDEX_LO;
		m_start_src_index |= (uint64_t)SAMPLER_SRC_SAMPLE_INDEX_HI << 32;

		uint32_t start_sfaddr = (SDRAM_DMA_SFADDR & SDRAM_DMA_SFADDR_PTR_bm);
		m_start_recv_index = SDRAM_DMA_CURRENT_MARKER & SDRAM_DMA_MARKER_IDX_bm;

		uint8_t choke_addr = (start_sfaddr + 0xff0000) >> 16;
		SDRAM_DMA_CTRL = (choke_addr << SDRAM_DMA_CHOKE_ADDR_gp) | SDRAM_DMA_CHOKE_ENABLE_bm | SDRAM_DMA_CHOKE_bm | SDRAM_DMA_CLEAR_MARKER_STATE_bm;

		SAMPLER_TMR_PER = tmr;
		SAMPLER_CTRL = (edge_ctrl << SAMPLER_EDGE_CTRL_gp) | (shift << SAMPLER_LOG_CHANNELS_gp) | SAMPLER_CLEAR_PIPELINE_bm | SAMPLER_ENABLE_bm;

		m_running = true;
	}

	uint8_t on_data_in(usb_ctrl_req_t & req, uint8_t * p)
	{
		uint8_t * p_orig = p;
		switch (req.cmd())
		{
		case cmd_get_sample_index:
			return this->get_trail(p);
		case cmd_get_config:
			p = store_le<uint8_t>(p, (m_running? 0x80: 0) | m_serializer_shift);
			p = store_le<uint8_t>(p, (SAMPLER_CTRL & SAMPLER_EDGE_CTRL_gm) >> SAMPLER_EDGE_CTRL_gp);
			p = store_le(p, SAMPLER_TMR_PER);
			p = store_le(p, SAMPLER_MUX1);
			p = store_le(p, SAMPLER_MUX2);
			p = store_le(p, SAMPLER_MUX3);
			break;
		case cmd_read_voltage:
			{
				uint16_t v = AD0;
				if (v & 0x100)
					p = store_le<uint32_t>(p, v & 0xff);
			}
			return true;
		case cmd_unchoke:
			p = store_le(p, SDRAM_DMA_SFADDR & SDRAM_DMA_SFADDR_PTR_bm);
			SDRAM_DMA_CTRL |= SDRAM_DMA_UNCHOKE_bm;
			p = store_le(p, SDRAM_DMA_START_MARKER);
			break;
		}
		return p - p_orig;
	}

	uint8_t get_trail(uint8_t * p)
	{
		uint64_t src_idx = SAMPLER_SRC_SAMPLE_INDEX_LO;
		src_idx |= ((uint64_t)SAMPLER_SRC_SAMPLE_INDEX_HI<<32);
		src_idx -= m_start_src_index;

	restart:
		SAMPLER_CTRL = SAMPLER_CTRL | SAMPLER_SET_MONITOR_bm;
		uint32_t sfaddr = SDRAM_DMA_SFADDR;
		uint64_t recv_idx = SDRAM_DMA_CURRENT_MARKER & SDRAM_DMA_MARKER_IDX_bm;
		recv_idx -= m_start_recv_index;
		recv_idx <<= 4 - m_serializer_shift;

	restart2:
		uint8_t * cur = p;

		cur = store_le<uint32_t>(cur, sfaddr & SDRAM_DMA_SFADDR_PTR_bm);
		if (SDRAM_DMA_CTRL & SDRAM_DMA_CHOKED_bm)
		{
			cur = store_le<uint64_t>(cur, SDRAM_DMA_STOP_MARKER);
			return cur - p;
		}

		cur = store_le<uint64_t>(cur, recv_idx);

		if (recv_idx >= src_idx)
		{
			*cur++ = 0;
			*cur++ = 0;
			return cur - p;
		}

		if (sfaddr & SDRAM_DMA_SFADDR_BUSY_bm)
			goto restart;

		uint32_t compressor_state = SAMPLER_COMPRESSOR_STATE;
		uint32_t compressor_samples;
		switch ((compressor_state >> 16) & 0x3)
		{
		case 0:
		case 1:
			*cur++ = 0;
			compressor_samples = 0;
			break;
		case 2:
			compressor_samples = (compressor_state & 0xffff);
			*cur++ = 2;
			*cur++ = compressor_samples;
			*cur++ = compressor_samples >> 8;
			break;
		case 3:
			goto restart;
		}

		compressor_samples <<= 4 - m_serializer_shift;
		if (recv_idx + compressor_samples < src_idx)
		{
			uint32_t serializer_state = SAMPLER_SERIALIZER_STATE;
			if ((SAMPLER_STATUS & SAMPLER_COMPRESSOR_MONITOR_bm) == 0)
				goto restart;

			uint8_t ser_samples = (serializer_state >> 16) & 0xf;
			*cur++ = ser_samples;
			*cur++ = serializer_state;
			*cur++ = serializer_state >> 8;
		}
		else
		{
			*cur++ = 0;
		}

		SAMPLER_CTRL = SAMPLER_CTRL | SAMPLER_SET_MONITOR_bm;
		uint32_t sfaddr2 = SDRAM_DMA_SFADDR;
		uint64_t recv_idx2 = SDRAM_DMA_CURRENT_MARKER & SDRAM_DMA_MARKER_IDX_bm;
		recv_idx2 -= m_start_recv_index;
		recv_idx2 <<= 4 - m_serializer_shift;

		if (recv_idx2 != recv_idx || (SDRAM_DMA_CTRL & SDRAM_DMA_CHOKED_bm))
		{
			sfaddr = sfaddr2;
			recv_idx = recv_idx2;
			goto restart2;
		}

		return cur - p;
	}

	void stop()
	{
		SAMPLER_CTRL = 0;
		while (SAMPLER_STATUS & SAMPLER_PIPELINE_BUSY_bm)
		{
		}

		while (SDRAM_DMA_SFADDR & SDRAM_DMA_SFADDR_BUSY_bm)
		{
		}

		SDRAM_DMA_CTRL = SDRAM_DMA_CHOKE_bm;
		m_running = false;
	}

	enum voltage_t { v_0, v_33, v_50 };
	voltage_t get_voltage() const
	{
		return static_cast<voltage_t>(VOUT0 & VOUT_VALUE_bm);
	}

	void set_voltage(voltage_t v)
	{
		m_next_voltage = v;
		VOUT0 = 0;
		VOUT0_MIN_THRES = g_min_voltage_thres[v_0];
		VOUT0_MAX_THRES = g_max_voltage_thres[v_0];
		m_voltage_timeout.restart();
	}

	void process()
	{
		uint16_t vout = VOUT0;
		uint8_t vout_value = vout & VOUT_VALUE_bm;
		if (m_voltage_timeout)
		{
			VOUT0_MIN_THRES = g_min_voltage_thres[vout_value];
			m_next_voltage = v_0;
			m_voltage_timeout.cancel();
		}

		if (vout_value)
			LEDBITS |= 2;
		else
			LEDBITS &= ~2;

		if (m_next_voltage && (vout & VOUT_VALID_bm))
		{
			VOUT0_MAX_THRES = g_max_voltage_thres[m_next_voltage];
			VOUT0 = m_next_voltage;
			m_next_voltage = v_0;
		}
	}

private:
	enum
	{
		cmd_set_wraddr = 0x4101,
		cmd_set_rdaddr = 0x4102,
		cmd_start = 0x4103,
		cmd_stop = 0x4104,
		cmd_get_sample_index = 0xc105,
		cmd_get_config = 0xc106,
		cmd_unchoke = 0xc107,
		cmd_move_choke = 0x4108,
		cmd_read_voltage = 0xc109,
		cmd_set_voltage = 0x410a,
	};

	bool m_running;
	uint8_t m_serializer_shift;
	uint64_t m_start_src_index;
	uint64_t m_start_recv_index;

	voltage_t m_next_voltage;
	timeout<timer_t> m_voltage_timeout;
};

int main()
{
	USB_CTRL = USB_CTRL_ATTACH;
	SDRAM_CTRL = SDRAM_ENABLE_bm;
	LEDBITS = 0;

	pic_flash_handler pfh;
	dfu_handler dh(pfh);
	usb_core_handler uc;
	usb_omicron_handler oh;
	usb_control_handler * usb_handler = 0;
	usb_ctrl_req_t usb_req;

	bool last_reset_state = false;

	for (;;)
	{
		if (rxready())
		{
			switch (recv())
			{
			case 'b':
				LEDBITS ^= 1;
				break;
			case 'L':
				dh.reconfigure();
				break;
			case 'a':
				sendch('a');
				sendhex(AD0);
				sendch('\n');
				break;
			case '1':
				oh.set_voltage(usb_omicron_handler::v_0);
				break;
			case '2':
				oh.set_voltage(usb_omicron_handler::v_33);
				break;
			case '3':
				oh.set_voltage(usb_omicron_handler::v_50);
				break;
			default:
				send("omicron analyzer -- DFU loader\nbL?");
				send("\nVOUT0: ");
				sendhex(VOUT0);
				send("\nVOUT0_MIN_THRES: ");
				sendhex(VOUT0_MIN_THRES);
				send("\nVOUT0_MAX_THRES: ");
				sendhex(VOUT0_MAX_THRES);
				if (g_assert_lineno)
				{
					send("\nAssertion failed: ");
					sendhex(g_assert_lineno);
				}
				sendch('\n');
			}
		}

		pfh.process();
		dh.process();
		oh.process();

		if (USB_CTRL & USB_CTRL_RST)
		{
			USB_ADDRESS = 0;
			USB_CTRL |= USB_CTRL_RST_CLR;
			USB_EP0_OUT_CTRL = USB_EP_STALL_SET | USB_EP_SETUP_CLR;
			USB_EP0_IN_CTRL = USB_EP_STALL_SET | USB_EP_SETUP_CLR;
			USB_EP1_IN_CTRL = USB_EP_TOGGLE_CLR | USB_EP_SETUP_CLR;
			USB_EP1_OUT_CTRL = USB_EP_TOGGLE_CLR | USB_EP_SETUP_CLR | USB_EP_PUSH;
			USB_EP1_IN_CNT = 0;
			USB_EP2_IN_CTRL = USB_EP_TOGGLE_CLR | USB_EP_SETUP_CLR;
			USB_EP2_OUT_CTRL = USB_EP_STALL_SET | USB_EP_SETUP_CLR;
			USB_EP3_IN_CTRL = USB_EP_TOGGLE_CLR | USB_EP_SETUP_CLR;
			USB_EP3_OUT_CTRL = USB_EP_TOGGLE_CLR | USB_EP_SETUP_CLR;

			if (!last_reset_state)
			{
				uc.on_usb_reset();
				dh.on_usb_reset();
				oh.on_usb_reset();
				usb_handler = 0;

				uint8_t ds = dh.is_dfu_mode()? 1: 0;
				uc.set_desc_set(ds);
				last_reset_state = true;
			}
		}
		else
		{
			last_reset_state = false;
		}

		if ((USB_EP1_IN_STATUS & USB_EP_EMPTY) && usb_dbg_tx_pos)
		{
			USB_EP1_IN_CNT = usb_dbg_tx_pos;
			USB_EP1_IN_CTRL = USB_EP_PUSH;
			usb_dbg_tx_pos = 0;
		}

		if (usb_handler && (USB_EP0_IN_STATUS & USB_EP_EMPTY) != 0)
		{
			if (usb_req.is_write() && !usb_req.wLength)
			{
				usb_handler->commit_write(usb_req);
				usb_handler = 0;
			}
		}

		if (usb_handler && (USB_EP0_IN_STATUS & USB_EP_FULL) == 0)
		{
			if (usb_req.is_read() && usb_req.wLength)
			{
				uint8_t len = usb_handler->on_data_in(usb_req, (uint8_t *)USB_EP0_IN);
				USB_EP0_IN_CNT = len;
				USB_EP0_IN_CTRL = USB_EP_PUSH;
				usb_req.wLength -= len;
			}
		}

		if (usb_handler && usb_req.is_write() && (USB_EP0_OUT_CTRL & USB_EP_EMPTY) == 0)
		{
			uint8_t cnt = USB_EP0_OUT_CNT;
			if (cnt > usb_req.wLength)
			{
				USB_EP0_OUT_CTRL = USB_EP_STALL_SET;
				USB_EP0_IN_CTRL = USB_EP_STALL_SET;
				usb_handler = 0;
			}
			else
			{
				assert(usb_req.wLength);
				if (!usb_handler->on_data_out(usb_req, (uint8_t const *)USB_EP0_OUT, cnt))
				{
					USB_EP0_OUT_CTRL = USB_EP_STALL_SET;
					USB_EP0_IN_CTRL = USB_EP_STALL_SET;
					usb_handler = 0;
				}
				else if ((usb_req.wLength -= cnt) != 0)
				{
					USB_EP0_OUT_CTRL = USB_EP_PULL | USB_EP_PUSH;
				}
				else
				{
					USB_EP0_OUT_CTRL = USB_EP_STALL_SET;
					if (!usb_handler->on_data_out_done(usb_req))
					{
						USB_EP0_IN_CTRL = USB_EP_STALL_SET;
						usb_handler = 0;
					}
					else
					{
						USB_EP0_IN_CTRL = USB_EP_PUSH;
					}
				}
			}
		}

		if (USB_EP0_OUT_CTRL & USB_EP_SETUP)
		{
			if (usb_handler && usb_req.is_write())
				usb_handler->commit_write(usb_req);
			usb_handler = 0;

			memcpy(&usb_req, (void const *)USB_EP0_OUT, 8);

			USB_EP0_IN_CTRL = USB_EP_TOGGLE_SET;
			USB_EP0_OUT_CTRL = USB_EP_TOGGLE_SET | USB_EP_SETUP_CLR;

#if 0
			if (enable_setup_print)
			{
				sendch('S');
				sendhex(usb_req.bmRequestType);
				sendhex(usb_req.bRequest);
				sendch(' ');
				sendhex(usb_req.wValue);
				sendch(' ');
				sendhex(usb_req.wIndex);
				sendch(' ');
				sendhex(usb_req.wLength);
				sendch('\n');
			}
#endif

			if ((usb_req.bmRequestType & 0x1f) == 0)
			{
				if (uc.on_control_transfer(usb_req))
					usb_handler = &uc;
			}
			else if ((usb_req.bmRequestType & 0x1f) == 1 && usb_req.wIndex == 2)
			{
				if (oh.on_control_transfer(usb_req))
					usb_handler = &oh;
			}
			else if ((usb_req.bmRequestType & 0x1f) == 1 && usb_req.wIndex == 0)
			{
				if (dh.on_control_transfer(usb_req))
					usb_handler = &dh;
			}

			if (!usb_handler)
			{
				USB_EP0_OUT_CTRL = USB_EP_STALL_SET;
				USB_EP0_IN_CTRL = USB_EP_STALL_SET;
			}
			else if (usb_req.is_write())
			{
				USB_EP0_IN_CNT = 0;
				if (usb_req.wLength)
				{
					USB_EP0_OUT_CTRL = USB_EP_PUSH;
				}
				else
				{
					USB_EP0_OUT_CTRL = USB_EP_STALL_SET;
					USB_EP0_IN_CTRL = USB_EP_PUSH;
				}
			}
			else
			{
				assert(usb_req.is_read());
				USB_EP0_OUT_CTRL = USB_EP_PUSH;
			}
		}
	}

	return 0;
}
