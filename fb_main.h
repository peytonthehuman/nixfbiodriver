#ifndef FB_MAIN_H
#define FB_MAIN_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/tty.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/kd.h>

struct triple {
	u_int8_t x;
	u_int8_t y;
	u_int8_t z;
};

class fb_driver {
	private:
		int fbfd;
		int tty_fd;
		bool use_tty;
		bool fbpan;
		struct fb_var_screeninfo orig_vinfo;
		struct fb_var_screeninfo vinfo;
		struct fb_fix_screeninfo finfo;
		long int screensize;
		u_int8_t *front_buffer;
		u_int8_t *back_buffer;
		
		u_int8_t *bg_buffer;

		void commitVinfo();
		long position(unsigned x, unsigned y) const;

		bool* bppflags;

		u_int32_t makePixelColor(triple RGB);
		triple getPixelColor(long position) const;

	public:
		fb_driver(bool kdmode, bool pan);
		void init();

		int getScreenX() const;
		int getScreenY() const;
		int getScreenBPP() const;

		int getScreenVX() const;
		int getScreenVY() const;

		int getLineLength() const;

		void setScreenX(unsigned inX);
		void setScreenY(unsigned inY);
		void setScreenVX(unsigned inVX);
		void setScreenVY(unsigned inVY);

		void setScreenBPP(unsigned inBPP);

		triple getPixel(unsigned x, unsigned y) const;
		void setPixel(triple& RGB, unsigned x, unsigned y);

		void swapBuffer();
		
		void changeBGColor(triple& in);
		void clearToBG(bool immediateRefresh);
		
		~fb_driver();
};

#endif
