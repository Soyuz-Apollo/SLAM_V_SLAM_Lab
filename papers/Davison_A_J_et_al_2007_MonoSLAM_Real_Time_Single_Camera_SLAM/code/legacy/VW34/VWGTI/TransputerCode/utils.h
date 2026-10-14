/** 
 * @file   utils.h
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
 */


extern df_type atan2_c(df_type y, df_type x);
extern df_type div_c(df_type denom, df_type num);
extern void ca(int num, ...);
extern void prob(void);
extern double sqr(double t);
extern df_type sqrf(df_type t);
extern df_type csqrtf(df_type t);
extern int tround(df_type a);
extern df_type hyppot(df_type a, df_type b);
extern void print_var(void);
extern df_type add(df_type a1, df_type a2);
extern df_type sub(df_type a1, df_type a2);
extern df_type fix(df_type tmp_f, df_type tmp_s);
extern df_type fix_gam(df_type tmp_ff, df_type tmp_ss);


