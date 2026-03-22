#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "Graphics.h"

/* NOTE:
The coord origin for the OS is top left
The coord origin for OpenGL is bottom left
*/

using Point = glm::ivec2;
using Edge = glm::ivec3;
using Color = glm::vec3;

struct PolyElem {
	Point v; // Vertex coords
	Edge e; // Edge coefficients
	bool isLeft;

	PolyElem(int x, int y) : v(x, y), e(0), isLeft(false) {}
};

int width = 97;
int height = 97;

std::vector<PolyElem> points;
bool finished = false;

Color colorClear = Color(0.0, 0.0, 0.0);
Color colorCheckerboard1 = Color(0.1, 0.1, 0.1);
Color colorCheckerboard2 = Color(0.15, 0.15, 0.2);
Color colorPoint = Color(0.5, 0.0, 0.0);
Color colorLastPoint = Color(1.0, 0.0, 0.0);
Color colorLine = Color(0.0, 0.0, 1.0);

inline void swap(int &x, int &y) {
	int tmp = x;
	x = y;
	y = tmp;
}

void drawLine(Graphics &graphics, Point p1, Point p2, Color color) {
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

void calcPolyEdgeCoefs() {
	std::size_t i0 = points.size() - 1;

	for (std::size_t i = 0 ; i < points.size() ; i++) {
		Point p1 = points[i0].v;
		Point p2 = points[i].v;

		points[i0].e.x = p1.y - p2.y;
		points[i0].e.y = p2.x - p1.x;
		points[i0].e.z = p1.x * p2.y - p1.y * p2.x;

		points[i0].isLeft = p1.y < p2.y;

		i0 = i;
	}
}

void drawPoly(Graphics &graphics) {
	for (std::size_t i = 1 ; i < points.size() ; i++) {
		drawLine(graphics, points[i-1].v, points[i].v, colorLine);
	}
	if (finished) {
		drawLine(graphics, points.back().v, points[0].v, colorLine);
	}
}

void fillPoly(Graphics &graphics) {
	int xmin = points[0].v.x, xmax = points[0].v.x, ymin = points[0].v.y, ymax = points[0].v.y;
	double L, D;

	for (std::size_t i = 1 ; i < points.size() ; i++) {
		if (xmin > points[i].v.x) xmin = points[i].v.x;
		if (xmax < points[i].v.x) xmax = points[i].v.x;
		if (ymin > points[i].v.y) ymin = points[i].v.y;
		if (ymax < points[i].v.y) ymax = points[i].v.y;
	}

	for (int y = ymin ; y <= ymax ; y++) {
		L = xmin;
		D = xmax;
		std::size_t i0 = points.size() - 1;

		for (std::size_t i = 0 ; i < points.size() ; i++) {
			if (points[i0].e.x == 0) { // Horizonal edge
				if (points[i0].v.y == y) {
					if (points[i0].v.x < points[i].v.x) {
						L = points[i0].v.x;
						D = points[i].v.x;
					} else {
						L = points[i].v.x;
						D = points[i0].v.x;
					}
					break;
				}
			} else { // Non-horizontal edge
				double x = (-points[i0].e.y * y - points[i0].e.z) / (double)points[i0].e.x;
				if (points[i0].isLeft) {
					if (L < x) {
						L = x;
					}
				} else {
					if (D > x) {
						D = x;
					}
				}
			}

			i0 = i;
		}

		drawLine(graphics, {std::round(L), y}, {std::round(D), y}, colorLine);
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
				finished = true;
				calcPolyEdgeCoefs();
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

		// Draw polygon lines
		drawPoly(graphics);

		if (finished) { // Draw polygon fill
			fillPoly(graphics);
		} else { // Draw a pixel for each vertex of the unfinished polygon
			for (std::size_t i = 0 ; i + 1 < points.size() ; i++) {
				graphics.lightFragment(points[i].v.x, points[i].v.y, colorPoint);
			}
			if (points.size() > 0) {
				graphics.lightFragment(points.back().v.x, points.back().v.y, colorLastPoint);
			}
		}

		graphics.drawRaster();
	}   
	
    return EXIT_SUCCESS;
}
