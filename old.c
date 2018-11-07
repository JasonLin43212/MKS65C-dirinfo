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
#include <errno.h>

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
  printf("%ld %s %s",file_stat.st_size,time,filename);

  return file_stat.st_size;
}

void printarr(char * * name,int size){
  int c = 0;
  for(;c < size;c++){
    printf("%s \n",name[c]);
  }
}

//two passes
void print_dir(char * dir_name){
  // First Pass
  DIR * stream = opendir(dir_name);
  if (errno > 0){
    printf("Error #%d: %s\n\n",errno,strerror(errno));
    exit(0);
  }

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

  DIR * new_stream = opendir(dir_name);
  struct dirent * new_entry = readdir(new_stream);

  printf("\nStatistics for directory: %s\n",dir_name);
  int num_directory = 0;
  int num_file = 0;

  char * * directory_names = calloc(total_things,sizeof(char *));
  char * * file_names = calloc(total_things,sizeof(char *));

  long long directory_size = 0;

  int max_filesize_length = 0;

  while (new_entry){
    char * current_file_name = malloc(255);
    sprintf(current_file_name,"%s",new_entry->d_name);

    char * current_file_path = malloc(4096);
    sprintf(current_file_path,"%s/%s",dir_name,new_entry->d_name);

    struct stat file_stat;
    if (stat(current_file_path,&file_stat) == 0){
      int size = length_of_num(file_stat.st_size);
      if (size > max_filesize_length){
        max_filesize_length = size;
      }
    }
    else{
      printf("Error #%d: %s\n\n",errno,strerror(errno));
      exit(1);
    }
    //It is a directory
    if (S_ISDIR(file_stat.st_mode)) {
      directory_names[num_directory] = new_entry->d_name;
      num_directory++;
    }
    //It is a file
    else {
      file_names[num_file] = current_file_name;
      directory_size += file_stat.st_size;
      num_file++;
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
  for (i=0;i<num_file-1;i++){

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

//argc should be 1 cuz that is the filename
int main(int argc, char * argv[]) {
  if (argc == 1) {
    printf("Hello, to run this program, please type this with args separated by spaces:\n\n\tmake all run args=\"<directory_path_1> <directory_path_2> ...\"\n\n");
  }
  else {
    int i=1;
    while(i < argc){
      print_dir(argv[i]);
      i++;
    }
  }
  return 0;
}
