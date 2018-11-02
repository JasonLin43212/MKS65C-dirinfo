#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

//check error no
void print_sizebytes(long bytes){
  char * names[9] = {"B","KB","MB","GB","TB","PB","EB","ZB","YB"};
  int sizeofboi = 0;
  while(bytes > 1000){
    bytes /= 1000;
    sizeofboi++;
  }
  printf("%ld %s\n",bytes,names[sizeofboi]);
}

//two passes
void print_dir(char * filename){
  // First Pass
  DIR * stream = opendir(filename);
  struct dirent * entry = readdir(stream);
  int total_things = 0; // Diretory and files

  while (entry) {
    total_things++;
    entry = readdir(stream);
  }

  stream = opendir(filename);
  entry = readdir(stream);

  printf("Statistics for directory: %s\n\n",filename);
  int num_directory = 0;
  int num_file = 0;

  char * directory_names[total_things];
  char * file_names[total_things];

  int directory_size = 0;

  while (entry){
    //It is a directory
    if (entry->d_type == 4) {
      directory_names[num_directory] = entry->d_name;
      num_directory++;
    }
    //It is a file
    else {
      char * current_file_name = entry->d_name;
      file_names[num_file] = current_file_name;

      struct stat file_stat;
      stat(current_file_name,&file_stat);
      directory_size += file_stat.st_size;
      num_file++;
    }
    entry = readdir(stream);
  }
  closedir(stream);

  printf("Directories:\nTotal Direcotry Size: ");
  print_sizebytes(directory_size);
  int i;
  for (i=0;i<num_directory;i++){
    printf("%s\n",directory_names[i]);
  }
  printf("\nRegular Files:\n");
  for (i=0;i<num_file;i++){
    char * current_file_name = file_names[i];
    printf("%s\n",current_file_name);
  }
}

int main() {
  print_dir("../MKS65C-stat");
  return 0;
}
