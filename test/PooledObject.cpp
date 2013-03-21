#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "test/PooledObject.h"
#include "common/Debug.h"

MemoryPool* PooledObject::pool = new MemoryPool ((char*) "pooledObject",
						 (char*) "/pooledObject",
						 O_RDWR | O_CREAT | O_EXCL, PROT_READ | PROT_WRITE, 0666, 
						 sizeof (PooledObject), 20);

PooledObject::PooledObject () :
  eenie (0),
  meenie (1),
  mynie (2),
  moe (3)
{
}

PooledObject::~PooledObject ()
{
  //std::cout << "Calling overloaded PooledObject delete (via destructor)..." << std::endl;
  //pool->removeFromPool (reinterpret_cast<void*>(this));
  // DTOR WILL AUTOMATICALLY CALL DELETE! 03/15/2012.
}

void* PooledObject::operator new (size_t size)
{
  //std::cout << "Calling overloaded PooledObject new..." << std::endl;
  return pool->getFromPool ();
}

void PooledObject::operator delete (void* object)
{
  // std::cout << "Calling overloaded PooledObject delete..." << std::hex << object << std::dec << std::endl;
  pool->removeFromPool (reinterpret_cast<void*>(object));
}
