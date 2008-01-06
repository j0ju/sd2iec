/* sd2iec - SD/MMC to Commodore serial bus interface/controller
   Copyright (C) 2007,2008  Ingo Korb <ingo@akana.de>

   Inspiration and low-level SD/MMC access based on code from MMC2IEC
     by Lars Pontoppidan et al., see sdcard.c|h and config.h.

   FAT filesystem access based on code from ChaN, see tff.c|h.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License only.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   
   buffers.h: Data structures for the internal buffers
*/

#ifndef BUFFERS_H
#define BUFFERS_H

#include <stdint.h>
#include "dirent.h"

/**
 * struct buffer_s - buffer handling structire
 * @data     : Pointer to the data area of the buffer
 * @lastused : Index to the last used byted
 * @position : Index of the byte that will be read/written next
 * @seconday : Secondary address the buffer is associated with
 * @allocated: Flags if the buffer is allocated or not
 * @dirty    : Flags if the data needs to be saved
 * @read     : Flags if the buffer was opened for reading
 * @write    : Flags if the buffer was opened for writing
 * @sendeoi  : Flags if the last byte should be sent with EOI
 * @refill   : Callback to refill/write out the buffer, returns true on error
 * @cleanup  : Callback to clean up and save remaining data, returns true on error
 * 
 * Most allocated buffers point into the same bufferdata array, but
 * the error channel uses the same structure to avoid special-casing it
 * everywhere.
 */
typedef struct buffer_s {
  /* The error channel uses the same data structure for convenience reasons, */
  /* so data must be a pointer. It also allows swapping the buffers around   */
  /* in case I ever add external ram (not XRAM) to the design (which will    */
  /* require locking =( ).                                                   */
  uint8_t *data; 
  uint8_t lastused;
  uint8_t position;
  uint8_t secondary;
  int     allocated:1;
  int     dirty:1;
  int     read:1;
  int     write:1;
  int     sendeoi:1;
  uint8_t (*refill)(struct buffer_s *buffer);
  uint8_t (*cleanup)(struct buffer_s *buffer);

  /* private: */
  union {
    struct {
      dh_t dh;          /* Directory handle */
      uint8_t filetype; /* File type */
      char *matchstr;   /* Pointer to filename pattern */
    } dir;
    FIL fh;             /* File access */
  } pvt;
} buffer_t;

extern dh_t matchdh;         /// Directory handle used in file matching
extern buffer_t buffer[];    /// Simplifies access to the error buffer length

extern FIL imagehandle;      /// Filehandle for mounted image files
extern uint8_t entrybuf[33]; /// Buffer for directory entries to be parsed

/* Initializes the buffer structures */
void init_buffers(void);

/* Allocates a buffer - Returns pointer to buffer or NULL of failure */
buffer_t *alloc_buffer(void);

/* Deallocates a buffer */
void free_buffer(buffer_t *buffer);

/* Deallocates all buffers, cleanup optional */
uint8_t free_all_buffers(uint8_t cleanup);

/* Finds the buffer corresponding to a secondary address */
/* Returns pointer to buffer on success or NULL on failure */
buffer_t *find_buffer(uint8_t secondary);

/* Number of currently allocated buffers */
uint8_t active_buffers;

#endif
