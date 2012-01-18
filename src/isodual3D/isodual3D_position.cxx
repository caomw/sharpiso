/// \file isodual3D_position.cxx
/// Position dual isosurface vertices

/*
 IJK: Isosurface Jeneration Kode
 Copyright (C) 2011 Rephael Wenger

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

#include "isodual3D_position.h"
//#include "ijkcoord.txx"
#include "ijkinterpolate.txx"

#include "sharpiso_types.h"
#include "sharpiso_feature.h"

#include <iostream>
#include <iomanip>


using namespace ISODUAL3D;
using namespace SHARPISO;
using namespace std;


// **************************************************
// Position routines
// **************************************************

/// Position dual isosurface vertices in cube centers
void ISODUAL3D::position_dual_isovertices_cube_center
(const ISODUAL_GRID & grid,
 const std::vector<ISO_VERTEX_INDEX> & vlist, COORD_TYPE * coord)
{
  const int dimension = grid.Dimension();
  GRID_COORD_TYPE grid_coord[dimension];
  COORD_TYPE unit_cube_center[dimension];
  IJK::PROCEDURE_ERROR error("position_dual_isovertices_center");

  IJK::set_coord(dimension, 0.5, unit_cube_center);

  for (VERTEX_INDEX i = 0; i < vlist.size(); i++) {
    VERTEX_INDEX iv = vlist[i];

    grid.ComputeCoord(iv, grid_coord);
    IJK::add_coord(dimension, grid_coord, unit_cube_center, coord+i*dimension);
  }
}

/// Position dual isosurface vertices in cube centers
void ISODUAL3D::position_dual_isovertices_cube_center
(const ISODUAL_GRID & grid,
 const std::vector<ISO_VERTEX_INDEX> & vlist,
 std::vector<COORD_TYPE> & coord)
{
  const int dimension = grid.Dimension();

  coord.resize(vlist.size()*dimension);
  position_dual_isovertices_cube_center(grid, vlist, &(coord.front()));
}

/// Position dual isosurface vertices in centroid
///   of isosurface-edge intersections
void ISODUAL3D::position_dual_isovertices_centroid
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const SCALAR_TYPE isovalue,
 const std::vector<ISO_VERTEX_INDEX> & vlist, COORD_TYPE * coord)
{
  const int dimension = scalar_grid.Dimension();

  for (VERTEX_INDEX i = 0; i < vlist.size(); i++) {
    VERTEX_INDEX iv = vlist[i];

    compute_isosurface_grid_edge_centroid
    (scalar_grid, isovalue, iv, coord+i*dimension);
  }
}


/// Position dual isosurface vertices in centroid
///   of isosurface-edge intersections
void ISODUAL3D::position_dual_isovertices_centroid
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const SCALAR_TYPE isovalue,
 const std::vector<ISO_VERTEX_INDEX> & vlist,
 std::vector<COORD_TYPE> & coord)

{
  const int dimension = scalar_grid.Dimension();

  coord.resize(vlist.size()*dimension);
  position_dual_isovertices_centroid
  (scalar_grid, isovalue, vlist, &(coord.front()));
}


/// Position dual isosurface vertices using gradients
void ISODUAL3D::position_dual_isovertices_using_gradients
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const SCALAR_TYPE isovalue,
 const std::vector<ISO_VERTEX_INDEX> & vlist, COORD_TYPE * coord)
{
  const int dimension = scalar_grid.Dimension();

  for (VERTEX_INDEX i = 0; i < vlist.size(); i++) {
    VERTEX_INDEX iv = vlist[i];

    compute_dual_isovertex_using_gradients
    (scalar_grid, gradient_grid, isovalue, iv, coord+i*dimension);
  }
}

/// Position dual isosurface vertices using gradients
void ISODUAL3D::position_dual_isovertices_using_gradients
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const SCALAR_TYPE isovalue,
 const std::vector<ISO_VERTEX_INDEX> & vlist,
 std::vector<COORD_TYPE> & coord)

{
  const int dimension = scalar_grid.Dimension();

  coord.resize(vlist.size()*dimension);
  position_dual_isovertices_using_gradients
  (scalar_grid, gradient_grid, isovalue, vlist, &(coord.front()));
}


// **************************************************
// Compute routines
// **************************************************

/// Compute centroid of intersections of isosurface and grid edges
void ISODUAL3D::compute_isosurface_grid_edge_centroid
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const SCALAR_TYPE isovalue, const VERTEX_INDEX iv,
 COORD_TYPE * coord)
{
  const int dimension = scalar_grid.Dimension();
  GRID_COORD_TYPE grid_coord[dimension];
  COORD_TYPE vcoord[dimension];
  COORD_TYPE coord0[dimension];
  COORD_TYPE coord1[dimension];
  COORD_TYPE coord2[dimension];

  IJK::PROCEDURE_ERROR error("compute_isosurface_grid_edge_centroid");

  int num_intersected_edges = 0;
  IJK::set_coord(dimension, 0.0, vcoord);

  for (int edge_dir = 0; edge_dir < dimension; edge_dir++)
    for (int k = 0; k < scalar_grid.NumFacetVertices(); k++) {
      VERTEX_INDEX iend0 = scalar_grid.FacetVertex(iv, edge_dir, k);
      VERTEX_INDEX iend1 = scalar_grid.NextVertex(iend0, edge_dir);

      SCALAR_TYPE s0 = scalar_grid.Scalar(iend0);
      bool is_end0_positive = true;
      if (s0 < isovalue)
        { is_end0_positive = false; };

      SCALAR_TYPE s1 = scalar_grid.Scalar(iend1);
      bool is_end1_positive = true;
      if (s1 < isovalue)
        { is_end1_positive = false; };

      if (is_end0_positive != is_end1_positive) {

        scalar_grid.ComputeCoord(iend0, coord0);
        scalar_grid.ComputeCoord(iend1, coord1);

        IJK::linear_interpolate_coord
        (dimension, s0, coord0, s1, coord1, isovalue, coord2);

        IJK::add_coord(dimension, vcoord, coord2, vcoord);

        num_intersected_edges++;
      }
    }

  if (num_intersected_edges > 0) {
    IJK::multiply_coord
    (dimension, 1.0/num_intersected_edges, vcoord, vcoord);
  }
  else {
    scalar_grid.ComputeCoord(iv, vcoord);
    for (int d = 0; d < dimension; d++)
      { vcoord[iv] += 0.5; };
  }

  IJK::copy_coord(dimension, vcoord, coord);
}


/* DEBUG
/// Compute dual isosurface vertex using gradients
void ISODUAL3D::compute_dual_isovertex_using_gradients
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const SCALAR_TYPE isovalue, const VERTEX_INDEX iv,
 COORD_TYPE * coord)
{

//debug change this.
  const int dimension = scalar_grid.Dimension();
  COORD_TYPE coord0[dimension];

  vector<double> scalar_values;
  vector<double> gradient_values;
  vector<double> positions;
  //create the cube
  parameters param;

  param.error = 0.15;
  param.use_gradients_flag = false;

  double grads[3];
  //cout<<"iv "<<iv;
  cout.setf(ios::fixed);
  cout <<setprecision(3);

  for(int i=0; i<scalar_grid.NumCubeVertices(); i++)
    {
      VERTEX_INDEX jv = scalar_grid.CubeVertex(iv, i);
      scalar_values.push_back(scalar_grid.Scalar(jv));
      copy(gradient_grid.VectorPtrConst(jv), gradient_grid.VectorPtrConst(jv) + dimension,grads);
      scalar_grid.ComputeCoord(jv, coord0);
      for (int k=0; k<3; k++) {
        gradient_values.push_back(gradient_grid.Vector(jv, k));
        positions.push_back(coord0[k]);
      }
    }

  //set up each cube.
  cube cb(scalar_values, gradient_values, positions);
  cb.num_points = 8;
  cb.num_edges = 12;
  cb.dim =3;
  cb.points[0].print_point();

  //set edge intersects.
  cb.set_edge_intersects_isosurface(isovalue);

  //interpolate
	cb.interpolate(double(isovalue), false);

  //find the sharp point
	cb.FindSharpPoint();

  scalar_grid.ComputeCoord(iv, coord0);
  //set the coordinate.
  for (int i=0; i<cb.dim; i++) {
    coord[i]= coord0[i] + cb.cbintersect.position[i];
  }

}
*/
/// Compute dual isosurface vertex using gradients
void ISODUAL3D::compute_dual_isovertex_using_gradients
(const ISODUAL_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const SCALAR_TYPE isovalue, const VERTEX_INDEX iv,
 COORD_TYPE * sharp_coord)
{

//set up svd info
  SVD_INFO svd_debug_info;
  svd_debug_info.ray_intersect_cube = false;
  svd_debug_info.location = NONE;

  EIGENVALUE_TYPE eigenvalues[DIM3];

  GRADIENT_COORD_TYPE max_small_mag(0.0);
  EIGENVALUE_TYPE max_small_eigenvalue(0.1);
  VERTEX_INDEX cube_index(0);
  VERTEX_INDEX num_large_eigenvalues;

	svd_compute_sharp_vertex_in_cube
        (scalar_grid, gradient_grid, iv, isovalue,
         max_small_mag, max_small_eigenvalue, sharp_coord, eigenvalues,
         num_large_eigenvalues, svd_debug_info);
}