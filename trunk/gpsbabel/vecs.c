/*
    Describe vectors containing file operations.
 
    Copyright (C) 2002 Robert Lipe, robertlipe@usa.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

 */

#include <stdio.h>
#include "defs.h"

typedef struct {
	ff_vecs_t *vec;
	char *name;
	char *desc;
} vecs_t;

extern ff_vecs_t geo_vecs;
extern ff_vecs_t gpsman_vecs;
extern ff_vecs_t gpx_vecs;
extern ff_vecs_t mag_vecs;
extern ff_vecs_t mapsend_vecs;
extern ff_vecs_t mapsource_vecs;
extern ff_vecs_t gpsutil_vecs;
extern ff_vecs_t tiger_vecs;
extern ff_vecs_t pcx_vecs;
extern ff_vecs_t csv_vecs;
extern ff_vecs_t cetus_vecs;

static
vecs_t vec_list[] = {
	{
		&geo_vecs, 
		"geo",
		"Geocaching.com .loc"
	}, 
	{
		&gpsman_vecs,
		"gpsman",
		"GPSman"
	},
	{
		&gpx_vecs,
		"gpx",
		"GPX XML"
	},
	{
		&mag_vecs,
		"magellan",
		"Magellan protocol"
	},
	{
		&mapsend_vecs,
		"mapsend",
		"Magellan Mapsend"
	},
	{
		&pcx_vecs,
		"pcx",
		"Garmin PCX5"
	},
	{
		&mapsource_vecs,
		"mapsource",
		"Garmin Mapsource"
	},
	{
		&gpsutil_vecs,
		"gpsutil",
		"gpsutil"
	},
	{
		&tiger_vecs,
		"tiger",
		"U.S. Census Bureau Tiger Mapping Service"
	},
	{
		&csv_vecs,
		"csv",
		"Comma separated values"
	},
	{
		&cetus_vecs,
		"cetus",
		"Cetus for Palm/OS"
	},
	{
		NULL,
		NULL,
		NULL
	}
};

ff_vecs_t *
find_vec(char *const vecname)
{
	vecs_t *vec = vec_list;
	while (vec->vec) {
		if (strcmp(vecname, vec->name) == 0) {
			return vec->vec;
		}
		vec++;
	}
	return NULL;
}

void
disp_vecs(void)
{
	vecs_t *vec;
	for (vec = vec_list; vec->vec; vec++) {
		printf("%-20.20s  %-50.50s\n",
			vec->name, vec->desc);
	}
}
