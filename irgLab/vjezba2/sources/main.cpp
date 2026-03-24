#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <limits>

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

struct TestPoint {
	Point p;
	bool inside;
};

int width = 97;
int height = 97;

Polygon polygon;
bool finished = false;
std::vector<TestPoint> testPoints;

Color colorClear = Color(0.0, 0.0, 0.0);
Color colorCheckerboard1 = Color(0.1, 0.1, 0.1);
Color colorCheckerboard2 = Color(0.15, 0.15, 0.2);
Color colorPolyPoint = Color(0.5, 0.0, 0.0);
Color colorLastPoint = Color(1.0, 0.0, 0.0);
Color colorLine = Color(0.0, 0.0, 1.0);
Color colorFill = Color(1.0, 0.0, 1.0);
Color colorPointInside = Color(0.0, 1.0, 0.0);
Color colorPointOutside = Color(1.0, 0.5, 0.0);

Color calcRasterColor(double dist) {
	return Color(dist / width, dist / width, dist / width);
}

void swap(int &x, int &y) {
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

int halfSpaceTest(Point p, Edge e) {
	return e.x * p.x + e.y * p.y + e.z;
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
	Point p1 = poly.points[n - 1].v;
	Edge e2 = poly.points[n - 2].e;
	Point p2 = poly.points[0].v;
	Edge e3 = poly.points[n - 1].e;
	Point p3 = poly.points[1].v;

	int r1 = halfSpaceTest(p1, e1);
	int r2 = halfSpaceTest(p2, e2);
	int r3 = halfSpaceTest(p3, e3);
	if (r1 < 0 || r2 < 0 || r3 < 0) {
		poly.hasBelow = true;
		poly.clockwise = true;
	} 
	if (r1 > 0 || r2 > 0 || r3 > 0) {
		poly.hasAbove = true;
		poly.clockwise = false;
	}

	if (r1 == 0 || r2 == 0 || r3 == 0 || (poly.hasAbove && poly.hasBelow)) {
		poly.convex = false;
	}

	return poly.convex;
}

bool isPointInPoly(Point p, Polygon poly) {
	for (PolyElem pe : poly.points) {
		int r = halfSpaceTest(p, pe.e);
		if ((poly.clockwise && r > 0) || (!poly.clockwise && r < 0)) {
			return false;
		}
	}

	return true;
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
				if (poly.points[i0].isLeft ^ !poly.clockwise) {
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

double calcAngle(Edge e1, Edge e2) {
	e1 *= -1;
	double l1 = std::sqrt(e1.x * e1.x + e1.y * e1.y);
	double l2 = std::sqrt(e2.x * e2.x + e2.y * e2.y);

	double cosPhi = (e1.x * e2.x + e1.y * e2.y) / (l1 * l2);

	return std::acos(cosPhi) * 180 / M_PI;
}

double calcDistPointEdge(Point p, Point a, Point b) {
	int dx = b.x - a.x;
	int dy = b.y - a.y;
	int l = dx * dx + dy * dy;

	if (l == 0) {
		double px = p.x - a.x;
		double py = p.y - a.y;
		return std::sqrt(px * px + py * py);
	}

	// Projection parameter
	double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / (double)l;

	if (t < 0.0) { // Not on line, but closer to a
		double px = p.x - a.x;
		double py = p.y - a.y;
		return std::sqrt(px * px + py * py);
	} else if (t > 1.0) { // Not on line, but closer to b 
		double px = p.x - b.x;
		double py = p.y - b.y;
		return std::sqrt(px * px + py * py);
	}

	// Projection on line
	double projX = a.x + t * dx;
	double projY = a.y + t * dy;

	double px = p.x - projX;
	double py = p.y - projY;

	return std::sqrt(px * px + py * py);
}

double calcDistPointPolygon(Point p, Polygon poly) {
	std::size_t n = poly.points.size();
	double minDist = std::numeric_limits<double>::infinity();
	std::size_t i0 = n - 1;

	for (std::size_t i = 0 ; i < n ; i++) {
		double dist = calcDistPointEdge(p, poly.points[i0].v, poly.points[i].v);
		if (dist < minDist) {
			minDist = dist;
		}
		i0 = i;
	}

	return minDist;
}

void mouseClick(int x, int y, int type) {
	y = height - y - 1;

	if (!finished) {
		if (type == 0) {
			std::cout << "Placing: " << x << " " << y << '\n';
			polygon.points.push_back({x, y});
			calcPolyEdgeCoefs(polygon);
			if (polygon.convex && !calcPolyConvex(polygon)) {
				std::cout << "Polygon is no longer convex\n";
			}
			if (polygon.points.size() >= 3) {
				std::size_t n =  polygon.points.size();
				std::cout << "Last angle made: " 
						  << calcAngle(polygon.points[n - 3].e, polygon.points[n - 2].e)
						  << '\n';
			}
		} else if (type == 1) {
			if (polygon.points.size() < 3) {
				std::cout << "Can't finish a polygon with less than 3 points\n";
			} else {
				std::cout << "Finishing polygon\n";
				finished = true;
				std::size_t n =  polygon.points.size();
				std::cout << "Last angle made: " 
						  << calcAngle(polygon.points[n - 2].e, polygon.points[n - 1].e)
						  << '\n';
				std::cout << "Last angle made: " 
						  << calcAngle(polygon.points[n - 1].e, polygon.points[0].e)
						  << '\n';
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
		Point p{x, y};
		testPoints.push_back({p, isPointInPoly(p, polygon)});
	}
}

int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	Graphics graphics(width, height, colorClear, argv[0]);

	graphics.registerMouseClickFunction(mouseClick);

	while (graphics.shouldClose()) {
		graphics.clearWindow();

		if (finished) {
			// Color background pixels based on the distance from the closest edge of the polygon
			for (int i = 0 ; i < height ; i += 1) {
				for (int j = 0 ; j < width ; j += 1) {
					graphics.lightFragment(i, j, calcRasterColor(calcDistPointPolygon({i, j}, polygon)));
				}
			}
		} else {
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
		}

		// Draw polygon lines
		drawPoly(graphics, polygon);

		if (finished) { // Draw polygon fill
			fillPoly(graphics, polygon);
		} else { // Draw a pixel for each vertex of the unfinished polygon
			for (std::size_t i = 0 ; i + 1 < polygon.points.size() ; i++) {
				graphics.lightFragment(polygon.points[i].v.x, polygon.points[i].v.y, colorPolyPoint);
			}
			if (polygon.points.size() > 0) {
				graphics.lightFragment(polygon.points.back().v.x, polygon.points.back().v.y, colorLastPoint);
			}
		}

		// Draw test points
		for (TestPoint p : testPoints) {
			if (p.inside) {
				graphics.lightFragment(p.p.x, p.p.y, colorPointInside);
			} else {
				graphics.lightFragment(p.p.x, p.p.y, colorPointOutside);
			}
		}

		graphics.drawRaster();
	}   
	
    return EXIT_SUCCESS;
}
