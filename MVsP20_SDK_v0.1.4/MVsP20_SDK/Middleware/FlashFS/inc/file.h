
/**
* @addtogroup Flash-FS
* @{
* @defgroup file file.h
* @{
*/

#ifndef    _FILE_H_
#define    _FILE_H_
#include <stdio.h>
//#include "file.h"


struct __FFILE {
	unsigned char *_bufptr;          /* buffer pointer              */
  long  _bufcnt;          /* buffer count ,for read       */
  unsigned char *_bufbase;         /* buffer base pointer         */
  long  _buflen;          /* buffer length               */
  char  _ioflag1;         /* I/O control flag 1          */
  char  _ioflag2;         /* I/O control flag 2          */
  char  _iofd;
};

typedef struct __FFILE   FFILE;



#ifndef _NFILE
#define _NFILE 8//7//
/*#define _NFILE _nfiles*/
#endif
//extern struct __FFILE {
//       unsigned char *_bufptr;          /* buffer pointer              */
//                long  _bufcnt;          /* buffer count ,for read       */
//       unsigned char *_bufbase;         /* buffer base pointer         */
//                long  _buflen;          /* buffer length               */
//                char  _ioflag1;         /* I/O control flag 1          */
//                char  _ioflag2;         /* I/O control flag 2          */
//                char  _iofd;
//}
extern FFILE _iob[_NFILE];

//typedef struct _iobuf FFILE;

#define _IOREAD    1                    /* only read or update read    */
#define _IOWRITE   2                    /* only write or update write  */
#define _IORW      4                    /* update file                 */
#define _IOUNBUF   8                    /* unbffering I/O              */
#define _IOBGBUF  16                    /* automatic buffer allocate   */
#define _IOEOF    32                    /* end of file                 */
#define _IOERR    64                    /* I/O error                   */
#define _UNGTC     1                    /* ungetc issued (_ioflag2)    */
#define _IOLINBUF  2                    /* line buffering              */
#define _IOBIN     0x04                 /* binary stream */

#define	O_RDONLY	0x01
#define	O_WRONLY	0x02
#define	O_RDWR		0x04
#define	O_CREAT		0x08
#define	O_TRUNC		0x10
#define	O_APPEND	0x20

	#define	FILE_CREATE	1
	#define	FILE_OPEN	2
	#define	FILE_CLOSE	3
	#define	FILE_READ	4
	#define	FILE_WRITE	5
	#define	FILE_REMOVE	6
	#define	FILE_RENAME	7
	#define	FILE_LSEEK	8
	#define	FILE_GETATTR	9
	#define	FILE_SETATTR	10
	#define	FILE_NAMESET	11
	#define	FILE_PAGESET	12
	#define	GET_PROG_VER	13
	#define	SCI_READ	20
	#define	SCI_WRITE	21
	#define	__A_RDONLY	0x01
	#define	__A_HIDDEN	0x02
	#define	__A_SYSTEM	0x04
	#define	__A_VOLID	0x08
	#define	__A_SUBDIR	0x10
	#define	__A_ARCH	0x20
	#define	CODE_READ_ONLY	0x0
	#define	CODE_WRITE_ONLY	0x1
	#define	CODE_READ_WRITE	0x2
	
	#define SEEK_SET 0 /* start of stream (see fseek) */
	#define SEEK_CUR 1 /* current position in stream (see fseek) */
	#define SEEK_END 2 /* end of stream (see fseek) */
  #define BUFSIZ       (512)
  
#ifdef _cplusplus
extern "C"{
#endif	
 FFILE* Fopen(const char* name, const char* mode);

 int Open(const char* name, int mode);

 int Write( int handle, const void* buf, size_t len);

 int Close( int handle );

 int Fclose(FFILE* fp);

 int Read(int handle, char* buffer, size_t size);

 int Fread(void* buffer ,size_t size, size_t block, FFILE* fp);

 int Fwrite(void* buffer ,size_t size, size_t block, FFILE* fp);

 int Rename( char* oldname, char* newname );

 int Remove( const char* filename );

 int Fseek(FFILE *stream, long offset, int fromwhere);

 int Fflush( FFILE *stream );

 int FlashFSInit(void);

#define Feof(fp)    ( (fp)->_ioflag1 & _IOEOF )
#define Ferror(fp)  ( (fp)->_ioflag1 & _IOERR )
#define Clearerr(fp) ((void)((fp)->_ioflag1&=~(_IOEOF|_IOERR)))

#ifdef _cpluscplus
}
#endif


#endif
