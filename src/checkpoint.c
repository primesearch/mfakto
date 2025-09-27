/*
This file is part of mfaktc (mfakto).
Copyright (C) 2009 - 2014  Oliver Weihe (o.weihe@t-online.de)

mfaktc (mfakto) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

mfaktc (mfakto) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with mfaktc (mfakto).  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>

#include "crc.h"
#include "output.h"
#include "params.h"
#include "timer.h"
#include "my_types.h"

extern mystuff_t    mystuff;

/*
checkpoint_write() writes the checkpoint file.
*/
void checkpoint_write(unsigned int exp, int bit_min, int bit_max, unsigned int cur_class, int num_factors, int96 factors[MAX_FACTORS_PER_JOB], unsigned long long int bit_level_time)
{
  FILE *f;
  char buffer[600], filename[32], filename_save[32], filename_write[32], factors_buffer[MAX_FACTOR_BUFFER_LENGTH];
  unsigned int i, res, factors_buffer_length;

  sprintf(filename, "M%u.ckp", exp);
  sprintf(filename_save, "M%u.ckp.bu", exp);
  sprintf(filename_write, "M%u.ckp.write", exp);

  if (factors[0].d0 || factors[0].d1 || factors[0].d2) {
      i = 0;
      char factor[MAX_DEZ_96_STRING_LENGTH];
      print_dez96(factors[i++], factor);
      factors_buffer_length = sprintf(factors_buffer, "%s", factor);
      for (; i < MAX_FACTORS_PER_JOB; i++) {
          if (factors[i].d0 || factors[i].d1 || factors[i].d2) {
              print_dez96(factors[i], factor);
              factors_buffer_length += sprintf(factors_buffer + factors_buffer_length, ",%s", factor);
          }
      }
  }
  else {
      sprintf(factors_buffer, "0");
  }
  
  f=fopen(filename_write, "w");
  if(f==NULL)
  {
    printf("Warning: Could not create checkpoint file \"%s\"\n", filename_write);
    return;
  }
  else
  {
    /*struct timeval cptimer;
              unsigned long long c1, c2, c3, c4, c5;
              timer_init(&cptimer); */

      sprintf(buffer, "%u %d %d %d %s: %d %d %s %llu", exp, bit_min, bit_max, mystuff.num_classes, MFAKTO_VERSION, cur_class, num_factors, strlen(factors_buffer) ? factors_buffer : "0", bit_level_time);
      i = crc32_checksum(buffer, (int)strlen(buffer));
      //              c1 = timer_diff(&cptimer);
      i = fprintf(f, "%u %d %d %d %s: %d %d %s %llu %08X\n", exp, bit_min, bit_max, mystuff.num_classes, MFAKTO_VERSION, cur_class, num_factors, strlen(factors_buffer) ? factors_buffer : "0", bit_level_time, i);
//              c2 = timer_diff(&cptimer);
    res=fclose(f);
//              c3 = timer_diff(&cptimer);
    if ((i>34) && (res==0))
    {
      remove(filename_save); // don't care if it fails, it may not have existed
//              c4 = timer_diff(&cptimer);
      (void) rename(filename, filename_save); // ditto
      if (rename(filename_write, filename))
      {
        printf("Warning: renaming %s to %s failed.\n", filename_write, filename);
      }
    }
    else
    {
      printf("Warning: Could not write checkpoint file \"%s\", %u chars written.\n", filename_write, i);
    }
/*               c5 = timer_diff(&cptimer);
    printf("\nCP in %lld us, %lld us, %lld us, %lld us, %lld us\n", c1, c2, c3, c4, c5); */
  }
}


