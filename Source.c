#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dirent.h"

typedef struct Cvor {
	int frekvencija;
	char data;
	struct Cvor* levi;
	struct Cvor* desni;
	struct Cvor* sledeci;
}Cvor;
typedef struct HuffmanKod {
	char data;
	char* kod;
	int duzinaKoda;
	struct HuffmanKod* sledeci;
}HKod;

HKod* lista = NULL;
Cvor* pq = NULL;
Cvor* huffmanRoot = NULL;
int frekvencijaPojavljivanja[256] = { 0 };


HKod* napraviNoviHKod(char data, char* kod, int duzinaKoda) {
	HKod* novi = (HKod*)malloc(sizeof(HKod));
	novi->data = data;
	novi->kod = kod;
	novi->duzinaKoda = duzinaKoda;
	novi->sledeci = NULL;
	return novi;
}
void ubaciUListu(HKod* novi, HKod** lista) {
	if (*lista == NULL) {
		(*lista) = novi;
		return;
	}

	HKod* pom = (*lista);
	(*lista) = novi;
	novi->sledeci = pom;
}
HKod* pronadjiCvorPoSlovu(char data, HKod* lista) {
	HKod* pom = lista;
	while (pom != NULL) {
		if (pom->data == data) {
			return pom;
		}
		pom = pom->sledeci;
	}
	return NULL;
}
HKod* pronadjiCvorPoKodu(char kod[], HKod* lista) {
	HKod* pom = lista;
	while (pom->sledeci != NULL) {
		if (strcmp(pom->kod, kod) == 0) {
			return pom;
		}
		pom = pom->sledeci;
	}
	return NULL;
}
void ispisiKodIzListe(HKod* lista) {
	HKod* pom = lista;
	while (pom != NULL) {
		printf("%c - %s\n", pom->data, pom->kod);
		pom = pom->sledeci;
	}
}



Cvor* napraviNovi(int frekvencija, char data) {
	Cvor* novi = (Cvor*)malloc(sizeof(Cvor));
	if (novi == NULL) return NULL;

	novi->sledeci = NULL;
	novi->frekvencija = frekvencija;
	novi->levi = novi->desni = NULL;
	novi->data = data;
	return novi;
}
void ubaciPosle(Cvor* p, Cvor* q) {
	Cvor* pom = p->sledeci;
	p->sledeci = q;
	q->sledeci = pom;
}
void ubaciURed(Cvor** lista, Cvor* novi) {
	//pronadji de da ga roknem
	Cvor* pomocni = *lista;
	Cvor* pom2 = NULL;

	while (pomocni != NULL && pomocni->frekvencija <= novi->frekvencija) {
		pom2 = pomocni;
		pomocni = pomocni->sledeci;
	}
	//insertAfter
	if (pom2 == NULL) {
		Cvor* pom3 = *lista;
		(*lista) = novi;
		novi->sledeci = pom3;
		return;
	}
	ubaciPosle(pom2, novi);
}
Cvor* izbaciIzReda(Cvor** lista) {
	if (*lista == NULL) {
		return NULL;
	}
	else if ((*lista)->sledeci == NULL) {
		Cvor* pom = (*lista);
		(*lista) = NULL;
		return pom;
	}
	Cvor* pom = (*lista);
	(*lista) = (*lista)->sledeci;

	return pom;
}
void ispis(Cvor* lista) {
	Cvor* pom = lista;

	while (pom != NULL) {
		printf("%d %c", pom->frekvencija, pom->data);
		pom = pom->sledeci;
	}
	printf("\n");
}

void dealocirajMemorijuZaListu() {
	HKod* pom = lista;
	while (pom != NULL) {
		HKod* pret = pom;
		pom = pom->sledeci;
		free(pret);
	}
}
void dealocirajMemorijuZaStablo(Cvor* root) {
	if (root != NULL) {
		dealocirajMemorijuZaStablo(root->levi);
		dealocirajMemorijuZaStablo(root->desni);
		free(root);
	}

}

void izracunajFrekvencijuPojavljivanja(){}
void generisiKodoveIzHufmanovogStabla(char arr[], int top, Cvor* root) {
	if (root->levi) {
		arr[top] = '0';
		generisiKodoveIzHufmanovogStabla(arr, top + 1, root->levi);
	}

	if (root->desni) {
		arr[top] = '1';
		generisiKodoveIzHufmanovogStabla(arr, top + 1, root->desni);
	}

	if (!root->levi && !root->desni) {
		arr[top] = '\0';
		int broj = strlen(arr);
		char* kod = (char*)malloc(sizeof(char)*broj);
		strcpy(kod, arr);
		ubaciUListu(napraviNoviHKod(root->data, kod, broj), &lista);
		return;
	}
}
void generisiHufmanovoStablo() {
	for (int i = 0; i < 256; i++) {
		if (frekvencijaPojavljivanja[i] == 0) continue;
		ubaciURed(&pq, napraviNovi(frekvencijaPojavljivanja[i], i));
	}

	//ovo radis dok ne ostane jedan cvor u listi
	while (pq->sledeci != NULL) {
		Cvor* prvi = izbaciIzReda(&pq);
		Cvor* drugi = izbaciIzReda(&pq);
		Cvor* novi = napraviNovi(prvi->frekvencija + drugi->frekvencija, '$');
		novi->levi = prvi;
		novi->desni = drugi;
		prvi->sledeci = NULL;
		drugi->sledeci = NULL;
		ubaciURed(&pq, novi);
	}

	Cvor* poslednji = izbaciIzReda(&pq);
	poslednji->sledeci = NULL;


	huffmanRoot = poslednji;
}

