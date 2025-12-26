/*---------------------------------------------------------------------------/
/  Configurations of FatFs Module
/---------------------------------------------------------------------------*/

#ifndef FFCONF_DEF
#define FFCONF_DEF	80386

/* A header file that overwrites with local project settings. */
#if __has_include(<ffconf_local.h>)
#	include <ffconf_local.h>
#endif

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#ifndef FF_FS_READONLY
#	define FF_FS_READONLY	0
#endif

#ifndef FF_FS_MINIMIZE
#	define FF_FS_MINIMIZE	0
#endif

#ifndef FF_USE_FIND
#	define FF_USE_FIND		0
#endif

#ifndef FF_USE_MKFS
#	define FF_USE_MKFS		0
#endif

#ifndef FF_USE_FASTSEEK
#	define FF_USE_FASTSEEK	0
#endif

#ifndef FF_USE_EXPAND
#	define FF_USE_EXPAND		0
#endif

#ifndef FF_USE_CHMOD
#	define FF_USE_CHMOD		0
#endif

#ifndef FF_USE_LABEL
#	define FF_USE_LABEL		0
#endif

#ifndef FF_USE_FORWARD
#	define FF_USE_FORWARD	0
#endif

#ifndef FF_USE_STRFUNC
#	define FF_USE_STRFUNC	0
#endif
#ifndef FF_PRINT_LLI
#	define FF_PRINT_LLI		0
#endif
#ifndef FF_PRINT_FLOAT
#	define FF_PRINT_FLOAT	0
#endif
#ifndef FF_STRF_ENCODE
#	define FF_STRF_ENCODE	0
#endif


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#ifndef FF_CODE_PAGE
#	define FF_CODE_PAGE		932
#endif

#ifndef FF_USE_LFN
#	define FF_USE_LFN			0
#endif
#ifndef FF_MAX_LFN
#	define FF_MAX_LFN			255
#endif

#ifndef FF_LFN_UNICODE
#	define FF_LFN_UNICODE	0
#endif

#ifndef FF_LFN_BUF
#	define FF_LFN_BUF			255
#endif
#ifndef FF_SFN_BUF
#	define FF_SFN_BUF			12
#endif

#ifndef FF_FS_RPATH
#	define FF_FS_RPATH		0
#endif

#ifndef FF_PATH_DEPTH
#	define FF_PATH_DEPTH		10
#endif

/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#ifndef FF_VOLUMES
#	define FF_VOLUMES				1
#endif

#ifndef FF_STR_VOLUME_ID
#	define FF_STR_VOLUME_ID		0
#endif
#ifndef FF_VOLUME_STRS
#	define FF_VOLUME_STRS		"RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
#endif

#ifndef FF_MULTI_PARTITION
#	define FF_MULTI_PARTITION	0
#endif

#ifndef FF_MIN_SS
#	define FF_MIN_SS				512
#endif
#ifndef FF_MAX_SS
#	define FF_MAX_SS				512
#endif

#ifndef FF_LBA64
#	define FF_LBA64				0
#endif

#ifndef FF_MIN_GPT
#	define FF_MIN_GPT				0x100000000
#endif

#ifndef FF_USE_TRIM
#	define FF_USE_TRIM			0
#endif


/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#ifndef FF_FS_TINY
#	define FF_FS_TINY			0
#endif

#ifndef FF_FS_EXFAT
#	define FF_FS_EXFAT		0
#endif

#ifndef FF_FS_NORTC
#	define FF_FS_NORTC		1
#endif
#ifndef FF_NORTC_MON
#	define FF_NORTC_MON		1
#endif
#ifndef FF_NORTC_MDAY
#	define FF_NORTC_MDAY		1
#endif
#ifndef FF_NORTC_YEAR
#	define FF_NORTC_YEAR		2019
#endif

#ifndef FF_FS_CRTIME
#	define FF_FS_CRTIME		0
#endif

#ifndef FF_FS_NOFSINFO
#	define FF_FS_NOFSINFO	0
#endif

#ifndef FF_FS_LOCK
#	define FF_FS_LOCK			0
#endif

#ifndef FF_FS_REENTRANT
#	define FF_FS_REENTRANT	0
#endif
#ifndef FF_FS_TIMEOUT
#	define FF_FS_TIMEOUT	1000
#endif

#endif /* FFCONF_DEF */
