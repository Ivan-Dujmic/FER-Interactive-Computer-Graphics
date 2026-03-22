#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "Graphics.h"

/* NOTE:
The coord origin for the OS is top left
The coord origin for OpenGL is bottom left
*/

struct Point {
	int x;
	int y;
};

struct Rect {
	Point p1;
	Point p2;
};

using CohenCode = uint8_t;
constexpr uint8_t CC_TOP =    0b1000;
constexpr uint8_t CC_BOTTOM = 0b0100;
constexpr uint8_t CC_RIGHT =  0b0010;
constexpr uint8_t CC_LEFT =   0b0001;
constexpr uint8_t CC_INSIDE = 0b0000;

int width = 97;
int height = 97;

std::vector<std::pair<int, int>> points;

bool crop = false;
Rect cropRect = { // OpenGL coords - p1: bottom left - p2: top right
	{ width / 4, height / 4 },
	{ 3 * width / 4, 3 * height / 4 }
};

glm::vec3 colorClear = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 colorCheckerboard1 = glm::vec3(0.1, 0.1, 0.1);
glm::vec3 colorCheckerboard2 = glm::vec3(0.15, 0.15, 0.2);
glm::vec3 colorCropRect = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 colorPoint = glm::vec3(1.0, 0.0, 0.0);

glm::vec3 calcLineColor(int x0, int y0, int x1, int y1) {
	return glm::vec3(
		0,
		0.25,
		std::max(0.1, std::min(0.9, std::sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0)) / 40))
	);
}

inline void swap(int &x, int &y) {
	int tmp = x;
	x = y;
	y = tmp;
}

inline CohenCode calcCohenCode(int x, int y) {
	CohenCode cc = CC_INSIDE;

	if (y > cropRect.p2.y) {
		cc |= CC_TOP;
	} else if (y < cropRect.p1.y) {
		cc |= CC_BOTTOM;
	}
	if (x > cropRect.p2.x) {
		cc |= CC_RIGHT;
	} else if (x < cropRect.p1.x) {
		cc |= CC_LEFT;
	}

	return cc;
}

void drawLine(Graphics &graphics, int x0, int y0, int x1, int y1, glm::vec3 color) {
	// Crop rectangle
	if (crop) {
		CohenCode cc0 = calcCohenCode(x0, y0);
		CohenCode cc1 = calcCohenCode(x1, y1);		

		while (true) {
			// The line is completely outside
			if ((cc0 & cc1) != CC_INSIDE) {
				return;
			}

			// The line is completely inside
			if ((cc0 | cc1) == CC_INSIDE) {
				break;
			}

			int x, y;
			CohenCode cc = cc0 > cc1 ? cc0 : cc1;

			if (cc & CC_TOP) {
				x = x0 + (x1 - x0) * (cropRect.p2.y - y0) / (y1 - y0);
				y = cropRect.p2.y;
			} else if (cc & CC_BOTTOM) {
				x = x0 + (x1 - x0) * (cropRect.p1.y - y0) / (y1 - y0);
				y = cropRect.p1.y;
			} else if (cc & CC_RIGHT) {
				y = y0 + (y1 - y0) * (cropRect.p2.x - x0) / (x1 - x0);
				x = cropRect.p2.x;
			} else if (cc & CC_LEFT) {
				y = y0 + (y1 - y0) * (cropRect.p1.x - x0) / (x1 - x0);
				x = cropRect.p1.x;
			}

			if (cc == cc0) {
				x0 = x;
				y0 = y;
				cc0 = calcCohenCode(x0, y0);
			} else {
				x1 = x;
				y1 = y;
				cc1 = calcCohenCode(x1, y1);
			}
		}
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int x, y, a, yf, correction;

	if (y0 <= y1) { // Bresenham Upper
		bool doSwap = y1 - y0 > x1 - x0;
		if (doSwap) {
			swap(x0, y0);
			swap(x1, y1);
		}

		a = 2 * (y1 - y0);
		y = y0;
		yf = -(x1 - x0);
		correction = 2 * yf;

		if (!doSwap) { // Not steep
			for (int x = x0 ; x <= x1 ; x++) {
				graphics.lightFragment(x, y, color);
				yf += a;
				if (yf >= 0) {
					yf += correction;
					y++;
				}
			}
		} else { // Steep
			for (int x = x0 ; x <= x1 ; x++) {
				graphics.lightFragment(y, x, color);
				yf += a;
				if (yf >= 0) {
					yf += correction;
					y++;
				}
			}
		}
	} else { // Bresenham Lower;
		bool doSwap = -(y1 - y0) > x1 - x0;
		if (doSwap) {
			swap(x0, y1);
			swap(x1, y0);
		}

		a = 2 * (y1 - y0);
		y = y0;
		yf = x1 - x0;
		correction = 2 * yf;

		if (!doSwap) { // Not steep
			for (int x = x0 ; x <= x1 ; x++) {
				graphics.lightFragment(x, y, color);
				yf += a;
				if (yf <= 0) {
					yf += correction;
					y--;
				}
			}
		} else { // Steep
			for (int x = x0 ; x <= x1 ; x++) {
				graphics.lightFragment(y, x, color);
				yf += a;
				if (yf <= 0) {
					yf += correction;
					y--;
				}
			}
		}
	}
}

void mouseClick(int x, int y, int type) {
	if (type == 0) {
		y = height - y - 1;
		std::cout << "Placing: " << x << " " << y << '\n';
		points.push_back(std::make_pair(x, y));
	} else if (type == 1) {
		std::cout << "Inverting crop\n";
		crop = !crop;
	}
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	Graphics graphics(width, height, colorClear, argv[0]);

	graphics.registerMouseClickFunction(mouseClick);

	while (graphics.shouldClose()) {
		graphics.clearWindow();

		// Draw checkerboard pattern
		for (int i = 0; i < height; i += 1)
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0)
					graphics.lightFragment(i, j, colorCheckerboard1);
				if (i % 10 == 0 && j % 10 == 0)
					graphics.lightFragment(i, j, colorCheckerboard2);
			}

		// Draw crop rectangle
		if (crop) {
			for (int x = cropRect.p1.x ; x <= cropRect.p2.x ; x++) {
				graphics.lightFragment(x, cropRect.p1.y, colorCropRect);
				graphics.lightFragment(x, cropRect.p2.y, colorCropRect);
			}

			for (int y = cropRect.p1.y ; y < cropRect.p2.y ; y++) {
				graphics.lightFragment(cropRect.p1.x, y, colorCropRect);
				graphics.lightFragment(cropRect.p2.x, y, colorCropRect);
			}
		}

		// Draw lines
		for (std::size_t i = 0; i + 1 < points.size() ; i+=2) {
			int x0 = points[i].first;
			int y0 = points[i].second;
			int x1 = points[i+1].first;
			int y1 = points[i+1].second;

			glm::vec3 color = calcLineColor(x0, y0, x1, y1);

			drawLine(graphics, x0, y0, x1, y1, color);
		}

		// Draw a pixel for the starting point of an unfinished line if it exists
		if (points.size() % 2 == 1) {
			std::size_t index = points.size() - 1;
			graphics.lightFragment(points[index].first, points[index].second, colorPoint);
		}

		graphics.drawRaster();
	}   
	
    return EXIT_SUCCESS;
}