void enkodujTekst(char* tekst, char* fileDestination) {
	//treba popuniti tabelu frekvencije pojavljivanja slova
	//generisati hufmanovo stablo
	//generistati kodove iz hufmanovog stabla
	dealocirajMemorijuZaListu();
	dealocirajMemorijuZaStablo(huffmanRoot);
	huffmanRoot = NULL;
	lista = NULL;
	pq = NULL;

	for (int i = 0; i < 256; i++) {
		frekvencijaPojavljivanja[i] = 0;
	}
	for (int i = 0; i < strlen(tekst); i++) {
		frekvencijaPojavljivanja[tekst[i]]++;
	}

	char arr[100];
	generisiHufmanovoStablo();
	generisiKodoveIzHufmanovogStabla(arr, 0, huffmanRoot);




	FILE* fptr = fopen(fileDestination, "w");
	//upisi hufmanovo stablo/frekvenciju pojavljivanja/kodove za kasnije dekodovanje
	for (int i = 0; i < 256; i++) {
		if (frekvencijaPojavljivanja[i] != 0) {
			fprintf(fptr, "%c\n", i);
			fprintf(fptr, "%d\n", frekvencijaPojavljivanja[i]);
		}
	}
	fprintf(fptr, "0\n");
	fprintf(fptr, "%d\n", INT_MAX);
	

	int encodedBits = 0;
	fprintf(fptr, "START\n");//oznaci pocetak poruke
	for (int i = 0; i < strlen(tekst); i++) {
		HKod* kod = pronadjiCvorPoSlovu(tekst[i], lista);
		if (kod == NULL) {
			continue;
		}
		printf("%s", kod->kod);
		fprintf(fptr, "%s", kod->kod);
		encodedBits += kod->duzinaKoda; 
	}

	fclose(fptr);
	//printf("\nOriginal msg in bits: %d\n", strlen(tekst) * 8);
	//printf("Encoded msg in bits: %d", encodedBits);
}

void encodujFromFile(char* filedestinationFrom, char* filedestinationTo) {
	//izracunaj broj bitova
	FILE* fptr = fopen(filedestinationFrom, "r");
	int brojBitova = 0;
	while (getc(fptr) != EOF) {
		brojBitova++;
	
	}
	brojBitova++;
	fclose(fptr);


	char* tekst = (char*)malloc(sizeof(char) * brojBitova);
	fptr = fopen(filedestinationFrom, "r");
	fgets(tekst, brojBitova, fptr);


	enkodujTekst(tekst, filedestinationTo);

}

char* substring(char* tekst, int start, int kraj) {
	char* novi = (char *)malloc(sizeof(char) * 5);
	int indeks = 0;
	for (int i = start; i < kraj; i++) {
		novi[indeks] = tekst[i];
		indeks++;
	}
	novi[indeks] = '\0';
	return novi;
}

int daLiJeFajlTXT(char* prvi) {
	char* ekstenzija = substring(prvi, strlen(prvi) - 4, strlen(prvi));
	return strcmp(ekstenzija, ".txt") == 0? 1:0;
}

char* napraviFileDestination(char *folderDestination, char* fileName) {
	int duzinaFileDestination = (strlen(folderDestination) + strlen(fileName) + 2);
	char* fileDestination = (char*)malloc(sizeof(char) * duzinaFileDestination);
	
	int i = 0;
	for (; i < strlen(folderDestination); i++) {
		fileDestination[i] = folderDestination[i];
	}
	fileDestination[i] = '\\';
	i++;
	int j = 0;
	for(; i < duzinaFileDestination; i++) {
		fileDestination[i] = fileName[j];
		j++;
	}
	fileDestination[i] = '\0';

	return fileDestination;
}

char* generisiFilePathZaKompresiju(char* folderDestination, char* fileName) {
	char* dodatak = "-zipovan.txt";
	int noviStringLen = strlen(folderDestination) + strlen(fileName) + strlen(dodatak) + 2;
	char *novi = (char*)malloc(sizeof(char) * noviStringLen);

	int i = 0;
	for (; i < strlen(folderDestination); i++) {
		novi[i] = folderDestination[i];
	}
	novi[i] = '\\';
	i++;
	int j = 0;
	for (; i < strlen(folderDestination) + strlen(fileName) - 3; i++) {
		novi[i] = fileName[j];
		j++;
	}
	j = 0;
	for (; i < noviStringLen - 4; i++) {
		novi[i] = dodatak[j];
		j++;
	}
	novi[i] = '.';
	i++;
	novi[i] = 't';
	i++;
	novi[i] = 'x';
	i++;
	novi[i] = 't';
	i++;
	novi[i] = '\0';

	return novi;

}

