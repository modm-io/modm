/*---------------------------------------------------------------------------/
/  Petit FatFs - Configuration file
/---------------------------------------------------------------------------*/

#ifndef PFCONF_DEF
#define PFCONF_DEF 8088	/* Revision ID */

/* A header file that overwrites with local project settings. */
#if __has_include(<pffconf_local.h>)
#include <pffconf_local.h>
#endif

/*---------------------------------------------------------------------------/
/ Function Configurations (0:Disable, 1:Enable)
/---------------------------------------------------------------------------*/

#ifndef PF_USE_READ
#	define PF_USE_READ   1	/* pf_read() function */
#endif
#ifndef PF_USE_DIR
#	define PF_USE_DIR    0	/* pf_opendir() and pf_readdir() function */
#endif
#ifndef PF_USE_LSEEK
#	define PF_USE_LSEEK  0	/* pf_lseek() function */
#endif
#ifndef PF_USE_WRITE
#	define PF_USE_WRITE  0	/* pf_write() function */
#endif

#ifndef PF_FS_FAT12
#	define PF_FS_FAT12   0	/* FAT12 */
#endif
#ifndef PF_FS_FAT16
#	define PF_FS_FAT16   1	/* FAT16 */
#endif
#ifndef PF_FS_FAT32
#	define PF_FS_FAT32   0	/* FAT32 */
#endif


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#ifndef PF_USE_LCC
#	define PF_USE_LCC    0	/* Allow lower case ASCII and non-ASCII chars */
#endif

#ifndef PF_CODE_PAGE
#	define PF_CODE_PAGE  437
#endif
/* The PF_CODE_PAGE specifies the code page to be used on the target system.
/  SBCS code pages with PF_USE_LCC == 1 requiers a 128 byte of case conversion
/  table. It might occupy RAM on some platforms, e.g. avr-gcc.
/  When PF_USE_LCC == 0, PF_CODE_PAGE has no effect.
/
/   437 - U.S.
/   720 - Arabic
/   737 - Greek
/   771 - KBL
/   775 - Baltic
/   850 - Latin 1
/   852 - Latin 2
/   855 - Cyrillic
/   857 - Turkish
/   860 - Portuguese
/   861 - Icelandic
/   862 - Hebrew
/   863 - Canadian French
/   864 - Arabic
/   865 - Nordic
/   866 - Russian
/   869 - Greek 2
/   932 - Japanese (DBCS)
/   936 - Simplified Chinese (DBCS)
/   949 - Korean (DBCS)
/   950 - Traditional Chinese (DBCS)
*/


#endif /* PF_CONF */
