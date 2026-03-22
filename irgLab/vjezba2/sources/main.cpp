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

std::vector<glm::ivec2> points;
bool finished = false;

glm::vec3 colorClear = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 colorCheckerboard1 = glm::vec3(0.1, 0.1, 0.1);
glm::vec3 colorCheckerboard2 = glm::vec3(0.15, 0.15, 0.2);
glm::vec3 colorPoint = glm::vec3(0.5, 0.0, 0.0);
glm::vec3 colorLastPoint = glm::vec3(1.0, 0.0, 0.0);
glm::vec3 colorLine = glm::vec3(0.0, 0.0, 1.0);

inline void swap(int &x, int &y) {
	int tmp = x;
	x = y;
	y = tmp;
}

void drawLine(Graphics &graphics, glm::ivec2 p1, glm::ivec2 p2, glm::vec3 color) {
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
	if (!finished) {
		if (type == 0) {
			y = height - y - 1;
			std::cout << "Placing: " << x << " " << y << '\n';
			points.push_back({x, y});
		} else if (type == 1) {
			if (points.size() < 3) {
				std::cout << "Can't finish a polygon with less than 3 points\n";
			} else {
				std::cout << "Finishing polygon\n";
				points.push_back(points[0]);
				finished = true;
			}
		}
	} else {
		// TODO: Test if the point is inside or outside the polygon 
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
			for (int j = 0 ; j < width ; j += 1) {
				if ((i + j) % 2 == 0) {
					graphics.lightFragment(i, j, colorCheckerboard1);
				}
				if (i % 10 == 0 && j % 10 == 0) {
					graphics.lightFragment(i, j, colorCheckerboard2);
				}
			}
		}

		// Draw lines
		for (std::size_t j = 1 ; j < points.size() ; j++) {
			drawLine(graphics, points[j-1], points[j], colorLine);
		}

		// Draw a pixel for each point of the unfinished polygon
		if (!finished) {
			for (std::size_t i = 0 ; i + 1 < points.size() ; i++) {
				graphics.lightFragment(points[i].x, points[i].y, colorPoint);
			}
			if (points.size() > 0) {
				graphics.lightFragment(points.back().x, points.back().y, colorLastPoint);
			}
		}
		
		graphics.drawRaster();
	}   
	
    return EXIT_SUCCESS;
}
