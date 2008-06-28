//  original original original exif code: Writen by Kuren Charles <kuren@kuren.org>  
//  1/22/2000



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE !FALSE
#endif

struct exif_tag
{
   char does_exist;
   char name[30];
   char value[4200];
};

static struct exif_tag
    tag00001,  tag00254, 	tag00301,	tag33421, 	tag37121, 	tag40960,
    tag00002,  tag00255, 	tag00305,	tag33422, 	tag37122, 	tag40961,
               tag00256, 	tag00306,	tag33423, 	tag37377, 	tag40962,
               tag00257, 	tag00315,	tag33432, 	tag37378, 	tag40963,
               tag00258, 	tag00317,	tag33434, 	tag37379, 	tag40964,
               tag00259, 	tag00318,	tag33437, 	tag37380, 	tag40965,
               tag00262, 	tag00319,	tag33723, 	tag37381, 	tag41483,
               tag00270, 	tag00322,	tag34665, 	tag37382, 	tag41484,
               tag00271, 	tag00323,	tag34675, 	tag37383, 	tag41486,
               tag00272, 	tag00324,	tag34850, 	tag37384, 	tag41487,
               tag00273, 	tag00325,	tag34852, 	tag37385, 	tag41488,
               tag00274, 	tag00330,	tag34853, 	tag37386, 	tag41492,
               tag00277, 	tag00347,	tag34855, 	tag37387, 	tag41493,
               tag00278, 	tag00513,	tag34856, 	tag37388, 	tag41495,
               tag00279, 	tag00514,	tag34857, 	tag37389, 	tag41728,
               tag00282, 	tag00529,	tag34858, 	tag37393, 	tag41729,
               tag00283, 	tag00530,	tag34859, 	tag37394, 	tag41730,
               tag00284, 	tag00531,	tag36864,	tag37395,
               tag00296, 	tag00532,	tag36867,	tag37396,
				                           tag36868,	tag37397,
						                                 tag37398,
						                                 tag37500,
						                                 tag37510,
						                                 tag37520,
						                                 tag37521,
						                                 tag37522;

static char  SubSubIFD=FALSE;
static int   i=0,j=0,k=0, jj=0, kk=0, mm=0, ll=0,
      exit_whileloop=0, 
      temp2=0,
      exif_header[12],
      exif_data[100000], 
      length,
      place_in_file=0,
      is_motorola=0,
      Offset_to_first_IFD=8,
      ExifSubIFDOffset=0,
      SubSubOffset=0,
      IFD_Offset=0,
      number_of_directory_entry_contains_in_this_IFD=0,
      diag=1,
      short_tag1,
      short_tag2,
      short_tag3,
      short_tag4;
static int   result0,
      result1,
      result2,
      tag_number,
      data_format,
      number_of_components,
      offset_to_data_value,
      next_IFD_exists=0;
static char  temp_char=0;
      FILE *JPEG_file;
static char  temp_string[20];

static int PlanarConfiguration;
      
static void PrintTags();
static void InitializeVariables();
static int  find_start_of_image();
static int  examine_header();
static int  read_exif_data();
static void discover_byte_order();
static void calculate_offset();
static void calculate_number_of_tags();
static void process_IFD_entry();
static void calculate_number_of_tags_in_SUB_IFD();
static void process_SUB_IFD_entry();
static void process_tag_number();
static void check_for_another_IFD();
static void calculate_number_of_tags_in_SubSubIFD();
static void process_SubSubIFD();
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static int is_digits(char *s) {
	while (*s != '\0') {
		if (*s < '0' || *s > '9') {
			return 0;
		}
		s++;
	}
	return 1;
}


time_t get_exifdate(char* filename, char* exifvalue) {
	time_t exiftimestamp = -1;
	InitializeVariables();
	JPEG_file=fopen(filename, "rb");
	//printf("                   filename=%s\n",filename);
	if (JPEG_file != NULL)
	{
		result0=find_start_of_image();
		if (result0 == 0) {
			result1=examine_header();
			if (result1 == 0) {
				result2=read_exif_data();
			}
		}
		if (result0==0 && result1==0 && result2==0)
		{
			discover_byte_order();
			calculate_offset();
			do
			{
				calculate_number_of_tags();
				for (i=0;i<number_of_directory_entry_contains_in_this_IFD;i++)
					process_IFD_entry();
				check_for_another_IFD();
			}while (next_IFD_exists);
			calculate_number_of_tags_in_SUB_IFD();
			for (i=0;i<number_of_directory_entry_contains_in_this_IFD;i++)
				process_SUB_IFD_entry();
			if (SubSubIFD)
			{
				calculate_number_of_tags_in_SubSubIFD();
				for (i=0;i<number_of_directory_entry_contains_in_this_IFD;i++)
					process_SubSubIFD();
			}
			//PrintTags();
			if (tag36867.does_exist==TRUE) {
				char datetime[128];
        char *datetimetag = tag36867.value;
				strcpy(datetime, datetimetag);
				if (exifvalue) strcpy(exifvalue, datetime);

        //printf("%27s=%s\n",tag36867.name,tag36867.value);
        {
					struct tm nt;
					nt.tm_isdst = -1;
          if (sscanf(datetimetag, "%d:%d:%d %d:%d:%d",
            &(nt.tm_year),
            &(nt.tm_mon),
            &(nt.tm_mday),
            &(nt.tm_hour),
            &(nt.tm_min),
            &(nt.tm_sec)
            ) == 6) {
            nt.tm_mon -= 1;
            nt.tm_year -= 1900;
					  nt.tm_yday = 0; // days since January 1 - [0,365]
					  nt.tm_wday = 0; // days since Sunday - [0,6]
					  //printf("y:%d m:%d d:%d  h:%d m:%d s:%d\n", nt.tm_year, nt.tm_mon, nt.tm_mday, nt.tm_hour, nt.tm_min, nt.tm_sec);
					  exiftimestamp = mktime(&nt);
          }
        }
/*
				//2002:01:24 21:56:54
            //00000000001111111111
				//01234567890123456789
				datetimetag[4]='\0';
				datetimetag[7]='\0';
				datetimetag[10]='\0';
				datetimetag[13]='\0';
				datetimetag[16]='\0';
				datetimetag[19]='\0';

				if (is_digits(datetimetag) && is_digits(datetimetag+5) && is_digits(datetimetag+8) && is_digits(datetimetag+11) && is_digits(datetimetag+14) && is_digits(datetimetag+17)) {
					if (strlen(datetimetag) == 4 && strlen(datetimetag+5) == 2 && strlen(datetimetag+8) == 2 && strlen(datetimetag+11) == 2 && strlen(datetimetag+14) == 2 && strlen(datetimetag+17) == 2) {

						struct tm nt;
						nt.tm_isdst = -1;
						nt.tm_hour = atoi(datetimetag+11);
						nt.tm_min = atoi(datetimetag+14);
						nt.tm_sec = atoi(datetimetag+17);
						nt.tm_year = atoi(datetimetag+0)-1900;
						nt.tm_mon = atoi(datetimetag+5);
						nt.tm_mday = atoi(datetimetag+8);
						nt.tm_yday = 0; // days since January 1 - [0,365]
						nt.tm_wday = 0; // days since Sunday - [0,6]
						printf("y:%d m:%d d:%d  h:%d m:%d s:%d\n", nt.tm_year, nt.tm_mon, nt.tm_mday, nt.tm_hour, nt.tm_min, nt.tm_sec);
						exiftimestamp = mktime(&nt);
					}
				}
    */
			}
		}
		fclose(JPEG_file);
	}
	//printf("\\\\ %d\n", exiftimestamp);
	return exiftimestamp;
}