/*
checkpoint_read() reads the checkpoint file and compares values for exp,
bit_min, bit_max, NUM_CLASSES read from file with current values.
If these parameters are equal than it sets cur_class and num_factors,
factors, and bit_level_time to the values from the checkpoint file.

returns 1 on success (valid checkpoint file)
returns 0 otherwise
*/
int checkpoint_read(unsigned int exp, int bit_min, int bit_max, unsigned int *cur_class, int* num_factors, int96 factors[MAX_FACTORS_PER_JOB], unsigned long long int* bit_level_time, int verbosity)
{
  FILE *f;
  int ret=0,i,chksum;
  char buffer[600], buffer2[600], *ptr, *ptr2, filename[20], filename_save[32], version[81], factors_buffer[MAX_FACTOR_BUFFER_LENGTH];
  
  for(i=0;i<600;i++)buffer[i]=0;

  *cur_class=-1;
  *num_factors=0;
  
  sprintf(filename, "M%u.ckp", exp);
  
  f=fopen(filename, "r");
  if(f==NULL)
  {
    if (verbosity>1) printf("No checkpoint file \"%s\" found.\n", filename);
    return 0;
  }
  i=(int)fread(buffer,sizeof(char),599,f);
  buffer[i] = 0;
  sprintf(buffer2,"%u %d %d %d ", exp, bit_min, bit_max, mystuff.num_classes);
  ptr=strstr(buffer, buffer2);
  if(ptr==buffer)
  {
    i=(int)strlen(buffer2);
    if(i<70)
    {
      ptr2=&(buffer[i]);
      ptr=strstr(ptr2, ": ");
      if (ptr > ptr2)
      {
        strncpy(version, ptr2, ptr-ptr2);
        version[ptr-ptr2]='\0';
      }
      else sprintf(version, "%s", MFAKTO_VERSION);
      (void) sscanf(ptr,": %d %d %s %llu", cur_class, num_factors, factors_buffer, bit_level_time);
      sprintf(buffer2,"%u %d %d %d %s: %d %d %s %llu", exp, bit_min, bit_max, mystuff.num_classes, version, *cur_class, *num_factors, factors_buffer, *bit_level_time);
      chksum= crc32_checksum(buffer2,(int)strlen(buffer2));
      // no trainling '\n' for the compare buffer to allow interchanging \n\r and \n files 
      i=sprintf(buffer2,"%u %d %d %d %s: %d %d %s %llu %08X", exp, bit_min, bit_max, mystuff.num_classes, version, *cur_class, *num_factors, factors_buffer, *bit_level_time, chksum);
      if(*cur_class >= 0 && \
         *cur_class < mystuff.num_classes && \
         *num_factors >= 0 && \
         strncmp(buffer, buffer2, i) == 0 && \
         ((*num_factors == 0 && strlen(factors_buffer) == 1) || \
          (*num_factors >= 1 && strlen(factors_buffer) > 1)))
      {
        ret=1;
      }
      else
      {
        if (verbosity>0) printf("Cannot use checkpoint file \"%s\": Bad content \"%s\".\n", filename, buffer);
      }

      // checkpoint file has no factors
      if (factors_buffer[0] == '0') {
          for (i = 0; i < MAX_FACTORS_PER_JOB; i++) {
              factors[i].d0 = 0;
              factors[i].d1 = 0;
              factors[i].d2 = 0;
          }
      }
      else {
          // checkpoint file may have factors
          char* tok = strtok(factors_buffer, ",");
          for (i = 0; i < MAX_FACTORS_PER_JOB; i++) {
              if (tok == NULL) {
                  factors[i].d0 = 0;
                  factors[i].d1 = 0;
                  factors[i].d2 = 0;
              }
              else {
                  factors[i] = parse_dez96(tok);
                  tok = strtok(NULL, ",");
              }
          }
      }
    }
  }
  else
  {
    if (verbosity>0) printf("Cannot use checkpoint file \"%s\": Content \"%s\" does not match expected \"%s\".\n", filename, buffer, buffer2);
  }
  fclose(f);
  if (ret==0)
  {
    sprintf(filename_save, "M%u.ckp.bad-%08X", exp, crc32_checksum(buffer,(int)strlen(buffer))); // append some "random" number (same number means same content)
    if (rename(filename, filename_save) == 0)
    {
      if (verbosity>0) printf("Renamed bad checkpoint file \"%s\" to \"%s\"\n", filename, filename_save);
    }

    sprintf(filename_save, "M%u.ckp.bu", exp);
    if (rename(filename_save, filename) == 0)
    {
      if (verbosity>1) printf("Renamed backup file \"%s\" to \"%s\", trying to load it.\n", filename_save, filename);
      return checkpoint_read(exp, bit_min, bit_max, cur_class, num_factors, factors, bit_level_time, mystuff.verbosity);
    }
  }
  return ret;
}


void checkpoint_delete(unsigned int exp)
/*
tries to delete the checkpoint file
*/
{
  char filename[32];
  sprintf(filename, "M%u.ckp", exp);
  remove(filename);
  sprintf(filename, "M%u.ckp.bu", exp);
  remove(filename);
  sprintf(filename, "M%u.ckp.write", exp);
  remove(filename);
}
