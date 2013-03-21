#include <iostream>
#include "test/PooledObject.h"
#include "common/Types.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int main()
{
  //PooledObject* obj = new PooledObject();
  // Allocate a bunch of objects first
  // Delete and allocate next

  PooledObject* pobj[15];
  for (int j=0; j<10; j++) pobj[j] = new PooledObject();
  //for (int j=0; j<10; j++) delete pobj[j];
  // Delete operations
  //sleep (10); 
  delete pobj[2];
  delete pobj[5];
  pobj[2] = new PooledObject ();
  pobj[11] = new PooledObject ();
  pobj[10] = new PooledObject ();
  delete pobj [8];
  PooledObject* someOtherObject = new PooledObject ();
  PooledObject* anotherObject = new PooledObject ();
  //sleep (10); 
  // Exit operations
  shm_unlink ("/timerpool");
}
