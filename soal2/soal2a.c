#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

long matriks1[4][3], matriks2[3][6]; 

void scanning(int baris, int column, long matriks[baris][column]) //ambil input matriks
{
    printf("Input Matriks : \n");
    int i;
    int j;
    for (i = 0; i < baris; i++) {
        for (j = 0; j < column; j++) {
            scanf("%ld", &matriks[i][j]);
        
        }
    }
}

void printMatriks(int baris, int column, long matriks[baris][column]) //print matriks
{
    printf("Matriks %d %d:\n",baris, column);
    int i;
    int j;
    for (i = 0; i < baris; i++) {
        for (j = 0; j < column; j++) {
            printf("%ld\t", matriks[i][j]);
        }
        printf("\n");
    }
}

void main() {
    key_t key = 2468; 
    long *hasil;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    hasil = shmat(shmid, NULL, 0);

    scanning(4,3,matriks1);
    scanning(3,6,matriks2);

    long jumlah = 0;

    int i;
    int j;
    int k;
    for (i = 0; i < 4; i++) { //perkalian matriks
        for (j = 0; j < 6; j++) {
            for (k = 0; k < 3; k++) {
            jumlah = jumlah + matriks1[i][k] * matriks2[k][j];
            }
            *(hasil + 6*i + j) = jumlah;
            jumlah = 0;
        }
    }

    printMatriks(4,3,matriks1);
    printf("\n");

    printMatriks(3,6,matriks2);
    printf("\n");

    printf("Hasil Perkalian Matriks:\n");
    for (int i = 0; i < 4; i++) { //print matriks
        for (int j = 0; j < 6; j++) {
            printf("%ld\t", *(hasil + 6*i + j));
        }
        printf("\n");
    }

    sleep(10);

    shmdt(hasil);
}
