#include <time.h>
#include <cstdlib>
#include <ctime>
#include "./fb_main.h"

struct blob {
	unsigned int w;
	unsigned int h;
	int x;
	int y;
	int dx;
	int dy;
	triple color;
};

void drawShape(int x, int y, int w, int h, triple& color, fb_driver& fb) {
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
// put color defs here
	triple black;
	black.x = 0;
	black.y = 0;
	black.z = 0;

// put setup code here

	bool negateinit;
	unsigned int fps = 60;
	unsigned int secs = 10;
	int maxspeed = 3;
	int width = fb.getScreenX();
	int height = fb.getScreenY();
	srand(time(NULL));
	int numBounce = 100;
	blob* bouncers = new blob[numBounce];
	for(int i = 0; i < numBounce; i++) {
		bouncers[i].w = width/30;
		bouncers[i].h = bouncers[i].w;
		bouncers[i].x = rand() % (width - bouncers[i].w);
		bouncers[i].y = rand() % (height - bouncers[i].h);
		negateinit = rand() % 1;
		bouncers[i].dx = rand() % maxspeed;
		bouncers[i].dx *= negateinit ? -1 : 1;
		negateinit = rand() & 1;
		bouncers[i].dy = rand() % maxspeed;
		bouncers[i].dy *= negateinit ? -1 : 1;
		bouncers[i].color.x = rand() % 256;
		bouncers[i].color.y = rand() % 256;
		bouncers[i].color.z = rand() % 256;
	}

	wipeScreen(black, fb);

	struct timespec cursor;
	clock_gettime(CLOCK_MONOTONIC, &cursor);
	int timestep = ((double)1/(double)60) * 1000000000;

	for(int i = 0; i < (fps * secs); i++) {
		int start = cursor.tv_nsec;
		wipeScreen(black, fb);

		//put drawing code in here
		for(int i = 0; i < numBounce; i++) {
			bouncers[i].x += bouncers[i].dx;
			if(bouncers[i].x < 0) {
				bouncers[i].dx *= -1;
				bouncers[i].x = 0;
			} else if(bouncers[i].x >= (width - bouncers[i].w)) {
				bouncers[i].dx *= -1;
				bouncers[i].x = width - bouncers[i].w - 1;
			}

			bouncers[i].y += bouncers[i].dy;
			if(bouncers[i].y < 0) {
				bouncers[i].dy *= -1;
				bouncers[i].y = 0;
			} else if(bouncers[i].y >= (height - bouncers[i].h)) {
				bouncers[i].dy *= -1;
				bouncers[i].y = height - bouncers[i].h - 1;
			}

			drawShape(bouncers[i].x, bouncers[i].y, bouncers[i].w, bouncers[i].h, bouncers[i].color, fb);
		}
		//end drawing code

		fb.swapBuffer();

		int wait = timestep - (cursor.tv_nsec - start);
		usleep(wait < 0 ? 0 : wait/1000);
	}
	wipeScreen(black, fb);
	delete [] bouncers;
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
