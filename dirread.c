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

char * * alphaboi(char * * list,int size){
  int c = 0;
  int i = 0;
  for(;c < size-1;c++){
    for(i = c ;i < size;i++){
      if(0<strcmp(list[c],list[i])){
        char * temp = list[c];
        list[c] = list[i];
        list[i] = temp;
      }
    }
  }

  return list;
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

void to_rwx(int permissions, char * output) {
  int i;
  char * possible_rwx [8] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
  strcat(output,"-");
  for (i=0; i<3; i++){
    strcat(output,possible_rwx[permissions%8]);
    permissions /= 8;
  }
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
void print_dir(char * filename){
  // First Pass
  DIR * stream = opendir(filename);
  struct dirent * entry = readdir(stream);
  int total_things = 0; // Diretory and files
  int max_filename_length = 0;

  while (entry) {
    total_things++;
    int filename_length = strlen(entry->d_name);
    if (max_filename_length < filename_length){
      max_filename_length = filename_length;
    }
    entry = readdir(stream);
  }

  DIR * new_stream = opendir(filename);
  struct dirent * new_entry = readdir(new_stream);

  printf("\nStatistics for directory: %s\n",filename);
  int num_directory = 0;
  int num_file = 0;

  char * directory_names[total_things];
  char * file_names[total_things];

  long long directory_size = 0;

  int max_filesize_length = 0;

  while (new_entry){
    //It is a directory
    if (new_entry->d_type == 4) {
      directory_names[num_directory] = new_entry->d_name;
      num_directory++;
    }
    //It is a file
    else {
      char * current_file_name = new_entry->d_name;
      file_names[num_file] = current_file_name;
      num_file++;

      struct stat file_stat;
      stat(current_file_name,&file_stat);
      directory_size += file_stat.st_size;
      int size = length_of_num(file_stat.st_size);
      if (size > max_filesize_length){
        max_filesize_length = size;
      }
    }
    new_entry = readdir(new_stream);
  }
  closedir(new_stream);

  //Sorting
  char * * new_directory_names = alphaboi(directory_names,num_directory);
  char * * new_file_names = alphaboi(file_names,num_file);

  printf("\nDirectories:\n");
  int i;
  for (i=0;i<num_directory;i++){
    printf("%s\n",new_directory_names[i]);
  }

  printf("\nFiles:\n");
  for (i=0;i<num_file;i++){
    if (strcmp(new_file_names[i],"") == 0){
      print_file_details("README.md",max_filesize_length);
    }
    else {
      print_file_details(new_file_names[i],max_filesize_length);
    }
    printf("\n");
  }

  printf("\nTotal Directory Size: ");
  print_sizebytes(directory_size);
}

int main() {
  print_dir(".");
  return 0;
}
