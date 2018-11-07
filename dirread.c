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

void check_error(){
  if (errno > 0){
    printf("Error %d: %s\n\n",errno,strerror(errno));
    exit(0);
  }
}

int length_of_num(long num){
  int length = 0;
  while(num>=10){
    num /= 10;
    length++;
  }
  return length;
}

void print_sizebytes(long long bytes){
  long long original = bytes;
  char * names[9] = {"B","KB","MB","GB","TB","PB","EB","ZB","YB"};
  int sizeofboi = 0;
  while(bytes > 1000){
    bytes /= 1000;
    sizeofboi++;
  }
  printf("%lld %s or %lld bytes\n",bytes,names[sizeofboi],original);
}

void print_file_details(char * file_name, char * dir_path, int max_size, int max_user, int max_group) {
  char file_path[4096];
  sprintf(file_path,"%s/%s",dir_path,file_name);

  struct stat file_stat;
  stat(file_path,&file_stat);
  check_error();

  S_ISDIR(file_stat.st_mode) ? printf("d") : printf("-");
  char * rwx [8] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
  int permissions = file_stat.st_mode;
  int other = permissions % 8;
  permissions /= 8;
  int group = permissions % 8;
  permissions /= 8;
  int user = permissions % 8;
  printf("%s%s%s ",rwx[user],rwx[group],rwx[other]);

  printf("%-*s %-*s ",
         max_user,
         getpwuid(file_stat.st_uid)->pw_name,
         max_group,
         getgrgid(file_stat.st_gid)->gr_name);

  printf("%*ld",max_size+1,file_stat.st_size);

  char time[16];
  strncpy(time,ctime(&file_stat.st_atime)+3,13);
  printf("%s %s\n",time,file_name);
}

void print_dir(char * dir_path){
  DIR * stream = opendir(dir_path);
  check_error();

  struct dirent * entry = readdir(stream);
  check_error();

  int total_things = 0;
  int max_filename_length = 0;

  while (entry) {
    int filename_length = strlen(entry->d_name);
    if (max_filename_length < filename_length) {
      max_filename_length = filename_length;
    }
    total_things++;
    entry = readdir(stream);
    check_error();
  }

  stream = opendir(dir_path);
  check_error();
  entry = readdir(stream);
  check_error();

  printf("\nStatistics for directory: %s\n",dir_path);
  int num_files = 0;
  int max_filesize_length = 0;
  int max_user_length = 0;
  int max_group_length = 0;
  long long directory_size = 0;
  char file_names[total_things][256];

  while (entry) {
    char current_path[4096];
    sprintf(current_path,"%s/%s",dir_path,entry->d_name);

    struct stat cur_stat;
    if (stat(current_path,&cur_stat) == 0) {
      int size = length_of_num(cur_stat.st_size);
      if (size > max_filesize_length){
        max_filesize_length = size;
      }
      int user_length = strlen(getpwuid(cur_stat.st_uid)->pw_name);
      if (user_length > max_user_length){
        max_user_length = user_length;
      }
      int group_length = strlen(getgrgid(cur_stat.st_gid)->gr_name);
      if (group_length > max_group_length){
        max_group_length = group_length;
      }

      strcpy(file_names[num_files],entry->d_name);
      if (!S_ISDIR(cur_stat.st_mode)) {
         directory_size += cur_stat.st_size;
      }
      num_files++;
      entry = readdir(stream);
      check_error();
    }
    else {
      check_error();
    }
  }

  closedir(stream);
  free(entry);

  int i;

  for (i=0; i<num_files; i++){
    print_file_details(file_names[i],dir_path,max_filesize_length,max_user_length,max_group_length);
  }

  printf("\nTotal Directory Size: ");
  print_sizebytes(directory_size);
  printf("\n");
}

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
