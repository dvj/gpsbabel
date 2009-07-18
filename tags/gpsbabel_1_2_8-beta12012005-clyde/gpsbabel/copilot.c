/*
    Read and write CoPilot files.

    Copyright (C) 2002 Paul Tomblin, ptomblin@xcski.com

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
#include "coldsync/palm.h"
#include "coldsync/pdb.h"

static double conv = 180.0 / M_PI;

#define MYNAME		"CoPilot Waypoint"
#define MYTYPE 		0x77617970  	/* wayp */
#define MYCREATOR	0x47584255 		/* GXBU */


struct record {
	pdb_double	latitude; 	/* PDB double format, */
	pdb_double	longitude; 	/* similarly, neg = east */
	pdb_double	magvar; 	/* magnetic variation in degrees, neg = east */
	pdb_double	elevation; 	/* feet */
	char		flags;
};

static FILE *file_in;
static FILE *file_out;
static const char *out_fname;
static struct pdb *opdb;
static struct pdb_record *opdb_rec;

static void
rd_init(const char *fname)
{
	file_in = xfopen(fname, "rb", MYNAME);
}

static void
rd_deinit(void)
{
	fclose(file_in);
}

static void
wr_init(const char *fname)
{
	file_out = xfopen(fname, "wb", MYNAME);
	out_fname = fname;
}

static void
wr_deinit(void)
{
	fclose(file_out);
}

static void
data_read(void)
{
	struct record *rec;
	struct pdb *pdb;
	struct pdb_record *pdb_rec;

	if (NULL == (pdb = pdb_Read(fileno(file_in)))) {
		fatal(MYNAME ": pdb_Read failed\n");
	}

	if ((pdb->creator != MYCREATOR) || (pdb->type != MYTYPE)) {
		fatal(MYNAME ": Not a CoPilot file.\n");
	}

	for(pdb_rec = pdb->rec_index.rec; pdb_rec; pdb_rec=pdb_rec->next) {
		waypoint *wpt_tmp;
		char *vdata;

		wpt_tmp = waypt_new();

		rec = (struct record *) pdb_rec->data;
		wpt_tmp->longitude =
		  -pdb_read_double(&rec->longitude) * conv;
		wpt_tmp->latitude =
		  pdb_read_double(&rec->latitude) * conv;
		wpt_tmp->altitude =
		  pdb_read_double(&rec->elevation) * .3048;

		vdata = (char *) pdb_rec->data + sizeof(*rec);

		wpt_tmp->shortname = xstrdup(vdata);
		vdata = vdata + strlen(vdata) + 1;

		wpt_tmp->description = xstrdup(vdata);
		vdata = vdata + strlen(vdata) + 1;
		
		wpt_tmp->notes = xstrdup(vdata);

		waypt_add(wpt_tmp);

	} 
	free_pdb(pdb);
}


static void
copilot_writewpt(const waypoint *wpt)
{
	struct record *rec;
	static int ct = 0;
	char *vdata;

	rec = xcalloc(sizeof(*rec)+1141,1);

	pdb_write_double(&rec->latitude, wpt->latitude / conv);
	pdb_write_double(&rec->longitude,
		-wpt->longitude / conv);
	pdb_write_double(&rec->elevation,
		wpt->altitude / .3048);
	pdb_write_double(&rec->magvar, 0);

	vdata = (char *)rec + sizeof(*rec);
	if ( wpt->shortname ) {
			  strncpy( vdata, wpt->shortname, 10 );
			  vdata[9] = '\0';
	}
	else {
			  vdata[0] ='\0';
	}
    vdata += strlen( vdata ) + 1;
	if ( wpt->description ) {
			strncpy( vdata, wpt->description, 100 );
			vdata[99] = '\0';
	}
	else {
			vdata[0] ='\0';
	}
	vdata += strlen( vdata ) + 1;
	
	if ( wpt->notes ) {
			strncpy( vdata, wpt->notes, 1000 );
			vdata[999] = '\0';
	}
	else {
			vdata[0] ='\0';
	}
	vdata += strlen( vdata ) + 1;

	opdb_rec = new_Record (0, 2, ct++, (uword) (vdata-(char *)rec), (const ubyte *)rec);	       

	if (opdb_rec == NULL) {
		fatal(MYNAME ": libpdb couldn't create record\n");
	}

	if (pdb_AppendRecord(opdb, opdb_rec)) {
		fatal(MYNAME ": libpdb couldn't append record\n");
	}
	xfree(rec);
}

static void
data_write(void)
{
	if (NULL == (opdb = new_pdb())) { 
		fatal (MYNAME ": new_pdb failed\n");
	}

	strncpy(opdb->name, out_fname, PDB_DBNAMELEN);
	opdb->name[PDB_DBNAMELEN-1] = 0;
	opdb->attributes = PDB_ATTR_BACKUP;
	opdb->ctime = opdb->mtime = current_time() + 2082844800U;
	opdb->type = MYTYPE;
	opdb->creator = MYCREATOR; 
	opdb->version = 0;

	waypt_disp_all(copilot_writewpt);
	
	pdb_Write(opdb, fileno(file_out));
}


ff_vecs_t copilot_vecs = {
	ff_type_file,
	FF_CAP_RW_WPT,
	rd_init,
	wr_init,
	rd_deinit,
	wr_deinit,
	data_read,
	data_write,
	NULL,
	NULL,
	CET_CHARSET_ASCII, 0	/* CET-REVIEW */
};