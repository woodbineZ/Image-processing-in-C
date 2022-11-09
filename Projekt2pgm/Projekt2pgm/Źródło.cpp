#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct obraz {
	int szerokosc, wysokosc, jasnosc;
	char obraz_nazwa[20];
	int **tab_piksele;
};

int odczytaj_z_pliku(FILE *plik) {
	char komentarz[2];
	int piksel;

	while (1)
	{
		if (fscanf(plik, "%d", &piksel) == 1)
			return piksel;
		if (fscanf(plik, "%1s", komentarz) == 1 && komentarz[0] == '#')
			fscanf(plik, "%*[^\n]");
	}
}
struct obraz* realokuj(struct obraz* tablica_obrazow, int nowy_rozmiar) {
	if (nowy_rozmiar == 1) {
		tablica_obrazow=(struct obraz*)calloc(nowy_rozmiar, sizeof (struct obraz));
	}
	else {
		tablica_obrazow = (struct obraz*)realloc(tablica_obrazow, nowy_rozmiar * sizeof(struct obraz));
	}
	return tablica_obrazow;
}
struct obraz *wczytaj(struct obraz *tablica_obrazow, int *nr) {
	FILE *plik;
	int i, j, w;
	char nazwa_pliku[30];

	scanf("%29s", nazwa_pliku);
	plik = fopen(nazwa_pliku, "r");
	if (plik != NULL) {
		(*nr) = (*nr) + 1;
		w = (*nr)-1;
		tablica_obrazow[w].szerokosc = odczytaj_z_pliku(plik);
		tablica_obrazow[w].wysokosc = odczytaj_z_pliku(plik);
		tablica_obrazow[w].jasnosc = odczytaj_z_pliku(plik);

		tablica_obrazow[w].tab_piksele = (int**)calloc(tablica_obrazow[w].wysokosc, sizeof(int));
		for (i = 0; i < tablica_obrazow[w].wysokosc; i = i + 1) {
			tablica_obrazow[w].tab_piksele[i] = (int*)calloc(tablica_obrazow[w].szerokosc, sizeof(int));
		}
		for (i = 0; i < tablica_obrazow[w].wysokosc; i = i + 1) {
			for (j = 0; j < tablica_obrazow[w].szerokosc; j = j + 1) {
				tablica_obrazow[w].tab_piksele[i][j] = odczytaj_z_pliku(plik);
			}
		}
		strcpy(tablica_obrazow[w].obraz_nazwa, nazwa_pliku);
		fclose(plik);
		printf("Zostal wczytany obraz\n");
	}
	else
	{
		printf("Nie ma takiego obrazu\n");
	}

	return tablica_obrazow;
}

void histogram(struct obraz *tablica_obrazow) {
	FILE *plik;
	int i, j, wartosc;
	int* tab_histogramu;
	tab_histogramu = (int*)calloc(tablica_obrazow->jasnosc + 1, sizeof(int));

	for (i = 0; i < tablica_obrazow->wysokosc; i = i + 1) {
		for (j = 0; j < tablica_obrazow->szerokosc; j = j + 1) {
			wartosc = tablica_obrazow->tab_piksele[i][j];
			tab_histogramu[wartosc] = tab_histogramu[wartosc] + 1;
		}
	}
	plik = fopen("histogram.csv", "w");
	for (i = 0; i < tablica_obrazow->jasnosc + 1; i = i + 1) {
		fprintf(plik, "%d\n", tab_histogramu[i]);
	}
	fclose(plik);
	free(tab_histogramu);
}

struct obraz *progowanie(struct obraz *tablica_obrazow) {
	int i, j, prog;
	printf("Prosze podac prog (zakres 1-255)");
	scanf("%d", &prog);

	if (prog > 0 && prog < 255) {
		for (i = 0; i < tablica_obrazow->wysokosc; i = i + 1) {
			for (j = 0; j < tablica_obrazow->szerokosc; j = j + 1) {
				if (tablica_obrazow->tab_piksele[i][j] > prog) {
					tablica_obrazow->tab_piksele[i][j] = 255;
				}
				else {
					tablica_obrazow->tab_piksele[i][j] = 0;
				}
			}
		}
		printf("Progowanie zostalo wykonane dla progu:%d \n", prog);
	}
	else {
		printf("Podano wartosc nie jest z przedzialu 1-99, prosze sprobowac ponownie\n");
	}
	return tablica_obrazow;
}

struct obraz *negatyw(struct obraz *tablica_obrazow) {
	int i, j;

