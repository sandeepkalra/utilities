utilities
=========

This repo contain simple utilities. Here are their details:-

a. CfgCmd.h - this utility gives option to create a ASCII or BINARY command handling module withing your project. A command may have upto 3 levels , say (1) module,(2) submodule and (3) parameter. 
 A ascii command such as 
 $ Program "Module" "SubModule" "Param" <value> will then automatically 
  get routed to correct callback, and executed.
 

b. Phonebook.h - This utility gives a VoIP like phonebook.

c. log.h (and logbase.h) - This utility is C++ wrapper on syslog. The logger interface function names  D, I , C  and E are picked from Android world. C++ by default gives std::clog, but that push things to stderr and not syslog. A sample usage file log.c is there to guide on how to use it.
