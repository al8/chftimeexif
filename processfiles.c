#include <windows.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>
#include <string.h>
#include <sys/utime.h>

#include <malloc.h>

#include "exifread.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE !FALSE
#endif


/* these two must match, original values: "%04d%02d%02d_%02d%02d-%02d", 16 */
#define FILE_FORMAT_STRING "%04d%02d%02d_%02d%02d-%02d"
#define FILE_FORMAT_SIZE 16


static int is_digits(char *s) {
	while (*s != '\0') {
		if (*s < '0' || *s > '9') {
			return 0;
		}
		s++;
	}
	return 1;
}

typedef struct list_entry_s {
   char* o_name;
   char* n_name;
   char* ext;
   int ordinal;
	time_t exiftime;
} list_entry_t;

#define MAX_LIST_SIZE 1000000
list_entry_t process_list[MAX_LIST_SIZE];
int process_list_size = 0; /* number of files to process */
int max_org_file_len = 0; /* stores the maximum size of the original file size so printing will look better */


int alpha_compare(const void *arg1, const void *arg2) {
   /* Compare all of both strings: */
   return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
}

int o_alpha_compare(const void *arg1, const void *arg2) {
   list_entry_t a, b;
   a = *((list_entry_t*)arg1); b = *((list_entry_t*)arg2);
   return _stricmp(a.o_name, b.o_name);
}
int n_alpha_compare(const void *arg1, const void *arg2) {
   int rc;
   list_entry_t a, b;
   a = *((list_entry_t*)arg1); b = *((list_entry_t*)arg2);

   rc = _stricmp(a.n_name, b.n_name);
   if (rc == 0) {
      return _stricmp(a.ext, b.ext);
   } else {
      return rc;
   }
}


int list_sort() {
   qsort( (void *)process_list, (size_t)process_list_size, sizeof( list_entry_t ), n_alpha_compare );
   return 0;
}
int list_print() {
   int i;
   char format_string[64];
	sprintf(format_string, "%%%ds    exiftime:%%s\n", max_org_file_len);
   for (i = 0; i < process_list_size; i++)
      printf(format_string, process_list[i].o_name, process_list[i].n_name);
   return 0;
}
int list_add_ext() {
   int i;
   for (i = 0; i < process_list_size; i++) {
      char *ext;
      ext = process_list[i].o_name + strlen(process_list[i].o_name);
      while(*ext != '.') {
         ext--;
         if (ext == process_list[i].o_name) {
            ext = NULL;
            break;
         }
      }
      if (ext != NULL) {
         //process_list[i].n_name = realloc(process_list[i].n_name, (strlen(process_list[i].n_name)+strlen(ext)+1)*sizeof(char));
         //strcat(process_list[i].n_name, ext);
         process_list[i].ext = realloc(process_list[i].ext, (strlen(ext)+1)*sizeof(char));
         strcpy(process_list[i].ext, ext);
      }
   }
   return 0;
}



int list_insert(char* o_name, char* n_name, time_t exiftime) {
   int len;
   if (process_list_size >= MAX_LIST_SIZE) return -1;
   process_list[process_list_size].o_name = malloc((strlen(o_name)+1) * sizeof(char));
   process_list[process_list_size].n_name = malloc((strlen(n_name)+1) * sizeof(char));
   process_list[process_list_size].ext = malloc(1);
   len = strlen(o_name);
   if (len > max_org_file_len) max_org_file_len = len;
   memcpy(process_list[process_list_size].o_name, o_name, len+1);
   strcpy(process_list[process_list_size].n_name, n_name);
   strcpy(process_list[process_list_size].ext, "");
   process_list[process_list_size].ordinal = 0;
	process_list[process_list_size].exiftime = exiftime;
   process_list_size++;
   return 0;
}
int list_clear() {
   int i;
   for (i = 0; i < process_list_size; i++) {
      free(process_list[i].o_name);
      free(process_list[i].n_name);
      free(process_list[i].ext);
   }
   process_list_size = 0;
   return 0;
}
int list_process(char verbose_print) {
   int errors = 0;
   int i;
   char output_format_string[64];
   /* construct output format string */
   sprintf(output_format_string, "%%%ds   ->   %%s\n", max_org_file_len);
   for (i = 0; i < process_list_size; i++) {
		struct _utimbuf newtimestamps;
		{
			newtimestamps.modtime = process_list[i].exiftime;
			newtimestamps.actime = process_list[i].exiftime;
		}
      /* int rename( const char *oldname, const char *newname ); */
		//int _utime( const char *filename, struct _utimbuf *times);
      if (_utime(process_list[i].o_name, &newtimestamps)) {
         /* EACCES  Path specifies directory or read-only file 
				EINVAL  Invalid times argument 
				EMFILE  Too many open files (the file must be opened to change its modification time) 
				ENOENT  Path or filename not found 
			*/

			printf("%s --> %s : ", process_list[i].o_name);
			switch(errno) {
				case EACCES: printf("EACCES (Path specifies directory or read-only file) errno: %d\n", errno); break;
				case EINVAL: printf("EINVAL (Invalid times argument) errno: %d\n", errno); break;
				case EMFILE: printf("EMFILE (Too many open files) errno: %d\n", errno); break;
				case ENOENT: printf("ENOENT (Path or filename not found) errno: %d\n", errno); break;
				default: printf("errno: %d\n", errno);
			}
         perror("_utime"); /* there is an error */
         errors++;
      } else {
         /* no error */
         if (verbose_print) printf(output_format_string, process_list[i].o_name, process_list[i].n_name, "");
      }
   }
   
   return errors;
}