int main2(int argc, char* argv[], char* envp[ ])
{
   if (argc >= 2)
      for (k=1;k<argc;++k)
      {
         InitializeVariables();
         JPEG_file=fopen(argv[k], "rb");
         printf("                   filename=%s\n",argv[k]);
         if (JPEG_file != NULL)
         {
            result0=find_start_of_image();
            result1=examine_header();
            result2=read_exif_data();
            if (result0==0 && result1==0 && result2==0)
            {
               discover_byte_order();
               calculate_offset();
               do
               {
                  calculate_number_of_tags();
                  for (i=0;i<number_of_directory_entry_contains_in_this_IFD;i++)
                      process_IFD_entry();
                  check_for_another_IFD();
               }while (next_IFD_exists);
               calculate_number_of_tags_in_SUB_IFD();
               for (i=0;i<number_of_directory_entry_contains_in_this_IFD;i++)
                  process_SUB_IFD_entry();
               if (SubSubIFD)
               {
                  calculate_number_of_tags_in_SubSubIFD();
                  for (i=0;i<number_of_directory_entry_contains_in_this_IFD;i++)
                     process_SubSubIFD();
               }
               //PrintTags();
             if (tag00306.does_exist==TRUE) printf("%27s=%s\n",tag00306.name,tag00306.value);
				}
				fclose(JPEG_file);
         }
      }
   else
   {
      printf("EXIF jpeg reader, version 0.20000216 by Kuren <kuren@kuren.org>.\n");
      printf("Tell this program which file(s) you want to read the EXIF tags from.\n");
      printf("usage:\nexifread DSCN0004.jpg\n");
      return 1;
   }
   return 0;
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
int find_start_of_image()
{
   // searching for JPEG header
   while (JPEG_file != NULL && !feof(JPEG_file))
   {
      temp2=fgetc(JPEG_file);
      ++place_in_file;
      if (temp2==255)
      {
         temp2=fgetc(JPEG_file);
         ++place_in_file;
         if (temp2==216)
         {
            //look for exif marker FFE1 
            while (JPEG_file != NULL && !feof(JPEG_file))
            {
               temp2=fgetc(JPEG_file);
               ++place_in_file;
               if (temp2==255)
               {
                  temp2=fgetc(JPEG_file);
                  ++place_in_file;
                  if (temp2==225)
                  {
                     place_in_file-=4;
                     fseek(JPEG_file,place_in_file,0);
                     return 0;
                  }
               }
            }
         }
      }
   }
   return 1;
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
int examine_header()
{
   //examine header
   for(i=0;i<12;i++)
   if ((exif_header[i]=fgetc(JPEG_file))==EOF)
   {
      printf("End of file found prematurely.\n");
      printf("ERROR\n");
      return(1);
   }
   // reading TIFF header
   // check for APP1 Marker used by Exif
   if (exif_header[2]!=255 && exif_header[3]!=225)
      printf("This file is not an Exif file.\n");
   // APP1 marker found

   // calculate length of APP1 data
   length=(exif_header[4]*256)+(exif_header[5]);
   // APP1 data length = length
   if (length >= 100000) 
   {
      printf("The data is too large for this program to cope with\n");
      printf("ERROR\n");
      return(1);
   }
   //check to see if "Exif" tag is there
   if (!(exif_header[6]=='E' && exif_header[7]=='x' && exif_header[8]=='i' && exif_header[9]=='f' && exif_header[10]==0 && exif_header[11]==0))
   {
      //printf("\"Exif\" was not found as expected.\n");
      //if ((exif_header[6]=='J' && exif_header[7]=='F' && exif_header[8]=='I' && exif_header[9]=='F'))
      //   printf("\"JFIF\" was found instead.\n");
      //printf("ERROR\n");
      return(1);
   }
   return(0);
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
int read_exif_data()
{
   fseek(JPEG_file,-8,SEEK_CUR);
   for (i=0;i<length;++i)
   {
      if ((exif_data[i]=fgetc(JPEG_file))==EOF)
      {
         printf("End of file found unexpectedly while reading APP1 data.\n");
         printf("ERROR\n");
         return(1);
      }
   }
   return(0);
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void discover_byte_order()
{
   if (exif_data[8]=='I' && exif_data[9]=='I')
   {
      is_motorola=0;
      //printf("intel byte order\n");
   }
   if (exif_data[8]=='M' && exif_data[9]=='M')
   {
      is_motorola=1;
      //printf("motorola byte order\n");
   }
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void calculate_offset()
{
   // calculate offset
   if (is_motorola)
      Offset_to_first_IFD=exif_data[12]*256*256*256+
                          exif_data[13]*256*256+
                          exif_data[14]*256+
                          exif_data[15];
   else
      Offset_to_first_IFD=exif_data[15]*256*256*256+
                          exif_data[14]*256*256+
                          exif_data[13]*256+
                          exif_data[12];
   IFD_Offset=8+Offset_to_first_IFD;
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void calculate_number_of_tags()
{
   // number of directory entry contains in this IFD
   if (is_motorola)
      number_of_directory_entry_contains_in_this_IFD=exif_data[0+IFD_Offset]*256+
                                                     exif_data[1+IFD_Offset];
   else
      number_of_directory_entry_contains_in_this_IFD=exif_data[1+IFD_Offset]*256+
                                                     exif_data[0+IFD_Offset];
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void process_IFD_entry()
{
   if (is_motorola)
   {
      tag_number=exif_data[2+IFD_Offset+i*12]*256+
                 exif_data[3+IFD_Offset+i*12];
      data_format=exif_data[4+IFD_Offset+i*12]*256+
                  exif_data[5+IFD_Offset+i*12];
      number_of_components=exif_data[6+IFD_Offset+i*12]*256*256*256+
                           exif_data[7+IFD_Offset+i*12]*256*256+
                           exif_data[8+IFD_Offset+i*12]*256+
                           exif_data[9+IFD_Offset+i*12];
      offset_to_data_value=exif_data[10+IFD_Offset+i*12]*256*256*256+
                           exif_data[11+IFD_Offset+i*12]*256*256+
                           exif_data[12+IFD_Offset+i*12]*256+
                           exif_data[13+IFD_Offset+i*12];
   }
   else
   {
      tag_number=exif_data[3+IFD_Offset+i*12]*256+
                 exif_data[2+IFD_Offset+i*12];
      data_format=exif_data[5+IFD_Offset+i*12]*256+
                  exif_data[4+IFD_Offset+i*12];
      number_of_components=exif_data[9+IFD_Offset+i*12]*256*256*256+
                           exif_data[8+IFD_Offset+i*12]*256*256+
                           exif_data[7+IFD_Offset+i*12]*256+
                           exif_data[6+IFD_Offset+i*12];
      offset_to_data_value=exif_data[13+IFD_Offset+i*12]*256*256*256+
                           exif_data[12+IFD_Offset+i*12]*256*256+
                           exif_data[11+IFD_Offset+i*12]*256+
                           exif_data[10+IFD_Offset+i*12];
   }
   process_tag_number();
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void calculate_number_of_tags_in_SUB_IFD()
{
   if (is_motorola)
      number_of_directory_entry_contains_in_this_IFD=exif_data[ExifSubIFDOffset]*256+exif_data[1+ExifSubIFDOffset];
   else
      number_of_directory_entry_contains_in_this_IFD=exif_data[1+ExifSubIFDOffset]*256+exif_data[ExifSubIFDOffset];
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void process_SUB_IFD_entry()
{
   IFD_Offset=ExifSubIFDOffset;
   if (is_motorola)
   {
      tag_number=exif_data[2+IFD_Offset+i*12]*256+
                 exif_data[3+IFD_Offset+i*12];
      data_format=exif_data[4+IFD_Offset+i*12]*256+
                  exif_data[5+IFD_Offset+i*12];
      number_of_components=exif_data[6+IFD_Offset+i*12]*256*256*256+
                           exif_data[7+IFD_Offset+i*12]*256*256+
                           exif_data[8+IFD_Offset+i*12]*256+
                           exif_data[9+IFD_Offset+i*12];
      offset_to_data_value=exif_data[10+IFD_Offset+i*12]*256*256*256+
                           exif_data[11+IFD_Offset+i*12]*256*256+
                           exif_data[12+IFD_Offset+i*12]*256+
                           exif_data[13+IFD_Offset+i*12];
      short_tag1=exif_data[11+IFD_Offset+i*12];
      short_tag2=exif_data[10+IFD_Offset+i*12];
      short_tag3=exif_data[13+IFD_Offset+i*12];
      short_tag4=exif_data[12+IFD_Offset+i*12];
   }
   else
   {
      tag_number=exif_data[3+IFD_Offset+i*12]*256+
                 exif_data[2+IFD_Offset+i*12];
      data_format=exif_data[5+IFD_Offset+i*12]*256+
                  exif_data[4+IFD_Offset+i*12];
      number_of_components=exif_data[9+IFD_Offset+i*12]*256*256*256+
                           exif_data[8+IFD_Offset+i*12]*256*256+
                           exif_data[7+IFD_Offset+i*12]*256+
                           exif_data[6+IFD_Offset+i*12];
      offset_to_data_value=exif_data[13+IFD_Offset+i*12]*256*256*256+
                           exif_data[12+IFD_Offset+i*12]*256*256+
                           exif_data[11+IFD_Offset+i*12]*256+
                           exif_data[10+IFD_Offset+i*12];
      short_tag1=exif_data[10+IFD_Offset+i*12];
      short_tag2=exif_data[11+IFD_Offset+i*12];
      short_tag3=exif_data[12+IFD_Offset+i*12];
      short_tag4=exif_data[13+IFD_Offset+i*12];
   }
   process_tag_number();
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void calculate_number_of_tags_in_SubSubIFD()
{
   if (is_motorola)
      number_of_directory_entry_contains_in_this_IFD=exif_data[SubSubOffset]*256+exif_data[1+SubSubOffset];
   else
      number_of_directory_entry_contains_in_this_IFD=exif_data[1+SubSubOffset]*256+exif_data[SubSubOffset];
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

void process_SubSubIFD()
{

   IFD_Offset=SubSubOffset;
   if (is_motorola)
   {
      tag_number=exif_data[2+IFD_Offset+i*12]*256+
                 exif_data[3+IFD_Offset+i*12];
      data_format=exif_data[4+IFD_Offset+i*12]*256+
                  exif_data[5+IFD_Offset+i*12];
      number_of_components=exif_data[6+IFD_Offset+i*12]*256*256*256+
                           exif_data[7+IFD_Offset+i*12]*256*256+
                           exif_data[8+IFD_Offset+i*12]*256+
                           exif_data[9+IFD_Offset+i*12];
      offset_to_data_value=exif_data[10+IFD_Offset+i*12]*256*256*256+
                           exif_data[11+IFD_Offset+i*12]*256*256+
                           exif_data[12+IFD_Offset+i*12]*256+
                           exif_data[13+IFD_Offset+i*12];
      short_tag1=exif_data[11+IFD_Offset+i*12];
      short_tag2=exif_data[10+IFD_Offset+i*12];
      short_tag3=exif_data[13+IFD_Offset+i*12];
      short_tag4=exif_data[12+IFD_Offset+i*12];
   }
   else
   {
      tag_number=exif_data[3+IFD_Offset+i*12]*256+
                 exif_data[2+IFD_Offset+i*12];
      data_format=exif_data[5+IFD_Offset+i*12]*256+
                  exif_data[4+IFD_Offset+i*12];
      number_of_components=exif_data[9+IFD_Offset+i*12]*256*256*256+
                           exif_data[8+IFD_Offset+i*12]*256*256+
                           exif_data[7+IFD_Offset+i*12]*256+
                           exif_data[6+IFD_Offset+i*12];
      offset_to_data_value=exif_data[13+IFD_Offset+i*12]*256*256*256+
                           exif_data[12+IFD_Offset+i*12]*256*256+
                           exif_data[11+IFD_Offset+i*12]*256+
                           exif_data[10+IFD_Offset+i*12];
      short_tag1=exif_data[10+IFD_Offset+i*12];
      short_tag2=exif_data[11+IFD_Offset+i*12];
      short_tag3=exif_data[12+IFD_Offset+i*12];
      short_tag4=exif_data[13+IFD_Offset+i*12];
   }
   process_tag_number();
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


























































































/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void process_tag_number()
{
   switch(tag_number)
   {





     case     1: // ImageDescription             ascii string          ");
                 tag00001.does_exist=TRUE;
                 tag00001.value[0]=short_tag1;
                 tag00001.value[1]=short_tag2;
                 tag00001.value[2]=short_tag3;
                 tag00001.value[3]=short_tag4;
                 break;
     case     2: // ImageDescription             ascii string          ");
                 tag00002.does_exist=TRUE;
                 sprintf(tag00002.value,"[%d.%d.%d.%d]",short_tag1,short_tag2,short_tag3,short_tag4);
                  break;
     case   270: // ImageDescription             ascii string          ");
                 tag00270.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag00270.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case   271: // Make                         ascii string          ");
                 tag00271.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag00271.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case   272: // Model                        ascii string          ");
                 tag00272.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag00272.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case   305: // Software                     ascii string          ");
                 tag00305.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag00305.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case   306: // DateTime                     ascii string        20");
                 tag00306.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag00306.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case   315: // Artist                       ascii string          ");
                 tag00315.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag00315.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 33432: // Copyright                    ascii string          ");
                 tag33432.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag33432.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 34852: // SpectralSensitivity          ascii string          ");
                 tag34852.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag34852.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 36867: // DateTimeOriginal             ascii string        20");
                 tag36867.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag36867.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 36868: // DateTimeDigitized            ascii string        20");
                 tag36868.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag36868.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 37395: // ImageHistory                 ascii string          ");
                 tag37395.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag37395.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 37520: // SubSecTime                   ascii string          ");
                 tag37520.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag37520.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 37521: // SubSecTimeOriginal           ascii string          ");
                 tag37521.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag37521.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 37522: // SubSecTimeDigitized          ascii string          ");
                 tag37522.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag37522.value[j]= exif_data[8+offset_to_data_value+j];
                  break;
     case 40964: // RelatedSoundFile             ascii string          ");
                 tag40964.does_exist=TRUE;
                 for (j=0;j<number_of_components-1;j++)
                     tag40964.value[j]= exif_data[8+offset_to_data_value+j];
                  break;

     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     case 41728: // FileSource                   undefined            1");
                 tag41728.does_exist=TRUE;
                 sprintf(tag41728.value,"%d",offset_to_data_value);
                 break;
     case 41729: // SceneType                    undefined            1");
                 tag41729.does_exist=TRUE;
                 sprintf(tag41729.value,"%d",offset_to_data_value);
                 break;
     case   255: // SubfileType                  unsigned short       1");
                 tag00255.does_exist=TRUE;
                 sprintf(tag00255.value,"%d",offset_to_data_value);
                 break;
     case   258: // BitsPerSample                unsigned short       3");
                 tag00258.does_exist=TRUE;
                 if (is_motorola)
                    for (j=0;j<number_of_components*2;j+=2)
                    {
                       sprintf(temp_string,"%d",exif_data[8+offset_to_data_value+j+1]);
                       strcat(tag00258.value,temp_string );
                       if (j+3<=number_of_components*2)
                           strcat(tag00258.value,",");
                    }
                 else
                    for (j=0;j<number_of_components*2;j+=2)
                    {
                       sprintf(temp_string,"%d",exif_data[8+offset_to_data_value+j]);
                       strcat(tag00258.value,temp_string );
                       if (j+3<=number_of_components*2)
                           strcat(tag00258.value,",");
                    }

                 break;
     case   279: // StripByteConunts             unsigned short/long   ");
                 tag00279.does_exist=TRUE;
                 sprintf(tag00279.value,"%d",offset_to_data_value);
                 break;
     case   301: // TransferFunction             unsigned short       3");
                 tag00301.does_exist=TRUE;
                 sprintf(tag00301.value,"%d",offset_to_data_value);
                 break;
     case   317: // Predictor                    unsigned short       1");
                 tag00317.does_exist=TRUE;
                 sprintf(tag00317.value,"%d",offset_to_data_value);
                 break;
     case   322: // TileWidth                    unsigned short       1");
                 tag00322.does_exist=TRUE;
                 sprintf(tag00322.value,"%d",offset_to_data_value);
                 break;
     case   323: // TileLength                   unsigned short       1");
                 tag00323.does_exist=TRUE;
                 sprintf(tag00323.value,"%d",offset_to_data_value);
                 break;
     case   324: // TileOffsets                  unsigned long         ");
                 tag00324.does_exist=TRUE;
                 sprintf(tag00324.value,"%d",offset_to_data_value);
                 break;
     case   325: // TileByteCounts               unsigned short        ");
                 tag00325.does_exist=TRUE;
                 sprintf(tag00325.value,"%d",offset_to_data_value);
                 break;
     case   330: // SubIFDs                      unsigned long         ");
                 tag00330.does_exist=TRUE;
                 sprintf(tag00330.value,"%d",offset_to_data_value);
                 break;
     case   347: // JPEGTables                   undefined             ");
                 tag34858.does_exist=TRUE;
                 sprintf(tag00347.value,"%d",offset_to_data_value);
                 break;
     case   513: // JpegIFOffset                 unsigned long        1");
                 tag00513.does_exist=TRUE;
                 sprintf(tag00513.value,"%d",offset_to_data_value);
                 break;
     case   514: // JpegIFByteCount              unsigned long        1");
                 tag00514.does_exist=TRUE;
                 sprintf(tag00514.value,"%d",offset_to_data_value);
                 break;
     case 33422: // CFAPattern                   unsigned byte         ");
                 tag33422.does_exist=TRUE;
                 sprintf(tag33422.value,"%d",offset_to_data_value);
                 break;
     case 34675: // InterColorProfile            undefined             ");
                 tag34675.does_exist=TRUE;
                 sprintf(tag34675.value,"%d",offset_to_data_value);
                 break;
     case 34856: // OECF                         undefined             ");
                 tag34856.does_exist=TRUE;
                 sprintf(tag34856.value,"%d",offset_to_data_value);
                 break;
     case 34859: // SelfTimerMode                unsigned short       1");
                 tag34859.does_exist=TRUE;
                 sprintf(tag34859.value,"%d",offset_to_data_value);
                 break;
     case 37388: // SpatialFrequencyResponse     undefined             ");
                 tag37388.does_exist=TRUE;
                 sprintf(tag37388.value,"%d",offset_to_data_value);
                 break;
     case 37389: // Noise                        undefined             ");
                 tag37389.does_exist=TRUE;
                 sprintf(tag37389.value,"%d",offset_to_data_value);
                 break;
     case 37393: // ImageNumber                  unsigned long        1");
                 tag37393.does_exist=TRUE;
                 sprintf(tag37393.value,"%d",offset_to_data_value);
                 break;
     case 37398: // TIFF/EPStandardID            unsigned byte        4");
                 tag37398.does_exist=TRUE;
                 sprintf(tag37398.value,"%d",offset_to_data_value);
                 break;
     case 40962: // ExifImageWidth               unsigned short/long  1");
                 tag40962.does_exist=TRUE;
                 sprintf(tag40962.value,"%d",offset_to_data_value);
                 break;
     case 40963: // ExifImageHeight              unsigned short/long  1");
                 tag40963.does_exist=TRUE;
                 sprintf(tag40963.value,"%d",offset_to_data_value);
                 break;
     case 41484: // SpatialFrequencyResponse     undefined            n");
                 tag41484.does_exist=TRUE;
                 sprintf(tag41484.value,"%d",offset_to_data_value);
                 break;
     case 41730: // CFAPattern                   undefined            1");
                 tag41730.does_exist=TRUE;
                 sprintf(tag41730.value,"%d",offset_to_data_value);
                 break;

     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     case 40960: // FlashPixVersion              undefined            4");
                 tag40960.does_exist=TRUE;
                 if (is_motorola)
                  {
                     sprintf(tag40960.value,"[%d.%d.%d.%d]",exif_data[10+IFD_Offset+i*12],exif_data[11+IFD_Offset+i*12],exif_data[12+IFD_Offset+i*12],exif_data[13+IFD_Offset+i*12]);
                  }
                  else
                  {
                     sprintf(tag40960.value,"[%d.%d.%d.%d]",exif_data[13+IFD_Offset+i*12],exif_data[12+IFD_Offset+i*12],exif_data[11+IFD_Offset+i*12],exif_data[10+IFD_Offset+i*12]);
                  }
                  break;
     case 40961: // ColorSpace                   unsigned short       1");
                 tag40961.does_exist=TRUE;
                 if (is_motorola)
                     switch (exif_data[11+IFD_Offset+i*12])
                     {
                        case 1  : sprintf(tag40961.value,"1"); break; 
                        default : sprintf(tag40961.value,"%d",exif_data[11+IFD_Offset+i*12]); 
                     }
                  else
                     switch (exif_data[10+IFD_Offset+i*12])
                     {
                        case 1  : sprintf(tag40961.value,"1"); break; 
                        default : sprintf(tag40961.value,"%d",exif_data[10+IFD_Offset+i*12]); 
                     }
                  break;
     case 40965: // ExifInteroperabilityOffset   unsigned long        1");
                 tag40965.does_exist=TRUE;
                 if (is_motorola)
                  {
                     sprintf(tag40965.value,"[%d.%d.%d.%d]",exif_data[10+IFD_Offset+i*12],exif_data[11+IFD_Offset+i*12],exif_data[12+IFD_Offset+i*12],exif_data[13+IFD_Offset+i*12]);
                  }
                  else
                  {
                     sprintf(tag40965.value,"[%d.%d.%d.%d]",exif_data[13+IFD_Offset+i*12],exif_data[12+IFD_Offset+i*12],exif_data[11+IFD_Offset+i*12],exif_data[10+IFD_Offset+i*12]);
                  }
                  SubSubOffset=offset_to_data_value+8;
                  SubSubIFD=TRUE;
                  break;

     
     
     
     
     
     
     
     case   273: // StripOffsets                 unsigned short/long   ");
                 tag00273.does_exist=TRUE;
                 sprintf(temp_string,"%d, ",offset_to_data_value);
                  strcat(tag00273.value,temp_string);
                  if (PlanarConfiguration==1)
                  {
                     sprintf(temp_string,"%d=StripsPerImage",number_of_components);
                     strcat(tag00273.value,temp_string);
                  }
                  else if (PlanarConfiguration==2)
                  {
                     sprintf(temp_string,"%d=SamplesPerPixel * StripsPerImage",number_of_components);
                     strcat(tag00273.value,temp_string);
                  }
                  break;

     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     case   278: // RowsPerStrip                 unsigned short/long  1");
                 tag00278.does_exist=TRUE;
                 sprintf(tag00278.value,"%d, Note that the number of StripsPerImage equals floor ((ImageLength + RowsPerStrip - 1) / RowsPerStrip)",offset_to_data_value);
                  break;
     case 34665: // ExifOffset                   unsigned long        1");
                 tag34665.does_exist=TRUE;
                 sprintf(tag34665.value,"%4x",offset_to_data_value);
                  ExifSubIFDOffset=offset_to_data_value+8;
                  break;
     case 34853: // GPSInfo                      unsigned long        1");
                 tag34853.does_exist=TRUE;
                 sprintf(tag34853.value,"%4x=place to find more GPS info, more code needs to be written to decode this data",offset_to_data_value);
                  break;
     case 34855: // ISOSpeedRatings              unsigned short       2");
                 tag34855.does_exist=TRUE;
                 sprintf(tag34855.value,"CCD sensitivity equivalent to Ag-Hr film speedrate=%d,%d", offset_to_data_value); break;
                  break;
     case 34858: // TimeZoneOffset               signed short         1");
                 tag34858.does_exist=TRUE;
                 sprintf(tag34858.value,"%d=Time Zone Offset (in hours) of DateTimeOriginal tag-value relative to Greenwich Mean Time",short_tag1-128);
                  if  (number_of_components==2)
                     sprintf(tag34858.value,", %d=Time Zone Offset (in hours) of DateTime tag-value relative to Greenwich Mean Time",short_tag2-128);
                  break;

     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     case 37396: // SubjectLocation              unsigned short       4");
                 tag37396.does_exist=TRUE;
                 sprintf(temp_string,"%d=column number, %d=row number",short_tag1, short_tag2);
                  strcat(tag37396.value,temp_string);
                  if (short_tag3!=0 && short_tag4==0)
                  {
                     sprintf(temp_string,", %d=diameter of circle", short_tag3);
                     strcat(tag37396.value,temp_string);
                  }
                  else if (short_tag3!=0 && short_tag4!=0)
                  {
                     sprintf(temp_string,", %d=width, %d=heigth", short_tag3, short_tag4);
                     strcat(tag37396.value,temp_string);
                  }
                  break;
     case 41492: // SubjectLocation              unsigned short       1");
                 tag41492.does_exist=TRUE;
                 sprintf(temp_string,"%d=column number, %d=row number",short_tag1, short_tag2);
                  strcat(tag41492.value,temp_string);
                  if (short_tag3!=0 && short_tag4==0)
                  {
                     sprintf(temp_string,", %d=diameter of circle", short_tag3);
                     strcat(tag41492.value,temp_string);
                  }
                  if (short_tag3!=0 && short_tag4!=0)
                  {
                     sprintf(temp_string,", %d=width, %d=heigth", short_tag3, short_tag4);
                     strcat(tag41492.value,temp_string);
                  }
     case   259: // Compression                  unsigned short       1");
                 tag00259.does_exist=TRUE;
                 if (is_motorola)
                     switch(exif_data[11+IFD_Offset+i*12])
                     {
                        case  1: sprintf(tag00259.value,"no compression"); break; 
                        case  7: sprintf(tag00259.value,"TIFF/JPEG compression"); break;
                        default: sprintf(tag00259.value,"%d=Vendor Unique Number",offset_to_data_value); 
                     }
                 else
                     switch (exif_data[10+IFD_Offset+i*12])
                     {
                        case  1: sprintf(tag00259.value,"no compression"); break; 
                        case  7: sprintf(tag00259.value,"TIFF/JPEG compression"); break;
                        default: sprintf(tag00259.value,"%d=Vendor Unique Number",offset_to_data_value); 
                     }
                  break;
     case   262: // PhotometricInterpretation    unsigned short       1");
                 tag00262.does_exist=TRUE;
                 switch (offset_to_data_value)
                  {
                     case      1: sprintf(tag00262.value,"BlackIsZero"); break; 
                     case      2: sprintf(tag00262.value,"RGB"); break; 
                     case      6: sprintf(tag00262.value,"YCbCr color space"); break;
                     case  32803: sprintf(tag00262.value,"CFA"); break;
                     default    : sprintf(tag00262.value,"%d=Vendor Unique Number",offset_to_data_value); 
                  }
                  break;
     case   277: // SamplesPerPixel              unsigned short       1");
                 tag00277.does_exist=TRUE;
                 switch (offset_to_data_value)
                  {
                     case      1: sprintf(tag00277.value,"1"); break; 
                     case      3: sprintf(tag00277.value,"3"); break; 
                     default    : sprintf(tag00277.value,"%d=Vendor Unique Number",offset_to_data_value); 
                  }
                  break;
     case 34850: // ExposureProgram              unsigned short       1");
                 tag34850.does_exist=TRUE;
                 switch (offset_to_data_value)
                  {
                     case  1: sprintf(tag34850.value,"manual control"); break;
                     case  2: sprintf(tag34850.value,"program normal"); break; 
                     case  3: sprintf(tag34850.value,"aperture priority"); break;
                     case  4: sprintf(tag34850.value,"shutter priority"); break;
                     case  5: sprintf(tag34850.value,"program creative (slow program)"); break;
                     case  6: sprintf(tag34850.value,"program action(high-speed program)"); break; 
                     case  7: sprintf(tag34850.value,"portrait mode"); break;
                     case  8: sprintf(tag34850.value,"landscape mode"); break;
                     default: sprintf(tag34850.value,"%d",offset_to_data_value); 
                  }
                  break;
     case 37383: // MeteringMode                 unsigned short       1");
                 tag37383.does_exist=TRUE;
                 switch (offset_to_data_value)
                  {
                     case   1: sprintf(tag37383.value,"average"); break;
                     case   2: sprintf(tag37383.value,"center weighted average"); break; 
                     case   3: sprintf(tag37383.value,"spot"); break;
                     case   4: sprintf(tag37383.value,"multi-spot"); break;
                     case   5: sprintf(tag37383.value,"multi-segment"); break;
                     default : sprintf(tag37383.value,"%d",offset_to_data_value); 
                  }
                  break;
     case 37384: // LightSource                  unsigned short       1");
                 tag37384.does_exist=TRUE;
                 switch (offset_to_data_value)
                  {
                     case   0: sprintf(tag37384.value,"auto"); break;
                     case   1: sprintf(tag37384.value,"daylight"); break; 
                     case   2: sprintf(tag37384.value,"fluorescent"); break;
                     case   3: sprintf(tag37384.value,"tungsten"); break;
                     case  10: sprintf(tag37384.value,"flash"); break;
                     default : sprintf(tag37384.value,"%d",offset_to_data_value); 
                  }
                  break;
     case 37385: // Flash                        unsigned short       1");
                 tag37385.does_exist=TRUE;
                 switch (offset_to_data_value)
                  {
                     case   0: sprintf(tag37385.value,"flash was not used"); break;
                     case   1: sprintf(tag37385.value,"flash was used"); break; 
                     default : sprintf(tag37384.value,"%d",offset_to_data_value); 
                  }
                  break;
     case 41488: // FocalPlaneResolutionUnit     unsigned short       1");
                 tag41488.does_exist=TRUE;
                 switch (offset_to_data_value)
                  {
                     case   1: sprintf(tag41488.value,"no-unit"); break;
                     case   2: sprintf(tag41488.value,"inch"); break; 
                     case   3: sprintf(tag41488.value,"centimeter"); break; 
                     default : sprintf(tag41488.value,"%d",offset_to_data_value); 
                  }
                  break;
     case 41495: // SensingMethod                unsigned short       1");
                 tag41495.does_exist=TRUE;
                 switch (offset_to_data_value)
                  {
                     case  2: sprintf(tag41495.value,"one chip color area sensor"); break;
                     default: sprintf(tag41495.value,"%d",offset_to_data_value); 
                  }
                  break;
     case   254: // NewSubfileType               unsigned long        1");
                 tag00254.does_exist=TRUE;
                 switch(short_tag1)
                     {
                        case 0: strcpy(tag00254.value,"indicates main image\""); break;
                        case 1: strcpy(tag00254.value,"indicates thumbnail\""); break;
                        default : sprintf(tag00254.value,"%d",short_tag1); 
                     }
                  break;
     case   284: // PlanarConfiguration          unsigned short       1");
                 tag00284.does_exist=TRUE;
                 PlanarConfiguration=short_tag1;
                  switch (short_tag1)
                  {
                     case      1: sprintf(tag00284.value,"chunky format"); break; 
                     case      2: sprintf(tag00284.value,"planer"); break; 
                     default    : sprintf(tag00284.value,"%d",short_tag1); 
                  }
                  break;

                  
                  
                  
                  
                  
                  
                  
                  
                  
                  
                  
                  
                  
                  
     case   282: // XResolution                  unsigned rational    1");
                 tag00282.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag00282.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag00282.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag00282.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag00282.value,temp_string);
                     }
                  }
                  break;
     case   283: // YResolution                  unsigned rational    1");
                 tag00283.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag00283.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag00283.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag00283.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag00283.value,temp_string);
                     }
                  }
                  break;
     case   318: // WhitePoint                   unsigned rational    2");
                 tag00318.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag00318.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag00318.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag00318.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag00318.value,temp_string);
                     }
                  }
                  break;
     case   319: // PrimaryChromaticities        unsigned rational    6");
                 tag00319.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag00319.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag00319.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag00319.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag00319.value,temp_string);
                     }
                  }
                  break;
     case   529: // YCbCrCoefficients            unsigned rational    3");
                 tag00529.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag00529.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag00529.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag00529.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag00529.value,temp_string);
                     }
                  }
                  break;
     case   532: // ReferenceBlackWhite          unsigned rational    6");
                 tag00532.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag00532.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag00532.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag00532.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag00532.value,temp_string);
                     }
                  }
                  break;
     case 33423: // BatteryLevel                 unsigned rational    1");
                 tag33423.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag33423.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag33423.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag33423.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag33423.value,temp_string);
                     }
                  }
                  break;
     case 33434: // ExposureTime                 unsigned rational    1");
                 tag33434.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag33434.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag33434.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag33434.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag33434.value,temp_string);
                     }
                  }
                  break;
     case 33437: // FNumber                      unsigned rational    1");
                 tag33437.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag33437.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag33437.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag33437.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag33437.value,temp_string);
                     }
                  }
                  break;
     case 37122: // CompressedBitsPerPixel       unsigned rational    1");
                 tag37122.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37122.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37122.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37122.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37122.value,temp_string);
                     }
                  }
                  break;
     case 37377: // ShutterSpeedValue            signed rational      1");
                 tag37377.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37377.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37377.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37377.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37377.value,temp_string);
                     }
                  }
                  break;
     case 37378: // ApertureValue                unsigned rational    1");
                 tag37378.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37378.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37378.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37378.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37378.value,temp_string);
                     }
                  }
                  break;
     case 37379: // BrightnessValue              signed rational      1");
                 tag37379.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37379.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37379.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37379.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37379.value,temp_string);
                     }
                  }
                  break;
     case 37380: // ExposureBiasValue            signed rational      1");
                 tag37380.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37380.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37380.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37380.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37380.value,temp_string);
                     }
                  }
                  break;
     case 37381: // MaxApertureValue             unsigned rational    1");
                 tag37381.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37381.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37381.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37381.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37381.value,temp_string);
                     }
                  }
                  break;
     case 37382: // SubjectDistance              signed rational      1");
                 tag37382.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37382.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37382.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37382.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37382.value,temp_string);
                     }
                  }
                  break;
     case 37386: // FocalLength                  unsigned rational    1");
                 tag37386.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37386.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37386.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37386.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37386.value,temp_string);
                     }
                  }
                  break;
     case 37387: // FlashEnergy                  unsigned rational    1");
                 tag37387.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37387.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37387.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37387.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37387.value,temp_string);
                     }
                  }
                  break;
     case 37397: // ExposureIndex                unsigned rational    1");
                 tag37397.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag37397.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag37397.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag37397.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag37397.value,temp_string);
                     }
                  }
                  break;

   
     
     
     
     
     case 37500: // MakerNote                    undefined             ");
                 tag37500.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                     if  (exif_data[8+offset_to_data_value+j]<16)
                     {
                        sprintf(temp_string,"[0%x]", exif_data[8+offset_to_data_value+j]);
                        strcat(tag37500.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"[%x]", exif_data[8+offset_to_data_value+j]);
                        strcat(tag37500.value,temp_string);
                     }
                  /*
                  for (ll=0;ll<12;ll++)
                  {
                     mm=0;
                     jj=0;
                     kk=0;
                     j=0;
                     printf("\n%6x:  ",8+offset_to_data_value+j);
                     for (j=0;j<number_of_components;j++)
                     {   
                        if (kk%12==0 && jj>0) printf("\n%6x:  ",8+offset_to_data_value+j);
                        if (exif_data[8+offset_to_data_value+j]<16)
                           printf("0%x ",exif_data[8+offset_to_data_value+j]);
                        else
                           printf("%x ",exif_data[8+offset_to_data_value+j]);
                        //if (exif_data[8+offset_to_data_value+j]>32 && exif_data[8+offset_to_data_value+j]<254)
                        //   printf("%c", exif_data[8+offset_to_data_value+j]);
                        //else
                           //printf("%x", exif_data[8+offset_to_data_value+j]);
                        if (jj>0) kk++;
                        if (mm==ll && jj==0) 
                           jj=1;
                        mm++;
                     }
                     printf("\n");
                  }
                  */
                  break;
     case 37510: // UserComment                  undefined             ");
                 tag37510.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                     if  (exif_data[8+offset_to_data_value+j]<16)
                     {
                        sprintf(temp_string,"[0%x]", exif_data[8+offset_to_data_value+j]);
                        strcat(tag37510.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"[%x]", exif_data[8+offset_to_data_value+j]);
                        strcat(tag37510.value,temp_string);
                     }
                  break;

     
     
     
     
     case 41483: // FlashEnergy                  unsigned rational    1");
                 tag41483.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag41483.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag41483.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag41483.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag41483.value,temp_string);
                     }
                  }
                  break;
     case 41486: // FocalPlaneXResolution        unsigned rational    1");
                 tag41486.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag41486.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag41486.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag41486.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag41486.value,temp_string);
                     }
                  }
                  break;
     case 41487: // FocalPlaneYResolution        unsigned rational    1");
                 tag41487.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag41487.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag41487.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag41487.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag41487.value,temp_string);
                     }
                  }
                  break;
     case 41493: // ExposureIndex                unsigned rational    1");
                 tag41493.does_exist=TRUE;
                 for (j=0;j<number_of_components;j++)
                  {
                     if (is_motorola)
                     {
                        sprintf(temp_string,"%d/",exif_data[8+offset_to_data_value+j*8]*256*256*256+exif_data[9+offset_to_data_value+j*8]*256*256+exif_data[10+offset_to_data_value+j*8]*256+exif_data[11+offset_to_data_value+j*8]);
                        strcat(tag41493.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[12+offset_to_data_value+j*8]*256*256*256+exif_data[13+offset_to_data_value+j*8]*256*256+exif_data[14+offset_to_data_value+j*8]*256+exif_data[15+offset_to_data_value+j*8]);
                        strcat(tag41493.value,temp_string);
                     }
                     else
                     {
                        sprintf(temp_string,"%d/",exif_data[11+offset_to_data_value+j*8]*256*256*256+exif_data[10+offset_to_data_value+j*8]*256*256+exif_data[9+offset_to_data_value+j*8]*256+exif_data[8+offset_to_data_value+j*8]);
                        strcat(tag41493.value,temp_string);
                        sprintf(temp_string,"%d ",exif_data[15+offset_to_data_value+j*8]*256*256*256+exif_data[14+offset_to_data_value+j*8]*256*256+exif_data[13+offset_to_data_value+j*8]*256+exif_data[12+offset_to_data_value+j*8]);
                        strcat(tag41493.value,temp_string);
                     }
                  }
                  break;

     
     
     
     
     
     
     
     
     
     
     case 33723: // IPTC/NAA                     unsigned long         ");
                 tag33723.does_exist=TRUE;
                 if (data_format==2)
                     for (j=0;j<number_of_components-1;j++)
                        tag33723.value[j]= exif_data[8+offset_to_data_value+j];
                  else
                      sprintf(tag33723.value,"%4x",offset_to_data_value);
                  break;
     case   256: // ImageWidth                   unsigned short/long  1");
                 tag00256.does_exist=TRUE;
                 if (is_motorola)
                     sprintf(tag00256.value,"%d",exif_data[10+IFD_Offset+i*12]*256*256*256+exif_data[11+IFD_Offset+i*12]*256*256+exif_data[12+IFD_Offset+i*12]*256+exif_data[13+IFD_Offset+i*12]);
                  else
                     sprintf(tag00256.value,"%d",exif_data[13+IFD_Offset+i*12]*256*256*256+exif_data[12+IFD_Offset+i*12]*256*256+exif_data[11+IFD_Offset+i*12]*256+exif_data[10+IFD_Offset+i*12]);
                  break;
     case   257: // ImageLength                  unsigned short/long  1");
                 tag00257.does_exist=TRUE;
                 if (is_motorola)
                     sprintf(tag00257.value,"%d",exif_data[10+IFD_Offset+i*12]*256*256*256+exif_data[11+IFD_Offset+i*12]*256*256+exif_data[12+IFD_Offset+i*12]*256+exif_data[13+IFD_Offset+i*12]);
                  else
                     sprintf(tag00257.value,"%d",exif_data[13+IFD_Offset+i*12]*256*256*256+exif_data[12+IFD_Offset+i*12]*256*256+exif_data[11+IFD_Offset+i*12]*256+exif_data[10+IFD_Offset+i*12]);
                  break;

     
     

     
     
     
     
     
     
     
     case   274: // Orientation                  unsigned short       1");
                 tag00274.does_exist=TRUE;
                 if (is_motorola)
                     switch (exif_data[11+IFD_Offset+i*12])
                     {
                        case  1: strcpy(tag00274.value,"upper left");
                                 break;
                        case  3: strcpy(tag00274.value,"lower right");
                                 break; 
                        case  6: strcpy(tag00274.value,"upper right");
                                 break;
                        case  8: strcpy(tag00274.value,"lower left");
                                 break;
                        case  9: strcpy(tag00274.value,"undefined");
                                 break;
                        default: sprintf(tag00274.value,"%d",exif_data[11+IFD_Offset+i*12]); 
                     }
                  else
                     switch (exif_data[10+IFD_Offset+i*12])
                     {
                        case  1: strcpy(tag00274.value,"upper left");
                                 break;
                        case  3: strcpy(tag00274.value,"lower right");
                                 break; 
                        case  6: strcpy(tag00274.value,"upper right");
                                 break;
                        case  8: strcpy(tag00274.value,"lower left");
                                 break;
                        case  9: strcpy(tag00274.value,"undefined");
                                 break;
                        default: sprintf(tag00274.value,"%d",exif_data[10+IFD_Offset+i*12]); 
                     }
                  break;
     case   296: // ResolutionUnit               unsigned short       1");
                 tag00296.does_exist=TRUE;
                 if (is_motorola)
                     switch (exif_data[11+IFD_Offset+i*12])
                     {
                        case 1: sprintf(tag00296.value,"no unit"); break;
                        case 2: sprintf(tag00296.value,"inch"); break; 
                        case 3: sprintf(tag00296.value,"centimeter"); break;
                        default : sprintf(tag00296.value,"?????????????????????????????\""); 
                     }
                  else
                     switch (exif_data[10+IFD_Offset+i*12])
                     {
                        case 1: sprintf(tag00296.value,"1=no unit"); break;
                        case 2: sprintf(tag00296.value,"2=inch"); break; 
                        case 3: sprintf(tag00296.value,"3=centimeter"); break;
                        default : sprintf(tag00296.value,"%d",exif_data[11+IFD_Offset+i*12]); 
                     }
                  break;
     case   530: // YCbCrSubSampling             unsigned short       2");
                 tag00530.does_exist=TRUE;
                 if (is_motorola)
                  {
                     switch(exif_data[11+IFD_Offset+i*12])
                     {
                        case  1: sprintf(tag00530.value,"ImageWidth of this chroma image is equal to the ImageWidth of the associated luma image"); break;
                        case  2: sprintf(tag00530.value,"ImageWidth of this chroma image is half the ImageWidth of the associated luma image"); break;
                        case  4: sprintf(tag00530.value,"ImageWidth of this chroma image is one-quarter the ImageWidth of the associated luma image"); break;
                        default: sprintf(tag00530.value,"%d",exif_data[11+IFD_Offset+i*12]); 
                     }
                  }
                  else
                     switch(exif_data[10+IFD_Offset+i*12])
                     {
                        case  1: sprintf(tag00530.value,"ImageWidth of this chroma image is equal to the ImageWidth of the associated luma image"); break;
                        case  2: sprintf(tag00530.value,"ImageWidth of this chroma image is half the ImageWidth of the associated luma image"); break;
                        case  4: sprintf(tag00530.value,"ImageWidth of this chroma image is one-quarter the ImageWidth of the associated luma image"); break;
                        default: sprintf(tag00530.value,"%d",exif_data[10+IFD_Offset+i*12]); 
                     }
                 strcpy(temp_string,tag00530.value);
                 if (is_motorola)
                  {
                     switch(exif_data[10+IFD_Offset+i*12])
                     {
                        case  1: sprintf(temp_string,"ImageLength (height) of this chroma image is equal to the ImageLength of the associated luma image"); break;
                        case  2: sprintf(temp_string,"ImageLength (height) of this chroma image is half the ImageLength of the associated luma image"); break;
                        case  4: sprintf(temp_string,"ImageLength (height) of this chroma image is one-quarter the ImageLength of the associated luma image"); break;
                        default: sprintf(temp_string,"%d",exif_data[10+IFD_Offset+i*12]); 
                     }
                  }
                  else
                     switch(exif_data[11+IFD_Offset+i*12])
                     {
                        case  1: sprintf(temp_string,"ImageLength (height) of this chroma image is equal to the ImageLength of the associated luma image"); break;
                        case  2: sprintf(temp_string,"ImageLength (height) of this chroma image is half the ImageLength of the associated luma image"); break;
                        case  4: sprintf(temp_string,"ImageLength (height) of this chroma image is one-quarter the ImageLength of the associated luma image"); break;
                        default: sprintf(temp_string,"%d",exif_data[11+IFD_Offset+i*12]); 
                     }
                  strcat(tag00530.value,temp_string);
                  break;
     case   531: // YCbCrPositioning             unsigned short       1");
                 tag00531.does_exist=TRUE;
                 if (is_motorola)
                     switch (exif_data[11+IFD_Offset+i*12])
                     {
                        case 1: sprintf(tag00531.value,"center of pixel array"); break;
                        case 2: sprintf(tag00531.value,"the datum point"); break; 
                        default : sprintf(tag00531.value,"%d",exif_data[11+IFD_Offset+i*12]); 
                     }
                  else
                     switch (exif_data[10+IFD_Offset+i*12])
                     {
                        case 1: sprintf(tag00531.value,"center of pixel array"); break;
                        case 2: sprintf(tag00531.value,"the datum point"); break; 
                        default : sprintf(tag00531.value,"%d",exif_data[10+IFD_Offset+i*12]); 
                     }
                  break;

     
     
     
     
     
     
     
     
     
     
     
     
  
     
     
     
     case 33421: // CFARepeatPatternDim          unsigned short       2");
                 tag33421.does_exist=TRUE;
                 if (is_motorola)
                     sprintf(tag33421.value,"CFARepeatRows=%d, CFARepeatCols=%d",exif_data[11+IFD_Offset+i*12], exif_data[10+IFD_Offset+i*12]);
                  else
                     sprintf(tag33421.value,"CFARepeatRows=%d, CFARepeatCols=%d",exif_data[10+IFD_Offset+i*12], exif_data[11+IFD_Offset+i*12]);
                  break;

     
     
     
     
     
     
     
     
     
     
     
     
     case 34857: // Interlace                    unsigned short       1");
                 tag34857.does_exist=TRUE;
                 if (is_motorola)
                  {
                     if (exif_data[11+IFD_Offset+i*12]==0)
                        sprintf(tag34857.value,"0=non-interlaced (progressive scan)");
                     else if (exif_data[11+IFD_Offset+i*12]==1)
                        sprintf(tag34857.value,"1=vertical field 1, horizontal field 1 (video field 1, \"odd\" lines)");
                     else if (exif_data[11+IFD_Offset+i*12]==2)
                        sprintf(tag34857.value,"2=vertical field 2, horizontal field 1 (video field 2, \"even\" lines)");
                     else if (exif_data[11+IFD_Offset+i*12]>=3 && exif_data[11+IFD_Offset+i*12]<=127)
                        sprintf(tag34857.value,"%d=vertical field 3-127, horizontal field 1",exif_data[11+IFD_Offset+i*12]);
                     else if (exif_data[11+IFD_Offset+i*12]==128)
                        sprintf(tag34857.value,"128=vertical field 1, horizontal field 2");
                     else if (exif_data[11+IFD_Offset+i*12]>=129 && exif_data[11+IFD_Offset+i*12]<=255)
                        sprintf(tag34857.value,"%d=vertical field 2-127, horizontal field 2",exif_data[11+IFD_Offset+i*12]);
                     else if (exif_data[11+IFD_Offset+i*12]>=256 && exif_data[11+IFD_Offset+i*12]<=16383)
                        sprintf(tag34857.value,"%d=vertical field 1-127, horizontal field 3-127",exif_data[11+IFD_Offset+i*12]);
                     else
                        sprintf(tag34857.value,"%d",exif_data[11+IFD_Offset+i*12]);
                  }
                  else
                  {
                     if (exif_data[10+IFD_Offset+i*12]==0)
                        sprintf(tag34857.value,"0=non-interlaced (progressive scan)");
                     else if (exif_data[10+IFD_Offset+i*12]==1)
                        sprintf(tag34857.value,"1=vertical field 1, horizontal field 1 (video field 1, \"odd\" lines)");
                     else if (exif_data[10+IFD_Offset+i*12]==2)
                        sprintf(tag34857.value,"2=vertical field 2, horizontal field 1 (video field 2, \"even\" lines)");
                     else if (exif_data[10+IFD_Offset+i*12]>=3 && exif_data[10+IFD_Offset+i*12]<=127)
                        sprintf(tag34857.value,"%d=vertical field 3-127, horizontal field 1",exif_data[10+IFD_Offset+i*12]);
                     else if (exif_data[10+IFD_Offset+i*12]==128)
                        sprintf(tag34857.value,"128=vertical field 1, horizontal field 2");
                     else if (exif_data[10+IFD_Offset+i*12]>=129 && exif_data[10+IFD_Offset+i*12]<=255)
                        sprintf(tag34857.value,"%d=vertical field 2-127, horizontal field 2",exif_data[10+IFD_Offset+i*12]);
                     else if (exif_data[10+IFD_Offset+i*12]>=256 && exif_data[10+IFD_Offset+i*12]<=16383)
                        sprintf(tag34857.value,"%d=vertical field 1-127, horizontal field 3-127",exif_data[10+IFD_Offset+i*12]);
                     else
                        sprintf(tag34857.value,"%d",exif_data[10+IFD_Offset+i*12]);
                  }
                  break;

     
     
     
     
        
     
     
     
     
     
     
     
     
     
     
     
     case 36864: // ExifVersion                  undefined 4           ");
                 tag36864.does_exist=TRUE;
                 if (is_motorola)
                  {
                     sprintf(tag36864.value,"[%d.%d.%d.%d]",exif_data[10+IFD_Offset+i*12],exif_data[11+IFD_Offset+i*12],exif_data[12+IFD_Offset+i*12],exif_data[13+IFD_Offset+i*12]);
                  }
                  else
                  {
                     sprintf(tag36864.value,"[%d.%d.%d.%d]",exif_data[13+IFD_Offset+i*12],exif_data[12+IFD_Offset+i*12],exif_data[11+IFD_Offset+i*12],exif_data[10+IFD_Offset+i*12]);
                  }
                  break;
     case 37121: // ComponentConfiguration       undefined             ");
                 tag37121.does_exist=TRUE;
                 if (is_motorola)
                  {
                     sprintf(tag37121.value,"[%d.%d.%d.%d]",exif_data[10+IFD_Offset+i*12],exif_data[11+IFD_Offset+i*12],exif_data[12+IFD_Offset+i*12],exif_data[13+IFD_Offset+i*12]);
                  }
                  else
                  {
                     sprintf(tag37121.value,"[%d.%d.%d.%d]",exif_data[13+IFD_Offset+i*12],exif_data[12+IFD_Offset+i*12],exif_data[11+IFD_Offset+i*12],exif_data[10+IFD_Offset+i*12]);
                  }
                  break;
     case 37394: // SecurityClassification       ascii string         1");
                 tag37394.does_exist=TRUE;
                 if (is_motorola)
                  {
                     switch(exif_data[11+IFD_Offset+i*12])
                     {
                        case 'T': strcpy(tag37394.value,"T=Top Secret"); break;
                        case 'S': strcpy(tag37394.value,"S=Secret"); break;
                        case 'C': strcpy(tag37394.value,"C=Confidential"); break;
                        case 'R': strcpy(tag37394.value,"R=Restricted"); break;
                        case 'U': strcpy(tag37394.value,"U=Unrestricted"); break;
                        default : sprintf(tag37394.value,"%c",exif_data[11+IFD_Offset+i*12]); 
                     }
                  }
                  else
                     switch(exif_data[10+IFD_Offset+i*12])
                     {
                        case 'T': strcpy(tag37394.value,"T=Top Secret"); break;
                        case 'S': strcpy(tag37394.value,"S=Secret"); break;
                        case 'C': strcpy(tag37394.value,"C=Confidential"); break;
                        case 'R': strcpy(tag37394.value,"R=Restricted"); break;
                        case 'U': strcpy(tag37394.value,"U=Unrestricted"); break;
                        default : sprintf(tag37394.value,"%c",exif_data[10+IFD_Offset+i*12]); 
                     }
                  break;

     
       
     
     
     
     
     //default   : printf("                 tag#=%5d=%d\n",tag_number,offset_to_data_value); // unknown tag
   }
   //printf("\n");
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

