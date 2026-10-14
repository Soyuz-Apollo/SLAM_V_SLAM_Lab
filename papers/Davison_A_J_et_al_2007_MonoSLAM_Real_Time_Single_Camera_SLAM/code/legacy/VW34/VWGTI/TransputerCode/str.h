/** 
 * @file   str.h
 * @author ID Reid
 * @date   02/08/2002
 *
 *              (c) Copyright University of Oxford
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * this file is a general structure file 
 */

/* structure to define the input
** to the vehicle inverse kinematics */
typedef struct{
df_type mu_bi;
df_type a_bi;
df_type x_bi;
df_type vel_i;
df_type vel_2;
df_type mu_i;
df_type bad_gri;
df_type o_gi;
df_type gri;
df_type obs_gi;
df_type ob_vel_i;
}
DATA;

/* structure to define the state X */
typedef struct{
	df_type vel_i, g_i;
	df_type vel_1, g_1;
	df_type vel_2, g_2;
}
Z_STATE;

/* structure to define the state X */
typedef struct{
	df_type vel_i, g_i;
}
Z;


/* structure to define the state X */
typedef struct{
df_type vel_b, ups, pd, p;
}
X_STATE;


/* structure to define the input to the vehicle inverse kinematics */
typedef struct{
df_type vel, ups, kap, x, y, pd, p;
int bool;
}
X;

/* define the structure to do with spline lengths */
typedef struct{
df_type c_s_l;
df_type t_s_l;
}
S_L;

/* struct to define the desired trajectory */
typedef struct{
df_type x, y, p;
}
POS;

/* structure to read in a two d point */
typedef struct{
df_type x, y;
}
TWO_D_POINT;

/* structure to define a bezier control point */
typedef struct{
	TWO_D_POINT p0, p1, p2, p3;
}
BEZ_C_POINT;

typedef struct{
	int step;
	df_type s;
}
RT_STRUCT;

/*
** define a structure that gives position and
** angle of the wheels (relative to
** the current wheel) to the virtual
** wheel
*/
typedef struct
{
	df_type x_bi; /* this wheel */
	df_type a_bi;

	df_type x_b1; /* wheel defined to be 1 */ 
	df_type a_b1;

	df_type x_b2; /* wheel defined to be 2 */
	df_type a_b2;
	
	df_type rad;
} DSU_POS;

typedef struct
{
    	df_type x, y, p, xd, yd, pd, g_i;  
        int bool;
} X_EST;

/* define a structure to hold all the observations from all the nodes */
typedef struct
{
	df_type w_b;
	df_type g_b;
	df_type w_lo;
	df_type	g_lo;
	df_type	w_1;
	df_type	g_1;
	df_type	w_2;
	df_type	g_2;
	df_type vel_b;
	df_type or_b;
	df_type pd;
	df_type p;
}INP_TOTAL;

/* define the input */
typedef struct
{
	df_type gi, gb, wi, wb;
	int bool;
} INPUT;

/* define the observation */
typedef struct
{
	df_type gi, wi;
	int bool;
}OBS;

/* define a structure to hold all the observations from all the nodes */
typedef struct
{
	df_type w_lo;
	df_type	g_lo;
	df_type	w_1;
	df_type	g_1;
	df_type	w_2;
	df_type	g_2;
}OBS_TOTAL;

/* define a structure to hold the data obtained from the local obserever */
typedef struct
{
    	int tc;
	df_type n_s_x_est, n_d_x_est;
	df_type n_s_xd_est, n_d_xd_est;
    	df_type n_s_z_obs, n_d_z_obs;
    	df_type o_s_x_est, o_d_x_est;
	df_type o_s_xd_est, o_d_xd_est;
} L_DATA;

typedef struct
{
	df_type x_pos;
	df_type y_pos;
	df_type phi;
	df_type o_x_pos;
	df_type o_y_pos;
	df_type o_phi;
	df_type d_t;
	df_type x_vel;
} INV_DATA;


/* define a structure to give the details
of a wheel */
typedef struct
{
    	df_type	xi_pos, yi_pos;
    	df_type	xj_pos, yj_pos;
    	df_type	xk_pos, yk_pos;
	df_type  radius;
	df_type  width, length;
	df_type  wwidth;
} WHEEL_TYPE;

typedef struct
{
	OBS ob;
	INPUT in;
} OB_IN;

typedef struct
{
	df_type	vel;
	df_type acc;
	df_type pd;
	df_type	pdd;
} MOTION_PARA;

