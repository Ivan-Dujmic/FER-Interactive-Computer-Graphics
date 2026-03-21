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

void iscrtajLiniju(Grafika &grafika, int x0, int y0, int x1, int y1) {
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
				grafika.osvijetliFragment(x, y, glm::vec3(0, 1, 1));
				yf += a;
				if (yf >= 0) {
					yf += correction;
					y++;
				}
			}
		} else {
			for (int x = x0 ; x <= x1 ; x++) {
				grafika.osvijetliFragment(y, x, glm::vec3(0, 1, 1));
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
				grafika.osvijetliFragment(x, y, glm::vec3(0, 1, 1));
				yf += a;
				if (yf <= 0) {
					yf += correction;
					y--;
				}
			}
		} else {
			for (int x = x0 ; x <= x1 ; x++) {
				grafika.osvijetliFragment(y, x, glm::vec3(0, 1, 1));
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

		// odsijecanje rectangle
		if (odsijecanje) {
			for (int x = width / 4 ; x <= 3 * width / 4 ; x++) {
				grafika.osvijetliFragment(x, height / 4, glm::vec3(0, 1, 0));
				grafika.osvijetliFragment(x, 3 * height / 4, glm::vec3(0, 1, 0));
			}

			for (int y = height / 4 ; y < 3 * height / 4 ; y++) {
				grafika.osvijetliFragment(width / 4, y, glm::vec3(0, 1, 0));
				grafika.osvijetliFragment(3 * width / 4, y, glm::vec3(0, 1, 0));
			}
		}

		//iscrtavanje pritisnutih fragmenata
		//ishodiste koordinatnog sustava za operacijski sustav je u gornjem lijevom kutu, a za OpenGL je u donjem lijevom, pa je potrebno okrenuti predznak
		for (std::size_t i = 0; i < klikovi.size(); i++) {
			grafika.osvijetliFragment(klikovi[i].first, height - klikovi[i].second -1, glm::vec3(0.6, 0.2, 0));
			if (i % 2 == 1) {
				iscrtajLiniju(grafika,
					klikovi[i-1].first,
					height - klikovi[i-1].second - 1,
					klikovi[i].first,
					height - klikovi[i].second - 1
				);
			}
		}

		grafika.iscrtajRaster();

		//kako program ne bi trosio previse resursa, na kraj petlje se moze dodati poziv funkcije za cekanje
		//kao npr Sleep ili od c++11 na dalje this_thread::sleep_for(chrono::milliseconds(16));
	}   
	

    return EXIT_SUCCESS;
}
