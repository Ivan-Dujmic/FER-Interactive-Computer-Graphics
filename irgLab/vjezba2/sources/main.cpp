#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "Graphics.h"

/* NOTE:
The coord origin for the OS is top left
The coord origin for OpenGL is bottom left
*/

int width = 97;
int height = 97;

std::vector<std::pair<int, int>> points;

glm::vec3 colorClear = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 colorCheckerboard1 = glm::vec3(0.1, 0.1, 0.1);
glm::vec3 colorCheckerboard2 = glm::vec3(0.15, 0.15, 0.2);
glm::vec3 colorPoint = glm::vec3(1.0, 0.0, 0.0);
glm::vec3 colorLine = glm::vec3(0.0, 0.0, 1.0);

inline void swap(int &x, int &y) {
	int tmp = x;
	x = y;
	y = tmp;
}

void drawLine(Graphics &graphics, int x0, int y0, int x1, int y1, glm::vec3 color) {
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
		std::cout << "Finishing polygon\n";
		// TODO
	}
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	Graphics graphics(width, height, colorClear, argv[0]);

	graphics.registerMouseClickFunction(mouseClick);

	while (graphics.shouldClose()) {
		graphics.clearWindow();

		// Draw checkerboard pattern
		for (int i = 0; i < height; i += 1) {
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0)
					graphics.lightFragment(i, j, colorCheckerboard1);
				if (i % 10 == 0 && j % 10 == 0)
					graphics.lightFragment(i, j, colorCheckerboard2);
			}
		}

		// Draw lines
		for (std::size_t i = 0; i + 1 < points.size() ; i+=2) {
			int x0 = points[i].first;
			int y0 = points[i].second;
			int x1 = points[i+1].first;
			int y1 = points[i+1].second;

			drawLine(graphics, x0, y0, x1, y1, colorLine);
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
