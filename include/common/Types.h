#ifndef TYPES_H
#define TYPES_H

#include <ostream>
#include <istream>
#include <map>
#include <list>
#include <string>
#include <uuid/uuid.h>

#define MILLISECONDS * 1
#define SECONDS      * 1000
#define MINUTES      * 1000 * 60
#define HOURS        * 1000 * 60 * 60
#define DAYS         * 1000 * 60 * 60 * 24

typedef std::string       BasicString;
typedef char              CHAR; /**< Just a plain old character */

typedef unsigned int      UINT32; /**< Unsigned integer, 32 bits */
typedef unsigned short    UINT16; /**< Unsigned short, 16 bits */
typedef unsigned char     UCHAR8; /**< Unsigned char, 8 bits */
typedef unsigned char     UINT8; /**< Unsigned char, 8 bits */

typedef int               SINT32;

/**
   Unsigned 64-bit integers
*/
typedef u_int64_t __UINT64; // Cannot have unsigned long long or -pedantic flag will complain in g++
typedef struct Unsigned64
{
  UINT32 lsb;
  UINT32 msb;
} __Unsigned64;

typedef union
{
  __UINT64     uint64;
  __Unsigned64 uint64s;
} UINT64;

/**
   Signed 64-bit integers
*/
typedef int64_t   __SINT64; // Cannot have long long or -pedantic flag will complain in g++
typedef struct Integer64
{
  UINT32 lsb;
  SINT32 msb;
} __Integer64;

typedef union
{
  __SINT64    int64;
  __Integer64 int64s;
} SINT64;

/**
   Floating point
*/
typedef float             FLT32;
typedef long double       __FLT64;
typedef struct Float64
{
  FLT32 lsb;
  FLT32 msb;
} __Float64;

typedef union
{
  __FLT64    flt64;
  __Float64  flt64s;
} FLT64;

/**
   Stream operations
*/
typedef std::ostream  OutputStream; /**< Output stream */
typedef std::istream  InputStream; /**< Input stream */


/**
   UUID related
*/

typedef uuid_t UUID;

typedef enum __Result
  {
    OK       = 0,
    WARNING  = 1,
    ERROR    = 2,
    SEVERE   = 3,
    FATAL    = 4,
    // Following introduced for TimedExpiryBuffer
    NOT_UNIQUE = 5,
    BUFFER_PROBLEMS = 6,
    // Insert new Result codes below
    INVALID = 999999
  } Result;

// The following are types on probation:
typedef UINT32 EventId;

#endif
