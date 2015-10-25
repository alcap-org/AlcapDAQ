/* sis3100_tools.h --- 
 * 
 * Filename:        sis3100_tools.h
 * Description:     auxiliary function for communication with VME crate through SIS3100 interface
 * Author:          Vladimir Tishchenko
 * Maintainer: 
 * Created: Thu Oct 20 16:14:30 2011 (-0400)
 * Version:         $Id$
 * Last-Updated: Fri Feb 17 11:38:04 2012 (-0500)
 *           By: g minus two
 *     Update #: 14
 * URL: 
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change Log:
 *  
 *   $Log$
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */

#ifndef sis3100_tools_h
#define sis3100_tools_h

#ifdef sis3100_tools_c
#define EXTERN
#else
#define EXTERN extern
#endif

// EXTERN int fd_vme;

/* make functions callable from a C++ program */
#ifdef __cplusplus
EXTERN "C" {
#endif

  EXTERN int sis3100_open(int *fd);
  EXTERN int sis3100_close(void);

  EXTERN int vme_A32D32_write(int p, u_int32_t vme_adr, u_int32_t vme_data ) ;
  EXTERN int vme_A32D32_read(int p, u_int32_t vme_adr, u_int32_t* vme_data ) ;
  EXTERN int vme_A32MBLT64_read(int p, u_int32_t vme_adr, u_int32_t* vme_data, 
				u_int32_t req_num_of_lwords, u_int32_t* got_no_of_lwords) ;
  EXTERN int vme_A32BLT32_read(int p, u_int32_t vme_adr, u_int32_t* vme_data, 
			       u_int32_t req_num_of_lwords, u_int32_t* got_no_of_lwords) ;
  EXTERN int s3100_control_read(int p, int offset, u_int32_t* data) ;
  EXTERN int s3100_control_write(int p, int offset, u_int32_t data) ;

#ifdef __cplusplus
}
#endif 


#undef EXTERN
#endif   /* sis3100_tools_h defined  */
/* sis3100_tools.h ends here */
