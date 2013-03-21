#ifndef TIMED_EXPIRY_BUFFER
#define TIMED_EXPIRY_BUFFER

/**

   A timed expiry buffer  is a buffer that is required to be  maintained for a fixed time,
   after which the buffer  is to be purged. There may be  other constraints such as making
   sure that the buffer contains only unique entries.

   The class is not designed to be thread/process/fork safe. This is because this class is
   only for buffers that will maintain per-process-/fork-/thread- unique buffers.

*/

#include <vector>
#include "platform/Timer.h"
#include "platform/TEBCb.h"

template <class T>
class TimedExpiryBuffer
{
 public:

  enum TEBType
  {
    DuplicateNotOk,
    DuplicatesOk
  };

  TimedExpiryBuffer (Timer* _pTimer, TEBType _type);
  ~TimedExpiryBuffer ();

  virtual Result insertIntoBuffer (T) = 0;
  virtual Result insertUniqueIntoBuffer (T) = 0;
  virtual bool   isInBuffer (T) = 0;
  
 private:

  /**

     (1) Set timer time period to be time period of buffer maintenance per requirement.

     (2) When the timer fires (actions in the TEBCb::triggered ()):


     if (type == TEBType::DuplicateNotOk)
     {
        tickTime = ! tickTime;

        if (tickTime)
	{
          purge the tickBuffer;
	}
        else
	{
          purge the tockBuffer;
	}
     }
     else
     {
        // Do nothing: buffer insertion not dependent on tick-tock
     }

     (3) For buffer insertion:


     if (type == TEBType::DuplicateNotOk)
     {
        if (tickTime) 
          insertIntoTickBuffer (T);
        else
          insertIntoTockBuffer (T);
     }
     else
     {
        insertIntoTickBuffer (T); // Note: neither buffer is purged u
     }
  */

  TEBType type;
  Timer* pTimer; // Periodic timer
  TEBCb* pTEBCb;
  bool tickTime; // If false, it is tockTime, else it is tickTime. Initialized to false.
  std::vector <T>      tickBuffer;
  std::vector <UINT32> tickBufferInsertionTime;
  std::vector <T>      tockBuffer;
  std::vector <UINT32> tockBufferInsertionTime;
  
  // Empty explicit copy contructor here; illegal operation
  TimedExpiryBuffer (const TimedExpiryBuffer& anotherTimedExpiryBuffer);

  // Emply explicit copy assignment operator here; illegal operation
  TimedExpiryBuffer& operator= (const TimedExpiryBuffer& anotherTimedExpiryBuffer);

};

#endif
