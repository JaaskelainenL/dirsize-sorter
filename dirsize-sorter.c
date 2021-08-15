// A program which lists all the files in a directory and sorts by size.

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define MAXFILES 2000

// Holds the name and the size of a read file.
typedef struct File{
   char name[1024];
   unsigned long size;
}File;


File *storedData; // Store all read files, to be sorted later
int storedFiles = 0;  // Amount of read files
unsigned long totSize = 0; // Total size of files
int read = 1; // Needs to be 1 for the program to read files (used as a halt for the loop)




// Add to storedData and allocate more memory to it.
// Also stops the reading loop when the maximum filecount is reached. (the user can decide if they want to continue)
void addData(File newData){

   storedData = (File*)realloc(storedData, 1032*(storedFiles+1)); // the size of a File type is 1032 bytes
   storedData[storedFiles++] = newData;


   // If the maximum readamount is reached, ask the user if they want to continue.
   if(storedFiles == MAXFILES){
      printf("\nNOTE! \n The default maximum amount of files (%i) has been reached. This program might run into issues when handling many files. This program is not meant to handle a huge amount of data.\nDo you want to continue reading files (y/n)? : ",MAXFILES);
      char response = getchar();

      switch(response){
         case 'y':   printf("Continuing... \n"); 
                     break;

         case 'n':   printf("Stopped scanning files at %i\n", MAXFILES); 
                     read = 0; 
                     break;

         default:    printf("Something else than (y/n) was entered. Continuing is probably not a good idea so: Stopped scanning files at %i\n", MAXFILES);
                     read = 0;
                     break;
      }

   }
}


void bubbleSort(){
   
   for(int i= 0; i < storedFiles; i++){
     for(int j = 0; j < storedFiles; j++){
        if(storedData[j].size < storedData[i].size){
           File temp;
           temp = storedData[j];
           storedData[j] = storedData[i];
           storedData[i] = temp;
        }

     }
   }
}


// Takes the file size and returns a more human readable version.
// For example 1572864 -> 1.5MiB
char* formatBytes(unsigned long size){
   char *result = (char*) malloc(128);

   if(size >= 1073741824){ // GiB
      if(size % 1073741824 == 0)
         sprintf(result, "%d GiB", size / 1073741824);
      else
         sprintf(result, "%.1f GiB", (float) size / 1073741824);

   } else if(size >= 1048576){ // MiB
      if (size % 1048576 == 0)
         sprintf(result, "%d MiB", size / 1048576);
      else
         sprintf(result, "%.1f MiB", (float) size / 1048576);   
   } else if(size >= 1024){ // kiB

      if (size % 1024 == 0)
         sprintf(result, "%d kiB", size / 1024);
      else
         sprintf(result, "%.1f kiB", (float) size / 1024);         

   } else{ // B
      sprintf(result,"%lu B", size);
   }

   return result;


}


// Get the size of the file given
unsigned long getSize(char fileName[]){
   FILE* f = fopen(fileName, "r");

   if(f == NULL)
      return 0;
   
   fseek(f, 0L, SEEK_END);

   unsigned long size = (unsigned long)ftell(f);

   fclose(f);

   return size;

}

// Scan a directory, including subdirectories
void scanDir(char* d){

   DIR *dir;
   struct dirent *f;
   
   if ((dir = opendir (d)) != NULL) {
   
   // Loop through the files in the directory
   
      while (((f = readdir (dir))!=NULL) && read) {

         if(strcmp(f->d_name,".")==0 || strcmp(f->d_name,"..")==0){
            continue;
         }
         if(f->d_type == DT_DIR){

               // Create a new pointer char to store a string with directory + new directory

               char *d2;

               d2 = malloc(strlen(d)+strlen(f->d_name)+256);
               
               strcpy(d2,d);

               if(d2[strlen(d2)-1] != '/')
                  strcat(d2,"/");

               strcat(d2,f->d_name);

               scanDir(d2);

               free(d2);
         }
         else{
               // Create a new pointer char to store a new string with the directory + filename
               char *d2;

               d2 = malloc(strlen(d)+strlen(f->d_name)+256);
               
               strcpy(d2,d);

               if(d2[strlen(d2)-1] != '/')
                  strcat(d2,"/");

               strcat(d2,f->d_name);

               unsigned long size = getSize(d2);

               if(size<=0)
                  continue;

               totSize += size;

               // Create a File and store it in storedDate   
               File readFile;

               if(strlen(d2) <= 1024)
                  strcpy(readFile.name, d2);

               else
                  strncpy(readFile.name, d2,1024);
               
               readFile.size = size;
               addData(readFile);
               

               free(d2);
         }


      }
         closedir (dir);

   } else {
      // In case the directory cannot be opened
      perror ("");

   }

}



// Pass the arguments and call the other functions
int main(int argc, char *argv[]){ 

   char *dir; 

   if(argc > 1){
      dir = argv[1];
   } else
      dir = ".";

   // A small indicator that the program is in use (the scanning may take a while if the user has a lot of files to scan)
   printf("Scanning %s...\n",dir);
   scanDir(dir);

   printf("Sorting %i files...\n", storedFiles);
   bubbleSort();

   // Print the sorted order of files
   for(int i = 0; i < storedFiles; i++){
      printf("#%i (%s) %s \n",i+1, formatBytes(storedData[i].size), storedData[i].name);
   }

   // Print the total size of the directory
   printf("Total size: %s \n", formatBytes(totSize));


   free(storedData);

   return 0;
}
 