	for (i = 0; i < tablica_obrazow->wysokosc; i = i + 1) {
		for (j = 0; j < tablica_obrazow->szerokosc; j = j + 1) {
			tablica_obrazow->tab_piksele[i][j] = tablica_obrazow->jasnosc - tablica_obrazow->tab_piksele[i][j];

		}
	}
	return tablica_obrazow;
}


struct obraz* obracanie(struct obraz* tablica_obrazow) {

	int i, j;
	int** tab_obrazow;
	int wysokosc = tablica_obrazow->wysokosc;
	int szerokosc = tablica_obrazow->szerokosc;

	tab_obrazow = (int**)calloc(szerokosc, sizeof(int*));
	for (i = 0; i < szerokosc; i = i + 1) {
		tab_obrazow[i] = (int*)calloc(wysokosc, sizeof(int));
	}

	for (i = 0; i < wysokosc; i = i + 1) {
		for (j = 0; j < szerokosc; j = j + 1) {
			tab_obrazow[j][wysokosc - 1 - i] = tablica_obrazow->tab_piksele[i][j];
		}
	}
	for (i = 0; i < tablica_obrazow->wysokosc; i = i + 1)
		free(tablica_obrazow->tab_piksele[i]);
	free(tablica_obrazow->tab_piksele);

	tablica_obrazow->szerokosc = wysokosc;
	tablica_obrazow->wysokosc = szerokosc;
	tablica_obrazow->tab_piksele = tab_obrazow;
	return tablica_obrazow;
}

struct obraz *zaszumianie(struct obraz* tablica_obrazow) {
	srand(time(NULL));
	int i, j, procentowo, wartosc, prog = 20;			// procentowo - wartosc od 0 do 100 / gdy wartosc 1 to bia³e, gdy wartosc 0 to czarne
	int wysokosc = tablica_obrazow->wysokosc;
	int szerokosc = tablica_obrazow->szerokosc;

	for (i = 0; i < wysokosc; i = i + 1) {
		for (j = 0; j < szerokosc; j = j + 1) {
			procentowo = rand() % 100;
			wartosc = rand() % 2;
			if (wartosc == 1) wartosc = 255;
			if (procentowo < prog) tablica_obrazow->tab_piksele[i][j] = wartosc;
		}
	}
	return tablica_obrazow;
}

struct obraz *filtracja(struct obraz* tablica_obrazow) {
	int i, j, k;
	int a, A = 0, temp;
	int wysokosc = tablica_obrazow->wysokosc;
	int szerokosc = tablica_obrazow->szerokosc;
	int *tab;

	tab = (int*)calloc(9, sizeof(int));

	for (i = 1; i < tablica_obrazow->wysokosc - 1; i = i + 1) {
		for (j = 1; j < tablica_obrazow->szerokosc - 1; j = j + 1) {

			tab[0] = tablica_obrazow->tab_piksele[i - 1][j - 1];
			tab[1] = tablica_obrazow->tab_piksele[i - 1][j];
			tab[2] = tablica_obrazow->tab_piksele[i - 1][j + 1];
			tab[3] = tablica_obrazow->tab_piksele[i][j - 1];
			tab[4] = tablica_obrazow->tab_piksele[i][j];
			tab[5] = tablica_obrazow->tab_piksele[i][j + 1];
			tab[6] = tablica_obrazow->tab_piksele[i + 1][j - 1];
			tab[7] = tablica_obrazow->tab_piksele[i + 1][j];
			tab[8] = tablica_obrazow->tab_piksele[i + 1][j + 1];

			for (k = 0; k<9; k = k + 1) {
				A = 0;
				for (a = 0; a<8; a = a + 1) {
					if (tab[a]>tab[a + 1]) {
						temp = tab[a + 1];
						tab[a + 1] = tab[a];
						tab[a] = temp;
						A = 1;
					}
				}
				if (A == 0) break;
			}
			tablica_obrazow->tab_piksele[i][j] = tab[4];

		}
	}
	free(tab);

	return tablica_obrazow;
}



void stworz(struct obraz* tablica_obrazow) {
	FILE *plik;
	int i, j;
	plik = fopen("Nowy.pgm", "w");
	fprintf(plik, "P2\n#nazwa\n%d %d\n%d\n", tablica_obrazow->szerokosc, tablica_obrazow->wysokosc, tablica_obrazow->jasnosc);
	for (i = 0; i < tablica_obrazow->wysokosc; i = i + 1) {
		for (j = 0; j < tablica_obrazow->szerokosc; j = j + 1) {
			fprintf(plik, "%d ", tablica_obrazow->tab_piksele[i][j]);
		}
		fprintf(plik, "\n");
	}
	fclose(plik);
}

