#include <string>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <string.h>
#include <uuid/uuid.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <assert.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <openssl/rand.h>
#include "common/Util.h"
#include "common/Debug.h"

/**
  Serializing objects of size unknown at compile-time
*/

void serializeString (const BasicString& value, OutputStream& os)
{
  char* buffer = new char[value.size()+1];
  
  strcpy(buffer, value.c_str());
  os.write (buffer, value.size());

  delete[] buffer;
}

void deserializeString (BasicString& value, InputStream& in, UINT32 datalen)
{
  char* buffer = new char[datalen+1];
  
  in.read (buffer, datalen);
  buffer[datalen] = '\0';

  value.assign(buffer);

  delete[] buffer;
}

void serializePadding (const UINT32& padLen, OutputStream& os)
{
  char* buffer = new char[padLen];

  cout << "Adding padding for " << padLen << " bytes" << endl;
  memset (buffer, 0, padLen);
  os.write (buffer, padLen);

  delete[] buffer;
}

void deserializePadding (const UINT32& padLen, InputStream& in)
{
  char* buffer = new char[padLen];

  in.read (buffer, padLen);
  
  delete[] buffer;
}

/**
   Serializing objects of 8 byte size
*/

void serializeUINT64 (const UINT64& value, OutputStream& os)
{
  serializeUINT32 (value.uint64s.msb, os);
  serializeUINT32 (value.uint64s.lsb, os);
}

void deserializeUINT64 (UINT64& value, InputStream& in)
{
  deserializeUINT32 (value.uint64s.msb, in);
  deserializeUINT32 (value.uint64s.lsb, in);
}

void serializeSINT64 (const SINT64& value, OutputStream& os)
{
  serializeSINT32 (value.int64s.msb, os);
  serializeUINT32 (value.int64s.lsb, os);
}

void deserializeSINT64 (SINT64& value, InputStream& in)
{
  deserializeSINT32 (value.int64s.msb, in);
  deserializeUINT32 (value.int64s.lsb, in);
}

void serializeFLT64 (const FLT64& value, OutputStream& os)
{
  serializeFLT32 (value.flt64s.msb, os);
  serializeFLT32 (value.flt64s.lsb, os);
}

void deserializeFLT64 (FLT64& value, InputStream& in)
{
  deserializeFLT32 (value.flt64s.msb, in);
  deserializeFLT32 (value.flt64s.lsb, in);
}

void serializeUINT64ToCharBuf (const UINT64& value, CHAR* buf)
{
  serializeUINT32ToCharBuf (value.uint64s.msb, buf);
  serializeUINT32ToCharBuf (value.uint64s.lsb, (buf + 4));
  //printf ("serializeUINT64ToCharBuf wrote %2x %2x %2x %2x %2x %2x %2x %2x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
}

void deserializeUINT64FromCharBuf (UINT64& value, const CHAR* buf)
{
  deserializeUINT32FromCharBuf (value.uint64s.msb, buf);
  deserializeUINT32FromCharBuf (value.uint64s.lsb, (buf + 4));
  //printf ("deserializeUINT64FromCharBuf wrote %2x %2x %2x %2x %2x %2x %2x %2x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
  //printf ("deserializeUINT64FromCharBuf = %u %u\n", value.uint64s.msb, value.uint64s.lsb);
}

/**
   Serializing objects of 4-byte size
*/

void serializeUINT32 (const UINT32& value, OutputStream& os)
{
  char* buffer = new char[4];
  UINT32 valueN = htonl (value);
  
  memcpy (buffer, &valueN, 4);
  os.write (buffer, 4);

  delete[] buffer;  
}

void serializeUINT32ToCharBuf (const UINT32& value, CHAR* buf)
{
  UINT32 valueN = htonl (value);
  memcpy (buf, &valueN, 4);
}

void deserializeUINT32FromCharBuf (UINT32& value, const CHAR* buf)
{
  UINT32 valueH;
  memcpy (&valueH, buf, 4);

  value = ntohl (valueH);
}

