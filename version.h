
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


/*********************************************************************************************

    This program is public domain software that was developed by 
    the U.S. Naval Oceanographic Office.

    This is a work of the US Government. In accordance with 17 USC 105,
    copyright protection is not available for any work of the US Government.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

*********************************************************************************************/

#ifndef VERSION

#define     VERSION     "PFM Software - build_feature V4.05 - 07/29/14"

#endif

/*

    Version 1.0
    Jan C. Depner
    06/07/01


    Version 2.0
    Jan C. Depner
    03/09/02

    Modified the output file location.  Now we don't check for separate areas
    we just put the target file in the $U_SPROOT survey plan directory.  Also
    changed names of input .trg files to .trm files so that they won't be
    loaded multiple times.


    Version 2.1
    Steve Farr
    09/12/02

    Modified program (and library) to handle new and old OIC file formats.


    Version 3.0
    Jan C. Depner
    02/20/04

    Modified program to output in the new XML target file format (from Wade 
    Ladner).  Changed the output location and command line arguments.  It 
    doesn't use the ISS60 U_SPROOT variable and plan directory.  On the 
    command line you now specify the target file name instead of the plan 
    name.  The OIC file names get changed to unique names now (if they haven't
    already been changed - looks for _BHY_ string in unqualified file name) 
    but the extension isn't changed.  The program now checks the contact ID 
    (unqualified filename) against the already existing file so it won't 
    duplicate the target even if you try to load the same one twice.


    Version 3.1
    Wade Ladner/Jan C. Depner
    03/04/04

    Supports OIC versions 3.0, 4.0, and 5.0.  Also, works in Windoze.


    Version 3.2
    Jan C. Depner
    10/16/04

    Against my better judgement I have added the ability to read ASCII
    comma separated value targets.  The format is VERY restrictive and
    really only supports one type of NAVO data.  Look at the Usage
    message to see what it looks like.


    Version 3.3
    Jan C. Depner
    11/14/05

    Also against my better judgement I have added the ability to read more ASCII
    comma separated value targets.  The format is VERY restrictive and
    really only supports one type of NAVO data.  Look at the Usage
    message to see what it looks like.


    Version 3.31
    Jan C. Depner
    02/09/06

    No longer store the contact ID as the description.


    Version 3.4
    Jan C. Depner
    04/07/06

    Reads yet another stupid ASCII format - no more, that's it!


    Version 3.5
    Jan C. Depner
    06/30/06

    Added depth fields to .csv and .txt input formats.


    Version 3.51
    Jan C. Depner
    10/22/07

    Added fflush calls after prints to stderr since flush is not automatic in Windows.


    Version 3.52
    Jan C. Depner
    04/07/08

    Replaced single .h files from utility library with include of nvutility.h


    Version 3.60
    Jan C. Depner
    04/02/09

    Changed name to build_feature from build_target.  Replaced support for NAVO standard XML target file with
    Binary Feature Data (BFD) file support.


    Version 3.61
    Jan C. Depner
    06/24/09

    Changed the BFD names to avoid collision with GNU Binary File Descriptor library.


    Version 4.00
    Jan C. Depner
    08/17/10

    Removed support for OIC target file input.  We don't use that anymore and it was kind of pointless.


    Version 4.01
    Jan C. Depner
    06/07/13

    - Changed build_target to build_feature in instructions.
    - Raised confidence level to 3.
    - Moved from NAVO programs to ABE programs at the request of Optech.


    Version 4.02
    Jan C. Depner (PFM Software)
    02/26/14

    Cleaned up "Set but not used" variables that show up using the 4.8.2 version of gcc.


    Version 4.03
    Jan C. Depner (PFM Software)
    05/07/14

    Check return values from getcwd and chdir to get rid of GCC warnings.


    Version 4.04
    Jan C. Depner (PFM Software)
    07/23/14

    - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
      inttypes.h sized data types (e.g. int64_t and uint32_t).


    Version 4.05
    Jan C. Depner (PFM Software)
    07/29/14

    - Fixed errors discovered by cppcheck.

*/
