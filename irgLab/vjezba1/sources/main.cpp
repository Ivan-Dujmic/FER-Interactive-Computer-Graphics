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

int width = 97;
int height = 97;

std::vector<std::pair<int, int>> points;

bool crop = false;
Rect cropRect = { // OpenGL coords
	{ width / 4, height / 4 },
	{ 3 * width / 4, 3 * height / 4 }
};

inline void swap(int &x, int &y) {
	int tmp = x;
	x = y;
	y = tmp;
}

void drawLine(Graphics &graphics, int x0, int y0, int x1, int y1) {
	glm::vec3 color = glm::vec3(
		0,
		0.25,
		std::max(0.1, std::min(0.9, std::sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0)) / 40))
	);

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

		if (!doSwap) {
			for (int x = x0 ; x <= x1 ; x++) {
				if (!crop || (x > cropRect.p1.x && x < cropRect.p2.x && y > cropRect.p1.y && y < cropRect.p2.y)) {
					graphics.lightFragment(x, y, color);
				}
				yf += a;
				if (yf >= 0) {
					yf += correction;
					y++;
				}
			}
		} else {
			for (int x = x0 ; x <= x1 ; x++) {
				if (!crop || (x > cropRect.p1.x && x < cropRect.p2.x && y > cropRect.p1.y && y < cropRect.p2.y)) {
					graphics.lightFragment(y, x, color);
				}
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

		if (!doSwap) {
			for (int x = x0 ; x <= x1 ; x++) {
				if (!crop || (x > cropRect.p1.x && x < cropRect.p2.x && y > cropRect.p1.y && y < cropRect.p2.y)) {
					graphics.lightFragment(x, y, color);
				}
				yf += a;
				if (yf <= 0) {
					yf += correction;
					y--;
				}
			}
		} else {
			for (int x = x0 ; x <= x1 ; x++) {
				if (!crop || (x > cropRect.p1.x && x < cropRect.p2.x && y > cropRect.p1.y && y < cropRect.p2.y)) {
					graphics.lightFragment(y, x, color);
				}
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
		std::cout << "Placing: " << x << " " << y << '\n';
		points.push_back(std::make_pair(x, y));
	} else if (type == 1) {
		std::cout << "Inverting crop\n";
		crop = !crop;
	}
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	Graphics graphics(width, height, glm::vec3(0, 0, 0), argv[0]);

	graphics.registerMouseClickFunction(mouseClick);

	while (graphics.shouldClose()) {
		graphics.clearWindow();

		// Draw checkerboard pattern
		for (int i = 0; i < height; i += 1)
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0)
					graphics.lightFragment(i, j, glm::vec3(0.1, 0.1, 0.1));
				if (i % 10 == 0 && j % 10 == 0)
					graphics.lightFragment(i, j, glm::vec3(0.15, 0.15, 0.2));
			}

		// Draw crop rectangle
		if (crop) {
			for (int x = cropRect.p1.x ; x <= cropRect.p2.x ; x++) {
				graphics.lightFragment(x, cropRect.p1.y, glm::vec3(0, 1, 0));
				graphics.lightFragment(x, cropRect.p2.y, glm::vec3(0, 1, 0));
			}

			for (int y = cropRect.p1.y ; y < cropRect.p2.y ; y++) {
				graphics.lightFragment(cropRect.p1.x, y, glm::vec3(0, 1, 0));
				graphics.lightFragment(cropRect.p2.x, y, glm::vec3(0, 1, 0));
			}
		}

		// Draw lines
		for (std::size_t i = 0; i < points.size(); i++) {
			if (i % 2 == 1) {
				drawLine(graphics,
					points[i-1].first,
					height - points[i-1].second - 1,
					points[i].first,
					height - points[i].second - 1
				);
			}
		}

		// Draw a pixel for the starting point of an unfinished line if it exists
		if (points.size() % 2 == 1) {
			std::size_t index = points.size() - 1;
			graphics.lightFragment(points[index].first, height - points[index].second - 1, glm::vec3(1, 0, 0));
		}

		graphics.drawRaster();
	}   
	
    return EXIT_SUCCESS;
}
