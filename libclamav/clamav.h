/*
 *  Copyright (C) 2002 - 2004 Tomasz Kojm <tkojm@clamav.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __CLAMAV_H
#define __CLAMAV_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
 
#ifdef __cplusplus
extern "C"
{
#endif
 


#define CL_COUNT_PRECISION 4096


/* return codes */
#define CL_CLEAN	0   /* virus not found */
#define CL_VIRUS	1   /* virus found */

#define CL_EMAXREC	10  /* recursion level limit exceeded */
#define CL_EMAXSIZE	11  /* size limit exceeded */
#define CL_EMAXFILES	12  /* files limit exceeded */
#define CL_ERAR		100 /* rar handler error */
#define CL_EZIP		101 /* zip handler error */
#define	CL_EMALFZIP	102 /* malformed zip */
#define CL_EGZIP	103 /* gzip handler error */
#define CL_EBZIP	104 /* bzip2 handler error */
#define CL_EOLE2	105 /* OLE2 handler error */
#define CL_EMSCOMP	106 /* compress.exe handler error */
#define CL_EMSCAB	107 /* MS CAB module error */
#define CL_EACCES	200 /* access denied */
#define CL_ENULLARG	300 /* null argument error */

#define CL_ETMPFILE	-1  /* tmpfile() failed */
#define CL_EFSYNC	-2  /* fsync() failed */
#define CL_EMEM		-3  /* memory allocation error */
#define CL_EOPEN	-4  /* file open error */
#define CL_EMALFDB	-5  /* malformed database */
#define CL_EPATSHORT	-6  /* pattern too short */
#define CL_ETMPDIR	-7  /* mkdir() failed */
#define CL_ECVD		-8  /* not a CVD file (or broken) */
#define CL_ECVDEXTR	-9  /* CVD extraction failure */
#define CL_EMD5		-10 /* MD5 verification error */
#define CL_EDSIG	-11 /* digital signature verification error */
#define CL_EIO		-12 /* general I/O error */

/* options */
#define CL_RAW		0
#define CL_ARCHIVE	1
#define CL_MAIL		2
#define CL_DISABLERAR	4
#define CL_OLE2		8
#define CL_ENCRYPTED    16
#define CL_HTML		32
#define CL_PE		64


struct cli_bm_patt {
    const char *pattern;
    char *virname;
    int length; 
    struct cli_bm_patt *next;
};

struct cli_ac_patt {
    short int *pattern;
    unsigned int length, mindist, maxdist;
    char *virname;
    unsigned short int sigid, parts, partno, type, alt, *altn;
    char **altc;
    struct cli_ac_patt *next;
};

struct cli_ac_node {
    char islast;
    struct cli_ac_patt *list;
    struct cli_ac_node *trans[256], *fail;
};

struct cli_md5_node {
    char *virname, *viralias, *md5;
    struct cli_md5_node *next;
};

struct cl_node {
    unsigned int maxpatlen; /* maximal length of pattern in db */

    /* Extended Boyer-Moore */
    int *bm_shift;
    struct cli_bm_patt **bm_suffix;

    /* Extended Aho-Corasick */
    struct cli_ac_node *ac_root, **ac_nodetable;
    unsigned int ac_partsigs, ac_nodes;

    /* MD5 */
    struct cli_md5_node **md5_hlist;
};

struct cl_limits {
    int maxreclevel; /* maximal recursion level */
    int maxfiles; /* maximal number of files to be
		   * scanned within an archive
		   */
    int maxratio; /* maximal compression ratio */
    short archivememlim; /* limit memory usage for bzip2 (0/1) */
    long int maxfilesize; /* files in an archive larger than
			   * this value will not be scanned
			   */
};

struct cl_stat {
    char *dir;
    int no;
    struct stat *stattab;
};

struct cl_cvd {
    char *time;	    /* 2 */
    int version;    /* 3 */
    int sigs;	    /* 4 */
    short int fl;   /* 5 */
    char *md5;	    /* 6 */
    char *dsig;	    /* 7 */
    char *builder;  /* 8 */
};

/* file scanning */
extern int cl_scanbuff(const char *buffer, unsigned int length, const char **virname, const struct cl_node *root);

extern int cl_scandesc(int desc, const char **virname, unsigned long int *scanned, const struct cl_node *root, const struct cl_limits *limits, int options);

extern int cl_scanfile(const char *filename, const char **virname, unsigned long int *scanned, const struct cl_node *root, const struct cl_limits *limits, int options);

/* database */
extern int cl_loaddb(const char *filename, struct cl_node **root, int *virnum);
extern int cl_loaddbdir(const char *dirname, struct cl_node **root, int *virnum);
extern const char *cl_retdbdir(void);
extern int cl_retflevel(void);

/* CVD */
extern struct cl_cvd *cl_cvdhead(const char *file);
extern struct cl_cvd *cl_cvdparse(const char *head);
extern int cl_cvdverify(const char *file);
extern void cl_cvdfree(struct cl_cvd *cvd);

/* data dir stat functions */
extern int cl_statinidir(const char *dirname, struct cl_stat *dbstat);
extern int cl_statchkdir(const struct cl_stat *dbstat);
extern int cl_statfree(struct cl_stat *dbstat);

/* enable debug information */
extern void cl_debug(void);

extern void cl_settempdir(const char *dir, short leavetemps);

extern int cl_build(struct cl_node *root);
extern void cl_free(struct cl_node *root);

extern const char *cl_strerror(int clerror);
extern const char *cl_perror(int clerror); /* deprecated */

#ifdef __cplusplus
};
#endif
 
#endif
