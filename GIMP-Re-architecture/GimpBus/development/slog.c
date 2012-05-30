/**
 * slog.c
 *   Debugging/message logging routines.for GimpBus.
 *   If these routines are called directly, they always produce
 *     output.  Use the macro versions instead if you want to be
 *     able to turn them on or off.
 *
 * Copyright (c) 2008-2011 Samuel A. Rebelsky
 *
 * This file is part of SLOG.
 *
 * SLOG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or (at your option) any later version.
 *
 * SLOG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SLOG.  If not, see 
 *   <http://www.gnu.org/licenses/>.
 */


// +---------+--------------------------------------------------------
// | Headers |
// +---------+

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <glib.h>
#include <glib/gstdio.h>

#include "slog.h"


// +-----------+------------------------------------------------------
// | Constants |
// +-----------+

/**
 * The maximum depth of procedure calls we trace.
 */
#define SLOG_MAX_DEPTH 128


// +---------+--------------------------------------------------------
// | Globals |
// +---------+

/**
 * The port used for printing log messages.
 */
static FILE *slog_port = NULL;

/**
 * The current indent for log messages.
 */
static int slog_depth = 0;

/**
 * The time of the last log message.
 */
static struct timeval slog_start [SLOG_MAX_DEPTH];


// +-----------------+------------------------------------------------
// | Local Functions |
// +-----------------+

/**
 * Determine the amount of time that has elapsed since this depth of log 
 * has started.
 */
static int
slog_elapsed (void)
{
  int depth = (slog_depth < SLOG_MAX_DEPTH) 
              ? slog_depth 
              : (SLOG_MAX_DEPTH - 1);
  struct timeval tv;
  gettimeofday (&tv, NULL);
  int elapsed = tv.tv_usec - slog_start[depth].tv_usec
                + (1000000 * (tv.tv_sec - slog_start[depth].tv_sec));
  return elapsed;
} // slog_elapsed


// +--------------------+---------------------------------------------
// | Exported Functions |
// +--------------------+

/**
 * Log to a file.
 */
gboolean
slog_to_file (char *filename)
{
  slog_port = g_fopen (filename, "a");
  return (slog_port != NULL);
} // slog_to_file

/**
 * End logging.
 */
void
slog_finish (void)
{
  fclose (slog_port);
  slog_port = stderr;
} // slog_finish

/**
 * Increase the indentation of log messages
 */
void
slog_increase_indent (void)
{
  ++slog_depth;
  if (slog_depth < SLOG_MAX_DEPTH)
    gettimeofday (&(slog_start[slog_depth]), NULL);
} // slog_increase_indent

/**
 * Decrease the indentation of log messages.
 */
void
slog_decrease_indent (void)
{
  --slog_depth;
}  // slog_decrease_indent

/**
 * Report something, using printf style.  E.g.,
 *   slog_kernel ("line 110", "silly = %d", silly);
 * or, better yet,
 *   slog_kernel (G_STRLOC, "gilly = %d"", silly);
 * 
 * Generally, slog_kernel is not called directly.  Rather,
 *   SLOG_ENTER, SLOG_LEAVE, or SLOG_MESSGE (defined in slog.h) are used.
 */
void
slog_kernel (char *position, char *format, ...)
{
  // Sanity check
  if (slog_port == NULL)
    slog_port = stderr;

  // Print the process id
  fprintf (slog_port, "P:%4d ", getpid ());

  // Print the time
  fprintf (slog_port, "E:%6d ", slog_elapsed ());

  // Print the position as a prefix
  fprintf (slog_port, "[%s]\t|", position);

  // Indent
  int i;
  for (i = 0; i < slog_depth; i++)
    fprintf (slog_port, " ");

  // Print the remaining arguments.  This code is based closely
  // on the sample code found at
  //   http://www.eskimo.com/~scs/cclass/int/sx11c.html
  va_list argp;
  va_start (argp, format);
  vfprintf (slog_port, format, argp);
  va_end (argp);

  // We don't require a carriage return in the format line b/c we want
  // to match the calling structure of gtk_message (well, kind of).
  fprintf (slog_port, "\n");

  // We may be watching the log, so flush the output
  fflush (slog_port); 
} // slog_kernel