void deserializeUINT32 (UINT32& value, InputStream& in)
{
  char* buffer = new char [4];
  UINT32 valueH;

  in.read (buffer, 4);
  memcpy(&valueH, buffer, 4);

  value = ntohl (valueH);

  delete[] buffer;
}

void serializeSINT32 (const SINT32& value, OutputStream& os)
{
  char* buffer = new char[4];
  UINT32 valueN = htonl (*reinterpret_cast<const uint32_t*>(&value)); // 
  
  memcpy (buffer, &valueN, 4);
  os.write (buffer, 4);

  delete[] buffer;  
}

void deserializeSINT32 (SINT32& value, InputStream& in)
{
  char* buffer = new char [4];
  SINT32 valueH;

  in.read (buffer, 4);
  memcpy(&valueH, buffer, 4);

  value = ntohl (valueH);

  delete[] buffer;
}

void serializeFLT32 (const FLT32& value, OutputStream& os)
{
  char* buffer = new char[4];
  UINT32 valueN = htonl (*reinterpret_cast<const uint32_t*>(&value));
  
  memcpy (buffer, &valueN, 4);
  os.write (buffer, 4);

  delete[] buffer;  
}

void deserializeFLT32 (FLT32& value, InputStream& in)
{
  char* buffer = new char [4];
  UINT32 valueH;

  in.read (buffer, 4);
  memcpy(&valueH, buffer, 4);

  value = ntohl (valueH);

  delete[] buffer;
}

/**
   Serializing objects of 3-byte size
*/

void serialize3ByteUnsignedChar (const UINT8 threeByteInteger[], OutputStream& os)
{
  char* buffer = new char[3];

//   cout << hex << static_cast<int>(threeByteInteger[0]) << " ";
//   cout << hex << static_cast<int>(threeByteInteger[1]) << " ";
//   cout << hex << static_cast<int>(threeByteInteger[2]);
//   cout << dec << endl;
  memcpy (buffer, &threeByteInteger[2], 1);
  memcpy (buffer+1, &threeByteInteger[1], 1);
  memcpy (buffer+2, &threeByteInteger[0], 1);

  os.write (buffer, 3);

  delete[] buffer;
}

void deserializeTo3ByteUnsignedChar (UINT8 threeByteInteger[], InputStream& in)
{
  char* buffer = new char[3];

  in.read (buffer, 3);

  threeByteInteger[2] = buffer[0];
  threeByteInteger[1] = buffer[1];
  threeByteInteger[0] = buffer[2];

//   cout << hex << static_cast<int>(threeByteInteger[0]) << " ";
//   cout << hex << static_cast<int>(threeByteInteger[1]) << " ";
//   cout << hex << static_cast<int>(threeByteInteger[2]);
//   cout << dec << endl;
  delete[] buffer;
}

/**
   Serializing and deserializing objects of byte size
*/

void serializeUINT8 (const UINT8& value, OutputStream& os)
{
  char* buffer = new char[1];

  memcpy(buffer, &value, 1);
  os.write (buffer, 1);

  delete[] buffer;
}

void deserializeUINT8 (UINT8& value, InputStream& in)
{
  char* buffer = new char[1];

  in.read (buffer, 1);
  value = buffer[0];

  delete[] buffer;
}
/**
  Splitting and unsplitting of UINT8 arrays (typically 3-byte) to
  UINT32
*/


void splitUINT32To3ByteUINT8Array (const UINT32& value, UINT8 bytes[])
{
  bytes[0] = (value & 0x00FF0000) >> 16;
  bytes[1] = (value & 0x0000FF00) >> 8;
  bytes[2] = (value & 0x000000FF);
}

void unsplit3ByteUINT8ArrayToUINT32 (UINT32& value, const UINT8 bytes[])
{
  value = (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];
}

UINT32 getUINT32From3ByteUINT8Array (const UINT8 bytes[])
{
  return (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];
}