int process_file (struct _finddata_t *target_file) {
   if (!strcmp(target_file->name, ".") || !strcmp(target_file->name, "..")) return 0; /* no "." or ".." files */
   if (target_file->attrib & _A_SUBDIR) return 0; /* no subdirectories */
   {
		char exifvalue[128];
		time_t exiftime;
		exifvalue[0] = '\0';
		exiftime = get_exifdate(target_file->name, exifvalue);
		if (exiftime > 0) {
			//printf("%s\n", exifvalue);
			list_insert(target_file->name, exifvalue, exiftime);
		} else {
			printf("%s has no exif filedate or exif filedate (%s) invalid.\n", target_file->name, exifvalue);
		}
		/*
      struct tm *file_time;

      char new_name[1024];

      file_time = localtime( &target_file->time_write );

      // printf("%s", asctime(file_time));
      sprintf(new_name, FILE_FORMAT_STRING, file_time->tm_year+1900, file_time->tm_mon+1, file_time->tm_mday,
         file_time->tm_hour, file_time->tm_min, file_time->tm_sec);

      list_insert(target_file->name, new_name);
		*/
   }
   return 0;
}


int parse_cmd_line(char* cmd_line) {
   return 0;
}

void main( int argc, char** argv )
{
   char fpattern[40];

   if (0) {
      char c;
      printf("%s\n", GetCommandLine());
      scanf("%c", &c);
      exit(0);
   }
   

   if (argc > 2) {
      printf("USAGE: %s <file_pattern>\n", argv[0]);
      exit(-1);
   } else if (argc == 2) {
      strcpy(fpattern, argv[1]);
   } else {
      strcpy(fpattern, "*");
   }



   /* find files */
   {
	   struct _finddata_t target_file;
	   long hFile;

		/* Find first file in current directory */
      if( (hFile = _findfirst( fpattern, &target_file )) == -1L ) {
         printf( "No files matching pattern '%s'.\n", fpattern);
         exit(-1); /* */
      } else {
         process_file(&target_file); /* process first file */
         /* Find the rest of the files */
         while( _findnext( hFile, &target_file ) == 0 ) {
            process_file(&target_file);
         }
         _findclose( hFile );
      }
   }

   /* processing */
   {
      /* sort list. fix duplicates. add extensions. */
      //list_sort();
      //list_add_ext();
   }


   /* if multiple files, ask if want this to happen. */
   {
      //if (process_list_size > 0) {
         {
            char c, c1 = 0;
ask_to_process_again:
            printf("Are you sure you want to process %d files match by pattern '%s'?\n(Y/N, or L to list files) ", process_list_size, fpattern);
            scanf("%c", &c);
            if (c != '\n') scanf("%c", &c1);
            if (c == 'L' || c == 'l') {
               printf("the following files will have its date changed:\n"); list_print();
               goto ask_to_process_again;
            }
            if (c != 'y' && c != 'Y' /*|| c1 != '\n'*/) { /* user response must by "Y\n" or "y\n" */
               printf("Aborted by user\n");
               exit(-1);
            }
         }
      //}
   }

   
   if (1) { /* do renaming */
      int errors = 0;
      errors = list_process(FALSE);
      if (errors) printf("\nTotal errors: %d\n", errors);
   } else {
      printf("\nprocess temporarily disabled\n");
   }

}





/*
time_t = 00:00:00 GMT, Jan. 1, 1970

struct stat {
          mode_t   st_mode;     // File mode (type, perms) 
          ino_t    st_ino;      // Inode number 
          dev_t    st_dev;      // ID of device containing 
                                // a directory entry for this file 
          dev_t    st_rdev;     // ID of device 
                                // This entry is defined only for 
                                // char special or block special files 
          nlink_t  st_nlink;    // Number of links 
          uid_t    st_uid;      // User ID of the file's owner 
          gid_t    st_gid;      // Group ID of the file's group 
          off_t    st_size;     // File size in bytes 
          time_t   st_atime;    // Time of last access 
          time_t   st_mtime;    // Time of last data modification 
          time_t   st_ctime;    // Time of last file status change 
                                // Times measured in seconds since 
                                // 00:00:00 UTC, Jan. 1, 1970 
          long     st_blksize;  // Preferred I/O block size 
          blkcnt_t st_blocks;   // Number of 512 byte blocks allocated
}
*/


/*
int main2 (int argc, char* argv[]) {
   if (0 && argc != 2) {
      printf("USAGE: %s <filename>\n", argv[0]);
      exit(-1);
   }
   

   { // get file detail
      struct stat stat_buf;
      char fname[40];
      //strcpy(fname, argv[1]);
      strcpy(fname, "test.txt");
      
      if (stat(fname, &stat_buf)) {
         //printf("error stat: %d\n", errno);
         perror("stat"); //strerror(errno)
         exit(-1);
      }

      {
         struct tm *newtime;
         newtime = localtime( &stat_buf.st_mtime );
         //newtime = gmtime( &stat_buf.st_mtime );
         printf("%s", asctime(newtime));
         printf("date: %04d-%02d-%02d\n", newtime->tm_year+1900, newtime->tm_mon, newtime->tm_mday);
         printf("time: %02d:%02d:%02d\n", newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
      }

   }


   {
   //int rename( const char *oldname, const char *newname );
   }
   
   return 0;
}


*/