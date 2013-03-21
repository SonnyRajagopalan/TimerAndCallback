TIMER AND CALLBACK FRAMEWORK

Author: Sonny Rajagopalan/sonny.rajagopalan@gmail.com
Date: Fall 2011 (first version)/several cleanups: Spring 2012/Uploaded to Github: Spring 2013.

Description:

The C/C++ code presented herein is my take on an implementation of a memory pooled infrastruture
server that is used for signaling (e.g., SIP signaling or RADIUS signaling). The timer management
that this code base presents is a min-heap based timer management with callbacks. The accuracy is
platform dependent, of course. I will fill in those details as soon as I get some time...