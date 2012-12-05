/// \file ijkcoord.txx
/// ijk templates for coordinate arithmetic
/// Version 0.1.0

/*
  IJK: Isosurface Jeneration Kode
  Copyright (C) 2009, 2011 Rephael Wenger

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  (LGPL) as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _IJKCOORD_
#define _IJKCOORD_

#include <algorithm>
#include <cmath>

/// Coordinate arithmetic functions.
namespace IJK {

  ///
  /// \defgroup coordinates Coordinate Operations
  /* \ingroup coordinates */
  /* \{ */

  /// Set all vertex coordinates to \a c.
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param c = Scalar constant.  Set vertex coordinates to \a c.
  /// @param[out] coord[] = Output coordinates.
  template <class DTYPE, class STYPE, class CTYPE>
  void set_coord(const DTYPE dimension, const STYPE c, CTYPE coord[])
  {
    for (DTYPE d = 0; d < dimension; d++)
      { coord[d] = CTYPE(c); };
  }

  /// Copy \a coord0[] to \a coord1[].
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param[out] coord1 = Output coordinates.
  template <class DTYPE, class CTYPE0, class CTYPE1>
  void copy_coord(const DTYPE dimension, 
                  const CTYPE0 coord0[], CTYPE1 coord1[])
  {
    for (DTYPE d = 0; d < dimension; d++)
      { coord1[d] = coord0[d]; };
  }

  /// Copy \a coord0[] to \a coord1[].
  /// Faster algorithm when \a coord0[] and \a coord1[] are both type float.
  /// @param[out] coord1 = Output coordinates.
  template <class DTYPE>
  void copy_coord(const DTYPE dimension, 
                  const float * coord0[], const float * coord1[])
  {
    std::copy(coord0, coord0+dimension, coord1);
  }

  /// Copy \a coord0[] to \a coord1[].
  /// Faster algorithm when \a coord0[] and \a coord1[] are both type double.
  /// @param[out] coord1 = Output coordinates.
  template <class DTYPE>
  void copy_coord(const DTYPE dimension, 
                  const double * coord0[], const double * coord1[])
  {
    std::copy(coord0, coord0+dimension, coord1);
  }

  /// Add \a coord0[] to \a coord1[].
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] coord2 = Output coordinate equal to (\a coord0[] + \a coord1[]).
  template <class DTYPE, class CTYPE0, class CTYPE1, class CTYPE2>
  void add_coord(const DTYPE dimension, const CTYPE0 coord0[],
                 const CTYPE1 coord1[], CTYPE2 coord2[])
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { coord2[d] = coord0[d] + coord1[d]; };
  }

  /// Subtract \a coord1[] from \a coord0[].
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] coord2 = Output coordinate equal to (\a coord0[] - \a coord1[]).
  template <class DTYPE, class CTYPE0, class CTYPE1, class CTYPE2>
  inline void subtract_coord
  (const DTYPE dimension, const CTYPE0 coord0[], const CTYPE1 coord1[],
   CTYPE2 coord2[])
  {
    for (DTYPE d = 0; d < dimension; d++)
      { coord2[d] = coord0[d] - coord1[d]; };
  }

  /// Multiply \a coord0[] by the scalar \a s.
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param s = Scalar.
  /// @param[out] coord1 = Output coordinate equal to (\a s * \a coord0[]).
  template <class DTYPE, class STYPE, class CTYPE0, class CTYPE1>
  inline void multiply_coord
  (const DTYPE dimension, const STYPE s, const CTYPE0 coord0[],
   CTYPE1 coord1[])
  {
    for (DTYPE d = 0; d < dimension; d++)
      { coord1[d] = s*coord0[d]; };
  }

  /// Add \a s * \a coord0[] to \a coord1[]).
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] coord2 = Output coordinate.
  ///                      Equals (\a s * \a coord0[] + \a coord1[]).
  template <class DTYPE, class STYPE, 
            class CTYPE0, class CTYPE1, class CTYPE2>
  void add_scaled_coord
  (const DTYPE dimension, const STYPE s, const CTYPE0 coord0[],
   const CTYPE1 coord1[], CTYPE2 coord2[])
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { coord2[d] = s*coord0[d] + coord1[d]; };
  }

  /// Compute the midpoint of two coordinates.
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] midpoint_coord = Coordinates of midpoint.
  template <class DTYPE, class CTYPE0, class CTYPE1, class CTYPE2>
  inline void compute_midpoint
  (const DTYPE dimension, const CTYPE0 coord0[], const CTYPE1 coord1[],
   CTYPE2 midpoint_coord[])
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { midpoint_coord[d] = (coord0[d] + coord1[d])/2.0; };
  }

  /// Compute sum of squares of coordinates.
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param[out] sum = sum of squares
  template <class DTYPE, class CTYPE0, class STYPE>
  void compute_sum_of_squares(const DTYPE dimension, const CTYPE0 coord0[],
                              STYPE & sum)
  {
    sum = 0;
    for (DTYPE d = 0; d < dimension; d++) 
      { sum = sum + coord0[d]*coord0[d]; }
  }

  /// Compute magnitude of coordinate vector.
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param[out] magnitude = magnitude
  template <class DTYPE, class CTYPE0, class STYPE>
  void compute_magnitude(const DTYPE dimension, const CTYPE0 coord0[],
                         STYPE & magnitude)
  {
    compute_sum_of_squares(dimension, coord0, magnitude);
    magnitude = std::sqrt(magnitude);
  }

  // **************************************************
  // Distance operations
  // **************************************************

  /// Compute square of distance between two points.
  template <class DTYPE, class CTYPE0, class CTYPE1, class DIST_TYPE>
  void compute_distance_squared
  (const DTYPE dimension, const CTYPE0 coord0[], const CTYPE1 coord1[],
   DIST_TYPE & distance_squared)
  {
    distance_squared = 0.0;
    for (DTYPE d = 0; d < dimension; d++) {
      CTYPE0 diff = coord0[d] - coord1[d];
      distance_squared += diff*diff;
    }
  }

  /// Compute distance between two points.
  template <class DTYPE, class CTYPE0, class CTYPE1, class DIST_TYPE>
  void compute_distance
  (const DTYPE dimension, const CTYPE0 coord0[], const CTYPE1 coord1[],
   DIST_TYPE & distance)
  {
    compute_distance_squared(dimension, coord0, coord1, distance);
    distance = std::sqrt(distance);
  }

  // **************************************************
  // Vector operations
  // **************************************************

  /// Compute inner product of \a coord0[] and \a coord1[].
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] product = Inner product.
  template <class DTYPE, class CTYPE0, class CTYPE1, class STYPE>
  void compute_inner_product(const DTYPE dimension, const CTYPE0 coord0[],
                             const CTYPE1 coord1[], STYPE & product)
  {
    product = 0;
    for (DTYPE d = 0; d < dimension; d++) 
      { product = product + coord0[d]*coord1[d]; }
  }


  /// Project vector v onto direction dir.
  /// @pre dir is a unit vector.
  template <typename DTYPE, typename VTYPE0, typename VTYPE1,
            typename VTYPE2>
  void project_vector(const DTYPE dimension, const VTYPE0 v[], 
                      const VTYPE1 dir[], VTYPE2 v_proj[])
  {
    VTYPE0 inner_product;
    compute_inner_product(dimension, v, dir, inner_product);
    for (DTYPE d = 0; d < dimension; d++) 
      { v_proj[d] = dir[d]*inner_product; }
  }

  /// Compute component of vector orthogonal to given direction
  /// @pre dir is a unit vector.
  template <typename DTYPE, typename VTYPE0, typename VTYPE1,
            typename VTYPE2>
  void compute_orthogonal_vector(const DTYPE dimension, const VTYPE0 v[], 
                                 const VTYPE1 dir[], VTYPE2 v_orth[])
  {
    VTYPE0 inner_product;
    compute_inner_product(dimension, v, dir, inner_product);
    for (DTYPE d = 0; d < dimension; d++) 
      { v_orth[d] = v[d]-dir[d]*inner_product; }
  }

  /// Compute magnitude of coordinate vector.
  /// @param coord0 = Input coordinates.
  /// @param[out] magnitude = magnitude
  template <typename DTYPE, typename CTYPE0, typename MTYPE, 
            typename CTYPE1>
  void normalize_vector
  (const DTYPE dimension, const CTYPE0 coord0[], 
   const MTYPE max_small_magnitude, CTYPE1 coord1[])
  { 
    MTYPE magnitude;

    compute_magnitude(dimension, coord0, magnitude); 
    if (magnitude > max_small_magnitude) {
      multiply_coord(dimension, 1.0/magnitude, coord0, coord1);
    }
    else {
      set_coord(dimension, 0, coord1);
    }

  }

  
  // **************************************************
  // Comparison operators
  // **************************************************

  /// Return true if coordinates are the same.
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  template <class DTYPE, class CTYPE0, class CTYPE1>
  bool is_coord_equal(const DTYPE dimension, 
                      const CTYPE0 coord0[], const CTYPE1 coord1[])
  {
    for (DTYPE d = 0; d < dimension; d++)
      { if (coord0[d] != coord1[d]) { return(false); }; }

    return(true);
  }

  /// Return true if all elements of coord0[] are less than or equal
  ///   to all elements of coord1[].
  template <class DTYPE, class CTYPE0, class CTYPE1>
  bool is_coord_less_than_or_equal
  (const DTYPE dimension, const CTYPE0 coord0, const CTYPE1 coord1)
  {
    for (DTYPE d = 0; d < dimension; d++)
      { if (coord0[d] > coord1[d]) { return(false); }; };

    return(true);
  }

  // **************************************************
  // Rounding operators
  // **************************************************

  /// Round single coordinate
  template <int n, typename T>
  T round_coord(const T x)
  {
    T y = round(n*x);
    return(y/n);
  }

  /// Round single coordinate
  template <typename ITYPE, typename T>
  T round_coord(const ITYPE n, const T x)
  {
    T y = round(n*x);
    return(y/n);
  }

  /// Round coordinates.
  template <int n, typename DTYPE, typename T0, typename T1>
  void round_coord(const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round_coord<n>(v0[d]); }
  }

  /// Round coordinates.
  template <typename ITYPE, typename DTYPE, typename T0, typename T1>
  void round_coord
  (const ITYPE n, const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round_coord(n, v0[d]); }
  }

  // Specialized rounding operators.

  /// Round single coordinate to nearest 1/16'th.
  template <typename T>
  T round16_coord(const T x) { return(round_coord<16>(x)); }

  /// Round single coordinate to nearest 1/32'th.
  template <typename T>
  T round32_coord(const T x) { return(round_coord<32>(x)); }

  /// Round single coordinate to nearest 1/64'th.
  template <typename T>
  T round64_coord(const T x) { return(round_coord<64>(x)); }

  /// Round single coordinate to nearest 1/128'th.
  template <typename T>
  T round128_coord(const T x) { return(round_coord<128>(x)); }

  /// Round single coordinate to nearest 1/256'th.
  template <typename T>
  T round256_coord(const T x) { return(round_coord<256>(x)); }

  /// Round single coordinate to nearest 1/512'th.
  template <typename T>
  T round512_coord(const T x) { return(round_coord<512>(x)); }

  /// Round single coordinate to nearest 1/1024'th.
  template <typename T>
  T round1024_coord(const T x) { return(round_coord<1024>(x)); }

  /// Round coordinates to nearest 1/16'th.
  template <typename DTYPE, typename T0, typename T1>
  void round16_coord(const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round16_coord(v0[d]); }
  }

  /// Round coordinates to nearest 1/32'th.
  template <typename DTYPE, typename T0, typename T1>
  void round32_coord(const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round32_coord(v0[d]); }
  }

  /// Round coordinates to nearest 1/64'th.
  template <typename DTYPE, typename T0, typename T1>
  void round64_coord(const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round64_coord(v0[d]); }
  }

  /// Round coordinates to nearest 1/128'th.
  template <typename DTYPE, typename T0, typename T1>
  void round128_coord(const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round128_coord(v0[d]); }
  }

  /// Round coordinates to nearest 1/256'th.
  template <typename DTYPE, typename T0, typename T1>
  void round256_coord(const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round256_coord(v0[d]); }
  }

  /// Round coordinates to nearest 1/512'th.
  template <typename DTYPE, typename T0, typename T1>
  void round512_coord(const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round512_coord(v0[d]); }
  }

  /// Round coordinates to nearest 1/1024'th.
  template <typename DTYPE, typename T0, typename T1>
  void round1024_coord(const DTYPE dimension, const T0 * v0, T1 * v1)
  {
    for (DTYPE d = 0; d < dimension; d++) 
      { v1[d] = round1024_coord(v0[d]); }
  }
  
  /* \} */

  // **************************************************
  // OLD DEPRECATED Comparison operators
  // **************************************************

  /// DEPRECATED: Replaced by is_coord_equal()
  template <class DTYPE, class CTYPE0, class CTYPE1>
  bool compare_coord(const DTYPE dimension, 
                     const CTYPE0 coord0[], const CTYPE1 coord1[])
  {
    return(is_coord_equal(dimension, coord0, coord1));
  }

  /// DEPRECATED: Replaced by is_coord_less_than_or_equal()
  template <class DTYPE, class CTYPE0, class CTYPE1>
  bool is_less_than_or_equal_to
  (const DTYPE dimension, const CTYPE0 coord0, const CTYPE1 coord1)
  {
    return(is_coord_less_than_or_equal(dimension, coord0, coord1));
  }

  // **************************************************
  // 3D versions
  // **************************************************

  ///
  /// \defgroup coordinates3D 3D Coordinate Operations
  /* \ingroup coordinates3D */
  /* \{ */

  /// Set all vertex coordinates to \a c.
  /// @param c = Scalar constant.  Set vertex coordinates to \a c.
  /// @param[out] coord[] = Output coordinates.
  template <class STYPE, class CTYPE>
  void set_coord_3D(const STYPE c, CTYPE coord[])
  { 
    const int DIM3 = 3;
    set_coord(DIM3, c, coord); 
  }

  /// Copy \a coord0[] to \a coord1[].
  /// @param coord0 = Input coordinates.
  /// @param[out] coord1 = Output coordinates.
  template <class CTYPE0, class CTYPE1>
  void copy_coord_3D(const CTYPE0 coord0[], CTYPE1 coord1[])
  { 
    const int DIM3 = 3;
    copy_coord(DIM3, coord0, coord1); 
  }

  /// Add \a coord0[] to \a coord1[].
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] coord2 = Output coordinate equal to (\a coord0[] + \a coord1[]).
  template <class CTYPE0, class CTYPE1, class CTYPE2>
  void add_coord_3D(const CTYPE0 coord0[],
                    const CTYPE1 coord1[], CTYPE2 coord2[])
  { 
    const int DIM3 = 3;
    add_coord(DIM3, coord0, coord1, coord2); 
  }

  /// Subtract \a coord1[] from \a coord0[].
  /// @param dimension = Coordinate dimension (= number of coordinates.)
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] coord2 = Output coordinate equal to (\a coord1[] - \a coord1[]).
  template <class CTYPE0, class CTYPE1, class CTYPE2>
  inline void subtract_coord_3D
  (const CTYPE0 coord0[], const CTYPE1 coord1[], CTYPE2 coord2[])
  { 
    const int DIM3 = 3;
    subtract_coord(DIM3, coord0, coord1, coord2); 
  }

  /// Multiply \a coord0[] by the scalar \a s.
  template <class STYPE, class CTYPE0, class CTYPE1>
  inline void multiply_coord_3D
  (const STYPE s, const CTYPE0 coord0[], CTYPE1 coord1[])
  { 
    const int DIM3 = 3;
    multiply_coord(DIM3, s, coord0, coord1); 
  }

  /// Add \a s * \a coord0[] to \a coord1[]).
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] coord2 = Output coordinate.
  ///                      Equals (\a s * \a coord0[] + \a coord1[]).
  template <class STYPE, class CTYPE0, class CTYPE1, class CTYPE2>
  void add_scaled_coord_3D
  (const STYPE s, 
   const CTYPE0 coord0[], const CTYPE1 coord1[], CTYPE2 coord2[])
  {
    const int DIM3 = 3;
    add_scaled_coord(DIM3, s, coord0, coord1, coord2);
  }

  /// Compute the midpoint of two coordinates.
  template <class CTYPE0, class CTYPE1, class CTYPE2>
  inline void compute_midpoint_3D
  (const CTYPE0 coord0[], const CTYPE1 coord1[], CTYPE2 midpoint_coord[])
  { 
    const int DIM3 = 3;
    compute_midpoint(DIM3, coord0, coord1, midpoint_coord); 
  }

  /// Compute inner product of \a coord0[] and \a coord1[].
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  /// @param[out] product = Inner product.
  template <class CTYPE0, class CTYPE1, class STYPE>
  void compute_inner_product_3D
  (const CTYPE0 coord0[], const CTYPE1 coord1[], STYPE & product)
  { 
    const int DIM3 = 3;
    compute_inner_product(DIM3, coord0, coord1, product); 
  }

  /// Compute sum of squares of coordinates.
  template <class CTYPE0, class STYPE>
  void compute_sum_of_squares_3D(const CTYPE0 coord0[], STYPE & sum)
  { 
    const int DIM3 = 3;
    compute_sum_of_squares(DIM3, coord0, sum); 
  }

  /// Compute magnitude of coordinate vector.
  /// @param coord0 = Input coordinates.
  /// @param[out] magnitude = magnitude
  template <class CTYPE0, class STYPE>
  void compute_magnitude_3D(const CTYPE0 coord0[], STYPE & magnitude)
  { 
    const int DIM3 = 3;
    compute_magnitude(DIM3, coord0, magnitude); 
  }

  /// Return true if coordinates are the same.
  /// @param coord0 = Input coordinates.
  /// @param coord1 = Input coordinates.
  template <class CTYPE0, class CTYPE1>
  bool is_coord_equal_3D(const CTYPE0 coord0[], const CTYPE1 coord1[])
  { 
    const int DIM3 = 3;
    return(is_coord_equal(DIM3, coord0, coord1)); 
  }

  /// Return true if all elements of coord0[] are less than or equal
  ///   to all elements of coord1[].
  template <class CTYPE0, class CTYPE1>
  bool is_coord_less_than_or_equal_3D
  (const CTYPE0 coord0, const CTYPE1 coord1)
  { 
    const int DIM3 = 3;
    return(is_coord_less_than_or_equal(DIM3, coord0, coord1)); 
  }

  /* \} */

}

#endif
