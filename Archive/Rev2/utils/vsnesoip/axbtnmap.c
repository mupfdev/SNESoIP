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

#include <errno.h>
#include <stdint.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/input.h>
#include <linux/joystick.h>

#include "axbtnmap.h"

/* The following values come from include/joystick.h in the kernel source. */
#define JSIOCSBTNMAP_LARGE _IOW('j', 0x33, __u16[KEY_MAX_LARGE - BTN_MISC + 1])
#define JSIOCSBTNMAP_SMALL _IOW('j', 0x33, __u16[KEY_MAX_SMALL - BTN_MISC + 1])
#define JSIOCGBTNMAP_LARGE _IOR('j', 0x34, __u16[KEY_MAX_LARGE - BTN_MISC + 1])
#define JSIOCGBTNMAP_SMALL _IOR('j', 0x34, __u16[KEY_MAX_SMALL - BTN_MISC + 1])

int determine_ioctl(int fd, int *ioctls, int *ioctl_used, void *argp)
{
	int i, retval = 0;

	/* Try each ioctl in turn. */
	for (i = 0; ioctls[i]; i++) {
		if ((retval = ioctl(fd, ioctls[i], argp)) >= 0) {
			/* The ioctl did something. */
			*ioctl_used = ioctls[i];
			return retval;
		} else if (errno != -EINVAL) {
			/* Some other error occurred. */
			return retval;
		}
	}
	return retval;
}

int getbtnmap(int fd, uint16_t *btnmap)
{
	static int jsiocgbtnmap = 0;
	int ioctls[] = { JSIOCGBTNMAP, JSIOCGBTNMAP_LARGE, JSIOCGBTNMAP_SMALL, 0 };

	if (jsiocgbtnmap != 0) {
		/* We already know which ioctl to use. */
		return ioctl(fd, jsiocgbtnmap, btnmap);
	} else {
		return determine_ioctl(fd, ioctls, &jsiocgbtnmap, btnmap);
	}
}

int setbtnmap(int fd, uint16_t *btnmap)
{
	static int jsiocsbtnmap = 0;
	int ioctls[] = { JSIOCSBTNMAP, JSIOCSBTNMAP_LARGE, JSIOCSBTNMAP_SMALL, 0 };

	if (jsiocsbtnmap != 0) {
		/* We already know which ioctl to use. */
		return ioctl(fd, jsiocsbtnmap, btnmap);
	} else {
		return determine_ioctl(fd, ioctls, &jsiocsbtnmap, btnmap);
	}
}

int getaxmap(int fd, uint8_t *axmap)
{
	return ioctl(fd, JSIOCGAXMAP, axmap);
}

int setaxmap(int fd, uint8_t *axmap)
{
	return ioctl(fd, JSIOCSAXMAP, axmap);
}
