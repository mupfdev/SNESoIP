/*
 * Axis and button map support functions.
 * Copyright © 2009 Stephen Kitt <steve@sk2.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __AXBTNMAP_H__
#define __AXBTNMAP_H__

#include <stdint.h>
#include <linux/input.h>

/* The following values come from include/input.h in the kernel
   source; the small variant is used up to version 2.6.27, the large
   one from 2.6.28 onwards. We need to handle both values because the
   kernel doesn't; it only expects one of the values, and we need to
   determine which one at run-time. */
#define KEY_MAX_LARGE 0x2FF
#define KEY_MAX_SMALL 0x1FF

/* Axis map size. */
#define AXMAP_SIZE (ABS_MAX + 1)

/* Button map size. */
#define BTNMAP_SIZE (KEY_MAX_LARGE - BTN_MISC + 1)

/* Retrieves the current axis map in the given array, which must
   contain at least AXMAP_SIZE elements. Returns the result of the
   ioctl(): negative in case of an error, 0 otherwise for kernels up
   to 2.6.30, the length of the array actually copied for later
   kernels. */
int getaxmap(int fd, uint8_t *axmap);

/* Uses the given array as the axis map. The array must contain at
   least AXMAP_SIZE elements. Returns the result of the ioctl():
   negative in case of an error, 0 otherwise. */
int setaxmap(int fd, uint8_t *axmap);

/* Retrieves the current button map in the given array, which must
   contain at least BTNMAP_SIZE elements. Returns the result of the
   ioctl(): negative in case of an error, 0 otherwise for kernels up
   to 2.6.30, the length of the array actually copied for later
   kernels. */
int getbtnmap(int fd, uint16_t *btnmap);

/* Uses the given array as the button map. The array must contain at
   least BTNMAP_SIZE elements. Returns the result of the ioctl():
   negative in case of an error, 0 otherwise. */
int setbtnmap(int fd, uint16_t *btnmap);

#endif

