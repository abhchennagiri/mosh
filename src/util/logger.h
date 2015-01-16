/*
Copyright (c) 2014 by Matthieu Boutier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#include "utils.h"

extern int log_level;
extern int log_indent_level;
extern FILE *log_output;

void log_msg(int level, const char *format, ...) ATTRIBUTE ((format (printf, 2, 3))) COLD;

/* parse message of the form: "error|debug|dcommon|dall", and set the log level appropriately. */
void log_parse_level(const char *string);

/* If debugging is disabled, we want to avoid calling format_address
   for every omitted debugging message.  So debug is a macro.  But
   vararg macros are not portable. */
#if defined NO_DEBUG

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define log_dbg(...) do {} while(0)
#elif defined __GNUC__
#define log_dbg(_args...) do {} while(0)
#else
#define log_dbg if(0) log_msg
#endif

#define DEBUG(level, statement)

#else /* !NO_DEBUG */

/* log levels */
#define LOG_ERROR               2
#define LOG_DEBUG               4
#define LOG_MAX                 7
#define LOG_GET_LEVEL(x)        ((x) & 0xF)

/* log special flags */
#define LOG_PRINT_ERROR         (1 << 7)

/* log debug types */
#define LOG_DEBUG_COMMON        ((1 << 8) | LOG_DEBUG)
#define LOG_DEBUG_ALL           ((0xFF00) | LOG_DEBUG)
#define LOG_GET_TYPE(x)         ((x) & 0xFFF0)

/* shortcuts */
#define LOG_PERROR              (LOG_PRINT_ERROR | LOG_ERROR)

#define LOG_DO_DEBUG(level)                                       \
  UNLIKELY((LOG_GET_TYPE(level) & LOG_GET_TYPE(log_level)) &&     \
           (LOG_DEBUG <= LOG_GET_LEVEL(log_level)))

#define DEBUG(level, statement)                 \
  if(LOG_DO_DEBUG(level)) {statement;}

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define log_dbg(level, ...)                                             \
  do {                                                                  \
    if(LOG_DO_DEBUG(level)) log_msg(level, __VA_ARGS__);                \
  } while(0)
#elif defined __GNUC__
#define log_dbg(level, _args...)                                        \
  do {                                                                  \
    if(LOG_DO_DEBUG(level)) log_msg(level, _args);                      \
  } while(0)
#else
#warning No debug available.
static inline void log_dbg(int level, const char *format, ...) { return; }
#endif

#endif /* NO_DEBUG */
