#ifndef __SLOG_H__
#define __SLOG_H__

/**
 * slog.h
 *   A set of routines to help in logging message.  See the README for
 *   more details.
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

#include <glib.h>


// +---------------------------+--------------------------------------
// | Basic Reporting Functions |
// +---------------------------+

int slog_to_file (gchar *filename);
void slog_finish (void);
void slog_kernel (char *position, char *format, ...);
void slog_increase_indent (void);
void slog_decrease_indent (void);


// +--------+---------------------------------------------------------
// | Macros |
// +--------+

#define slog_message(stuff...) slog_kernel (G_STRLOC, stuff)
#define slog_enter(stuff...) \
  do { slog_increase_indent (); slog_message (stuff); } while (0)
#define slog_leave(stuff...) \
  do { slog_message (stuff); slog_decrease_indent (); } while (0)

#ifdef SLOG
#define SLOG_TO_FILE(fname)         slog_begin (fname);
#define SLOG_FINISH()               slog_finish ();
#define SLOG_MESSAGE(stuff...)      slog_message (stuff)
#define SLOG_ENTER(stuff...)        slog_enter (stuff)
#define SLOG_LEAVE(stuff...)        slog_leave (stuff)

#else // #ifndef SLOG
#define SLOG_TO_FILE()
#define SLOG_FINISH()
#define SLOG_MESSAGE(stuff...)
#define SLOG_ENTER(stuff...)
#define SLOG_LEAVE(stuff...)

#endif // #ifndef SLOG

#endif // __SLOG_H__