int main() {
	int j = 0, b = 0, i = 0, ilosc = 0, nr, powiekszenie = 2;
	char opcja[50];
	struct obraz *tablica_obrazow;
	struct obraz *aktywny_obraz;

	

	printf("Jakub Makuch\nPrzetwarzanie obrazow\n\n");
	printf("Prosze wybrac jedna z opcji:\na - Wychodzenie z programu\nb - Dodanie obrazu\nd - Usuniecie ostatniego obrazu\ne - Wyswietlenie listy obrazow\nf - Wybranie aktywnego obrazu\n\nCzynnosci ktore mozna wykonac na aktywnym obrazie\n\ng - Obrot aktywnego obrazu o 90*\nh - Utworzenie histogramu dla aktywnego obrazu\ni - Wykonanie operacji progowania dla zadanego progu\nj - Utworzenie negatywu z aktywnego obrazu\nk - Wykonanie operacji zaszumiania typu sol i pieprz\nl - Filtrowanie medianowe w oknie 3x3\nm - Zapis do pliku\n");

	while (1) {
	odniesienie:
		scanf("%49s", opcja);

		switch (opcja[0]) {
		case 'a': {
			printf("Wychodzenie z programu\n");
		}; break;
		case 'b': {
			printf("Prosze podac nazwe obrazu .pgm\n");
			if (ilosc == 0) {
				tablica_obrazow = (struct obraz*)calloc(ilosc+1, sizeof(struct obraz));
			}
			else if(ilosc>0){
				tablica_obrazow = (struct obraz*)realloc(tablica_obrazow, (ilosc+1) * sizeof(struct obraz));
			}
			tablica_obrazow = wczytaj(tablica_obrazow, &ilosc);
			if (ilosc == 0) {
				free(tablica_obrazow);
			}
			else if (ilosc > 0) {
				tablica_obrazow = (struct obraz*)realloc(tablica_obrazow, (ilosc) * sizeof(struct obraz));
			}
		}; break;
		case 'd': {
			ilosc = ilosc-1;
			if (ilosc == 0) {
				free(tablica_obrazow);
			}
			else if (ilosc > 0) {
				tablica_obrazow = (struct obraz*)realloc(tablica_obrazow, (ilosc) * sizeof(struct obraz));
			}
			
			printf("Ostatni obraz zostal usuniety\n");
		}; break;
		case 'e': {
			printf("Lista obrazow:\n");
			for (i = 0; i < ilosc; i = i + 1) {
				printf("%s\n", tablica_obrazow[i].obraz_nazwa);
			}
		}; break;
		case 'f': {
			printf("Prosze podac numer obrazu");
			scanf("%d", &i);
			aktywny_obraz = &tablica_obrazow[i];
			printf("Wybrano aktywny obraz nr %d\n", i);
		}; break;
		case 'g': {
			aktywny_obraz = obracanie(aktywny_obraz);
			printf("Aktywny obraz zostal obrocony o 90*\n");
		}; break;
		case 'h': {
			histogram(aktywny_obraz);
			printf("Histogram zostal utworzony\n");
		}; break;
		case 'i': {
			tablica_obrazow = progowanie(aktywny_obraz);
		}; break;
		case 'j': {
			tablica_obrazow = negatyw(aktywny_obraz);
			printf("Negatyw zostal utworzony\n");
		}; break;
		case 'k': {
			tablica_obrazow = zaszumianie(aktywny_obraz);
			printf("Obraz zostal zaszumiony szumem typu sol i pieprz\n");
		}; break;
		case 'l': {
			tablica_obrazow = filtracja(aktywny_obraz);
			printf("Odfiltrowywanie zostalo wykonane\n");
		}; break;
		case 'm': {
			stworz(aktywny_obraz);
			printf("Plik zostal zapisany\n");
		}; break;
		default: {
			printf("Nie ma takiej opcji, prosze sprobowac ponownie\n");
		}; break;
		}
		if (opcja[0] == 'a') break;
	}

	for (j = 0; j < ilosc; j = j + 1)
	{
		for (i = 0; i < tablica_obrazow[j].wysokosc; i = i + 1) {
			free(tablica_obrazow[j].tab_piksele[i]);
		}
		free(tablica_obrazow[j].tab_piksele);
		tablica_obrazow = NULL;
	}
	system("PAUSE");
	return 0;
}