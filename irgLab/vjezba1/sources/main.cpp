// Local Headers

// System Headers
#include "Grafika.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>

bool odsijecanje = false;
std::vector<std::pair<int, int>> klikovi;

inline void swap(int &x, int &y) {
	int tmp = x;
	x = y;
	y = tmp;
}

struct Point {
	int x;
	int y;
};

struct Rect {
	Point p1;
	Point p2;
};

void iscrtajLiniju(Grafika &grafika, int x0, int y0, int x1, int y1, Rect rect) {
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
				if (!odsijecanje || (x > rect.p1.x && x < rect.p2.x && y > rect.p1.y && y < rect.p2.y)) {
					grafika.osvijetliFragment(x, y, glm::vec3(0, 1, 1));
				}
				yf += a;
				if (yf >= 0) {
					yf += correction;
					y++;
				}
			}
		} else {
			for (int x = x0 ; x <= x1 ; x++) {
				if (!odsijecanje || (x > rect.p1.x && x < rect.p2.x && y > rect.p1.y && y < rect.p2.y)) {
					grafika.osvijetliFragment(y, x, glm::vec3(0, 1, 1));
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
				if (!odsijecanje || (x > rect.p1.x && x < rect.p2.x && y > rect.p1.y && y < rect.p2.y)) {
					grafika.osvijetliFragment(x, y, glm::vec3(0, 1, 1));
				}
				yf += a;
				if (yf <= 0) {
					yf += correction;
					y--;
				}
			}
		} else {
			for (int x = x0 ; x <= x1 ; x++) {
				if (!odsijecanje || (x > rect.p1.x && x < rect.p2.x && y > rect.p1.y && y < rect.p2.y)) {
					grafika.osvijetliFragment(y, x, glm::vec3(0, 1, 1));
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

void klikMisa(int x, int y, int vrsta) {
	if (vrsta == 0) {
		std::cout << "placing: " << x << " " << y << '\n';
		klikovi.push_back(std::make_pair(x, y));
	} else if (vrsta == 1) {
		std::cout << "inverting odsijecanje\n";
		odsijecanje = !odsijecanje;
	}
}

int main(int argc, char * argv[]) {

	int width = 97, height = 97;
	Rect rect = { // OpenGL coords
		{ width / 4, height / 4 },
		{ 3 * width / 4, 3 * height / 4 }
	};
	std::cout << argv[0] << std::endl;
	Grafika grafika(width, height, glm::vec3(0, 0, 0), argv[0]);

	//prosljeduje se pokazivac na funkciju. Ta funkcija ce se pozvati kada korisnik pritisne misem
	grafika.registrirajFunkcijuZaKlikMisa(klikMisa);

	while (grafika.trebaZatvoriti()) {

		//osvjezavanje pozicija razlicitih objekata i fizikalna simulacija bi se izvrsavala u ovom dijelu

		grafika.pobrisiProzor();

		//iscrtavanje uzorka sahovnice
		for (int i = 0; i < height; i += 1)
			for (int j = 0; j < width; j += 1) {
				if ((i + j) % 2 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.1, 0.1, 0.1));
				if (i % 10 == 0 && j % 10 == 0)
					grafika.osvijetliFragment(i, j, glm::vec3(0.15, 0.15, 0.2));
			}

		// draw odsijecanje rectangle
		if (odsijecanje) {
			for (int x = rect.p1.x ; x <= rect.p2.x ; x++) {
				grafika.osvijetliFragment(x, rect.p1.y, glm::vec3(0, 1, 0));
				grafika.osvijetliFragment(x, rect.p2.y, glm::vec3(0, 1, 0));
			}

			for (int y = rect.p1.y ; y < rect.p2.y ; y++) {
				grafika.osvijetliFragment(rect.p1.x, y, glm::vec3(0, 1, 0));
				grafika.osvijetliFragment(rect.p2.x, y, glm::vec3(0, 1, 0));
			}
		}

		//iscrtavanje pritisnutih fragmenata
		//ishodiste koordinatnog sustava za operacijski sustav je u gornjem lijevom kutu, a za OpenGL je u donjem lijevom, pa je potrebno okrenuti predznak
		for (std::size_t i = 0; i < klikovi.size(); i++) {
			if (i % 2 == 1) {
				iscrtajLiniju(grafika,
					klikovi[i-1].first,
					height - klikovi[i-1].second - 1,
					klikovi[i].first,
					height - klikovi[i].second - 1,
					rect
				);
			}
		}

		// draw a pixel for the starting point of a line if it exists
		if (klikovi.size() % 2 == 1) {
			std::size_t index = klikovi.size() - 1;
			grafika.osvijetliFragment(klikovi[index].first, height - klikovi[index].second - 1, glm::vec3(1, 0, 0));
		}

		grafika.iscrtajRaster();

		//kako program ne bi trosio previse resursa, na kraj petlje se moze dodati poziv funkcije za cekanje
		//kao npr Sleep ili od c++11 na dalje this_thread::sleep_for(chrono::milliseconds(16));
	}   
	

    return EXIT_SUCCESS;
}