BasicString getNewUUIDString ()
{
  UUID newUuid;
  char uuidString[37];
  
  uuid_generate_random (newUuid);
  uuid_unparse (newUuid, uuidString);
  
  return uuidString;

  // See http://stackoverflow.com/questions/5328544/returning-char-array-as-stdstring
  // We are quite sure that buffer is a null terminated string, so we can
  // do the above, instead of having to do the below.
  //BasicString uuidStr;
  //uuidStr.assign (buffer, 37);
  //return uuidStr;
  
}

UINT32 get32BitRandomNumber ()
{
  /*
    The following code will NOT work without OpenSSL 0.9.8.

    Also, it did not appear like any code is using this function, so I commented this out...    
  */
  // UINT32 randomNumberFromSSLLibrary;
  
  // if( !(RAND_pseudo_bytes ((unsigned char *)randomNumberFromSSLLibrary, sizeof (randomNumberFromSSLLibrary))))
  //   {
  //     COUT << "ERROR: call to RAND_pseudo_bytes() failed\n" << endl;
  //     exit (1);
  //   }
}

bool uint64LT (UINT64& _value1, UINT64& _value2)
{
  if (_value1.uint64 < _value2.uint64)
    return true;
  else
    return false;
}

bool uint64LEQ (UINT64& _value1, UINT64& _value2)
{
  if (_value1.uint64 <= _value2.uint64)
    return true;
  else
    return false;
}

bool uint64EQ (UINT64& _value1, UINT64& _value2)
{
  if (_value1.uint64 == _value2.uint64)
    return true;
  else
    return false;
}

bool uint64GEQ (UINT64& _value1, UINT64& _value2)
{
  if (_value1.uint64 >= _value2.uint64)
    return true;
  else
    return false;
}

BasicString getMyHostname ()
{
  CHAR hostname[MAX_HOSTNAME_LEN];
  BasicString hostnameStr ("Invalid.Hostname");

  if (gethostname (hostname, sizeof (hostname)) == 0)
    {
      hostnameStr.assign (hostname);
    }
  else
    {
    }
  return hostnameStr;
}

BasicString getMyIPAddress ()
{
  struct ifaddrs* pInterfaceAddresses = NULL;
  struct ifaddrs* pThisInterface = NULL;
  void* pTempAddress = NULL;
  BasicString myIp ("Invalid.IP");

  getifaddrs (& pInterfaceAddresses);
  
  for (pThisInterface = pInterfaceAddresses; pThisInterface != NULL; pThisInterface = pThisInterface->ifa_next)
    {
      if (pThisInterface ->ifa_addr->sa_family==AF_INET) 
	{
	  char addrString [INET_ADDRSTRLEN];
	  pTempAddress = &((struct sockaddr_in *)pThisInterface->ifa_addr)->sin_addr;
	  inet_ntop (AF_INET, pTempAddress, addrString, INET_ADDRSTRLEN);
	  myIp.assign (addrString);

	  if (strcmp (pThisInterface->ifa_name, "eth0") == 0)
	    return myIp;
	}
      else if (pThisInterface->ifa_addr->sa_family==AF_INET6)
	{
	  char addrString [INET6_ADDRSTRLEN];
	  pTempAddress = &((struct sockaddr_in6 *)pThisInterface->ifa_addr)->sin6_addr;
	  inet_ntop(AF_INET6, pTempAddress, addrString, INET6_ADDRSTRLEN);	  
	}
    }
  return myIp;
}

void dumpBinaryToFile (const CHAR* filepath, std::ostringstream& os)
{
  std::stringbuf* sbuf = os.rdbuf();
  std::ofstream outfile (filepath, std::ios::binary);

  outfile << sbuf->str();
  outfile.close ();
}

UINT32 getIPAddressFromName (const CHAR* name)
{
  struct hostent* host = gethostbyname (name);

  if (host == NULL)
    {
      // LOG this.
      assert (1);
    }

  return *((UINT32*) host->h_addr_list [0]);
}

const CHAR* getNameFromIPAddress (const CHAR* ipAddressBuf)
{
  struct hostent* host = gethostbyaddr (ipAddressBuf, 4, AF_INET);

  return host->h_name;
}
