/// \file ijkmesh.txx
/// ijk templates for handling polyhedral meshes
/// Version 0.1.0

/*
  IJK: Isosurface Jeneration Kode
  Copyright (C) 2010,2012 Rephael Wenger

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

#ifndef _IJKMESH_
#define _IJKMESH_

#include "ijk.txx"
#include "ijklist.txx"

#include <algorithm>
#include <vector>

namespace IJK {

  // **************************************************
  // FUNCTION CLASS FOR TUPLE COMPARISON
  // **************************************************

  /// comparison function template
  template <class DTYPE, class T> class TUPLE_LESS_THAN {
  protected:
    const DTYPE dimension;
    const T * array;

  public:
    TUPLE_LESS_THAN(const DTYPE d, const T * a):
      dimension(d), array(a) {};
  

    bool operator ()(const int i0, const int i1) const
    { 
      const T * p0 = i0*dimension+array;
      const T * p1 = i1*dimension+array;

      return(std::lexicographical_compare(p0, p0+dimension, p1, p1+dimension)); 
    };
  };

  // **************************************************
  // COMPUTE SIMPLEX FACETS
  // **************************************************

  /// Compute simplex facets
  template <typename DTYPE, typename SVERT, typename NTYPE,
            typename FVERT, typename FACET_INDEX>
  void compute_simplex_facets
  (const DTYPE simplex_dimension, 
   const SVERT * simplex_vertex, const NTYPE num_simplices,
   std::vector<FVERT> & facet_vertex, 
   std::vector<FACET_INDEX> & simplex_facet)
  {
    const NTYPE numv_per_simplex = simplex_dimension+1;
    const NTYPE num_facets_per_simplex = simplex_dimension+1;
    const NTYPE numv_per_facet =  numv_per_simplex-1;
    const NTYPE total_num_facets = num_simplices*num_facets_per_simplex;
    IJK::PROCEDURE_ERROR error("compute_simplex_facets");

    facet_vertex.clear();
    simplex_facet.clear();

    if (num_simplices == 0) { return; };
    if (num_facets_per_simplex < 1) { return; }

    IJK::ARRAY<FVERT> facet_vertex2(total_num_facets*numv_per_facet);

    // store facets in facet_vertex2[]
    NTYPE m = 0;
    for (NTYPE i = 0; i < num_simplices; i++) {
      const SVERT * firstv = simplex_vertex + i*numv_per_simplex;
      for (NTYPE j = 0; j < num_facets_per_simplex; j++) {
        for (NTYPE k = 0; k < numv_per_simplex; k++) {
          if (j != k) {
            facet_vertex2[m] = firstv[k];
            m++;
          }
        }
      }
    }

    if (m != total_num_facets*numv_per_facet) {
      error.AddMessage
        ("Programming error.  Incorrect number of vertices in facet_vertex2[].");
      throw error;
    }

    // Sort facet vertices
    for (NTYPE i = 0; i < total_num_facets; i++) {
      SVERT * firstv = facet_vertex2.Ptr() + i*numv_per_facet;
      std::sort(firstv, firstv+numv_per_facet);
    }

    // Sort facets
    IJK::ARRAY<NTYPE> index_sorted(total_num_facets);
    for (NTYPE i = 0; i < total_num_facets; i++) 
      { index_sorted[i] = i; }

    TUPLE_LESS_THAN<DTYPE, NTYPE> facet_less_than
      (numv_per_facet, facet_vertex2.PtrConst());
    sort(index_sorted.Ptr(), index_sorted.Ptr()+total_num_facets, 
         facet_less_than);

    // Count number of non-duplicate facets
    simplex_facet.resize(total_num_facets);
    NTYPE num_non_duplicates = 1;
    simplex_facet[0] = 0;
    for (NTYPE i = 1; i < total_num_facets; i++) {
      NTYPE j = index_sorted[i-1];
      NTYPE k = index_sorted[i];
      const FVERT * firstv_j = facet_vertex2.PtrConst()+j*numv_per_facet;
      const FVERT * firstv_k = facet_vertex2.PtrConst()+k*numv_per_facet;

      if (!equal(firstv_j, firstv_j+numv_per_facet, firstv_k))
        { num_non_duplicates++; };

      simplex_facet[k] = num_non_duplicates-1;
    }

    facet_vertex.resize(num_non_duplicates*numv_per_facet);

    // Copy first facet from facet_vertex2[] into facet_vertex[]
    FVERT * fvert = &(facet_vertex[0]);
    FVERT * fvert2 = facet_vertex2.Ptr()+index_sorted[0]*numv_per_facet;
    std::copy(fvert2, fvert2+numv_per_facet, fvert);
    fvert += numv_per_facet;

    // Copy facet_vertex2[] into facet_vertex[], avoiding duplicates
    for (NTYPE i = 1; i < total_num_facets; i++) {
      NTYPE j = index_sorted[i-1];
      NTYPE k = index_sorted[i];
      if (simplex_facet[k] != simplex_facet[j]) {
        // Copy simplex_facet[k] into facet_vertex[]
        SVERT * fvert2 = facet_vertex2.Ptr() + k*numv_per_facet;
        std::copy(fvert2, fvert2+numv_per_facet, fvert);

        fvert += numv_per_facet;
      }
    }

    if (fvert != &(facet_vertex[0]) + num_non_duplicates*numv_per_facet) {
      error.AddMessage("Programming error.  Incorrect number of vertices in facet_vertex[].");
      throw error;
    }
  }

  // **************************************************
  // REMOVE UNREFERENCED VERTICES
  // **************************************************

  /// Set flag[x] to true for every x in list.
  template <bool bool_value, typename ETYPE, typename NTYPE>
  void set_bool_flag
  (const ETYPE * list, const NTYPE list_length, bool * flag)
  {
    for (NTYPE i = 0; i < list_length; i++)
      { flag[list[i]] = bool_value; }
  }

  /// Copy coord0 to coord1, removing coordinates for any vertices iv 
  ///   where flag[iv] is false.
  template <typename DTYPE, typename CTYPE0, typename CTYPE1,
            typename NTYPE0, typename NTYPE1, typename ITYPE>
  void compact_coord_array
  (const DTYPE dimension, const CTYPE0 * coord0, const NTYPE0 numv0,
   const bool * flag, ITYPE * new_index, CTYPE1 * coord1, NTYPE1 & numv1)
  {
    NTYPE0 k = 0;
    for (NTYPE0 iv = 0; iv < numv0; iv++) {
      if (flag[iv]) {
        if (k < iv) {
          const CTYPE0 * coord0_ptr = coord0+dimension*iv;
          CTYPE1 * coord1_ptr = coord1+dimension*k;
          std::copy(coord0_ptr, coord0_ptr+dimension, coord1_ptr);
        }
        new_index[iv] = k;
        k++;
      }
    }

    numv1 = k;
  }

  /// Remove vertex coordinates of vertices which are not in vlist.
  /// Compact coord0[] and relabel remaining vertices.
  /// @param coord0[] Array of vertex coordinates.
  /// @param[out] coord1[] Compacted array of vertex coordinates.
  ///      Unreferenced vertices have been removed.
  ///      coord0[] and coord1[] could be the same array.
  /// @pre Array coord1[] is pre-allocated to length at least numv0*dimension.
  template <typename DTYPE, typename CTYPE0, typename CTYPE1,
            typename NTYPE0, typename NTYPE1,
            typename VTYPE, typename NUMV_TYPE>
  void remove_unreferenced_vertices
  (const DTYPE dimension, const CTYPE0 * coord0, const NTYPE0 numv0,
   VTYPE * vlist, const NUMV_TYPE vlist_length,
   CTYPE1 * coord1, NTYPE1 & numv1)
  {
    // flag_referenced[i] = true if vertex i is referenced.
    IJK::ARRAY<bool> flag_referenced(numv0, false); 

    // new_index[i] = new index of vertex i.
    IJK::ARRAY<VTYPE> new_index(numv0);

    set_bool_flag<true>(vlist, vlist_length, flag_referenced.Ptr());

    compact_coord_array
      (dimension, coord0, numv0, flag_referenced.PtrConst(), new_index.Ptr(),
       coord1, numv1);

    for (NUMV_TYPE j = 0; j < vlist_length; j++)
      { vlist[j] = new_index[vlist[j]]; }
  }

  /// Remove vertex coordinates of vertices which are not in vlist.
  /// Compact coord[] and relabel remaining vertices.
  /// Version which alters coord[] and numv.
  /// @param coord0[] Array of vertex coordinates.
  template <typename DTYPE, typename CTYPE, typename NTYPE,
            typename VTYPE, typename NUMV_TYPE>
  void remove_unreferenced_vertices
  (const DTYPE dimension, CTYPE * coord, NTYPE & numv,
   VTYPE * vlist, const NUMV_TYPE vlist_length)
  {
    NTYPE numv1;
    remove_unreferenced_vertices
      (dimension, coord, numv, vlist, vlist_length, coord, numv1);
    numv = numv1;
  }

  /// Remove vertex coordinates of vertices which are not in vlistA or vlistB.
  /// Compact coord0[] and relabel remaining vertices.
  /// @param coord0[] Array of vertex coordinates.
  /// @param[out] coord1[] Compacted array of vertex coordinates.
  ///      Unreferenced vertices have been removed.
  ///      coord0[] and coord1[] could be the same array.
  /// @pre Array coord1[] is pre-allocated to length at least numv0*dimension.
  template <typename DTYPE, typename CTYPE0, typename CTYPE1,
            typename NTYPE0, typename NTYPE1,
            typename VTYPEA, typename VTYPEB,
            typename NUMV_TYPEA, typename NUMV_TYPEB>
  void remove_unreferenced_vertices_two_lists
  (const DTYPE dimension, const CTYPE0 * coord0, const NTYPE0 numv0,
   VTYPEA * vlistA, const NUMV_TYPEA vlistA_length,
   VTYPEB * vlistB, const NUMV_TYPEB vlistB_length,
   CTYPE1 * coord1, NTYPE1 & numv1)
  {
    // flag_referenced[i] = true if vertex i is referenced.
    IJK::ARRAY<bool> flag_referenced(numv0, false); 

    // new_index[i] = new index of vertex i.
    IJK::ARRAY<NTYPE0> new_index(numv0);

    set_bool_flag<true>(vlistA, vlistA_length, flag_referenced.Ptr());
    set_bool_flag<true>(vlistB, vlistB_length, flag_referenced.Ptr());

    compact_coord_array
      (dimension, coord0, numv0, flag_referenced.PtrConst(), new_index.Ptr(),
       coord1, numv1);

    for (NUMV_TYPEA j = 0; j < vlistA_length; j++)
      { vlistA[j] = new_index[vlistA[j]]; }

    for (NUMV_TYPEB j = 0; j < vlistB_length; j++)
      { vlistB[j] = new_index[vlistB[j]]; }
  }

  /// Remove vertex coordinates of vertices which are not in vlistA or vlistB.
  /// Compact coord[] and relabel remaining vertices.
  /// Version which alters coord[] and numv.
  /// @param coord0[] Array of vertex coordinates.
  template <typename DTYPE, typename CTYPE, typename NTYPE,
            typename VTYPEA, typename VTYPEB,
            typename NUMV_TYPEA, typename NUMV_TYPEB>
  void remove_unreferenced_vertices_two_lists
  (const DTYPE dimension, CTYPE * coord, NTYPE & numv,
   VTYPEA * vlistA, const NUMV_TYPEA vlistA_length,
   VTYPEB * vlistB, const NUMV_TYPEB vlistB_length)
  {
    NTYPE numv1;
    remove_unreferenced_vertices_two_lists
      (dimension, coord, numv, vlistA, vlistA_length, vlistB, vlistB_length,
       coord, numv1);
    numv = numv1;
  }

  /// Remove vertex coordinates of vertices which are not in vlistA or vlistB.
  /// C++ STL vector format for coord, vlistA and vlistB
  template <typename DTYPE, typename CTYPE,
            typename VTYPEA, typename VTYPEB>
  void remove_unreferenced_vertices_two_lists
  (const DTYPE dimension, std::vector<CTYPE> & coord,
   std::vector<VTYPEA> & vlistA, std::vector<VTYPEB> & vlistB)
  {
    typedef typename std::vector<CTYPE>::size_type SIZE_TYPE;

    if (coord.size() == 0) { return; }

    SIZE_TYPE numv = coord.size()/dimension;
    VTYPEA * vlistA_ptr = NULL;
    VTYPEB * vlistB_ptr = NULL;

    if (vlistA.size() > 0) { vlistA_ptr = &(vlistA.front()); }
    if (vlistB.size() > 0) { vlistB_ptr = &(vlistB.front()); }

    remove_unreferenced_vertices_two_lists
      (dimension, &(coord[0]), numv, 
       vlistA_ptr, vlistA.size(), vlistB_ptr, vlistB.size());

    coord.resize(numv*dimension);
  }

  // **************************************************
  // REORDER QUADRILATERALS
  // **************************************************

  /// Reorder quad vertices.
  /// Swap last two vertices.
  /// Changes counter-clockwise order to lower-left, lower-right, 
  ///   upper-left, upper-right order.
  /// Changes lower-left, lower-right, upper-left, upper-right order
  ///   to counter-clockwise order.
  template <typename VTYPE, typename NTYPE>
  void reorder_quad_vertices
  (VTYPE * quad_vert, const NTYPE num_quad)
  {
    const NTYPE NUM_VERT_PER_QUAD = 4;

    for (NTYPE iquad = 0; iquad < num_quad; iquad++) {
      NTYPE k = iquad*NUM_VERT_PER_QUAD;
      std::swap(quad_vert[k+2], quad_vert[k+3]);
    }
  }

  /// Reorder quad vertices.
  /// C++ vector version of quad_vert.
  template <typename VTYPE>
  void reorder_quad_vertices(std::vector<VTYPE> & quad_vert)
  {
    typedef typename std::vector<VTYPE>::size_type SIZE_TYPE;
    const SIZE_TYPE NUM_VERT_PER_QUAD = 4;
    const SIZE_TYPE num_quad = quad_vert.size()/NUM_VERT_PER_QUAD;

    if (num_quad == 0) { return; }

    reorder_quad_vertices(&(quad_vert.front()), num_quad);
  }

  // **************************************************
  // GET NON-DEGENERATE QUADRILATERALS
  // **************************************************

  /// Get a single non-degenerate quadrilateral.
  /// A quadrilateral with a single edge collapse are reported as triangles.
  /// Note: Does not clear new_quad_vert or new_tri_vert.
  /// @param quad_vert Four quadrilateral vertices.
  /// @pre quadrilateral vertices are in counter-clockwise order
  ///        around the quadrilateral.
  template <typename VTYPE1, typename VTYPE2, typename VTYPE3>
  void get_non_degenerate_quad_ccw
  (const VTYPE1 * quad_vert,
   std::vector<VTYPE2> & new_tri_vert,
   std::vector<VTYPE3> & new_quad_vert)
  {
    typedef typename std::vector<VTYPE1>::size_type SIZE_TYPE;
    const SIZE_TYPE NUM_VERT_PER_QUAD = 4;

    for (SIZE_TYPE j1 = 0; j1 < 2; j1++) {
      SIZE_TYPE j2 = j1+2;
      if (quad_vert[j1] == quad_vert[j2]) {
        // Degenerate quad. Collapses to two edges.
        return;
      }
    }

    for (SIZE_TYPE j1 = 0; j1 < NUM_VERT_PER_QUAD; j1++) {
      SIZE_TYPE j2 = (j1+1)%NUM_VERT_PER_QUAD;
      SIZE_TYPE j3 = (j2+1)%NUM_VERT_PER_QUAD;
      SIZE_TYPE j4 = (j3+1)%NUM_VERT_PER_QUAD;

      if (quad_vert[j1] == quad_vert[j2]) {
        if (quad_vert[j3] != quad_vert[j4]) {
          // Single edge collapse.
          new_tri_vert.push_back(quad_vert[j2]);
          new_tri_vert.push_back(quad_vert[j3]);
          new_tri_vert.push_back(quad_vert[j4]);
          return;
        }
        else {
          // Degenerate quad. Collapses to two edges.
          return;
        }
      }
    }

    // Non-degenerate quadrilateral.
    for (SIZE_TYPE j = 0; j < NUM_VERT_PER_QUAD; j++) {
      new_quad_vert.push_back(quad_vert[j]);
    }
  }

  /// Get non-degenerate quadrilaterals.
  /// Quadrilaterals with a single edge collapse are reported as triangles.
  /// Note: Does not clear new_quad_vert or new_tri_vert.
  /// @param quad_vert Array of quadrilateral vertices.
  ///        quad_vert[4*i+j] = j'th vertex of quadrilateral i.
  /// @pre Quadrilateral vertices are in counter-clockwise order 
  ///        around quadrilateral.
  template <typename VTYPE1, typename VTYPE2, typename VTYPE3,
            typename NTYPE>
  void get_non_degenerate_quad_ccw
  (const VTYPE1 * quad_vert, const NTYPE num_quad,
   std::vector<VTYPE2> & new_tri_vert,
   std::vector<VTYPE3> & new_quad_vert)
  {
    const NTYPE NUM_VERT_PER_QUAD = 4;
    NTYPE num_distinct;
    VTYPE1 vlist[NUM_VERT_PER_QUAD];

    for (NTYPE iquad = 0; iquad < num_quad; iquad++) {
      NTYPE k = iquad*NUM_VERT_PER_QUAD;

      IJK::get_non_degenerate_quad_ccw
        (quad_vert+k, new_tri_vert, new_quad_vert);
    }

  }

  /// Get non-degenerate quadrilaterals.
  /// C++ vector version for quad_vert.
  template <typename VTYPE1, typename VTYPE2, typename VTYPE3>
  void get_non_degenerate_quad_ccw
  (const std::vector<VTYPE1> & quad_vert,
   std::vector<VTYPE2> & new_tri_vert,
   std::vector<VTYPE3> & new_quad_vert)
  {
    typedef typename std::vector<VTYPE1>::size_type SIZE_TYPE;

    const SIZE_TYPE NUM_VERT_PER_QUAD = 4;

    SIZE_TYPE num_quad = quad_vert.size()/NUM_VERT_PER_QUAD;

    if (quad_vert.size() == 0) { return; }
    get_non_degenerate_quad_ccw
      (IJK::vector2pointer(quad_vert), num_quad,
       new_tri_vert, new_quad_vert);
  }


  /// Get a single non-degenerate quadrilateral.
  /// A quadrilateral with a single edge collapse is reported as a triangle.
  /// Note: Does not clear new_quad_vert or new_tri_vert.
  /// @param quad_vert Four quadrilateral vertices.
  /// @pre Quadrilateral vertices are ordered bottom-left, bottom-right,
  ///        top-left, top-right.
  template <typename VTYPE1, typename VTYPE2, typename VTYPE3>
  void get_non_degenerate_quad_btlr
  (const VTYPE1 * quad_vert,
   std::vector<VTYPE2> & new_tri_vert,
   std::vector<VTYPE3> & new_quad_vert)
  {
    typedef typename std::vector<VTYPE1>::size_type SIZE_TYPE;
    const SIZE_TYPE NUM_VERT_PER_QUAD = 4;
    static const SIZE_TYPE next_vert_ccw[NUM_VERT_PER_QUAD] = { 1, 3, 0, 2};

    for (SIZE_TYPE j1 = 0; j1 < 2; j1++) {
      SIZE_TYPE j2 = 3-j1;
      if (quad_vert[j1] == quad_vert[j2]) {
        // Degenerate quad. Collapses to two edges.
        return;
      }
    }

    for (SIZE_TYPE j1 = 0; j1 < NUM_VERT_PER_QUAD; j1++) {
      SIZE_TYPE j2 = next_vert_ccw[j1];
      SIZE_TYPE j3 = next_vert_ccw[j2];
      SIZE_TYPE j4 = next_vert_ccw[j3];


      if (quad_vert[j1] == quad_vert[j2]) {
        if (quad_vert[j3] != quad_vert[j4]) {
          // Single edge collapse.
          new_tri_vert.push_back(quad_vert[j2]);
          new_tri_vert.push_back(quad_vert[j3]);
          new_tri_vert.push_back(quad_vert[j4]);
          return;
        }
        else {
          // Degenerate quad. Collapses to two edges.
          return;
        }
      }
    }

    // Non-degenerate quadrilateral.
    for (SIZE_TYPE j = 0; j < NUM_VERT_PER_QUAD; j++) {
      new_quad_vert.push_back(quad_vert[j]);
    }
  }

  /// Get non-degenerate quadrilaterals.
  /// Quadrilaterals with a single edge collapse are reported as triangles.
  /// Note: Does not clear new_quad_vert or new_tri_vert.
  /// @param quad_vert Array of quadrilateral vertices.
  ///        quad_vert[4*i+j] = j'th vertex of quadrilateral i.
  /// @pre Quadrilateral vertices are ordered bottom-left, bottom-right,
  ///        top-left, top-right.
  template <typename VTYPE1, typename VTYPE2, typename VTYPE3,
            typename NTYPE>
  void get_non_degenerate_quad_btlr
  (const VTYPE1 * quad_vert, const NTYPE num_quad,
   std::vector<VTYPE2> & new_tri_vert,
   std::vector<VTYPE3> & new_quad_vert)
  {
    const NTYPE NUM_VERT_PER_QUAD = 4;
    NTYPE num_distinct;
    VTYPE1 vlist[NUM_VERT_PER_QUAD];

    for (NTYPE iquad = 0; iquad < num_quad; iquad++) {
      NTYPE k = iquad*NUM_VERT_PER_QUAD;

      IJK::get_non_degenerate_quad_btlr
        (quad_vert+k, new_tri_vert, new_quad_vert);
    }

  }

  /// Get non-degenerate quadrilaterals.
  /// C++ vector version for quad_vert.
  template <typename VTYPE1, typename VTYPE2, typename VTYPE3>
  void get_non_degenerate_quad_btlr
  (const std::vector<VTYPE1> & quad_vert,
   std::vector<VTYPE2> & new_tri_vert,
   std::vector<VTYPE3> & new_quad_vert)
  {
    typedef typename std::vector<VTYPE1>::size_type SIZE_TYPE;

    const SIZE_TYPE NUM_VERT_PER_QUAD = 4;

    SIZE_TYPE num_quad = quad_vert.size()/NUM_VERT_PER_QUAD;

    if (quad_vert.size() == 0) { return; }
    get_non_degenerate_quad_btlr
      (IJK::vector2pointer(quad_vert), num_quad,
       new_tri_vert, new_quad_vert);
  }

  // **************************************************
  // TRIANGULATE POLYGONS
  // **************************************************

  /// Triangulate a polygon.
  /// Polygon vertices are listed in clockwise or counter-clockwise order
  ///   around the polygon.
  /// Add new triangles to vector tri_vert.
  template <typename NTYPE, typename VTYPE0, typename VTYPE1>
  void triangulate_polygon
  (const NTYPE num_poly_vert, const VTYPE0 * poly_vert, 
   std::vector<VTYPE1> & tri_vert)
  {
    VTYPE0 v0 = poly_vert[0];
    for (NTYPE i = 1; i+1 < num_poly_vert; i++) {
      tri_vert.push_back(v0);
      tri_vert.push_back(poly_vert[i]);
      tri_vert.push_back(poly_vert[i+1]);
    }
  }

  /// Triangulate a polygon using diagonals from vertex poly_vert[index_v0].
  /// Polygon vertices are listed in clockwise or counter-clockwise order
  ///   around the polygon.
  /// Add new triangles to vector tri_vert.
  template <typename NTYPE, 
            typename VTYPE0, typename VTYPE1, typename VTYPE2>
  void triangulate_polygon
  (const NTYPE num_poly_vert, const VTYPE0 * poly_vert,
   const VTYPE1 index_v0,
   std::vector<VTYPE2> & tri_vert)
  {
    VTYPE0 v0 = poly_vert[index_v0];
    NTYPE i1 = (index_v0+1)%num_poly_vert;
    NTYPE i2 = (i1+1)%num_poly_vert;
    while (i2 != index_v0) {
      tri_vert.push_back(v0);
      tri_vert.push_back(poly_vert[i1]);
      tri_vert.push_back(poly_vert[i2]);
      i1 = i2;
      i2 = (i1+1)%num_poly_vert;
    }
  }

  /// Triangulate a set of quadrilaterals.
  /// Quadrilateral vertices are listed in clockwise or counter-clockwise order
  ///   around the polygon.
  /// Add new triangles to vector tri_vert.
  template <typename NTYPE, typename VTYPE0, typename VTYPE1>
  void triangulate_quad
  (const VTYPE0 * quad_vert, const NTYPE num_quad,
   std::vector<VTYPE1> & tri_vert)
  {
    const NTYPE NUM_VERT_PER_QUAD = 4;

    for (NTYPE iquad = 0; iquad < num_quad; iquad++) {

      NTYPE k = iquad*NUM_VERT_PER_QUAD;
      triangulate_polygon(NUM_VERT_PER_QUAD, quad_vert+k, tri_vert);
    }
  }

  /// Convert quadrilaterals to triangles.
  /// C++ STL vector format for quad_vert.
  template <typename VTYPE0, typename VTYPE1>
  void triangulate_quad
  (const std::vector<VTYPE0> quad_vert, std::vector<VTYPE1> & tri_vert)
  {
    typedef typename std::vector<VTYPE0>::size_type SIZE_TYPE;

    const SIZE_TYPE NUM_VERT_PER_QUAD = 4;

    SIZE_TYPE num_quad = quad_vert.size()/NUM_VERT_PER_QUAD;
    triangulate_quad(IJK::vector2pointer(quad_vert), num_quad, tri_vert);
  }

}

#endif
