#include "./fb_main.h"

// private members
void fb_driver::commitVinfo() {
	if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
		printf("Error setting vinfo.\n");
	}

	return;
}

long fb_driver::position(unsigned x, unsigned y) const {
	long pos = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
		(y + vinfo.yoffset) * finfo.line_length;

	return pos;
}

u_int32_t fb_driver::makePixelColor(triple RGB) {
	return (RGB.x<<vinfo.red.offset) | (RGB.y<<vinfo.green.offset) | (RGB.z<<vinfo.blue.offset);
}

triple fb_driver::getPixelColor(long position) const {
	triple RGB;
	RGB.x = *(fbp + position + vinfo.red.offset);
	RGB.y = *(fbp + position + vinfo.green.offset);
	RGB.z = *(fbp + position + vinfo.blue.offset);
	return RGB;
}

// public members

fb_driver::fb_driver() {
	fbfd = 0;
	tty_fd = 0;
	use_tty = true;
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
	tty_fd = open("/dev/tty0", O_RDWR);
	
	if(!fbfd) {
		printf("Error: cannot open framebuffer.\n");
		return;
	}
	printf("The framebuffer is open.\n");

	if(!tty_fd) {
		printf("Terminal not accessible. Ignoring KDSETMODE.");
		use_tty = false;
	}

	if(use_tty) {
		if(ioctl(tty_fd, KDSETMODE, KD_GRAPHICS)) {
			printf("Error setting Terminal to graphics. Ignoring.\n");
			use_tty = false;
			ioctl(tty_fd, KDSETMODE, KD_TEXT);
		}
	}

	if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Error reading vinfo.\n");
		return;
	}
	printf("Screen %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

	for(int i = 0; i < 3; i++) {
		vinfo.bits_per_pixel = 8 * (2 * i + 1);
		if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) != -1) {
			bppflags[i] = true;
		}
	}

	vinfo.grayscale = 0;
	vinfo.bits_per_pixel = 32;
	ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo);

	if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
		printf("Error reading finfo.\n");
	}

	screensize = vinfo.yres_virtual * finfo.line_length;

	fbp = static_cast<u_int8_t*>(mmap(0, screensize,
				PROT_READ | PROT_WRITE,
				MAP_SHARED,
				fbfd, 0));

	if((int)fbp == -1) {
		printf("Failed to map.\n");
		return;
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

int fb_driver::getLineLength() const {
	return static_cast<int>(finfo.line_length);
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

triple fb_driver::getPixel(unsigned x, unsigned y) const {
	long pos = position(x, y);
	return getPixelColor(pos);
}

void fb_driver::setPixel(triple& RGB, unsigned x, unsigned y) {
	long pos = position(x, y);
	u_int32_t color = makePixelColor(RGB);

	*((u_int32_t*)(fbp + pos)) = color;

	return;
}

fb_driver::~fb_driver() {
	munmap(fbp, screensize);
	if(ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_vinfo) == -1) {
		printf("Error restoring original configuration.\n");
	}

	close(fbfd);

	if(use_tty) {
		if(ioctl(tty_fd, KDSETMODE, KD_TEXT)) {
			printf("Error restoring terminal to text.\n");
			printf("You may need to reboot your computer.\n");
		}
	}

	close(tty_fd);
}