void check_for_another_IFD()
{
   int next_IFD;
   if (is_motorola)
   {
      next_IFD=exif_data[IFD_Offset+number_of_directory_entry_contains_in_this_IFD*12+2]*256*256*256+
               exif_data[IFD_Offset+number_of_directory_entry_contains_in_this_IFD*12+3]*256*256+
               exif_data[IFD_Offset+number_of_directory_entry_contains_in_this_IFD*12+4]*256+
               exif_data[IFD_Offset+number_of_directory_entry_contains_in_this_IFD*12+5];
   }
   else
   {
      next_IFD=exif_data[IFD_Offset+number_of_directory_entry_contains_in_this_IFD*12+5]*256*256*256+
               exif_data[IFD_Offset+number_of_directory_entry_contains_in_this_IFD*12+4]*256*256+
               exif_data[IFD_Offset+number_of_directory_entry_contains_in_this_IFD*12+3]*256+
               exif_data[IFD_Offset+number_of_directory_entry_contains_in_this_IFD*12+2];
   }
   IFD_Offset=next_IFD+8;
   next_IFD_exists=next_IFD;
   
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void InitializeVariables()
{
SubSubIFD=FALSE;
i=0,j=0,k=0, jj=0, kk=0, mm=0, ll=0,
      exit_whileloop=0, 
      temp2=0,
      exif_header[0] = '\0',
      exif_data[0] = '\0', 
      length,
      place_in_file=0,
      is_motorola=0,
      Offset_to_first_IFD=8,
      ExifSubIFDOffset=0,
      SubSubOffset=0,
      IFD_Offset=0,
      number_of_directory_entry_contains_in_this_IFD=0,
      diag=1,
      short_tag1 = 0,
      short_tag2 = 0,
      short_tag3 = 0,
      short_tag4 = 0;
result0 = 0,
      result1= 0,
      result2= 0,
      tag_number= 0,
      data_format= 0,
      number_of_components= 0,
      offset_to_data_value= 0,
      next_IFD_exists=0;
temp_char=0;
JPEG_file = NULL;
temp_string[0] = '\0';



      SubSubIFD=FALSE;
      tag00001.value[0]=0;	tag00001.does_exist=FALSE;   strcpy(tag00001.name,"InteroperabilityIndex");
      tag00002.value[0]=0;	tag00002.does_exist=FALSE;   strcpy(tag00002.name,"InteroperabilityVersion");
      tag00254.value[0]=0;	tag00254.does_exist=FALSE;   strcpy(tag00254.name,"NewSubfileType");
      tag00255.value[0]=0;	tag00255.does_exist=FALSE;   strcpy(tag00255.name,"SubfileType");
      tag00256.value[0]=0;	tag00256.does_exist=FALSE;   strcpy(tag00256.name,"ImageWidth");
      tag00257.value[0]=0;	tag00257.does_exist=FALSE;   strcpy(tag00257.name,"ImageLength");
      tag00258.value[0]=0;	tag00258.does_exist=FALSE;   strcpy(tag00258.name,"BitsPerSample");
      tag00259.value[0]=0;	tag00259.does_exist=FALSE;   strcpy(tag00259.name,"Compression");
      tag00262.value[0]=0;	tag00262.does_exist=FALSE;   strcpy(tag00262.name,"PhotometricInterpretation");
      tag00270.value[0]=0;	tag00270.does_exist=FALSE;   strcpy(tag00270.name,"ImageDescription");
      tag00271.value[0]=0;	tag00271.does_exist=FALSE;   strcpy(tag00271.name,"Make");
      tag00272.value[0]=0;	tag00272.does_exist=FALSE;   strcpy(tag00272.name,"Model");
      tag00273.value[0]=0;	tag00273.does_exist=FALSE;   strcpy(tag00273.name,"StripOffsets");
      tag00274.value[0]=0;	tag00274.does_exist=FALSE;   strcpy(tag00274.name,"Orientation");
      tag00277.value[0]=0;	tag00277.does_exist=FALSE;   strcpy(tag00277.name,"SamplesPerPixel");
      tag00278.value[0]=0;	tag00278.does_exist=FALSE;   strcpy(tag00278.name,"RowsPerStrip");
      tag00279.value[0]=0;	tag00279.does_exist=FALSE;   strcpy(tag00279.name,"StripByteConunts");
      tag00282.value[0]=0;	tag00282.does_exist=FALSE;   strcpy(tag00282.name,"XResolution");
      tag00283.value[0]=0;	tag00283.does_exist=FALSE;   strcpy(tag00283.name,"YResolution");
      tag00284.value[0]=0;	tag00284.does_exist=FALSE;   strcpy(tag00284.name,"PlanarConfiguration");
      tag00296.value[0]=0;	tag00296.does_exist=FALSE;   strcpy(tag00296.name,"ResolutionUnit");
      tag00301.value[0]=0;	tag00301.does_exist=FALSE;   strcpy(tag00301.name,"TransferFunction");
      tag00305.value[0]=0;	tag00305.does_exist=FALSE;   strcpy(tag00305.name,"Software");
      tag00306.value[0]=0;	tag00306.does_exist=FALSE;   strcpy(tag00306.name,"DateTime");
      tag00315.value[0]=0;	tag00315.does_exist=FALSE;   strcpy(tag00315.name,"Artist");
      tag00317.value[0]=0;	tag00317.does_exist=FALSE;   strcpy(tag00317.name,"Predictor");
      tag00318.value[0]=0;	tag00318.does_exist=FALSE;   strcpy(tag00318.name,"WhitePoint");
      tag00319.value[0]=0;	tag00319.does_exist=FALSE;   strcpy(tag00319.name,"PrimaryChromaticities");
      tag00322.value[0]=0;	tag00322.does_exist=FALSE;   strcpy(tag00322.name,"TileWidth");
      tag00323.value[0]=0;	tag00323.does_exist=FALSE;   strcpy(tag00323.name,"TileLength");
      tag00324.value[0]=0;	tag00324.does_exist=FALSE;   strcpy(tag00324.name,"TileOffsets");
      tag00325.value[0]=0;	tag00325.does_exist=FALSE;   strcpy(tag00325.name,"TileByteCounts");
      tag00330.value[0]=0;	tag00330.does_exist=FALSE;   strcpy(tag00330.name,"SubIFDs");
      tag00347.value[0]=0;	tag00347.does_exist=FALSE;   strcpy(tag00347.name,"JPEGTables");
      tag00513.value[0]=0;	tag00513.does_exist=FALSE;   strcpy(tag00513.name,"JpegIFOffset");
      tag00514.value[0]=0;	tag00514.does_exist=FALSE;   strcpy(tag00514.name,"JpegIFByteCount");
      tag00529.value[0]=0;	tag00529.does_exist=FALSE;   strcpy(tag00529.name,"YCbCrCoefficients");
      tag00530.value[0]=0;	tag00530.does_exist=FALSE;   strcpy(tag00530.name,"YCbCrSubSampling");
      tag00531.value[0]=0;	tag00531.does_exist=FALSE;   strcpy(tag00531.name,"YCbCrPositioning");
      tag00532.value[0]=0;	tag00532.does_exist=FALSE;   strcpy(tag00532.name,"ReferenceBlackWhite");
      tag33421.value[0]=0;	tag33421.does_exist=FALSE;   strcpy(tag33421.name,"CFARepeatPatternDim");
      tag33422.value[0]=0;	tag33422.does_exist=FALSE;   strcpy(tag33422.name,"CFAPattern");
      tag33423.value[0]=0;	tag33423.does_exist=FALSE;   strcpy(tag33423.name,"BatteryLevel");
      tag33432.value[0]=0;	tag33432.does_exist=FALSE;   strcpy(tag33432.name,"Copyright");
      tag33434.value[0]=0;	tag33434.does_exist=FALSE;   strcpy(tag33434.name,"ExposureTime");
      tag33437.value[0]=0;	tag33437.does_exist=FALSE;   strcpy(tag33437.name,"FNumber");
      tag33723.value[0]=0;	tag33723.does_exist=FALSE;   strcpy(tag33723.name,"IPTC/NAA");
      tag34665.value[0]=0;	tag34665.does_exist=FALSE;   strcpy(tag34665.name,"ExifOffset");
      tag34675.value[0]=0;	tag34675.does_exist=FALSE;   strcpy(tag34675.name,"InterColorProfile");
      tag34850.value[0]=0;	tag34850.does_exist=FALSE;   strcpy(tag34850.name,"ExposureProgram");
      tag34852.value[0]=0;	tag34852.does_exist=FALSE;   strcpy(tag34852.name,"SpectralSensitivity");
      tag34853.value[0]=0;	tag34853.does_exist=FALSE;   strcpy(tag34853.name,"GPSInfo");
      tag34855.value[0]=0;	tag34855.does_exist=FALSE;   strcpy(tag34855.name,"ISOSpeedRatings");
      tag34856.value[0]=0;	tag34856.does_exist=FALSE;   strcpy(tag34856.name,"OECF");
      tag34857.value[0]=0;	tag34857.does_exist=FALSE;   strcpy(tag34857.name,"Interlace");
      tag34858.value[0]=0;	tag34858.does_exist=FALSE;   strcpy(tag34858.name,"TimeZoneOffset");
      tag34859.value[0]=0;	tag34859.does_exist=FALSE;   strcpy(tag34859.name,"SelfTimerMode");
      tag36864.value[0]=0;	tag36864.does_exist=FALSE;   strcpy(tag36864.name,"ExifVersion");
      tag36867.value[0]=0;	tag36867.does_exist=FALSE;   strcpy(tag36867.name,"DateTimeOriginal");
      tag36868.value[0]=0;	tag36868.does_exist=FALSE;   strcpy(tag36868.name,"DateTimeDigitized");
      tag37121.value[0]=0;	tag37121.does_exist=FALSE;   strcpy(tag37121.name,"ComponentConfiguration");
      tag37122.value[0]=0;	tag37122.does_exist=FALSE;   strcpy(tag37122.name,"CompressedBitsPerPixel");
      tag37377.value[0]=0;	tag37377.does_exist=FALSE;   strcpy(tag37377.name,"ShutterSpeedValue");
      tag37378.value[0]=0;	tag37378.does_exist=FALSE;   strcpy(tag37378.name,"ApertureValue");
      tag37379.value[0]=0;	tag37379.does_exist=FALSE;   strcpy(tag37379.name,"BrightnessValue");
      tag37380.value[0]=0;	tag37380.does_exist=FALSE;   strcpy(tag37380.name,"ExposureBiasValue");
      tag37381.value[0]=0;	tag37381.does_exist=FALSE;   strcpy(tag37381.name,"MaxApertureValue");
      tag37382.value[0]=0;	tag37382.does_exist=FALSE;   strcpy(tag37382.name,"SubjectDistance");
      tag37383.value[0]=0;	tag37383.does_exist=FALSE;   strcpy(tag37383.name,"MeteringMode");
      tag37384.value[0]=0;	tag37384.does_exist=FALSE;   strcpy(tag37384.name,"LightSource");
      tag37385.value[0]=0;	tag37385.does_exist=FALSE;   strcpy(tag37385.name,"Flash");
      tag37386.value[0]=0;	tag37386.does_exist=FALSE;   strcpy(tag37386.name,"FocalLength");
      tag37387.value[0]=0;	tag37387.does_exist=FALSE;   strcpy(tag37387.name,"FlashEnergy");
      tag37388.value[0]=0;	tag37388.does_exist=FALSE;   strcpy(tag37388.name,"SpatialFrequencyResponse");
      tag37389.value[0]=0;	tag37389.does_exist=FALSE;   strcpy(tag37389.name,"Noise");
      tag37393.value[0]=0;	tag37393.does_exist=FALSE;   strcpy(tag37393.name,"ImageNumber");
      tag37394.value[0]=0;	tag37394.does_exist=FALSE;   strcpy(tag37394.name,"SecurityClassification");
      tag37395.value[0]=0;	tag37395.does_exist=FALSE;   strcpy(tag37395.name,"ImageHistory");
      tag37396.value[0]=0;	tag37396.does_exist=FALSE;   strcpy(tag37396.name,"SubjectLocation");
      tag37397.value[0]=0;	tag37397.does_exist=FALSE;   strcpy(tag37397.name,"ExposureIndex");
      tag37398.value[0]=0;	tag37398.does_exist=FALSE;   strcpy(tag37398.name,"TIFF/EPStandardID");
      tag37500.value[0]=0;	tag37500.does_exist=FALSE;   strcpy(tag37500.name,"MakerNote");
      tag37510.value[0]=0;	tag37510.does_exist=FALSE;   strcpy(tag37510.name,"UserComment");
      tag37520.value[0]=0;	tag37520.does_exist=FALSE;   strcpy(tag37520.name,"SubSecTime");
      tag37521.value[0]=0;	tag37521.does_exist=FALSE;   strcpy(tag37521.name,"SubSecTimeOriginal");
      tag37522.value[0]=0;	tag37522.does_exist=FALSE;   strcpy(tag37522.name,"SubSecTimeDigitized");
      tag40960.value[0]=0;	tag40960.does_exist=FALSE;   strcpy(tag40960.name,"FlashPixVersion");
      tag40961.value[0]=0;	tag40961.does_exist=FALSE;   strcpy(tag40961.name,"ColorSpace");
      tag40962.value[0]=0;	tag40962.does_exist=FALSE;   strcpy(tag40962.name,"ExifImageWidth");
      tag40963.value[0]=0;	tag40963.does_exist=FALSE;   strcpy(tag40963.name,"ExifImageHeight");
      tag40964.value[0]=0;	tag40964.does_exist=FALSE;   strcpy(tag40964.name,"RelatedSoundFile");
      tag40965.value[0]=0;	tag40965.does_exist=FALSE;   strcpy(tag40965.name,"ExifInteroperabilityOffset");
      tag41483.value[0]=0;	tag41483.does_exist=FALSE;   strcpy(tag41483.name,"FlashEnergy");
      tag41484.value[0]=0;	tag41484.does_exist=FALSE;   strcpy(tag41484.name,"SpatialFrequencyResponse");
      tag41486.value[0]=0;	tag41486.does_exist=FALSE;   strcpy(tag41486.name,"FocalPlaneXResolution");
      tag41487.value[0]=0;	tag41487.does_exist=FALSE;   strcpy(tag41487.name,"FocalPlaneYResolution");
      tag41488.value[0]=0;	tag41488.does_exist=FALSE;   strcpy(tag41488.name,"FocalPlaneResolutionUnit");
      tag41492.value[0]=0;	tag41492.does_exist=FALSE;   strcpy(tag41492.name,"SubjectLocation");
      tag41493.value[0]=0;	tag41493.does_exist=FALSE;   strcpy(tag41493.name,"ExposureIndex");
      tag41495.value[0]=0;	tag41495.does_exist=FALSE;   strcpy(tag41495.name,"SensingMethod");
      tag41728.value[0]=0;	tag41728.does_exist=FALSE;   strcpy(tag41728.name,"FileSource");
      tag41729.value[0]=0;	tag41729.does_exist=FALSE;   strcpy(tag41729.name,"SceneType");
      tag41730.value[0]=0;	tag41730.does_exist=FALSE;   strcpy(tag41730.name,"CFAPattern");
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void PrintTags()
{
             if (tag37378.does_exist==TRUE) printf("%27s=%s\n",tag37378.name,tag37378.value);
             if (tag00315.does_exist==TRUE) printf("%27s=%s\n",tag00315.name,tag00315.value);
             if (tag33423.does_exist==TRUE) printf("%27s=%s\n",tag33423.name,tag33423.value);
             if (tag00258.does_exist==TRUE) printf("%27s=%s\n",tag00258.name,tag00258.value);
             if (tag37379.does_exist==TRUE) printf("%27s=%s\n",tag37379.name,tag37379.value);
             if (tag33422.does_exist==TRUE) printf("%27s=%s\n",tag33422.name,tag33422.value);
             if (tag41730.does_exist==TRUE) printf("%27s=%s\n",tag41730.name,tag41730.value);
             if (tag33421.does_exist==TRUE) printf("%27s=%s\n",tag33421.name,tag33421.value);
             if (tag40961.does_exist==TRUE) printf("%27s=%s\n",tag40961.name,tag40961.value);
             if (tag37121.does_exist==TRUE) printf("%27s=%s\n",tag37121.name,tag37121.value);
             if (tag37122.does_exist==TRUE) printf("%27s=%s\n",tag37122.name,tag37122.value);
             if (tag00259.does_exist==TRUE) printf("%27s=%s\n",tag00259.name,tag00259.value);
             if (tag33432.does_exist==TRUE) printf("%27s=%s\n",tag33432.name,tag33432.value);
             if (tag00306.does_exist==TRUE) printf("%27s=%s\n",tag00306.name,tag00306.value);
             if (tag36868.does_exist==TRUE) printf("%27s=%s\n",tag36868.name,tag36868.value);
             if (tag36867.does_exist==TRUE) printf("%27s=%s\n",tag36867.name,tag36867.value);
             if (tag40963.does_exist==TRUE) printf("%27s=%s\n",tag40963.name,tag40963.value);
             if (tag40962.does_exist==TRUE) printf("%27s=%s\n",tag40962.name,tag40962.value);
             if (tag40965.does_exist==TRUE) printf("%27s=%s\n",tag40965.name,tag40965.value);
             if (tag00001.does_exist==TRUE) printf("%27s=%s\n",tag00001.name,tag00001.value);
             if (tag00002.does_exist==TRUE) printf("%27s=%s\n",tag00002.name,tag00002.value);
             if (tag34665.does_exist==TRUE) printf("%27s=%s\n",tag34665.name,tag34665.value);
             if (tag36864.does_exist==TRUE) printf("%27s=%s\n",tag36864.name,tag36864.value);
             if (tag37380.does_exist==TRUE) printf("%27s=%s\n",tag37380.name,tag37380.value);
             if (tag37397.does_exist==TRUE) printf("%27s=%s\n",tag37397.name,tag37397.value);
             if (tag41493.does_exist==TRUE) printf("%27s=%s\n",tag41493.name,tag41493.value);
             if (tag34850.does_exist==TRUE) printf("%27s=%s\n",tag34850.name,tag34850.value);
             if (tag33434.does_exist==TRUE) printf("%27s=%s\n",tag33434.name,tag33434.value);
             if (tag33437.does_exist==TRUE) printf("%27s=%s\n",tag33437.name,tag33437.value);
             if (tag41728.does_exist==TRUE) printf("%27s=%s\n",tag41728.name,tag41728.value);
             if (tag37385.does_exist==TRUE) printf("%27s=%s\n",tag37385.name,tag37385.value);
             if (tag37387.does_exist==TRUE) printf("%27s=%s\n",tag37387.name,tag37387.value);
             if (tag41483.does_exist==TRUE) printf("%27s=%s\n",tag41483.name,tag41483.value);
             if (tag40960.does_exist==TRUE) printf("%27s=%s\n",tag40960.name,tag40960.value);
             if (tag37386.does_exist==TRUE) printf("%27s=%s\n",tag37386.name,tag37386.value);
             if (tag41488.does_exist==TRUE) printf("%27s=%s\n",tag41488.name,tag41488.value);
             if (tag41486.does_exist==TRUE) printf("%27s=%s\n",tag41486.name,tag41486.value);
             if (tag41487.does_exist==TRUE) printf("%27s=%s\n",tag41487.name,tag41487.value);
             if (tag34853.does_exist==TRUE) printf("%27s=%s\n",tag34853.name,tag34853.value);
             if (tag33723.does_exist==TRUE) printf("%27s=%s\n",tag33723.name,tag33723.value);
             if (tag34855.does_exist==TRUE) printf("%27s=%s\n",tag34855.name,tag34855.value);
             if (tag00270.does_exist==TRUE) printf("%27s=%s\n",tag00270.name,tag00270.value);
             if (tag37395.does_exist==TRUE) printf("%27s=%s\n",tag37395.name,tag37395.value);
             if (tag00257.does_exist==TRUE) printf("%27s=%s\n",tag00257.name,tag00257.value);
             if (tag37393.does_exist==TRUE) printf("%27s=%s\n",tag37393.name,tag37393.value);
             if (tag00256.does_exist==TRUE) printf("%27s=%s\n",tag00256.name,tag00256.value);
             if (tag34675.does_exist==TRUE) printf("%27s=%s\n",tag34675.name,tag34675.value);
             if (tag34857.does_exist==TRUE) printf("%27s=%s\n",tag34857.name,tag34857.value);
             if (tag00347.does_exist==TRUE) printf("%27s=%s\n",tag00347.name,tag00347.value);
             if (tag00514.does_exist==TRUE) printf("%27s=%s\n",tag00514.name,tag00514.value);
             if (tag00513.does_exist==TRUE) printf("%27s=%s\n",tag00513.name,tag00513.value);
             if (tag37384.does_exist==TRUE) printf("%27s=%s\n",tag37384.name,tag37384.value);
             if (tag00271.does_exist==TRUE) printf("%27s=%s\n",tag00271.name,tag00271.value);
             if (tag37500.does_exist==TRUE) printf("%27s=%s\n",tag37500.name,tag37500.value);
             if (tag37381.does_exist==TRUE) printf("%27s=%s\n",tag37381.name,tag37381.value);
             if (tag37383.does_exist==TRUE) printf("%27s=%s\n",tag37383.name,tag37383.value);
             if (tag00272.does_exist==TRUE) printf("%27s=%s\n",tag00272.name,tag00272.value);
             if (tag00254.does_exist==TRUE) printf("%27s=%s\n",tag00254.name,tag00254.value);
             if (tag37389.does_exist==TRUE) printf("%27s=%s\n",tag37389.name,tag37389.value);
             if (tag34856.does_exist==TRUE) printf("%27s=%s\n",tag34856.name,tag34856.value);
             if (tag00274.does_exist==TRUE) printf("%27s=%s\n",tag00274.name,tag00274.value);
             if (tag00262.does_exist==TRUE) printf("%27s=%s\n",tag00262.name,tag00262.value);
             if (tag00284.does_exist==TRUE) printf("%27s=%s\n",tag00284.name,tag00284.value);
             if (tag00317.does_exist==TRUE) printf("%27s=%s\n",tag00317.name,tag00317.value);
             if (tag00319.does_exist==TRUE) printf("%27s=%s\n",tag00319.name,tag00319.value);
             if (tag00532.does_exist==TRUE) printf("%27s=%s\n",tag00532.name,tag00532.value);
             if (tag40964.does_exist==TRUE) printf("%27s=%s\n",tag40964.name,tag40964.value);
             if (tag00296.does_exist==TRUE) printf("%27s=%s\n",tag00296.name,tag00296.value);
             if (tag00278.does_exist==TRUE) printf("%27s=%s\n",tag00278.name,tag00278.value);
             if (tag00277.does_exist==TRUE) printf("%27s=%s\n",tag00277.name,tag00277.value);
             if (tag41729.does_exist==TRUE) printf("%27s=%s\n",tag41729.name,tag41729.value);
             if (tag37394.does_exist==TRUE) printf("%27s=%s\n",tag37394.name,tag37394.value);
             if (tag34859.does_exist==TRUE) printf("%27s=%s\n",tag34859.name,tag34859.value);
             if (tag41495.does_exist==TRUE) printf("%27s=%s\n",tag41495.name,tag41495.value);
             if (tag37377.does_exist==TRUE) printf("%27s=%s\n",tag37377.name,tag37377.value);
             if (tag00305.does_exist==TRUE) printf("%27s=%s\n",tag00305.name,tag00305.value);
             if (tag37388.does_exist==TRUE) printf("%27s=%s\n",tag37388.name,tag37388.value);
             if (tag41484.does_exist==TRUE) printf("%27s=%s\n",tag41484.name,tag41484.value);
             if (tag34852.does_exist==TRUE) printf("%27s=%s\n",tag34852.name,tag34852.value);
             if (tag00279.does_exist==TRUE) printf("%27s=%s\n",tag00279.name,tag00279.value);
             if (tag00273.does_exist==TRUE) printf("%27s=%s\n",tag00273.name,tag00273.value);
             if (tag00330.does_exist==TRUE) printf("%27s=%s\n",tag00330.name,tag00330.value);
             if (tag37520.does_exist==TRUE) printf("%27s=%s\n",tag37520.name,tag37520.value);
             if (tag37522.does_exist==TRUE) printf("%27s=%s\n",tag37522.name,tag37522.value);
             if (tag37521.does_exist==TRUE) printf("%27s=%s\n",tag37521.name,tag37521.value);
             if (tag00255.does_exist==TRUE) printf("%27s=%s\n",tag00255.name,tag00255.value);
             if (tag37382.does_exist==TRUE) printf("%27s=%s\n",tag37382.name,tag37382.value);
             if (tag37396.does_exist==TRUE) printf("%27s=%s\n",tag37396.name,tag37396.value);
             if (tag41492.does_exist==TRUE) printf("%27s=%s\n",tag41492.name,tag41492.value);
             if (tag37398.does_exist==TRUE) printf("%27s=%s\n",tag37398.name,tag37398.value);
             if (tag00325.does_exist==TRUE) printf("%27s=%s\n",tag00325.name,tag00325.value);
             if (tag00323.does_exist==TRUE) printf("%27s=%s\n",tag00323.name,tag00323.value);
             if (tag00324.does_exist==TRUE) printf("%27s=%s\n",tag00324.name,tag00324.value);
             if (tag00322.does_exist==TRUE) printf("%27s=%s\n",tag00322.name,tag00322.value);
             if (tag34858.does_exist==TRUE) printf("%27s=%s\n",tag34858.name,tag34858.value);
             if (tag00301.does_exist==TRUE) printf("%27s=%s\n",tag00301.name,tag00301.value);
             if (tag37510.does_exist==TRUE) printf("%27s=%s\n",tag37510.name,tag37510.value);
             if (tag00318.does_exist==TRUE) printf("%27s=%s\n",tag00318.name,tag00318.value);
             if (tag00282.does_exist==TRUE) printf("%27s=%s\n",tag00282.name,tag00282.value);
             if (tag00529.does_exist==TRUE) printf("%27s=%s\n",tag00529.name,tag00529.value);
             if (tag00531.does_exist==TRUE) printf("%27s=%s\n",tag00531.name,tag00531.value);
             if (tag00530.does_exist==TRUE) printf("%27s=%s\n",tag00530.name,tag00530.value);
             if (tag00283.does_exist==TRUE) printf("%27s=%s\n",tag00283.name,tag00283.value);
             printf("\n\n");
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
