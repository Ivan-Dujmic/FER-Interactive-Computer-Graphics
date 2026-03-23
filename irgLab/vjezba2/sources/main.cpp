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

struct Polygon {
	std::vector<PolyElem> points;
	bool hasBelow = false;
	bool hasAbove = false;
	bool convex = true;
	bool clockwise = true;
};

int width = 97;
int height = 97;

Polygon polygon;
bool finished = false;

Color colorClear = Color(0.0, 0.0, 0.0);
Color colorCheckerboard1 = Color(0.1, 0.1, 0.1);
Color colorCheckerboard2 = Color(0.15, 0.15, 0.2);
Color colorPoint = Color(0.5, 0.0, 0.0);
Color colorLastPoint = Color(1.0, 0.0, 0.0);
Color colorLine = Color(0.0, 0.0, 1.0);
Color colorFill = Color(1.0, 0.0, 1.0);

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

// Updates the edge coefficients for the edges made by the newest vertex of the polygon
void calcPolyEdgeCoefs(Polygon &poly) {
	std::size_t n = poly.points.size();

	if (n < 2) {
		return;
	}

	PolyElem &pe1 = poly.points[n - 2];
	PolyElem &pe2 = poly.points[n - 1];
	Point p1 = pe1.v;
	Point p2 = pe2.v;
	Point p3 = poly.points[0].v;

	pe1.e.x = p1.y - p2.y;
	pe1.e.y = p2.x - p1.x;
	pe1.e.z = p1.x * p2.y - p1.y * p2.x;
	pe1.isLeft = p1.y < p2.y;

	pe2.e.x = p2.y - p3.y;
	pe2.e.y = p3.x - p2.x;
	pe2.e.z = p2.x * p3.y - p2.y * p3.x;
	pe2.isLeft = p2.y < p3.y;
}

// Checks the polygon's convexity and direction, and updates it's attributes
// For the last added index
// Returns true if the polygon is convex
bool calcPolyConvex(Polygon &poly) {
	std::size_t n = poly.points.size();

	if (n < 3) {
		return true;
	}

	if (!poly.convex) {
		return false;
	}

	Edge e1 = poly.points[n - 3].e;
	Point v1 = poly.points[n - 1].v;
	Edge e2 = poly.points[n - 2].e;
	Point v2 = poly.points[0].v;

	int r1 = e1.x * v1.x + e1.y * v1.y + e1.z;
	int r2 = e2.x * v2.x + e2.y * v2.y + e2.z;
	if (r1 < 0 || r2 < 0) {
		poly.hasBelow = true;
		poly.clockwise = true;
	} 
	if (r1 > 0 || r2 > 0) {
		poly.hasAbove = true;
		poly.clockwise = false;
	}

	if (r1 == 0 || r2 == 0 || (poly.hasAbove && poly.hasBelow)) {
		poly.convex = false;
	}

	return poly.convex;
}

void drawPoly(Graphics &graphics, Polygon poly) {
	for (std::size_t i = 1 ; i < poly.points.size() ; i++) {
		drawLine(graphics, poly.points[i-1].v, poly.points[i].v, colorLine);
	}
	if (finished) {
		drawLine(graphics, poly.points.back().v, poly.points[0].v, colorLine);
	}
}

void fillPoly(Graphics &graphics, Polygon poly) {
	int xmin = poly.points[0].v.x, xmax = poly.points[0].v.x, ymin = poly.points[0].v.y, ymax = poly.points[0].v.y;

	for (std::size_t i = 1 ; i < poly.points.size() ; i++) {
		if (xmin > poly.points[i].v.x) xmin = poly.points[i].v.x;
		if (xmax < poly.points[i].v.x) xmax = poly.points[i].v.x;
		if (ymin > poly.points[i].v.y) ymin = poly.points[i].v.y;
		if (ymax < poly.points[i].v.y) ymax = poly.points[i].v.y;
	}

	for (int y = ymin ; y <= ymax ; y++) {
		double L = xmin;
		double D = xmax;
		std::size_t i0 = poly.points.size() - 1;

		for (std::size_t i = 0 ; i < poly.points.size() ; i0=i++) {
			if (poly.points[i0].e.x == 0) { // Horizonal edge
				if (poly.points[i0].v.y == y) {
					if (poly.points[i0].v.x < poly.points[i].v.x) {
						L = poly.points[i0].v.x;
						D = poly.points[i].v.x;
					} else {
						L = poly.points[i].v.x;
						D = poly.points[i0].v.x;
					}
					break;
				}
			} else { // Non-horizontal edge
				double x = (-poly.points[i0].e.y * y - poly.points[i0].e.z) / (double)poly.points[i0].e.x;
				if (poly.points[i0].isLeft) {
					if (L < x) {
						L = x;
					}
				} else {
					if (D > x) {
						D = x;
					}
				}
			}
		}

		drawLine(graphics, {std::round(L), y}, {std::round(D), y}, colorFill);
	}
}

void mouseClick(int x, int y, int type) {
	if (!finished) {
		if (type == 0) {
			y = height - y - 1;
			std::cout << "Placing: " << x << " " << y << '\n';
			polygon.points.push_back({x, y});
			calcPolyEdgeCoefs(polygon);
			if (polygon.convex && !calcPolyConvex(polygon)) {
				std::cout << "Polygon is no longer convex\n";
			}
		} else if (type == 1) {
			if (polygon.points.size() < 3) {
				std::cout << "Can't finish a polygon with less than 3 points\n";
			} else {
				std::cout << "Finishing polygon\n";
				finished = true;
				if (polygon.convex) {
					if (polygon.clockwise) {
						std::cout << "Polygon is clockwise\n";
					} else {
						std::cout << "Polygon is counterclockwise\n";
					}
				}
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
		drawPoly(graphics, polygon);

		if (finished) { // Draw polygon fill
			fillPoly(graphics, polygon);
		} else { // Draw a pixel for each vertex of the unfinished polygon
			for (std::size_t i = 0 ; i + 1 < polygon.points.size() ; i++) {
				graphics.lightFragment(polygon.points[i].v.x, polygon.points[i].v.y, colorPoint);
			}
			if (polygon.points.size() > 0) {
				graphics.lightFragment(polygon.points.back().v.x, polygon.points.back().v.y, colorLastPoint);
			}
		}

		graphics.drawRaster();
	}   
	
    return EXIT_SUCCESS;
}
