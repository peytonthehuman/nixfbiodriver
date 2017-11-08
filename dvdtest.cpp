#include <time.h>
#include "./fb_main.h"

void fill_rect(int x, int y, int w, int h, triple& color, fb_driver& fb) {
	for(int cy = 0; cy < h; cy++) {
		for(int cx = 0; cx < w; cx++) {
			fb.setPixel(color, x + cx, y + cy);
		}
	}
	return;
}

void wipeScreen(triple& color, fb_driver& fb) {
	for(int y = 0; y < fb.getScreenY(); y++) {
		for(int x = 0; x < fb.getScreenX(); x++) {
			fb.setPixel(color, x, y);
		}
	}
	return;
}

void draw(fb_driver& fb) {
	int x, y, w, h, dx, dy;

	x = 0;
	y = 0;
	dx = 2;
	dy = 2;

	int fps = 60;
	int secs = 30;

	triple black;
	black.x = 0;
	black.y = 0;
	black.z = 0;

	triple color;
	color.x = 255;
	color.y = 255;
	color.z = 255;

	w = fb.getScreenX() / 10;
	h = w;

	wipeScreen(black, fb);

	struct timespec cursor;
	clock_gettime(CLOCK_MONOTONIC, &cursor);
	int timestep = ((double)1/(double)60) * 1000000000;

	for(int i = 0; i < (fps * secs); i++) {
		int start = cursor.tv_nsec;
		//wipeScreen(black, fb);
		color.x = ((double)x/800) * 256;
		color.y = ((double)y/600) * 256;
		color.z = ((double)(x * y))/(800*600) * 256;
		fill_rect(x, y, w, h, color, fb);
		x = x + dx;
		y = y + dy;

		if ((x < 0) || (x > (fb.getScreenX() - w))) {
			dx = -dx;
			x = x + 2 * dx;
		}

		if ((y < 0) || (y > (fb.getScreenY() - h))) {
			dy = -dy;
			y = y + 2 * dy;
		}

		fb.swapBuffer();

		int wait = timestep - (cursor.tv_nsec - start);
		usleep(wait < 0 ? 0 : wait/1000);
	}
	wipeScreen(black, fb);
	return;
}

int main() {
	system("setterm --cursor off");
	system("clear");
	fb_driver fb(true, false);
	fb.init();

	draw(fb);

	system("setterm --cursor on");
	return 0;
}
