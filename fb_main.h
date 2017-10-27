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

template<class T>
struct triple<T> {
	T x;
	T y;
	T z;
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
		long position(unsigned x, unsigned y);

	public:
		fb_driver();
		void init();

		int getScreenX();
		int getScreenY();
		int getScreenBPP();

		int getScreenVX();
		int getScreenVY();

		void setScreenX(unsigned inX);
		void setScreenY(unsigned inY);
		void setScreenVX(unsigned inVX);
		void setScreenVY(unsigned inVY);

		triple<char> getPixel(unsigned x, unsigned y);
		void setPixel(triple<char> RGB, unsigned x, unsigned y);

		~fb_driver();
};

#endif
