include ./_include.mk # defines $(BASEDIR) among other things.

all: obj tpooltest ppooltest scaletest

setup:
	sudo mkdir /dev/mqueue;
	sudo mount -t mqueue none /dev/mqueue;

prep:
	@echo "\n!! ATTENTION !!!\n\n"
	@echo "First: Create and mount mqueues onto /dev/mqueue (Change for non ubuntu systems).\n"
	rm -rf /run/shm/*Pool /run/shm/sem.*;
	rm -rf /dev/mqueue/Test;
	rm -rf /dev/mqueue/*Task;
	killall pool || true;
	killall timerPoolTest || true;
	killall periodicTimerTest || true;
	killall longTermPeriodicTest || true;


obj:
	for dir in $(DIRS); do (cd $$dir; make $1 || exit 1) || exit 1; done # DO NOT DELETE

pool: $(BASEDIR)/test/pool.cpp
	@echo "Generating code to check if memory pool creation, new and delete work..."
	$(CPP) $(CPPFLAGS) $(INCLUDE) \
	$(BASEDIR)/test/PooledObject.o \
	$(BASEDIR)/platform/MemoryPool.o \
	$(BASEDIR)/platform/PosixSharedMemory.o \
	$(BASEDIR)/test/pool.cpp -o pool $(LIB)

tpooltest: $(BASEDIR)/test/timerPoolTest.cpp obj
	$(CPP) $(CPPFLAGS) $(INCLUDE) $(LIB) \
	$(BASEDIR)/platform/MemoryPool.o $(BASEDIR)/test/TestCallback.o \
	$(BASEDIR)/platform/Timer.o $(BASEDIR)/platform/TimerCallback.o \
	$(BASEDIR)/platform/PosixSharedMemory.o $(BASEDIR)/platform/PosixSemaphore.o \
	$(BASEDIR)/platform/Runnable.o \
	$(BASEDIR)/platform/TimerUtils.o $(BASEDIR)/platform/TimerMinHeap.o \
	$(BASEDIR)/platform/TimerTask.o $(BASEDIR)/platform/PosixMessageQueue.o \
	$(BASEDIR)/common/Util.o \
	-o $(BASEDIR)/timerPoolTest $(BASEDIR)/test/timerPoolTest.cpp $(LIB)

ppooltest: $(BASEDIR)/test/periodicTimerTest.cpp obj
	$(CPP) $(CPPFLAGS) $(INCLUDE) $(LIB) \
	$(BASEDIR)/platform/MemoryPool.o $(BASEDIR)/test/TestCallback.o \
	$(BASEDIR)/platform/Timer.o $(BASEDIR)/platform/TimerCallback.o \
	$(BASEDIR)/platform/PosixSharedMemory.o $(BASEDIR)/platform/PosixSemaphore.o \
	$(BASEDIR)/platform/Runnable.o \
	$(BASEDIR)/platform/TimerUtils.o $(BASEDIR)/platform/TimerMinHeap.o \
	$(BASEDIR)/platform/TimerTask.o $(BASEDIR)/platform/PosixMessageQueue.o \
	$(BASEDIR)/common/Util.o \
	-o $(BASEDIR)/periodicTimerTest $(BASEDIR)/test/periodicTimerTest.cpp $(LIB)

scaletest: $(BASEDIR)/test/longTermPeriodicTest.cpp obj
	$(CPP) $(CPPFLAGS) $(INCLUDE) $(LIB) \
	$(BASEDIR)/platform/MemoryPool.o $(BASEDIR)/test/TestCallback.o \
	$(BASEDIR)/platform/Timer.o $(BASEDIR)/platform/TimerCallback.o \
	$(BASEDIR)/platform/PosixSharedMemory.o $(BASEDIR)/platform/PosixSemaphore.o \
	$(BASEDIR)/platform/Runnable.o \
	$(BASEDIR)/platform/TimerUtils.o $(BASEDIR)/platform/TimerMinHeap.o \
	$(BASEDIR)/platform/TimerTask.o $(BASEDIR)/platform/PosixMessageQueue.o \
	$(BASEDIR)/common/Util.o \
	-o $(BASEDIR)/longTermPeriodicTest $(BASEDIR)/test/longTermPeriodicTest.cpp $(LIB)

tar:
	tar -cvjf diam-$(BUILDID).tgz \
	$(BASEDIR)/_include.mk $(BASEDIR)/Makefile \
	$(BASEDIR)/include/common/*.h  \
	$(BASEDIR)/include/platform/*.h \
	$(BASEDIR)/include/test/*.h \
	$(BASEDIR)/common/*.cpp \
	$(BASEDIR)/platform/*.cpp \
	$(BASEDIR)/test/*.cpp \
	$(BASEDIR)/common/Makefile \
	$(BASEDIR)/platform/Makefile  $(BASEDIR)/test/Makefile


clean:
	rm -rf $(BASEDIR)/client $(BASEDIR)/server $(BASEDIR)/timerPoolTest \
	$(BASEDIR)/periodicTimerTest $(BASEDIR)/longTermPeriodicTest \
	$(BASEDIR)/common/*.o \
	$(BASEDIR)/platform/*.o  $(BASEDIR)/test/*.o \
	$(BASEDIR)/common/*.depends \
	$(BASEDIR)/platform/*.depends $(BASEDIR)/test/*.depends \
	$(BASEDIR)/common/*.P \
	$(BASEDIR)/platform/*.P $(BASEDIR)/test/*.P
	$(BASEDIR)/common/*.*~ \
	$(BASEDIR)/platform/*.*~ $(BASEDIR)/test/*.*~ \
	$(BASEDIR)/include/common/*.h~ \
	$(BASEDIR)/include/platform/*.h~  $(BASEDIR)/include//test/*.h~ || true

tags:
	$(BASEDIR)/genTags.sh

backup: tar
	cp diam-$(BUILDID).tgz /tahoma/diam/