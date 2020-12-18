
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#if defined (OS2) || defined (WIN32)
    #include <process.h>
#if defined (__MINGW32__) || defined (__MINGW64__)
        #include <unistd.h>
    #endif
    #include <direct.h>
#else
    #include <unistd.h>
#endif

#include <time.h>
#include <errno.h>

#ifndef WIN32
    #include <libgen.h>
#endif

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "nvutility.h"

#include "binaryFeatureData.h"
#include "version.h"

char newdirname[256];


/***************************************************************************\
*                                                                           *
*   Module Name:        sget_coord                                          *
*                                                                           *
*   Programmer:         Jan C. Depner                                       *
*                                                                           *
*   Date Written:       November 2005                                       *
*                                                                           *
*   Module Security                                                         *
*   Classification:     Unclassified                                        *
*                                                                           *
*   Data Security                                                           *
*   Classification:     Unknown                                             *
*                                                                           *
*   Purpose:            Gets a pair of geographic coordinates, a remark,    *
*                       and a depth from a string.                          *
*                                                                           *
*   Inputs:             lat_hemi            -   latitude hemisphere         *
*                                               indicator (S or N)          *
*                       lat_deg             -   latitude degrees            *
*                       lat_min             -   latitude minutes            *
*                       lat_sec             -   latitude seconds            *
*                       lon_hemi            -   longitude hemisphere        *
*                                               indicator (S or N)          *
*                       lon_deg             -   longitude degrees           *
*                       lon_min             -   longitude minutes           *
*                       lon_sec             -   longitude seconds           *
*                                                                           *
*   Outputs:            uint8_t             -   False if end of file        *
*                                                                           *
*   Restrictions:       Geographic positions are entered as a lat, lon pair *
*                       separated by a comma.  A lat or lon may be in any   *
*                       of the following formats (degrees, minutes, and     *
*                       seconds must be separated by a space or tab) :      *
*                                                                           *
*                           Degrees decimal                 : S 28.4532     *
*                           Degrees minutes decimal         : S 28 27.192   *
*                           Degrees minutes seconds decimal : S 28 27 11.52 *
*                                                                           *
*                       Hemisphere may be indicated by letter or by sign.   *
*                       West longitude and south latitude are negative :    *
*                                                                           *
*                           Ex. : -28 27 11.52 = S28 27 11.52 = s 28 27.192 *
*                                                                           *
\***************************************************************************/

static char *sget_coord (char *string, char *lat_hemi, int32_t *lat_deg, int32_t *lat_min, float *lat_sec,
                            char *lon_hemi, int32_t *lon_deg, int32_t *lon_min, float *lon_sec, 
                            float *depth)
{
  int32_t     j, sign;
  uint32_t    i;
  char        lstring[160], lat[30], lon[30], depth_string[30];
  double      f1, f2, f3, fdeg, fmin, fsec;
  static char remarks[100];


  /*  Break the input into a lat and lon string.*/

  strcpy (lstring, string);

  strcpy (lat, strtok (lstring, ","));
  strcpy (lon, strtok (NULL, ","));
  strcpy (remarks, strtok (NULL, ","));
  strcpy (depth_string, strtok (NULL, ","));


  /*  Save the depth if it's there.  */

  if (!sscanf (depth_string, "%f", depth)) *depth = 0.0;


  /*  Handle the latitude (j = 0) and longitude (j = 1) portions of
      the input string. */

  for (j = 0 ; j < 2 ; j++)
    {
      if (j)
        {
          strcpy (lstring, lon);
        }
      else
        {
          strcpy (lstring, lat);
        }

      sign = 0;


      /*  Check for and clear sign or hemisphere indicators.*/

      for (i = 0 ; i < strlen (lstring) ; i++)
        {
          if (j)
            {
              if (lstring[i] == 'W' || lstring[i] == 'w' || lstring[i] == '-')
                {
                  lstring[i] = ' ';
                  sign = 1;
                }
            }
          else
            {
              if (lstring[i] == 'S' || lstring[i] == 's' || lstring[i] == '-')
                {
                  lstring[i] = ' ';
                  sign = 1;
                }
            }

          if (lstring[i] == 'n' || lstring[i] == 'N' || lstring[i] == 'e' ||
              lstring[i] == 'E' || lstring[i] == '+') lstring[i] = ' ';
        }
    
      fdeg = 0.0;
      fmin = 0.0;
      fsec = 0.0;
      f1 = 0.0;
      f2 = 0.0;
      f3 = 0.0;


      /*  Convert the string to degrees, minutes, and seconds.*/
        
      i = sscanf (lstring, "%lf %lf %lf", &f1, &f2, &f3);


      /*  Based on the number of values scanned, compute the total
          degrees.*/
        
      switch (i)
        {
        case 3:
          fsec = f3 / 3600.0;
#ifdef NVLinux
          __attribute__ ((fallthrough));
#endif

        case 2:
          fmin = f2 / 60.0;
#ifdef NVLinux
          __attribute__ ((fallthrough));
#endif

        case 1:
          fdeg = f1;
        }

      fdeg += fmin + fsec;


      /*  Get the sign and load the lat or lon values.*/
        
      if (j)
        {
          if (sign)
            {
              *lon_hemi = 'W';
            }
          else
            {
              *lon_hemi = 'E';
            }

          *lon_deg = (int32_t) fdeg;
          fmin = (fdeg - *lon_deg) * 60.0;
          *lon_min = (int32_t) (fmin + 0.00001);
          *lon_sec = (fmin - *lon_min) * 60.0 + 0.00001;
        }
      else
        {
          if (sign)
            {
              *lat_hemi = 'S';
            }
          else
            {
              *lat_hemi = 'N';
            }
            
          *lat_deg = (int32_t) fdeg;
          fmin = (fdeg - *lat_deg) * 60.0;
          *lat_min = (int32_t) (fmin + 0.00001);
          *lat_sec = (fmin - *lat_min) * 60.0 + 0.00001;
        }
    }

  return (remarks);
}



