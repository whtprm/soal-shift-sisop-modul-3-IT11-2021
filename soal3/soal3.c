#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> //format of directory entries
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h> //define toupper case
#include <stdbool.h>
#define _GNU_SOURCE  //untuk mendapatkan fungsi DR_DIR dan DT_REG
#define MAX 10000

bool x = true; //definisi variable untuk keberhasilan direktori
char workdir[MAX], *fullpath[MAX]; //workdir untuk menampung current work directory
pthread_t tid[3]; //thread sejumlah argumen yaitu 3

//menjadikan semua variable uppercase
char *uppercase(char *huruf) {
        int i=0;
        char kata[30], upper, result[30]; //deklarasi variabel 
        sprintf(kata, "%s", huruf);
        while (kata[i]) {
            upper = kata[i];
            result[i] = toupper(upper);//conver lowercase to uppercase
        i++;
        } 
        char *res =result;
        return res;
}

//untuk fungsi category masing2 directori    
void* directory(void *arg)
{
    char abs1[MAX], *abs2[MAX]; //untuk mengambil nama file dalam absolute path pada directory
    char hasilabs[MAX]; //hasil nama file dan ekstensi dr absolute path
    int pjgpath = 0, pjgeks = 0; //mendeklarasi panjang path dan panjang ekstensi
    char *namapath, *namafile;
    char upcase[MAX];
    char s[5]="/", t[5]= ".";
    char pindaharg[MAX]; //untuk memindah file menuju dirpath yang dituju
    char pindahdir[MAX];
    
    //mendapatkan nama file dari panjang path yang diambil 
    strcpy(abs1,arg);
    namapath = strtok(abs1, s); // untuk memisahkan string yang diparsing dengan karakter
    while(namapath != NULL){
        abs2[pjgpath] = namapath;
        pjgpath++;
        namapath = strtok(NULL, s);
    }
    strcpy(hasilabs,abs2[pjgpath-1]);
    
    //mendapatkan ekstensi file 
    namafile = strtok(abs2[pjgpath-1],t);
    while(namafile !=  NULL){
        fullpath[pjgeks] = namafile;
        pjgeks++;
        namafile = strtok(NULL, t);
    }

    //mengecek setiap huruf dr fullpath
    strcpy(upcase,fullpath[pjgeks-1]);
    char *hasilupcase = uppercase(upcase);
   
    //dirrent.h
    DIR *d; //type data directory stream 
    struct dirent *dir;  //return information about directory entries
    d = opendir(workdir); //membuka current working directory
    char fullpath[MAX];
    int check = 0;

    //apakah ada direktori yang bernama ektensi file
    if(pjgeks > 1){
        if(dir){
            while((dir = readdir(d)) != NULL){ //membcaca directory yang masuk
                if(strcmp(dir->d_name, hasilupcase) == 0 && dir->d_type == DT_DIR){ //d_type directory entry type
                    check = 1; //d_name point to string that gives the name of directory
                    break;
                }
            }
        }
        //jika belum, buat directory
        if(check == 0){
            strcpy(fullpath,workdir);
            strcat(fullpath,"/");
            strcat(fullpath,hasilupcase);
            mkdir(fullpath, 0333);
        } 
        }
        //jika tidak ada maka masuk ke unknown
    else{
        strcpy(fullpath,workdir);
        strcat(fullpath,"/");
        strcat(fullpath,"UNKNOWN");
        mkdir(fullpath, 0333);
        }

	// untuk memnidahkan file menuju path yang dituju
    strcpy(pindaharg,arg);
    strcpy(pindahdir,workdir);
    strcat(pindahdir,"/"); 
    if(pjgeks == 1){
        strcat(pindahdir,"UNKNOWN");
    }else{
        strcat(pindahdir,hasilupcase);
    }
    strcat(pindahdir,"/");
    strcat(pindahdir,hasilabs); //memasukkan nama file dari hasil absolute path ke direktori baru yyang dibuat
    if (rename(pindaharg,pindahdir)<0){ // di rename sesuai ekstensi
        x= false; 
    }
    pjgeks = 0; //direset ke nol lagi agar bisa digunakan kembali
    pjgpath = 0;
}


int main(int argc, char *argv[]) {
    DIR *d;
    struct dirent *dir;
    char dirpath[300];
    if(getcwd(workdir, sizeof(workdir)) != NULL) { 
    printf("Current directory: %s\n", workdir);
    }
        if (strcmp(argv[1],"-f") == 0) {
            int i=0;
            for(int j = 2 ; j < argc ; j++ ){
                pthread_create(&(tid[i]),NULL,directory,argv[j]); 
                pthread_join(tid[i],NULL);
                if (x){
                    printf("file %d : Berhasil di kategorikan\n",j-1);
                } else{
                    printf("file %d : sad gagal\n",j-1);
                    }
                i++;
            }
        }
        
         else if(strcmp(argv[1],"*") == 0){
            d = opendir(workdir);
            if(d){
                while((dir = readdir(d)) != NULL){
                    if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0) ) //skip . dan ..
                        continue;
                    strcpy(dirpath,workdir);
                    strcat(dirpath,"/");
                    strcat(dirpath,dir->d_name);
                    int i  = 0;
                    if(dir->d_type == DT_REG){ //reguler file
                        pthread_create(&(tid[i]),NULL,directory,dirpath); 
                        pthread_join(tid[i],NULL);
                        i++;
                    }
                }

            }
        }

        else(strcmp(argv[1], "-d") == 0); {
                d = opendir(argv[2]);
                if(d){
                    while ((dir = readdir(d)) != NULL){
                            if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0) )
                                continue;
                            strcpy(dirpath,argv[2]);
                            strcat(dirpath,"/");
                            strcat(dirpath,dir->d_name);
                            int i = 0;
                            bool x = true;
                            if(dir->d_type == DT_REG){ //directory entries for reg files
                                pthread_create(&(tid[i]),NULL,directory,dirpath); 
                                pthread_join(tid[i],NULL);
                                    if (x){
                                        printf("file %d : direktori sukses disimpan\n",i+1);
                                    } else{
                                        printf("file %d : sad gagal\n",i+1);
                                        }
                                i++;
                        }
                    }
                }
            }

    return 0; 
}