#include <time.h>
#include <cstdlib>
#include <ctime>
#include "./fb_main.h"

bool num0[25] = {
	0,0,1,0,0,
	0,1,0,1,0,
	0,1,0,1,0,
	0,1,0,1,0,
	0,0,1,0,0};

bool num1[25] = {
	0,0,1,0,0,
	0,1,1,0,0,
	0,0,1,0,0,
	0,0,1,0,0,
	0,1,1,1,0};

bool num2[25] = {
	0,0,1,0,0,
	0,1,0,1,0,
	0,0,0,1,0,
	0,0,1,0,0,
	0,1,1,1,0};

bool num3[25] = {
	0,1,1,1,0,
	0,0,0,1,0,
	0,0,1,1,0,
	0,0,0,1,0,
	0,1,1,1,0};

bool num4[25] = {
	0,1,0,1,0,
	0,1,0,1,0,
	0,1,1,1,0,
	0,0,0,1,0,
	0,0,0,1,0};

bool num5[25] = {
	0,1,1,1,0,
	0,1,0,0,0,
	0,1,1,1,0,
	0,0,0,1,0,
	0,1,1,1,0};

bool num6[25] = {
	0,1,1,1,0,
	0,1,0,0,0,
	0,1,1,1,0,
	0,1,0,1,0,
	0,1,1,1,0};

bool num7[25] = {
	0,1,1,1,0,
	0,0,0,1,0,
	0,0,0,1,0,
	0,0,1,0,0,
	0,0,1,0,0};

bool num8[25] = {
	0,1,1,1,0,
	0,1,0,1,0,
	0,1,1,1,0,
	0,1,0,1,0,
	0,1,1,1,0};

bool num9[25] = {
	0,1,1,1,0,
	0,1,0,1,0,
	0,1,1,1,0,
	0,0,0,1,0,
	0,1,1,1,0};

struct blob {
	unsigned int w;
	unsigned int h;
	int x;
	int y;
	int dx;
	int dy;
	triple color;
	int num;
};

bool pos(int num, int x, int y, int w, int h) {
	bool *number;
	switch(num) {
		case 0:
			number = num0;
			break;
		case 1:
			number = num1;
			break;
		case 2:
			number = num2;
			break;
		case 3:
			number = num3;
			break;
		case 4:
			number = num4;
			break;
		case 5:
			number = num5;
			break;
		case 6:
			number = num6;
			break;
		case 7:
			number = num7;
			break;
		case 8:
			number = num8;
			break;
		case 9:
			number = num9;
			break;
	}

	int numY = y / ((double)h/5);
	int numX = x / ((double)w/5);

	return number[numY*5 + numX];
}

void drawShape(blob& drawing, fb_driver& fb) {
	for(int cy = 0; cy < drawing.h; cy++) {
		for(int cx = 0; cx < drawing.w; cx++) {
			if(pos(drawing.num, cx, cy, drawing.w, drawing.h)) {
				fb.setPixel(drawing.color, drawing.x + cx, drawing.y + cy);
			}
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
	unsigned int secs = 30;
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
		bouncers[i].num = rand() % 10;
	}

	wipeScreen(black, fb);

	struct timespec cursor;
	clock_gettime(CLOCK_MONOTONIC, &cursor);
	int timestep = ((double)1/(double)fps) * 1000000000;
	int wait;
	bool bouncing = true;

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
				bouncers[i].dy = bouncers[i].dy * -1 - 1;
				bouncers[i].y = 0;
			} else if(bouncers[i].y >= (height - bouncers[i].h)) {
				if(bouncers[i].dx > 0) {
					bouncers[i].dx += -1;
				} else if(bouncers[i].dx < 0) {
					bouncers[i].dx += 1;
				}
				bouncers[i].dy = bouncers[i].dy * -1 + 1;
				bouncers[i].y = height - bouncers[i].h - 1;
			}

			bouncers[i].num = (bouncers[i].dy/10) % 10;
			bouncing = bouncing || (bouncers[i].dy != 1 || bouncers[i].dx != 0);

			drawShape(bouncers[i], fb);
			bouncers[i].dy += 1;
		}
		//end drawing code

		fb.swapBuffer();

		wait = timestep - (cursor.tv_nsec - start);
		usleep(wait < 0 ? 0 : wait/1000);
		if(!bouncing) {
			break;
		}
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
