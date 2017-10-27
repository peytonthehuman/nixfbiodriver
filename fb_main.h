#ifndef FB_MAIN_H
#define FB_MAIN_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

struct triple {
	char x;
	char y;
	char z;
};

class fb_driver {
	private:
		int fbfd;
		struct fb_var_screeninfo orig_vinfo;
		struct fb_var_screeninfo vinfo;
		struct fb_fix_screeninfo finfo;
		long int screensize;
		char* fbp;

		void commitVinfo();
		long position(unsigned x, unsigned y) const;

		bool* bppflags;

	public:
		fb_driver();
		void init();

		int getScreenX() const;
		int getScreenY() const;
		int getScreenBPP() const;

		int getScreenVX() const;
		int getScreenVY() const;

		void setScreenX(unsigned inX);
		void setScreenY(unsigned inY);
		void setScreenVX(unsigned inVX);
		void setScreenVY(unsigned inVY);

		void setScreenBPP(unsigned inBPP);

		triple getPixel(unsigned x, unsigned y) const;
		void setPixel(triple RGB, unsigned x, unsigned y);

		~fb_driver();
};

#endif
