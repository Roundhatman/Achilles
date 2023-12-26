#include <stdio.h>
#include <cstring>

#define MEM_SIZE 4096
#define DESTPORT 80

char stdReadStream[MEM_SIZE];
char stdWriteStream[MEM_SIZE];

unsigned int fhReadFile(char *filename){
    FILE* inf = fopen(filename, "r");

    // Get filesize
    fseek(inf, 0, SEEK_END);
    size_t fsize = ftell(inf);

    // Read file
    char* infBuff = new char[fsize];
    rewind(inf);
    fread(infBuff, sizeof(char), fsize, inf);

    strcpy(stdReadStream, infBuff);
    delete infBuff;
    fclose(inf);
    return fsize;
}

unsigned int fhWriteFile(char *filename){
    FILE *otf;
    otf = fopen(filename, "w");

   if(otf == NULL) return 0;

   fprintf(otf, "%s", stdWriteStream);
   fclose(otf);

   return 1;
}
