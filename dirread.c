#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

int main() {
  DIR * stream = opendir("some_dir");
  struct dirent * entry = readdir(stream);

  printf("Statistics for directory: %s\n\n","some_dir");
  int num_directory = 0;
  int num_file = 0;

  char * directory_names[200];
  char * file_names[200];

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

  printf("Directories:\nTotal Direcotry Size: %d\n",directory_size);
  int i;
  for (i=0;i<num_directory;i++){
    printf("%s\n",directory_names[i]);
  }
  printf("\nRegular Files:\n");
  for (i=0;i<num_file;i++){
    char * current_file_name = file_names[i];
    printf("%s\n",current_file_name);
  }
  return 0;
}
