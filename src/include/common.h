/*
 * Header for common definitions
 */
#ifndef COMMON_H
#define COMMON_H

#define TRUE	1
#define FALSE	0

#define BUFLEN_8	8
#define BUFLEN_16	16
#define BUFLEN_32	32
#define BUFLEN_64	64
#define BUFLEN_128	128
#define BUFLEN_256	256
#define BUFLEN_512	512

/*define some common types*/
typedef unsigned char	u8;
typedef char		s8;
typedef unsigned short	u16;
typedef short		s16;
typedef unsigned int	u32;
typedef int		s32;
typedef unsigned long	u64;
typedef long		s64;

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(*array))
/* @type: type of the struct object
 * @member: member of the struct object
 *
 * Get the offset of the member of a struct container
 */
#define offsetof(type, member)	\
	((size_t)(&((type *)0)->member))

/* @ptr: a pointer to the memnber
 * @type: type of the struct object
 * @member: member of the struct object
 *
 * Given a pointer to a member of the struct type, get a pointer to the
 * struct object.
 */
#define container_of(ptr, type, member)	\
	((type *)((void *)ptr - offsetof(type, member)))

#endif

