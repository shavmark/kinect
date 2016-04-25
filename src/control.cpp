#include "ofApp.h"
#include "control.h"

namespace Software2552 {

void Router::setup() {
	server.setup();
	client.setup();//bugbug move this to a reader
}
void Router::update() {
	//server.update();
	//client.update();//bugbug move this to a reader
}

void Router::bodyIndexFromTCP(ofImage& image) {
		string buffer;
//		client.read(buffer);
		if (buffer.size() == 0) {
			return;
		}

		string bufferobject;
		string s;
		int width = 512;
		int height = 424;

		if (snappy::Uncompress(buffer.c_str(), buffer.size(), &s)) {
			unsigned char*b2 = (unsigned char*)s.c_str();
			image.allocate(width, height, OF_IMAGE_COLOR);
			bool found = false;
			for (float y = 0; y < height; y++) {
				for (float x = 0; x < width; x++) {
					unsigned int index = y * width + x;
					if (b2[index] != 0xff) {
						float hue = x / width * 255;
						float sat = ofMap(y, 0, height / 2, 0, 255, true);
						float bri = ofMap(y, height / 2, height, 255, 0, true);
						// make a dynamic image, also there can be up to 6 images so we need them to be a little different 
						image.setColor(x, y, ofColor::fromHsb(hue, sat, bri));
						found = true;
					}
					else {
						image.setColor(x, y, ofColor::white);
					}
				}
			}
			if (!found) {
				image.clear();
			}
			image.update();
		}
	}
}
