/***************************************************************************
 *   Copyright (C) 2004 See the "CONTRIBUTORS.txt" file                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "log.h"

extern unsigned short int verbose;


/* log functions: log_info simply writes a line to the file elbot.log, log_error
 * write "ERROR at <time>" followed by an error message. */
void log_info (const char *fmt, ...) {
  FILE *fout = fopen ("elbot.log", "a");
  va_list ap;

  if (fout == NULL) {
    fprintf (stderr, "Error: unable to open log file!\n");
    exit (1);
  }

  va_start (ap, fmt);
  if(verbose)vprintf(fmt, ap);
  vfprintf (fout, fmt, ap);
  fclose (fout);
}

void log_error (const char *fmt, ...) {
  time_t tsec;
  FILE *fout = fopen ("elbot.log", "a");
  va_list ap;

  if (fout == NULL) {
    fprintf (stderr, "Error: unable to open log file!\n");
    exit (1);
  }

  tsec = time (NULL);
  fprintf (fout, "ERROR at %s--> ", ctime (&tsec));
  va_start (ap, fmt);
  if(verbose)vprintf (fmt, ap);
  vfprintf (fout, fmt, ap);
  fclose (fout);
}
