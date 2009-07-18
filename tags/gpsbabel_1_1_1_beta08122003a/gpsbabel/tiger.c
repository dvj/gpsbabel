/*
    Access to  U.S. Census Bureau "tiger" format.

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

#include "defs.h"
#include "magellan.h"

static FILE *file_in;
static FILE *file_out;
static FILE *linkf;
static void *mkshort_handle;

#define MYNAME "GPSUTIL"

static double maxlat, maxlon, minlat, minlon, latsum, lonsum;
int rec_cnt;
static char *nolabels = NULL;
static char *genurl = NULL;
static char *scale = "768";
int scalev;

/*
 *   The code bracketed by CLICKMAP is to generate clickable image maps
 *   for a web browser.   It's functional, but is missing the math to do
 *   the projection transformations.   Some trig geek can finish that.
 */
#if CLICKMAP
static char *clickmap = NULL;
#endif


static
arglist_t tiger_args[] = {
	{"nolabels", &nolabels, "Suppress labels on generated pins.",
		ARGTYPE_BOOL },
	{"genurl", &genurl, "Generate file with lat/lon for centering map.",
		ARGTYPE_OUTFILE },
	{"scale", &scale, "Dimension in pixels of map.",
		ARGTYPE_INT},
#if CLICKMAP
	{"clickmap", &clickmap, "Generate Clickable map web page.",
		ARGTYPE_BOOL},
#endif
	{0, 0, 0, 0}
};


static void
rd_init(const char *fname, const char *args)
{
	file_in = fopen(fname, "r");
	mkshort_handle = mkshort_new_handle();

	if (file_in == NULL) {
		fatal(MYNAME ": Cannot open %s for reading\n", fname);
	}
}

static void
rd_deinit(void)
{
	fclose(file_in);
	mkshort_del_handle(mkshort_handle);
}

static void
wr_init(const char *fname, const char *args)
{
	file_out = fopen(fname, "w");

	if (file_out == NULL) {
		fatal(MYNAME ": Cannot open %s for writing\n", fname);
	}
}

static void
wr_deinit(void)
{
	fclose(file_out);
}

static void
data_read(void)
{
	double lat,lon;
	char desc[100];
	char icon[100];
	char ibuf[1024];
	waypoint *wpt_tmp;

	while (fgets(ibuf, sizeof(ibuf), file_in)) {
		if( sscanf(ibuf, "%lf,%lf:%100[^:]:%100[^\n]", 
				&lon, &lat, icon, desc)) {
			wpt_tmp = xcalloc(sizeof (*wpt_tmp), 1);

			wpt_tmp->position.longitude.degrees = lon;
			wpt_tmp->position.latitude.degrees = lat;
			wpt_tmp->description = xstrdup(desc);
			wpt_tmp->shortname = mkshort(mkshort_handle, desc);

			waypt_add(wpt_tmp);
		}
	}
}

static void
tiger_disp(const waypoint *wpt)
{
	char *pin;
	double lat = wpt->position.latitude.degrees;
	double lon = wpt->position.longitude.degrees;
	if (wpt->creation_time > time(0) - 3600 * 24 * 14)
		pin = "greenpin";
	else
		pin = "redpin";

	if (genurl) {
		if (lat > maxlat) maxlat = lat;
		if (lon > maxlon) maxlon = lon;
		if (lat < minlat) minlat = lat;
		if (lon < minlon) minlon = lon;
		latsum += lat;
		lonsum += lon;
		rec_cnt++;
	}

	fprintf(file_out, "%f,%f:%s", lon, lat, pin);
	if (!nolabels) {
		fprintf(file_out, ":%s", wpt->description);
	}
	fprintf(file_out, "\n");
}

static void
map_plot(const waypoint *wpt)
{
	static int x,y;

	/* Replace with real math. */
	x+=10;
	y+=10;

	fprintf(linkf, "<area shape=\"circle\" coords=\"%d,%d,7\" href=\"%s\" alt=\"%s\"\n", x, y, wpt->url, wpt->description);
}

static void
data_write(void)
{
	maxlat = -9999.0;
	maxlon = -9999.0;
	minlat = 9999.0;
	minlon = 9999.0;
	rec_cnt = 0;
	fprintf(file_out, "#tms-marker\n");
	waypt_disp_all(tiger_disp);

	if (genurl) {
		FILE *urlf;

		urlf = fopen(genurl, "w");
		if (urlf == NULL) {
			fatal(MYNAME ": Cannot open '%s' for writing\n", 
					genurl);
		} 

		fprintf(urlf, "lat=%f&lon=%f&wid=%f&ht=%f",
				latsum / rec_cnt,
				lonsum / rec_cnt,
				maxlat - minlat,
				maxlon - minlon);

		if (scale) {
			fprintf(urlf, "&iwd=%s&iht=%s", scale, scale);
		}
		fclose(urlf);
#if CLICKMAP
		if (clickmap) {
			linkf = fopen(clickmap, "w");
			if (linkf == NULL) {
				fatal(MYNAME ": Cannot open '%s' for writing\n",
						clickmap);
			}
			fprintf(linkf, "<map name=\"map\">\n");
			waypt_disp_all(map_plot);
			fprintf(linkf, "</map>\n");
			fclose(linkf);
			linkf = NULL;
		}
#endif
	}
}


ff_vecs_t tiger_vecs = {
	rd_init,
	wr_init,
	rd_deinit,
	wr_deinit,
	data_read,
	data_write,
	tiger_args,
};