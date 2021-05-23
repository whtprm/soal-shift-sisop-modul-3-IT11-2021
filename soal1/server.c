#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PORT 8080


char cookie[100];

int checkLogin(char *kredensial, char *password)
{
    FILE *fptr;

    fptr = fopen("akun.txt", "r+");
    if (fptr == NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }
    char line[256];
    char user[1024] = {0};
    sprintf(user, "%s:%s", kredensial, password);
    while (fgets(line, sizeof(line) - 1, fptr))
    {
        if (strcmp(user, line) == 0)
        {
            strcpy(cookie, user);
            return 1;
        }
    }
    fclose(fptr);
    return 0;
}

void runningLog(char *message)
{
    FILE *fp;
    fp = fopen("running.log", "a+");
    if (fp == NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }
    strtok(cookie, "\n");
    fprintf(fp, "%s (%s)\n", message, cookie);
    fclose(fp);
}

void registerAkun(char *kredensial, char *password)
{

    FILE *fptr;
    fptr = fopen("akun.txt", "a+");
    if (fptr == NULL)
    {
      perror("[-]Error in reading file.");
        exit(1);  
    }
    fprintf(fptr, "%s:%s\n", kredensial, password);
    fclose(fptr);
}

void createFolderFile()
{
    mkdir("FILES", 0777);
    FILE *fp;
    fp = fopen("files.tsv", "a+");
    fclose(fp);

    FILE *fptr;
    fptr = fopen("akun.txt", "a+");

    fclose(fptr);
}

void addtoDatabase(int new_socket, char *publisher, char *tahun_publikasi, char *filenamepath)
{

    char tmp[1024] = {0};
    strcpy(tmp, "FILES/");
    strcat(tmp, filenamepath);
    
    char file_length[1024] = {0}, buffer[1024] = {0}, file_content[1024] = {0};
    memset(buffer, 0, sizeof(buffer));
    long fsize;

    // recieve checkfile
    char check[2] = {0};
    read(new_socket, check, 2);
    int checkyuk = atoi(check);
    if (checkyuk)
    {
        // recieve file size

        int valread = read(new_socket, file_length, 1024);
        if (valread < 1)
        {
            char *errorpesan = "error gan";
        }
        fsize = strtol(file_length, NULL, 0);
        memset(file_length, 0, sizeof(file_length));

        // recieve file content
        int i = 0;
        while (i < fsize)
        {
            int valread = read(new_socket, buffer, 1024);
            if (valread < 1)
            {
                char *errorpesan = "error gan";
            }
            strcat(file_content, buffer);
            memset(buffer, 0, sizeof(buffer));
            i += 1024;
        }

        memset(buffer, 0, sizeof(buffer));
        FILE *fpr;
        fpr = fopen(tmp, "w");
        fprintf(fpr, "%s", file_content);
        memset(file_content, 0, sizeof(file_content));
        fclose(fpr);
        

        FILE *fp;
        fp = fopen("files.tsv", "a+");
        fprintf(fp, "%s\t%s\t%s\n", publisher, tahun_publikasi, tmp);
        fclose(fp);
        char message[30] = "Tambah: ";
        strcat(message, filenamepath);
        runningLog(message);

        char *berhasil = "Berhasil ditambahkan";
        send(new_socket, berhasil, strlen(berhasil), 0);
    }
    else
    {
        char *gagal = "Gagal ditambahkan";
        send(new_socket, gagal, strlen(gagal), 0);
    }
}
