#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef unsigned long int blkcnt_t;	/* Used for file block counts */
typedef unsigned long int blksize_t;	/* Used for block sizes */

typedef uint64_t clock_t;	/* Used for system times in clock ticks or
							   CLOCKS_PER_SEC (see <time.h>). */

/* TODO:
     clockid_t
             Used for clock ID type in the clock and timer functions.
     fsblkcnt_t
             Used for file system block counts
     fsfilcnt_t
             Used for file system file counts
     pthread_attr_t
             Used to identify a thread attribute object.
     pthread_cond_t
             Used for condition variables.
     pthread_condattr_t
             Used to identify a condition attribute object.
     pthread_key_t
             Used for thread-specific data keys.
     pthread_mutex_t
             Used for mutexes.
     pthread_mutexattr_t
             Used to identify a mutex attribute object.
     pthread_once_t
             Used for dynamic package initialisation.
     pthread_rwlock_t
             Used for read-write locks.
     pthread_rwlockattr_t
             Used for read-write lock attributes.
     pthread_t
             Used to identify a thread.
     timer_t
             Used for timer ID returned by timer_create().
*/

#if defined(__alpha__) || defined(__ia64__) || defined(__sparc64__) || defined(__s390x__)
    typedef uint32_t dev_t;		/* Used for device IDs. */
    typedef uint32_t gid_t;		/* Used for group IDs. */
    typedef uint32_t mode_t;		/* Used for some file attributes. */
    typedef uint32_t nlink_t;		/* Used for link counts. */
    typedef uint32_t uid_t;		/* Used for user IDs. */
#elif defined(__arm__) || defined(__i386__) || defined(__sparc__) || defined(__s390__) /* make sure __s390x__ hits before __s390__ */
    typedef uint16_t dev_t;
    typedef uint16_t gid_t;
    typedef uint16_t mode_t;
    typedef uint16_t nlink_t;
    typedef uint16_t uid_t;
#endif

typedef int32_t id_t;			/* Used as a general identifier; can be used to
								   contain at least a pid_t, uid_t or a gid_t. */
typedef unsigned long ino_t;	/* Used for file serial numbers. */
typedef int32_t key_t;			/* Used for interprocess communication. */
typedef int32_t pid_t;			/* Used for process IDs and process group IDs. */
typedef signed long ssize_t;	/* Used for a count of bytes or an error indication. */
typedef signed long suseconds_t;/* Used for time in microseconds. */
typedef signed long time_t;		/* Used for time in seconds. */
typedef signed long useconds_t;	/* Used for time in microseconds. */

typedef uint32_t 	uid32_t;
typedef uint32_t 	gid32_t;

typedef int32_t 	clockid_t;
typedef int32_t 	timer_t;

#define __socklen_t_defined
typedef uint32_t 	socklen_t;
typedef uint16_t 	sa_family_t;


__END_DECLS

#endif
