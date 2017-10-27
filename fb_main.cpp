#include "./fb_main.h"

// private members
void commitVinfo() {
	if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
		printf("Error setting vinfo.\n");
		return 1;
	}
}

long position(unsigned x, unsigned y) {
	long pos = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
		(y + vinfo.yoffset) * finfo.line_len;

	return pos;
}

// public members

fb_driver::fb_driver() {
	fbfd = 0;
	screensize = 0;
	fbp = nullptr;
	bppflags = new bool[3]; // 8, 16, 32 bits
	for(int i = 0; i < 3; i++) {
		bppflags[i] = false;
	}
	return;
}

void fb_driver::init() {
	fbfd = open("/dev/fb0", O_RDWR);
	
	if(!fbfd) {
		printf("Error: cannot open framebuffer.\n");
		return 1;
	}
	printf("The framebuffer is open.\n");

	if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Error reading vinfo.\n");
		return 1;
	}
	printf("Screen %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

	for(int i = 0; i < 3; i++) {
		vinfo.bits_per_pixel = 8 * (2 * (i + 1));
		if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) != -1) {
			bppflags[i] = true;
		}
	}

	if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
		printf("Error reading finfo.\n");
	}

	screensize = finfo.smem_len;

	fbp = static_cast<char*>(mmap(0, screensize,
				PROT_READ | PROT_WRITE,
				MAP_SHARED,
				fbfd, 0));

	if(static_cast<int>(fbp) == -1) {
		printf("Failed to map.\n");
		return 1;
	}

	return;
}

int fb_driver::getScreenX() const {
	return static_cast<int>(vinfo.xres);
}

int fb_driver::getScreenY() const {
	return static_cast<int>(vinfo.yres);
}

int fb_driver::getScreenBPP() const {
	return static_cast<int>(vinfo.bits_per_pixel);
}


int fb_driver::getScreenVX() const {
	return static_cast<int>(vinfo.xres_virtual);
}

int fb_driver::getScreenVY() const {
	return static_cast<int>(vinfo.yres_virtual);
}


void fb_driver::setScreenX(unsigned inX) {
	vinfo.xres = static_cast<__u32>(inX);
	commitVinfo();
	return;
}

void fb_driver::setScreenY(unsigned inY) {
	vinfo.yres = static_cast<__u32>(inY);
	commitVinfo();
	return;
}

void fb_driver::setScreenVX(unsigned inVX) {
	vinfo.xres_virtual = static_cast<__u32>(inVX);
	commitVinfo();
	return;
}

void fb_driver::setScreenVY(unsigned inVY) {
	vinfo.yres_virtual = static_cast<__u32>(inVY);
	commitVinfo();
	return;
}

void fb_driver::setScreenBPP(unsigned inBPP) {
	if(inBPP == 8 && bppflags[0] || inBPP == 16 && bppflags[1] || inBPP == 32 && bppflags[2]) {
		vinfo.bits_per_pixel = static_cast<__u32>(inBPP);
		commitVinfo();
	}
	else {
		printf("Error: could not set BPP to %d.\n", inBPP);
	}
	return;
}

triple<char> fb_driver::getPixel(unsigned x, unsigned y) const {
	long pos = position(x, y);
	triple<char> retPixel;

	if(vinfo.bits_per_pixel == 32) {
		retPixel.x = *(fbp + pos + 2);
		retPixel.y = *(fbp + pos + 1);
		retPixel.z = *(fbp + pos);
	}

	return retPixel;
}

void setPixel(triple<char> RGB, unsigned x, unsigned y) {
	long pos = position(x, y);

	if(vinfo.bits_per_pixel == 32) {
		*(fbp + pos) = RGB.z;
		*(fbp + pos + 1) = RGB.y;
		*(fbp + pos + 2) = RGB.x;
		*(fbp + pos + 3) = 0;
	}

	return;
}

fb_driver::~fb_driver() {
	munmap(fbp, screensize);
	if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_vinfo) == -1) {
		printf("Error restoring original configuration.\n");
	}

	close(fbfd);
}
