#include "fb_main.h"

int main() {
	system("setterm -cursor off");
	int width, height;
	int x, y;
	fb_driver fb;

	fb.init();

	width = fb.getScreenX();
	height = fb.getScreenY();

	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			triple RGB;
			if(x <= width / 8) {
				RGB.x = 255;
				RGB.y = 0;
				RGB.z = 0;
			} else if(width / 8 < x && x <= (width / 8) * 2) {
				RGB.x = 0;
				RGB.y = 255;
				RGB.z = 0;
			} else if((width / 8) * 2 < x && x <= (width / 8) * 3) {
				RGB.x = 0;
				RGB.y = 0;
				RGB.z = 255;
			} else if((width / 8) * 3 < x && x <= (width / 8) * 4) {
				RGB.x = 255;
				RGB.y = 255;
				RGB.z = 0;
			} else if((width / 8) * 4 < x && x <= (width / 8) * 5) {
				RGB.x = 255;
				RGB.y = 0;
				RGB.z = 255;
			} else if((width / 8) * 5 < x && x <= (width / 8) * 6) {
				RGB.x = 255;
				RGB.y = 255;
				RGB.z = 255;
			} else if((width / 8) * 6 < x && x <= (width / 8) * 7) {
				RGB.x = ((double)y/(double)height) * 255;
				RGB.y = RGB.x;
				RGB.z = RGB.x;
			} else {
				RGB.x = 0;
				RGB.y = 0;
				RGB.z = 0;
			}
			fb.setPixel(RGB, x, y);
		}
	}

	sleep(4);
	
	system("clear");
	system("setterm -cursor on");
	return 0;
}