/***************************************************************************\
*                                                                           *
*   Module Name:        build_feature                                       *
*                                                                           *
*   Programmer:         Jan C. Depner                                       *
*                                                                           *
*   Date Written:       February 2000                                       *
*                                                                           *
*   Purpose:            This program builds a Binary Feature Data (BFD)     *
*                       file from text files, csv files, or uni text files. *
*                                                                           *
*   Inputs:             argn                -   number of command line      *
*                                               arguments                   *
*                       argv                -   command line arguments      *
*                                                                           *
*   Outputs:            none                                                *
*                                                                           *
\***************************************************************************/

int32_t main (int32_t argn, char **argv)
{
  char                TRGfil[512], bfd_name[512], image_name[512], string[1024], new_dir[512], old_dir[512],
                      lat_hemi = ' ', lon_hemi, contact_id[60], remarks[100], desc[100], dtg[24], cut[512],
                      fname[512];
  int32_t             year, day, hour, minute, month, mday, count = 0, check, latdeg, latmin, latsec,
                      londeg, lonmin, lonsec, row, col, bfd_handle;
  uint32_t            i;
  float               second, flatsec = 0.0, flonsec, depth;
  BFDATA_HEADER       bfd_header;
  BFDATA_RECORD       bfd_record;
  FILE                *fp;
  time_t              current_time;




  fprintf (stderr, "\n\n %s \n\n", VERSION);
  fflush (stderr);


  if (argn < 3)
    {
      fprintf (stderr, 
               "Usage: build_feature <.csv file | .uni file | .txt file> <bfd feature file>\n\n\n");

      fprintf (stderr, "Press 'Enter' to continue:");
      fflush (stderr);
      ngets (remarks, sizeof (remarks), stdin);

      fprintf (stderr, "\n\n.csv files must be in the following format:\n\n");
      fprintf (stderr, "\tDESCRIPTION,REMARKS,SDDD MM SS,SDD MM SS,DEPTH\n\n");
      fprintf (stderr, "Description and remarks cannot be more than 100 characters\n");
      fprintf (stderr, "SDDD is longitude, west negative, SDD is latitude, south negative\n\n\n");

      fprintf (stderr, "Press 'Enter' to continue:");
      fflush (stderr);
      ngets (remarks, sizeof (remarks), stdin);

      fprintf (stderr, "\n\n.uni files must be in the following format:\n\n");
      fprintf (stderr, "A one line text header (ignored) that looks like this:\n\n");
      fprintf (stderr, "unisips file,snippet file,latitude,longitude,row,column,depth,heading,length,width,height,dtg,desc,remarks\n\n");
      fprintf (stderr, "Followed by comma separated data records in the order indicated by the above header.\n");
      fprintf (stderr, "The depth field is broken up into three fields - B: xx.x / T: xx.x / A: xx.x where:\n\n");
      fprintf (stderr, "A is tow fish altitude above bottom, T is tow fish depth, and B is total water depth\n");
      fprintf (stderr, "A typical record would look something like this:\n\n");
      fprintf (stderr, "/unisips/k5-henson-2005306-071349-s.u,7976-606.gif,27.114496,32.570417,7975,605,B: 26.3 / T: 8.0 /  A: 18.3,227.7218, 86.04, 13.84, 14.22,11-02-2005  07:25:33.70,wreck lost in nadir\n\n\n");


      fprintf (stderr, "Press 'Enter' to continue:");
      fflush (stderr);
      ngets (remarks, sizeof (remarks), stdin);

      fprintf (stderr, "\n\n.txt file format contains geographic positions, a description field,\n");
      fprintf (stderr, "and a depth, separated by commas.  The geographic positions may be in any\n");
      fprintf (stderr, "of the following formats (degrees, minutes, and seconds must be separated\n");
      fprintf (stderr, "by a space or tab):\n\n");
      fprintf (stderr, "\tHemisphere Degrees decimal                 : S 28.4532\n");
      fprintf (stderr, "\tHemisphere Degrees minutes decimal         : S 28 27.192\n");
      fprintf (stderr, "\tHemisphere Degrees minutes seconds decimal : S 28 27 11.52\n");
      fprintf (stderr, "\tSigned Degrees decimal                     : -28.4532\n");
      fprintf (stderr, "\tSigned Degrees minutes decimal             : -28 27.192\n");
      fprintf (stderr, "\tSigned Degrees minutes seconds decimal     : -28 27 11.52\n\n");
      fprintf (stderr, "West longitude and south latitude are negative :\n\n");
      fprintf (stderr, "\tEx. : -28 27 11.52 = S28 27 11.52 = s 28 27.192\n\n");

      fprintf (stderr, "Description cannot be more than 100 characters\n\n");
      fprintf (stderr, "Example: build_feature trg31g.trg pensacola.bfd\n\n\n");
      fflush (stderr);
      exit (-1);
    }


  /*  Strip off leading ./ if it's there.  MSYS may cause mixed separators on Windoze.  */

  if (argv[1][0] == '.' && (argv[1][1] == '/' || argv[1][1] == '\\'))
    {
      strcpy (TRGfil, &argv[1][2]);
    }
  else
    {
      strcpy (TRGfil, argv[1]);
    }


  /*  Get the fully qualified input file name.  */

#ifdef WIN32

  if (_getcwd (old_dir, sizeof (old_dir)) == NULL)
    {
      fprintf (stderr, "Error return from getcwd in file %s, function %s at line %d.  This should never happen!", __FILE__, __FUNCTION__, __LINE__ - 2);
      fflush (stderr);
    }

  if (_chdir (gen_dirname (argv[1])) == -1)
    {
      fprintf (stderr, "Error return from chdir in file %s, function %s at line %d.  This should never happen!", __FILE__, __FUNCTION__, __LINE__ - 2);
      fflush (stderr);
    }

  if (_getcwd (new_dir, sizeof (new_dir)) == NULL)
    {
      fprintf (stderr, "Error return from getcwd in file %s, function %s at line %d.  This should never happen!", __FILE__, __FUNCTION__, __LINE__ - 2);
      fflush (stderr);
    }

  sprintf (string, "%s\\%s", new_dir, gen_basename(TRGfil));
  strcpy (TRGfil, string);

  if (_chdir (old_dir) == -1)
    {
      fprintf (stderr, "Error return from chdir in file %s, function %s at line %d.  This should never happen!", __FILE__, __FUNCTION__, __LINE__ - 2);
      fflush (stderr);
    }

#else 

  if (getcwd (old_dir, sizeof (old_dir)) == NULL)
    {
      fprintf (stderr, "Error return from getcwd in file %s, function %s at line %d.  This should never happen!", __FILE__, __FUNCTION__, __LINE__ - 2);
      fflush (stderr);
    }

  if (chdir (gen_dirname (argv[1])) == -1)
    {
      fprintf (stderr, "Error return from chdir in file %s, function %s at line %d.  This should never happen!", __FILE__, __FUNCTION__, __LINE__ - 2);
      fflush (stderr);
    }

  if (getcwd (new_dir, sizeof (new_dir)) == NULL)
    {
      fprintf (stderr, "Error return from getcwd in file %s, function %s at line %d.  This should never happen!", __FILE__, __FUNCTION__, __LINE__ - 2);
      fflush (stderr);
    }

  sprintf (string, "%s/%s", new_dir, TRGfil);
  strcpy (TRGfil, string);

  if (chdir (old_dir) == -1)
    {
      fprintf (stderr, "Error return from chdir in file %s, function %s at line %d.  This should never happen!", __FILE__, __FUNCTION__, __LINE__ - 2);
      fflush (stderr);
    }

#endif


  fprintf (stderr, "Processing %s\n", TRGfil);
  fflush (stderr);


  strcpy (bfd_name, argv[2]);


  /*  Check for .bfd extension.  */

  if (!strstr (bfd_name, ".bfd"))
    {
      fprintf (stderr, "\nOutput file %s doesn't have .bfd extension.\n\n", bfd_name);
      exit (-1);
    }


  /*  Make sure that we can open and write to the output .bfd file.  */

  if ((bfd_handle = binaryFeatureData_open_file (bfd_name, &bfd_header, BFDATA_UPDATE)) < 0)
    {
      strcpy (bfd_header.creation_software, VERSION);

      if ((bfd_handle = binaryFeatureData_create_file (bfd_name, bfd_header)) < 0)
        {
          binaryFeatureData_perror ();
          exit (-1);
        }

      printf ("\nCreating file %s\n\n", bfd_name);
    }
  else
    {
      printf ("\nAppending to file %s\n", bfd_name);
    }


  if ((fp = fopen (TRGfil, "r")) == NULL)
    {
      perror (TRGfil);
      exit (-1);
    }

  while (fgets (string, sizeof (string), fp) != NULL)
    {
      memset (&bfd_record, 0, sizeof (BFDATA_RECORD));


      /*  Drop the header if it's there.  */

      if ((strstr (string, "LONG") && strstr (string, "LAT")) || strstr (string, "latitude")) continue;


      current_time = time (&current_time);

      cvtime ((time_t) current_time, 0, &year, &day, &hour, &minute, &second);
      year += 1900;


      sprintf (contact_id, "%02d%03d%02d%02d%02d%03d", year % 100, day, hour, minute, (int32_t) second, count);


      strcpy (image_name, "");

      if (strstr (TRGfil, ".txt"))
        {
          strcpy (bfd_record.remarks, sget_coord (string, &lat_hemi, &latdeg, &latmin, &flatsec, &lon_hemi, 
                                                  &londeg, &lonmin, &flonsec, &bfd_record.depth));
          bfd_record.latitude = (double) latdeg + (double) latmin / 60.0 + (double) flatsec / 3600.0;
          if (lat_hemi == 'S') bfd_record.latitude = -bfd_record.latitude;

          bfd_record.longitude = (double) londeg + (double) lonmin / 60.0 + (double) flonsec / 3600.0;
          if (lon_hemi == 'W') bfd_record.longitude = -bfd_record.longitude;
        }
      else if (strstr (TRGfil, ".uni"))
        {
          strcpy (fname, strtok (string, ","));
          strcpy (image_name, strtok (NULL, ","));

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "%lf", &bfd_record.latitude);

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "%lf", &bfd_record.longitude);

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "%d", &row);

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "%d", &col);

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "B: %f / T: %f /  A: %f", &bfd_record.depth, &bfd_record.width, &bfd_record.height);

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "%f", &bfd_record.heading);

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "%f", &bfd_record.length);

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "%f", &bfd_record.width);

          strcpy (cut, strtok (NULL, ","));
          sscanf (cut, "%f", &bfd_record.height);

          strcpy (dtg, strtok (NULL, ","));
          sscanf (dtg, "%d-%d-%d  %d:%d:%f", &month, &mday, &year, &hour, &minute, &second);

          mday2jday (year, month, mday, &day);
          sprintf (contact_id, "%02d%03d%02d%02d%02d%03d", year % 100, day, hour, minute, (int32_t) second, count);

          strcpy (desc, strtok (NULL, ","));
        }
      else
        {
          /*  Look for the second comma.  */

          check = 0;
          for (i = 0 ; i < strlen (string) ; i++)
            {
              if (string[i] == ',') check++;
              if (check == 2) break;
            }

          strncpy (desc, string, i);
          desc[i] = 0;
          sprintf (bfd_record.remarks, "NAVO - %s", desc);

          sscanf (&string[i + 1], "%d %d %d,%d %d %d, %f", &londeg, &lonmin, &lonsec, &latdeg, &latmin, &latsec, &depth);


          bfd_record.latitude = (double) latdeg + (double) latmin / 60.0 + (double) latsec / 3600.0;
          if (latdeg < 0) bfd_record.latitude = -bfd_record.latitude;

          bfd_record.longitude = (double) londeg + (double) lonmin / 60.0 + (double) lonsec / 3600.0;
          if (londeg < 0) bfd_record.longitude = -bfd_record.longitude;
        }


      inv_cvtime (year - 1900, day, hour, minute, second, &bfd_record.event_tv_sec, &bfd_record.event_tv_nsec);

      bfd_record.confidence_level = 3;
      strcpy (bfd_record.analyst_activity, "NAVOCEANO BHY");

      count++;


      if (binaryFeatureData_write_record_image_file (bfd_handle, BFDATA_NEXT_RECORD, &bfd_record, NULL, image_name) < 0)
        {
          binaryFeatureData_perror ();
          exit (-1);
        }
    }

  fclose (fp);

  binaryFeatureData_close_file (bfd_handle);


  return (0);
}
