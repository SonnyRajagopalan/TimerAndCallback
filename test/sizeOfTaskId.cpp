#include "platform/TaskId.h"
#include "common/Debug.h"
#include <string.h>
#include <unistd.h>
using namespace std;

int main()
{
  cout << "The size of Task ID object is " << sizeof (TaskId) << endl;
}
