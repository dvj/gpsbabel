/*
	pushpins.h

	Extract data from MS Streets & Trips .est and Autoroute .axe files in GPX format.

    Copyright (C) 2003 James Sherring, james_sherring@yahoo.com

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

#ifdef __cplusplus
extern "C" {
#endif

//EXTERN_C 
struct pushpin_safelist * read_pushpins(char* ppin_file_name);
//EXTERN_C 
void write_pushpins_from_gpx(char* ppin_file_name, 
									  struct gpx_data * all_gpx, 
									  struct contents * conts,
									  char* conts_file_name);
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif