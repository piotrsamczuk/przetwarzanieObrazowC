#define _CRT_SECURE_NO_WARNINGS
#define NAMELEN 255 //maksymalna dlugosc nazwy pliku
#define LINELEN 4098 //maksymalna dlugosc bufora linijki dla czytanego pliku
#define FILTERSIZE 9 //musi miec pierwiastek
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <crtdbg.h>
#include <math.h>

typedef struct image
{
	int width;
	int height;
	int depth;
	int** pixels;
	int* pixelsBase;
	char type[2];
	char name[NAMELEN];
}image;

int getFilename(char* filename)
{
	if (filename == NULL)
	{
		printf("\nBlad alokacji pamieci.");
		return -1;
	}
	(void)scanf("%s", filename);
	if (filename == NULL)
	{
		printf("\nBlad wpisywnia nazwy pliku.");
		return -1;
	}
	return 1;
}

int commentHandler(char* filename, image* img)
{
	FILE* fptr = fopen(filename, "r");
	FILE* newptr = fopen("temp.pgm", "w");
	if (fptr == NULL)
	{
		printf("\nBlad wczytywania pliku");
		return -1;
	}
	char* line = calloc(LINELEN, sizeof(char));
	if (line == NULL)
	{
		printf("\nBlad alokacji pamieci");
		return -1;
	}
	while (fgets(line, LINELEN, fptr) != NULL)
	{
		int n = strlen(line);
		for (int i = 0; i < n; i++)
		{
			if (line[i] == '#')
			{
				for (int j = i; j < n; j++)
				{
					line[j] = '\0';
				}
				i = n;
			}
		}
		fputs(line, newptr);
	}
	fclose(fptr);
	fclose(newptr);
	if (remove(filename) == 0)
	{
		printf("\nOK. Usunieto komentarze.");
		(void)rename("temp.pgm", filename);
	}
	else
		printf("\nNie udalo sie usunac pliku.");
	free(line);
	return 1;
}

int loadImage(image* img, char* filename)
{
	FILE* fptr = fopen(filename, "r");
	if (fptr == NULL)
	{
		printf("\nBlad wczytywania pliku");
		return -1;
	}
	char* line = calloc(LINELEN, sizeof(char));
	if (line == NULL)
	{
		printf("\nBlad alokacji pamieci");
		return -1;
	}
	printf("\nPodaj alias/pseudonim dla tego obrazu\n");
	(void)scanf("%s", &(img->name)); //podanie przez uzytkownika nazwy dla wczytywanego obrazu
	if (img->name == NULL)
	{
		free(line);
		printf("\nBlad wpisywnia nazwy.");
		return -1;
	}
	if (fgets(line, LINELEN, fptr) == NULL)
	{
		free(line);
		return -1;
	}
	if (line[0] != 'P' || line[1] != '2')
	{
		free(line);
		return -1;
	}
	else
	{
		img->type[0] = line[0];
		img->type[1] = line[1];
	}

	if (fscanf(fptr, "%d %d %d", &(img->width), &(img->height), &(img->depth)) != 3) //czytanie rozmiarow obrazu
	{
		printf("\nNie odczytano wymiarow i glebi.");
		free(line);
		return -1;
	}

	img->pixelsBase = malloc(img->width * img->height * sizeof(int));
	img->pixels = malloc(img->height * sizeof(int*));
	for (int i = 0; i < img->height; i++)
	{
		img->pixels[i] = &img->pixelsBase[i * img->width];
	}


	for (int i = 0; i < img->height; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			(void)fscanf(fptr, "%d ", &(img->pixels[i][j]));
		}
	}

	free(line);
	fclose(fptr);
	return 1;
}

int saveImage(image* img, char* filename)
{
	FILE* fptr = fopen(filename, "w");
	if (fptr == NULL)
	{
		printf("\nBlad wczytywania pliku");
		return -1;
	}
	for (int i = 0; i < 2; i++)
		fprintf(fptr, "%c", img->type[i]);
	fprintf(fptr, "\n%d %d %d\n", img->width, img->height, img->depth);

	for (int i = 0; i < img->height; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			fprintf(fptr, "%d ", img->pixels[i][j]);
		}
		fprintf(fptr, "\n");
	}

	fclose(fptr);
	return 1;
}

void rotateImage(image* img, int k)
{
	if (img->height == img->width) //gdy obraz jest kwadratowy
	{
		for (int i = 0; i < img->height; i++)	//transponowanie macierzy
		{
			for (int j = i + 1; j < img->width; j++)
			{
				int temp = img->pixels[i][j];
				img->pixels[i][j] = img->pixels[j][i];
				img->pixels[j][i] = temp;
			}
		}
		int buff = img->height;		//zamiana szerokosci z wysokoscia
		img->height = img->width;
		img->width = buff;
		for (int i = 0; i < img->height; i++) //odbicie lustrzane
		{
			for (int j = 0; j < (img->width) / 2; j++)
			{
				int temp = img->pixels[i][j];
				img->pixels[i][j] = img->pixels[i][(img->width) - 1 - j];
				img->pixels[i][(img->width) - 1 - j] = temp;
			}
		}
	}
	else
	{
		int* arrBase;
		int** arr;
		arrBase = malloc(img->width * img->height * sizeof(int));
		arr = malloc(img->width * sizeof(int*));
		for (int i = 0; i < img->width; i++)
		{
			arr[i] = &arrBase[i * img->height];
		}

		for (int i = 0; i < img->height; i++) //transponowanie i zapisanie w pomocniczej tablicy
		{
			for (int j = 0; j < img->width; j++)
			{
				arr[j][i] = img->pixels[i][j];
			}
		}
		free(img->pixels);
		img->pixels = malloc(img->width * sizeof(int*));
		for (int i = 0; i < img->width; i++)
		{
			img->pixels[i] = &img->pixelsBase[i * img->height];
		}

		int buff = img->height;		//zamiana szerokosci z wysokoscia
		img->height = img->width;
		img->width = buff;

		for (int i = 0; i < img->height; i++)
		{
			for (int j = 0; j < img->width; j++)
			{
				img->pixels[i][j] = arr[i][j];
			}
		}
		free(arr);
		free(arrBase);
	}

	printf("\nObrocono obraz.\n");
	Sleep(1000);
}

void negativeEffect(image* img)
{
	for (int i = 0; i < img->height; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			img->pixels[i][j] = img->depth - img->pixels[i][j];
		}
	}
	printf("\nNalozono efekt negatywu.\n");
	Sleep(1000);
}

void noiseEffect(image* img)
{
	for (int i = 0; i < img->height; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			if ((rand() % 100) < 6)//szansa na dodanie szumu, mozna zmieniac 
			{
				if (rand() % 2 == 1)
					img->pixels[i][j] = 0;
				else
					img->pixels[i][j] = img->depth;
			}
		}
	}
	printf("\nNalozono efekt szumu soli i pieprzu.\n");
	Sleep(1000);
}

int generateHistogram(image* img, int* pixelNum, char* filename)
{
	pixelNum = malloc(img->depth * sizeof(int));
	if (pixelNum == NULL)
	{
		printf("\nAlokacja pamieci nie powiodla sie.\n");
		Sleep(1000);
		return -1;
	}
	for (int i = 0; i < img->depth; i++)
	{
		pixelNum[i] = 0;
	}
	int imgsize = img->height * img->width;
	for (int i = 0; i < imgsize; i++)
	{
		int j = img->pixelsBase[i];
		pixelNum[j] += 1;
	}

	FILE* fptr = fopen(filename, "w");
	if (fptr == NULL)
	{
		printf("\nBlad wczytywania pliku");
		return -1;
	}
	for (int i = 0; i < img->depth; i++)
	{
		fprintf(fptr, "%d;%d\n", i, pixelNum[i]);
	}
	fclose(fptr);
	free(*pixelNum);
	printf("\nStworzono histogram obrazu.\n");
	Sleep(1000);
	return 1;
}

void bubbleSort(int* arr)
{
	for (int i = 0; i < FILTERSIZE - 1; i++)
	{
		for (int j = 0; j < FILTERSIZE - i - 1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

void blurImage(image* img)
{
	int* filter = NULL;
	filter = malloc(FILTERSIZE * sizeof(int));
	int filterside = sqrt(FILTERSIZE);

	for (int i = 1; i < img->height - 1; i++)
	{
		for (int j = 1; j < img->width - 1; j++)
		{
			filter[0] = img->pixels[i - 1][j - 1];
			filter[1] = img->pixels[i - 1][j];
			filter[2] = img->pixels[i - 1][j + 1];
			filter[3] = img->pixels[i][j - 1];
			filter[4] = img->pixels[i][j];
			filter[5] = img->pixels[i][j + 1];
			filter[6] = img->pixels[i + 1][j - 1];
			filter[7] = img->pixels[i + 1][j];
			filter[8] = img->pixels[i + 1][j + 1];
			bubbleSort(filter);
			img->pixels[i][j] = filter[FILTERSIZE / 2];
		}
	}
}

int main()
{
	image img = { 0 };
	char choice = '0';
	image* base = NULL;
	int baseSize = 0;
	int num = 0;
	srand(time(NULL));
	while (choice != 'x')
	{
		system("cls");
		if (num != 0)
			printf("\nAktywny obraz: %d) %s\n", num, base[num - 1].name);
		printf("\n0)Wybor aktywnego obrazu z listy\n1)Zaladowanie obrazu z pliku do bazy\n2)Zapisanie obrazu z bazy do pliku pgm\n3)Lista zapisanych obrazow w bazie\n4)Usuniecie obrazu z tablicy\n5)Obroc obraz o 90*k stopni w prawo\n6)Efekt negatywu na obrazie\n7)Efekt soli i pieprzu na obrazie\n8)Utworzenie histogramu obrazu\n9)Filtr medianowy\nx)Wyjscie\n"); //moze cos z cls
		choice = _getch();
		switch (choice)
		{
		case '0': // wybranie aktywnego obrazu
		{
			if (baseSize < 1)
			{
				printf("\nNie ma jeszcze obrazow w bazie.\n");
				Sleep(1000);
				break;
			}
			for (int i = 0; i < baseSize; i++)
			{
				printf("\n%d)%s", i + 1, base[i].name);
			}
			printf("\nWybierz aktywny obraz z listy na ktorym beda wykonywane funkcje:\n");
			if (scanf("%d", &num) != 1)
			{
				printf("\nBlad wyboru z listy\n");
				Sleep(1000);
				num = 0;
			}
			else if (num > baseSize || num <= 0)
			{
				printf("\nTakiego obrazu nie ma na liscie.\n");
				Sleep(1000);
				num = 0;
			}
			break;
		}
		case '1': //wczytanie obrazu do struktury i bazy + obsluga komentarzy (w oryginalnym pliku nie bedzie juz komentarzy, nie jest to idealne rozwiazanie)
		{
			printf("\nPodaj nazwe pliku z rozszerzeniem z ktorego chcesz pozbyc sie komentarzy i zaladowac go do bazy.\n");
			char* filename = calloc(NAMELEN, sizeof(char));
			if (getFilename(filename) == -1)
			{
				printf("\nBlad czytania nazwy pliku.");
				Sleep(1000);
				break;
			}
			if (commentHandler(filename, &img) == -1)
			{
				printf("\nBlad usuwania komentarzy z pliku.\n");
				Sleep(1000);
				break;
			}

			int* temp = NULL;
			baseSize++;
			temp = realloc(base, baseSize * sizeof(image));
			if (temp == NULL)
			{
				printf("\nRealokacja pamieci nie powiodla sie\n");
				baseSize--;
				break;
			}
			else
				base = temp;
			if (loadImage(&base[baseSize - 1], filename) == -1)
			{
				//free(temp);
				break;
			}
			free(filename);
			//free(temp); nie wiem czemu nie dziala jak zwalniam pamiec
			break;
		}
		case '2': // zapisanie obrazu z bazy do pliku pgm
		{
			printf("\nPodaj nazwe pliku z rozszerzeniem .pgm do ktorego zostanie zapisany obraz.\n");
			char* filename = calloc(NAMELEN, sizeof(char));
			if (getFilename(filename) == -1)
			{
				printf("\nBlad czytania nazwy pliku.");
				Sleep(1000);
				break;
			}
			if (base == NULL)
			{
				printf("\nBaza jest pusta, nie ma co zapisac.\n");
				Sleep(1000);
				break;
			}
			if (num <= baseSize && num > 0)
			{
				if (saveImage(&base[num - 1], filename) != 1)
				{
					printf("\nZapisano jako %s!\n", filename);
					Sleep(1000);
					break;
				}
			}
			else
			{
				printf("\nNie wybrano obrazu z bazy lub nie istnieje on w bazie.\n");
				Sleep(1000);
				break;
			}
			free(filename);
			break;
		}
		case '3': // wyswietlenie bazy
		{
			if (baseSize < 1)
			{
				printf("\nBrak obrazow w bazie.\n");
				Sleep(1000);
				break;
			}
			for (int i = 0; i < baseSize; i++)
			{
				printf("\n%d)%s", i + 1, base[i].name);
			}
			Sleep(1000);
			break;
		}
		case '4': // usuniecie obrazu z bazy danych, jesli uzywam funkcji free na temp to cos nie dziala
		{
			if (num <= baseSize && num > 0)
			{
				if (baseSize < 1)
				{
					printf("\nBrak obrazow w bazie\n");
					Sleep(1000);
					break;
				}
				free(base[num - 1].pixels);
				free(base[num - 1].pixelsBase);
				if (num < baseSize) //jesli usuwamy ostatni element to nie ma potrzeby zamiany adresow
				{
					for (int i = num - 1; i < baseSize - 1; i++)
					{
						base[i] = base[i + 1];
					}
				}
				int* temp = NULL;
				baseSize--;
				temp = realloc(base, baseSize * sizeof(image));
				if (temp == NULL)
				{
					printf("\nRealokacja pamieci nie powiodla sie\n");
					Sleep(1000);
					baseSize++;
					break;
				}
				else
					base = temp;
				num = 0; //resetowanie wyboru
				//free(temp); // nie dziala jak zwalniam pamiec 
			}
			else
			{
				printf("\nNie wybrano obrazu z bazy lub nie istnieje on w bazie.\n");
				Sleep(1000);
				break;
			}
			break;
		}
		case '5':
		{
			int k = 0;
			printf("\nIle razy w prawo obrocic obraz? Podaj liczbe: ");
			if (scanf("%d", &k) != 1)
			{
				printf("\nBlednie wprowadzona dana\n");
				Sleep(1000);
				break;
			}
			else
			{
				if (num <= baseSize && num > 0)
				{
					rotateImage(&base[num - 1], k);
				}
				else
				{
					printf("\nNie wybrano obrazu z bazy lub nie istnieje on w bazie.\n");
					Sleep(1000);
					break;
				}
			}
			break;
		}
		case '6':
		{
			if (num <= baseSize && num > 0)
			{
				negativeEffect(&base[num - 1]);
			}
			else
			{
				printf("\nNie wybrano obrazu z bazy lub nie istnieje on w bazie.\n");
				Sleep(1000);
				break;
			}
			break;
		}
		case '7':
		{
			if (num <= baseSize && num > 0)
			{
				noiseEffect(&base[num - 1]);
			}
			else
			{
				printf("\nNie wybrano obrazu z bazy lub nie istnieje on w bazie.\n");
				Sleep(1000);
				break;
			}
			break;
		}
		case '8':
		{
			if (num <= baseSize && num > 0)
			{
				printf("Podaj nazwe pliku w ktorym zostanie zapisany histogram dla tego obrazu (razem z rozszerzeniem .csv): ");
				char* filename = calloc(NAMELEN, sizeof(char));
				if (getFilename(filename) == -1)
				{
					printf("\nBlad czytania nazwy pliku.");
					Sleep(1000);
					break;
				}
				int* histogram = NULL;
				if (generateHistogram(&base[num - 1], histogram, filename) == -1)
				{
					printf("\nNie udalo sie wygenerowac histogramu dla tego obrazu.\n");
					free(filename);
					Sleep(1000);
					break;
				}
				free(filename);
			}
			else
			{
				printf("\nNie wybrano obrazu z bazy lub nie istnieje on w bazie.\n");
				Sleep(1000);
				break;
			}
			break;
		}
		case '9':
		{
			if (num <= baseSize && num > 0)
			{
				blurImage(&base[num - 1]);
			}
			else
			{
				printf("\nNie wybrano obrazu z bazy lub nie istnieje on w bazie.\n");
				Sleep(1000);
				break;
			}
			break;
		}
		case 'x':
		{
			if (baseSize > 0)
			{
				for (int i = 0; i < baseSize; i++)
				{
					free(base[i].pixels);
					free(base[i].pixelsBase);
				}
				free(base);
			}
			break;
		}
		}
	}
	_CrtDumpMemoryLeaks();
	return 0;
}