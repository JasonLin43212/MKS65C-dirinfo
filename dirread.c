#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

//check error no

int length_of_num(long num){
  int length = 0;
  while(num>=10){
    num /= 10;
    length++;
  }
  return length;
}

void print_sizebytes(long long bytes){
  char * names[9] = {"B","KB","MB","GB","TB","PB","EB","ZB","YB"};
  int sizeofboi = 0;
  while(bytes > 1000){
    bytes /= 1000;
    sizeofboi++;
  }
  printf("%lld %s\n",bytes,names[sizeofboi]);
}

void tree_branch(int i) {
  int j;
  for (j=0; j<i; j++){
    printf("| ");
  }
}

void to_rwx(int permissions, char * output) {
  int i;
  char temp[10];
  char * possible_rwx [8] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
  strcat(output,"-");
  for (i=0; i<3; i++){
    strcat(output,possible_rwx[permissions%8]);
    permissions /= 8;
  }
  strcpy(temp, output);
}

void timify(char * old_time, char * output){
  old_time += 3;
  strncpy(output,old_time,13);
}

long long print_file_details(char * filename, int max_size){
  struct stat file_stat;
  char file_name[strlen(filename)];
  strcpy(file_name,filename);

  stat(filename,&file_stat);
  struct passwd * pw = getpwuid(file_stat.st_uid);
  struct group * gr = getgrgid(file_stat.st_gid);

  char rwx_output[12] = "";
  to_rwx(file_stat.st_mode, rwx_output);

  char time[16] = "";
  timify(ctime(&file_stat.st_atime),time);

  printf("%s %s %s ",rwx_output,pw->pw_name,gr->gr_name);

  int i;
  for (i=0; i<max_size - length_of_num(file_stat.st_size); i++){
    printf(" ");
  }
  printf("%ld %s %s",file_stat.st_size,time,file_name);
  return file_stat.st_size;
}

//two passes
long long print_dir(char * filename, int depth){
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

  printf("\nStatistics for directory: %s\n\n",filename);
  int num_directory = 0;
  int num_file = 0;

  char * directory_names[total_things];
  char * file_names[total_things];

  long long directory_size = 0;

  int max_filesize_length = 0;

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
      num_file++;

      struct stat file_stat;
      stat(current_file_name,&file_stat);
      int size = length_of_num(file_stat.st_size);
      if (size > max_filesize_length){
        max_filesize_length = size;
      }
    }
    entry = readdir(stream);
  }
  closedir(stream);

  printf("\nDirectories:\n ");
  int i;
  for (i=0;i<num_directory;i++){
    printf("%s\n",directory_names[i]);
  }
  for (i=0;i<num_file;i++){
    char * current_file_name = file_names[i];
    tree_branch(depth-1);
    printf("|____");
    print_file_details(current_file_name,max_filesize_length);
    printf("\n");
  }

  printf("Total Directory Size: ");
  print_sizebytes(directory_size);

  return directory_size;
}

int main() {
  print_dir(".",1);
  return 0;
}
