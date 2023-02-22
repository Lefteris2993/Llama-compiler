/******************************************************************************
 *  CVS version:
 *     $Id: error.h,v 1.1 2003/05/13 22:21:01 nickie Exp $
 ******************************************************************************
 *
 *  C header file : error.h
 *  Project       : PCL Compiler
 *  Version       : 1.0 alpha
 *  Written by    : Nikolaos S. Papaspyrou (nickie@softlab.ntua.gr)
 *  Date          : May 14, 2003
 *  Description   : Simple error handler
 *  Modified by   : Chaidemenos E., Siana A.
 *
 */


#ifndef __ERROR_HPP__
#define __ERROR_HPP__

class Logger {
public:
  static void internal (const char * fmt, ...);
  static void fatal    (const char * fmt, ...);
  static void error    (unsigned lineno, const char * fmt, ...);
  static void warning  (unsigned lineno, const char * fmt, ...);
};

#endif