void kompresijaSvihFajlovaIzFoldera(char* folderDestination) {
	DIR* directory;
	struct dirent* entry;
	directory = opendir(folderDestination);

	if (directory == NULL) {
		printf("Error opening directory\n");
		return;
	}
	while ((entry = readdir(directory)) != NULL) {
		if (daLiJeFajlTXT(entry->d_name)) {
			char* fileDestination = napraviFileDestination(folderDestination, entry->d_name);
			char* fileDestinationZaKompresiju = generisiFilePathZaKompresiju(folderDestination, entry->d_name);
			encodujFromFile(fileDestination, fileDestinationZaKompresiju);
			
		}
	}
}




void decodeTekst(char* tekst, Cvor* root) {
	int i = 0;
	Cvor* pom = root;
	printf("\n");
	while(tekst[i] != '\0'){
		pom = root;
		while (pom) {
			if (tekst[i] == '0') {
				pom = pom->levi;
			}
			else if (tekst[i] == '1') {
				pom = pom->desni;
			}
			i++;
			if (pom->levi == NULL && pom->desni == NULL) {
				printf("%c", pom->data);
				break;
			}
		}
	}
}
void decodeFromFile(char* fileDestination) {
	FILE* fptr = fopen(fileDestination, "r");
	//resetujemo frekvenciju pojavljivanja i da unesemo nove vrednosti u tabelu
	for (int i = 0; i < 256; i++) {
		frekvencijaPojavljivanja[i] = 0;
	}
	while (1) {
		char slovo;
		int frekvencija = 0;
		fscanf(fptr, "%c\n", &slovo);
		fscanf(fptr, "%d\n", &frekvencija);
		//printf("%c %d\n", slovo, frekvencija);
		if (frekvencija == INT_MAX) break;
		frekvencijaPojavljivanja[(int)slovo] = frekvencija;
	}
	char arr1[100];
	fscanf(fptr, "%s\n", arr1);
	
	

	//1.iz frekvencije pojavljivanja generisati hufmanovo stablo i generisi kodove iz Hufmanovog stabla

	//1.1 napisati f-ju koja ce prethodno da ocisti sve
	dealocirajMemorijuZaListu();
	dealocirajMemorijuZaStablo(huffmanRoot);
	huffmanRoot = NULL;
	lista = NULL;
	pq = NULL;
	

	generisiHufmanovoStablo();
	char arr[100];
	generisiKodoveIzHufmanovogStabla(arr, 0, huffmanRoot);



	//2.procitati ostatak fajla i dekodovati ga
	//2.1 izracunati broj bitova koje tekst ima
	int brojBitova = 0;
	while (fgetc(fptr) != EOF) {
		brojBitova++;
	}
	fclose(fptr);
	printf("\nBROJ BITOVA JE %d\n", brojBitova);
	//2.2. uneti sve bitove za dekodovanje u string
	fptr = fopen(fileDestination, "r");
	while (1) {
		char slovo;
		int frekvencija = 0;
		fscanf(fptr, "%c\n", &slovo);
		fscanf(fptr, "%d\n", &frekvencija);
		//printf("%c %d\n", slovo, frekvencija);
		if (frekvencija == INT_MAX) break;
	}
	char arr2[100];
	fscanf(fptr, "%s\n", arr2);

	char* tekstString = (char*)malloc(sizeof(char)*brojBitova);
	fscanf(fptr, "%s\n", tekstString);

	printf("\n%s\n", tekstString);
	decodeTekst(tekstString, huffmanRoot);
}






int main(int argc, char *argv[]) {

	if (argc == 1) {
		printf("Niste uneli nijedan parametar\n");
		printf("Unesi -help za dodatne informacije\n");
		return 1;
	}
	else if (argc == 2 && strcmp(argv[1], "-help") == 0) {
		printf("----------------------------HELP----------------------------\n");
		printf("Za dekompresiju podataka:\n");
		printf(".\\source filepath -d\n");
		printf("Za kompresiju podataka:\n");
		printf(".\\source filepathFrom filepathTo\n");
		printf("Za kompresiju svih fajlova iz foldera:\n");
		printf(".\\source folderpath -r\n");
		printf("-------------------------------------------------------------\n");
	}
	else if (argc == 3) {
		if (strcmp(argv[2], "-d") == 0) {
			decodeFromFile(argv[1]);
		}
		else if (strcmp(argv[2], "-r") == 0) {
			kompresijaSvihFajlovaIzFoldera(argv[1]);
		}
		else {
			encodujFromFile(argv[1], argv[2]);
		}
	}

	return 0;
}