#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "Graphics.h"

/* NOTE:
The coord origin for the OS is top left
The coord origin for OpenGL is bottom left
*/

using Point = glm::ivec2;
using Color = glm::vec3;
using CohenCode = uint8_t;

struct Rect {
	Point p1;
	Point p2;
};

constexpr CohenCode CC_TOP =    0b1000;
constexpr CohenCode CC_BOTTOM = 0b0100;
constexpr CohenCode CC_RIGHT =  0b0010;
constexpr CohenCode CC_LEFT =   0b0001;
constexpr CohenCode CC_INSIDE = 0b0000;

int width = 97;
int height = 97;

std::vector<Point> points;

bool crop = false;
Rect cropRect = { // OpenGL coords - p1: bottom left - p2: top right
	{ width / 4, height / 4 },
	{ 3 * width / 4, 3 * height / 4 }
};

Color colorClear = Color(0.0, 0.0, 0.0);
Color colorCheckerboard1 = Color(0.1, 0.1, 0.1);
Color colorCheckerboard2 = Color(0.15, 0.15, 0.2);
Color colorCropRect = Color(0.0, 1.0, 0.0);
Color colorPoint = Color(1.0, 0.0, 0.0);

Color calcLineColor(Point p1, Point p2) {
	return Color(
		0,
		0.25,
		std::max(0.1, std::min(0.9, std::sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y)) / 40))
	);
}

inline void swap(int &x, int &y) {
	int tmp = x;
	x = y;
	y = tmp;
}

inline CohenCode calcCohenCode(Point p) {
	CohenCode cc = CC_INSIDE;

	if (p.y > cropRect.p2.y) {
		cc |= CC_TOP;
	} else if (p.y < cropRect.p1.y) {
		cc |= CC_BOTTOM;
	}
	if (p.x > cropRect.p2.x) {
		cc |= CC_RIGHT;
	} else if (p.x < cropRect.p1.x) {
		cc |= CC_LEFT;
	}

	return cc;
}

void drawLine(Graphics &graphics, Point p1, Point p2, Color color) {
	// Crop rectangle
	if (crop) {
		CohenCode cc1 = calcCohenCode(p1);
		CohenCode cc2 = calcCohenCode(p2);		

		while (true) {
			// The line is completely outside
			if ((cc1 & cc2) != CC_INSIDE) {
				return;
			}

			// The line is completely inside
			if ((cc1 | cc2) == CC_INSIDE) {
				break;
			}

			int x, y;
			CohenCode cc = cc1 > cc2 ? cc1 : cc2;

			if (cc & CC_TOP) {
				x = p1.x + (p2.x - p1.x) * (cropRect.p2.y - p1.y) / (p2.y - p1.y);
				y = cropRect.p2.y;
			} else if (cc & CC_BOTTOM) {
				x = p1.x + (p2.x - p1.x) * (cropRect.p1.y - p1.y) / (p2.y - p1.y);
				y = cropRect.p1.y;
			} else if (cc & CC_RIGHT) {
				y = p1.y + (p2.y - p1.y) * (cropRect.p2.x - p1.x) / (p2.x - p1.x);
				x = cropRect.p2.x;
			} else if (cc & CC_LEFT) {
				y = p1.y + (p2.y - p1.y) * (cropRect.p1.x - p1.x) / (p2.x - p1.x);
				x = cropRect.p1.x;
			}

			if (cc == cc1) {
				p1.x = x;
				p1.y = y;
				cc1 = calcCohenCode(p1);
			} else {
				p2.x = x;
				p2.y = y;
				cc2 = calcCohenCode(p2);
			}
		}
	}

	if (p1.x > p2.x) {
		swap(p1.x, p2.x);
		swap(p1.y, p2.y);
	}

	int x, y, a, yf, correction;

	if (p1.y <= p2.y) { // Bresenham Upper
		bool doSwap = p2.y - p1.y > p2.x - p1.x;
		if (doSwap) {
			swap(p1.x, p1.y);
			swap(p2.x, p2.y);
		}

		a = 2 * (p2.y - p1.y);
		y = p1.y;
		yf = -(p2.x - p1.x);
		correction = 2 * yf;

		if (!doSwap) { // Not steep
			for (int x = p1.x ; x <= p2.x ; x++) {
				graphics.lightFragment(x, y, color);
				yf += a;
				if (yf >= 0) {
					yf += correction;
					y++;
				}
			}
		} else { // Steep
			for (int x = p1.x ; x <= p2.x ; x++) {
				graphics.lightFragment(y, x, color);
				yf += a;
				if (yf >= 0) {
					yf += correction;
					y++;
				}
			}
		}
	} else { // Bresenham Lower;
		bool doSwap = -(p2.y - p1.y) > p2.x - p1.x;
		if (doSwap) {
			swap(p1.x, p2.y);
			swap(p2.x, p1.y);
		}

		a = 2 * (p2.y - p1.y);
		y = p1.y;
		yf = p2.x - p1.x;
		correction = 2 * yf;

		if (!doSwap) { // Not steep
			for (int x = p1.x ; x <= p2.x ; x++) {
				graphics.lightFragment(x, y, color);
				yf += a;
				if (yf <= 0) {
					yf += correction;
					y--;
				}
			}
		} else { // Steep
			for (int x = p1.x ; x <= p2.x ; x++) {
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
		points.push_back({x, y});
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
		for (int i = 0 ; i < height ; i += 1) {
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0) {
					graphics.lightFragment(i, j, colorCheckerboard1);
				}
				if (i % 10 == 0 && j % 10 == 0) {
					graphics.lightFragment(i, j, colorCheckerboard2);
				}
			}
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
		for (std::size_t i = 0 ; i + 1 < points.size() ; i+=2) {
			Color color = calcLineColor(points[i], points[i+1]);

			drawLine(graphics, points[i], points[i+1], color);
		}

		// Draw a pixel for the starting point of an unfinished line if it exists
		if (points.size() % 2 == 1) {
			graphics.lightFragment(points.back().x, points.back().y, colorPoint);
		}

		graphics.drawRaster();
	}   
	
    return EXIT_SUCCESS;
}
