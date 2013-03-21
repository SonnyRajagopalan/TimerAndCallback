#include <string.h>
#include <assert.h>
#include <errno.h>
#include "platform/PosixMessageQueue.h"
#include "common/Debug.h"

PosixMessageQueue::PosixMessageQueue (CHAR* _name, SINT32 _flags, SINT32 _nonBlockingOrZero, UINT32 _mode, UINT32 _maxMsg, UINT32 _msgSize):
  flags (_flags),
  mode (_mode)
{
  strcpy (name, _name);
  mQAttr.mq_flags = _nonBlockingOrZero; // only two modes possible
  mQAttr.mq_maxmsg = _maxMsg;
  mQAttr.mq_msgsize = _msgSize;
  mQAttr.mq_curmsgs = 0; // The total number of messages in the queue

  pBuffer = (new char [mQAttr.mq_msgsize]);

  // In the following, if flags == O_RDWR (that is, not the exclusive creator), then mQAttr is zero-filled (not null, though
  // that would work as well).
  if ((msgFD = mq_open (name, (int) (flags), (mode_t) (mode), &mQAttr)) != -1)
    {
      cout << "[" << getpid () << "] POSIX message queue ("
	   << name << ") created with fd = " << msgFD << " [" 
	   << ((_flags & O_EXCL)?"Creator]":"User]") << endl;
    }
  else
    {
      cout << "[" << getpid () << "] POSIX message queue (" 
	   << name << ") could not be created [" 
	   << ((_flags & O_EXCL)?"Creator]":"User]") << endl;

      switch (errno)
	{
	case EACCES:
	  cout << "[" << getpid () 
	       << "] |->+(EACCES): The queue exists, but the caller does not have the permissions to open the queue in the specified mode." << endl;
	  cout << "[" << getpid () 
	       << "] |->+(EACCES): Name contains more than onen slash." << endl;
	  break;
	case EEXIST:
	  cout << "[" << getpid () 
	       << "] |->+(EEXIST): Both O_CREAT and O_EXCL specified, but a queue with this name already exists." << endl;
	  break;
	case EINVAL:
	  cout << "[" << getpid () 
	       << "] |->+(EINVAL): O_CREAT was specified, but attr was not NULL, but attribute values, maxmsg, msgsize was not valid." << endl;
	  break;
	case EMFILE: 
	  cout << "[" << getpid () 
	       << "] |->+(EMFILE): The process already has the maximum number of message queues open." << endl;
	  break;
	case ENAMETOOLONG:
	  cout << "[" << getpid () 
	       << "] |->+(ENAMETOOLONG): The specified name was too long." << endl;
	  break;
	case ENFILE:
	  cout << "[" << getpid () 
	       << "] |->+(ENFILE): The system limit on the total number of open files and queues has been reached." << endl;
	  break;
	case ENOENT:
	  cout << "[" << getpid () 
	       << "] |->+(ENOENT): O_CREAT was specified, but no MQ of this name exists." << endl;
	  cout << "[" << getpid () 
	       << "] |->+(ENOENT): The name as just a slash with no other characters." << endl;
	  break;
	case ENOMEM:
	  cout << "[" << getpid () 
	       << "] |->+(ENOMEM): Insufficient memory." << endl;
	  break;
	case ENOSPC:
	  cout << "[" << getpid () 
	       << "] |->+(ENOSPC): No space to create the message queue." << endl;
	  break;
	default:
	  cout << "[" << getpid () 
	       << "] Invalid errno" << endl;
	}
      //assert (0);
    }
}

PosixMessageQueue::~PosixMessageQueue ()
{
  delete [] pBuffer;

  if (mq_close (msgFD) == -1)
    {
      cout << "POSIX message queue (" << name << ") could not be closed properly." << endl;
      assert (0);
    }

  if (mq_unlink (name) == -1)
    {
      cout << "POSIX message queue (" << name << ") could not be unlinked properly." << endl;
      assert (0);
    }
}

void PosixMessageQueue::send (const CHAR* buf, const UINT32 msgSize)
{
  SINT32 retVal = 0;

  if ((retVal = mq_send (msgFD, buf, msgSize, 0)) == -1)
    {      
      switch (errno)
	{
	case EAGAIN:
	  {
	    cout << "EAGAIN" << endl;
	  }
	  break;
	case EBADF:
	  {
	    cout << "EBADF" << endl;
	  }
	  break;
	case EINTR:
	  {
	    cout << "EINTR" << endl;
	  }
	  break;
	case EINVAL:
	  {
	    cout << "EINVAL" << endl;
	  }
	  break;
	case EMSGSIZE:
	  {
	    cout << "EMSGSIZE" << endl;
	  }
	  break;
	case ETIMEDOUT:
	  {
	    cout << "ETIMEDOUT" << endl;
	  }
	  break;
	default:
	  break;
	}

      assert (0);
    }
  //cout << "Sent non-null terminated serialized data with retVal = " << retVal << endl;
  //   printf ("buf = %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n",
  // 	  buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
  // 	  buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
}

SINT32 PosixMessageQueue::receive ()
{
  SINT32  received = 0;

  memset (pBuffer, 0, mQAttr.mq_msgsize);
  if ((received = mq_receive (msgFD, pBuffer, mQAttr.mq_msgsize, NULL)) == -1)
    {
      switch (errno)
	{
	case EAGAIN:
	  {
	    cout << "EAGAIN" << endl;
	  }
	  break;
	case EBADF:
	  {
	    cout << "EBADF" << endl;
	  }
	  break;
	case EINTR:
	  {
	    cout << "EINTR" << endl;
	  }
	  break;
	case EINVAL:
	  {
	    cout << "EINVAL" << endl;
	  }
	  break;
	case EMSGSIZE:
	  {
	    cout << "EMSGSIZE" << endl;
	  }
	  break;
	case ETIMEDOUT:
	  {
	    cout << "ETIMEDOUT" << endl;
	  }
	  break;
	default:
	  break;
	}
      cout << "Assert...." << endl;
      assert (0);
    }

  //cout << "Received " << received << " non-null terminated, serialized bytes" << endl;
  return received;
}

UINT32 PosixMessageQueue::getNumberOfMessagesInQueue ()
{
  return (UINT32) mQAttr.mq_curmsgs;
}

void PosixMessageQueue::setNonBlockingMode ()
{
  mq_attr newAttr = mQAttr;

  newAttr.mq_flags |= O_NONBLOCK;
  if (mq_setattr (msgFD, &newAttr, &mQAttr) == -1)
    {
      if (errno == EBADF)
	{
	  // Error
	}
      else if (errno == EINVAL)
	{
	  // Error
	}
    }
}
