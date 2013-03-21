#include "platform/Runnable.h"

UINT32 Runnable::numberOfRunnables = 0;

Runnable::Runnable ()
{
  numberOfRunnables ++;
}
