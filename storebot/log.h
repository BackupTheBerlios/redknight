#ifndef LOG__H
#define LOG__H

/* log functions: log_info simply writes a line to the file elbot.log, log_error
 * write "ERROR at <time>" followed by an error message. */
void log_info (const char *fmt, ...);
void log_error (const char *fmt, ...);

#endif /*def LOG__H */
