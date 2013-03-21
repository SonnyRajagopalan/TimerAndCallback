#ifndef POSIX_MESSAGE_QUEUE_H
#define POSIX_MESSAGE_QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <mqueue.h>
#include "common/Types.h"

#define MAX_POSIX_MESSAGE_QUEUE_NAME_LENGTH 64
#define MAX_MQ_SIZE (10)
#define MAX_MQ_MSG_SIZE (8192)
#define POSIX_MQ_CREATOR (O_RDWR | O_EXCL | O_CREAT)
#define POSIX_MQ_USER    (O_RDWR)
#define POSIX_PERM_RW0RW0RW0 (0666)
#define POSIX_PERM_RWXRWXRWX (0777)

class PosixMessageQueue
{
 public:
  PosixMessageQueue (CHAR* _name, SINT32 _flags, SINT32 _nonBlockingOrZero, UINT32 _mode, UINT32 _maxMsg, UINT32 _msgSize);
  ~PosixMessageQueue ();

  void send (const CHAR* buf, const UINT32 msgSize);
  SINT32 receive ();
  CHAR* getMsg () { return (CHAR*) pBuffer; }
  SINT32 getMessageQDescriptor () { return (SINT32) msgFD; }
  UINT32 getNumberOfMessagesInQueue ();
  void   setNonBlockingMode ();
 private:
  CHAR   name [MAX_POSIX_MESSAGE_QUEUE_NAME_LENGTH];
  SINT32 flags;
  UINT32 mode;
  mqd_t  msgFD;
  mq_attr mQAttr;
  //UINT32 maxMsg; // The maximum number of messages you can put in the queue before it begins blocking  
  //UINT32 msgSize;
  char*  pBuffer;
};
#endif
