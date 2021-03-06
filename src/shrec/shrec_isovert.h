/// \file shrec_isovert.h
/// Data structures for creating and processing sharp isosurface vertices.

/*
Copyright (C) 2012-2015 Arindam Bhattacharya and Rephael Wenger

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
(LGPL) as published by the Free Software Foundation; either
version 2.1 of the License, or any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _SHREC_ISOVERT_H_
#define _SHREC_ISOVERT_H_

#include "ijkdualtable.h"

#include "sharpiso_array.txx"
#include "sharpiso_grids.h"
#include "sharpiso_feature.h"
#include "shrec_types.h"

#include <vector>

namespace SHREC {

// **************************************************
// TYPES
// **************************************************

typedef SHARPISO::FIXED_ARRAY
  <SHARPISO::NUM_CUBE_NEIGHBORS3D, SHARPISO::VERTEX_INDEX, SHARPISO::NUM_TYPE>
  CUBE_CONNECTED_ARRAY;


// **************************************************
// GRID CUBE DATA
// **************************************************

typedef enum {
	AVAILABLE_GCUBE,       ///< Cube is available for selection.
	SELECTED_GCUBE,        ///< Cube contains a sharp vertex.
	COVERED_A_GCUBE,       ///< Cube is near a cube containing a sharp vertex.
	COVERED_B_GCUBE,	     ///< Cube is covered by extended mapping.
	COVERED_CORNER_GCUBE,  ///< Covers a cube with >3 eigen value
	COVERED_POINT,         ///< The sharp vertex is in a covered cube.
	UNAVAILABLE_GCUBE,     ///< Cube is within 3x3 of a 2 covering.
	NON_DISK_GCUBE,        ///< Merging cube creates non-disk patch.
	SMOOTH_GCUBE		       ///< Cube contains smooth isosurface patch.
} GRID_CUBE_FLAG;


class GRID_CUBE_DATA {

protected:

  void Init();

public:
  GRID_COORD_TYPE cube_coord[DIM3];  ///< Cube coordinates (unscaled).
  COORD_TYPE isovert_coord[DIM3];    ///< Location of the sharp isovertex.
  COORD_TYPE isovert_coordB[DIM3];   ///< Substitute location.
  unsigned char num_eigenvalues;     ///< Number of eigenvalues.
  GRID_CUBE_FLAG flag;               ///< Type for this cube.
  BOUNDARY_BITS_TYPE boundary_bits;  ///< Boundary bits for the cube
  VERTEX_INDEX cube_index;           ///< Index of cube in scalar grid.

  /// Type of cube cover.
  CUBE_ADJACENCY_TYPE cover_type;

  /// If num_eigenvalues == 2, then direction = direction of isosurface edge.
  /// If num_eigenvalues == 1, then 
  ///   direction = direction orthogonal to isosurface.
  COORD_TYPE direction[DIM3];         

  /// Linf-dist from isovert_coord[] to cube-center.
  COORD_TYPE linf_dist;

  /// L1-dist from isovert_coord[] to cube.
  COORD_TYPE L1_dist_to_cube;

  /// If true, location is centroid of (grid edge)-isosurface intersections.
  bool flag_centroid_location;

  /// If true, some other non-empty cube contains the isovert coord.
  bool flag_conflict;

  /// If true, cube is near corner cube.
  bool flag_near_corner;

  /// Grid index of cube containing the isovert_coord.
  VERTEX_INDEX cube_containing_isovert;

  /// If true, isovert_coord[] determined by an adjacent cube.
  bool flag_coord_from_other_cube;

  /// If true, isovert_coord[] determined by a grid vertex.
  bool flag_coord_from_vertex;

  /// If true, isovert_coord[] determined by a grid edge.
  bool flag_coord_from_edge;

  /// If true, using replacement coordinate.
  bool flag_using_substitute_coord;

  /// If true, coordinates have been recomputed.
  bool flag_recomputed_coord;

  /// If true, coordinates have been recomputed
  ///   with min gradient cube offset.
  bool flag_recomputed_coord_min_offset;

  /// If true, location is recomputed from location of adjacent vertices.
  bool flag_recomputed_using_adjacent;

  /// If true, svd coord were farther than max_dist.
  bool flag_far;

  /// If true, cube is selected despite mismatch.
  bool flag_ignore_mismatch;

  /// Index of cube configuration is isosurface lookup table.
  IJKDUALTABLE::TABLE_INDEX table_index;

  /// Grid index of cube which covered this cube.
  VERTEX_INDEX covered_by;

  /// Grid index of cube which this cube maps to.
  /// Currently, only used for output information.
  VERTEX_INDEX maps_to_cube;

  /// Return true if cube is covered or selected.
  bool IsCoveredOrSelected() const;

  GRID_CUBE_DATA() { Init(); }
};

typedef std::vector<GRID_CUBE_DATA> GRID_CUBE_DATA_ARRAY;


// **************************************************
// GCUBE_COMPARE
// **************************************************

class GCUBE_COMPARE {

public:
  const std::vector<GRID_CUBE_DATA> * gcube_list;

  GCUBE_COMPARE(const std::vector<GRID_CUBE_DATA> & gcube_list)
  { this->gcube_list = &gcube_list; };

  bool operator () (int i,int j)
  {
    int num_eigen_i = gcube_list->at(i).num_eigenvalues;
    int num_eigen_j = gcube_list->at(j).num_eigenvalues;

    if (num_eigen_i == num_eigen_j) {

      COORD_TYPE d_i = 
        gcube_list->at(i).linf_dist + gcube_list->at(i).L1_dist_to_cube;
      COORD_TYPE d_j = 
        gcube_list->at(j).linf_dist + gcube_list->at(j).L1_dist_to_cube;

      return ((d_i < d_j));
    }
    else {
      return ((num_eigen_i > num_eigen_j));
    }
  }
};


// **************************************************
// ISOSURFACE VERTEX DATA
// **************************************************

class ISOVERT {
public:

	/// gcube_list containing the active cubes and their vertices.
	std::vector<GRID_CUBE_DATA> gcube_list;

	static const int NO_INDEX = -1;       ///< Flag for no index.

	/// Grid containing the index to the gcube_list.
	/// If cube is not active, then it is defined as NO_INDEX.
	SHARPISO_INDEX_GRID index_grid;

  /// Grid neighbor information.
  SHARPISO_GRID_NEIGHBORS grid;

  /// Return true if cube is active.
	bool isActive(const int cube_index) const;

  /// Return true if cube flag equals flag.
	bool isFlag(const int cube_index, GRID_CUBE_FLAG flag) const; 

  /// Return cube index.
  VERTEX_INDEX CubeIndex(const int gcube_index) const
  { return(gcube_list[gcube_index].cube_index); }

  /// Return gcube index or NO_INDEX.
  INDEX_DIFF_TYPE GCubeIndex(const int cube_index) const
  { return(index_grid.Scalar(cube_index)); }

  /// Return gcube index or NO_INDEX.
  /// Set error message if cube not active (gcube_index = NO_INDEX).
  INDEX_DIFF_TYPE GCubeIndex(const int cube_index, IJK::ERROR & error) const;

  /// Return number of eigenvalues.
  NUM_TYPE NumEigenvalues(const int gcube_index) const
  { return(gcube_list[gcube_index].num_eigenvalues); }

  /// Return pointer to isovert_coord[].
  const COORD_TYPE * IsoVertCoord(const int gcube_index) const
  { return(gcube_list[gcube_index].isovert_coord); }
};


// **************************************************
// MERGE PARAMETERS
// **************************************************

/// dual contouring parameters
class MERGE_PARAM:public SHARPISO::SHARP_ISOVERT_PARAM {

protected:
  
  /// Minimum triangle angle.
  ANGLE_TYPE min_triangle_angle;          

  /// Cos minimum triangle angle.
  COORD_TYPE cos_min_triangle_angle;

  /// Minimum sharp cube triangle angle.
  ANGLE_TYPE min_sharp_cube_triangle_angle;

  /// Cos minimum triangle angle.
  COORD_TYPE cos_min_sharp_cube_triangle_angle;

  /// Minimum angle between original and new normal.
  ANGLE_TYPE min_normal_angle;

  /// Cos minimum angle between original and new normal.
  COORD_TYPE cos_min_normal_angle;

  /// Attempt to collapse triangles with angles less than collapse angle.
  ANGLE_TYPE collapse_angle;

  /// Cos collapse_angle.
  COORD_TYPE cos_collapse_angle;

public:

  /// if true, attempt to collapse triangles with small angles.
  bool flag_collapse_triangles_with_small_angles;

protected:
  void Init();

public:
  
  MERGE_PARAM() { Init(); };

  /// Set minimum triangle angle.
  void SetMinTriangleAngle(const ANGLE_TYPE degrees);

  /// Set minimum sharp cube triangle angle.
  void SetMinSharpCubeTriangleAngle(const ANGLE_TYPE degrees);

  /// Set minimum normal angle.
  void SetMinNormalAngle(const ANGLE_TYPE degrees);

  /// Set collapse angle
  void SetCollapseAngle(const ANGLE_TYPE degrees);


  // Get functions
  ANGLE_TYPE MinTriangleAngle() const
  { return(min_triangle_angle); }
  COORD_TYPE CosMinTriangleAngle() const
  { return(cos_min_triangle_angle); }
  ANGLE_TYPE MinSharpCubeTriangleAngle() const
  { return(min_sharp_cube_triangle_angle); }
  COORD_TYPE CosMinSharpCubeTriangleAngle() const
  { return(cos_min_sharp_cube_triangle_angle); }
  ANGLE_TYPE MinNormalAngle() const
  { return(min_normal_angle); }
  COORD_TYPE CosMinNormalAngle() const
  { return(cos_min_normal_angle); }
  ANGLE_TYPE CollapseAngle() const
  { return(collapse_angle); }
  COORD_TYPE CosCollapseAngle() const
  { return(cos_collapse_angle); }
};


// **************************************************
// ISOVERT INFO
// **************************************************

class ISOVERT_INFO {

public:

	int num_sharp_corners;
	int num_sharp_edges;
	int num_smooth_vertices;
  int num_merged_iso_vertices;
  int num_conflicts;
  int num_Linf_iso_vertex_locations;

  void Clear();                          ///< Clear all values.

  /// Constructor.
  ISOVERT_INFO()
  { Clear(); };

  /// Set.
  void Set(const ISOVERT_INFO & info)
  { *this = info; }
};


// **************************************************
// ROUTINES
// **************************************************

/// Compute dual isosurface vertices.
void compute_dual_isovert
  (const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
   const GRADIENT_GRID_BASE & gradient_grid,
   const SCALAR_TYPE isovalue,
   const SHARP_ISOVERT_PARAM & isovert_param,
   const VERTEX_POSITION_METHOD vertex_position_method,
   ISOVERT & isovert);

/// Compute dual isosurface vertices.
void compute_dual_isovert
  (const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
   const std::vector<COORD_TYPE> & edgeI_coord,
   const std::vector<GRADIENT_COORD_TYPE> & edgeI_normal_coord,
   const SCALAR_TYPE isovalue,
   const SHARP_ISOVERT_PARAM & isovert_param,
   ISOVERT & isovert);

/// Recompute isosurface vertex positions for cubes 
///   which are not selected or covered.
/// also takes isovert_info as parameter
void recompute_isovert_positions(
	const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
	const GRADIENT_GRID_BASE & gradient_grid,
	const SCALAR_TYPE isovalue,
	const SHARP_ISOVERT_PARAM & isovert_param,
	ISOVERT & isovertData);


/// Recompute isosurface vertex positions for cubes 
///   which are not selected or covered.
void recompute_isovert_positions(
    const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
		const GRADIENT_GRID_BASE & gradient_grid,
		const SCALAR_TYPE isovalue,
		const SHARP_ISOVERT_PARAM & isovert_param,
    ISOVERT & isovertData);


/// Recompute isosurface vertex positions for cubes 
///   which are not selected or covered.
/// Version for hermite data.
void recompute_isovert_positions
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
 const std::vector<COORD_TYPE> & edgeI_coord,
 const SCALAR_TYPE isovalue,
 const SHARP_ISOVERT_PARAM & isovert_param,
 ISOVERT & isovert);

/// Recompute isovert positions for cubes containing covered points.
void recompute_covered_point_positions
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const SHARPISO_BOOL_GRID & covered_grid,
 const SCALAR_TYPE isovalue,
 const SHARP_ISOVERT_PARAM & isovert_param,
 ISOVERT & isovert);

/// Recompute isovert positions for cubes containing covered points.
/// Version which examines only cubes in gcube_index_list[].
/// @param gcube_index_list List of gcube indices.
void recompute_covered_point_positions
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const SHARPISO_BOOL_GRID & covered_grid,
 const SCALAR_TYPE isovalue,
 const std::vector<NUM_TYPE> gcube_index_list,
 const SHARP_ISOVERT_PARAM & isovert_param,
 ISOVERT & isovert);

/// Recompute isovert positions for cubes containing covered points.
/// Use voxel for gradient cube offset, 
///   not isovert_param.grad_selection_cube_offset.
/// @param flag_min_offset If true, voxel uses minimum gradient cube offset.
void recompute_covered_point_positions
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const SHARPISO_BOOL_GRID & covered_grid,
 const SCALAR_TYPE isovalue,
 const SHARP_ISOVERT_PARAM & isovert_param,
 const OFFSET_VOXEL & voxel,
 const bool flag_min_offset,
 ISOVERT & isovert);

/// Recompute isovert positions for cubes containing covered points.
/// Use voxel for gradient cube offset, 
///   not isovert_param.grad_selection_cube_offset.
/// Version which examines only cubes in gcube_index_list[].
/// @param gcube_index_list List of gcube indices.
/// @param flag_min_offset If true, voxel uses minimum gradient cube offset.
void recompute_covered_point_positions
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const SHARPISO_BOOL_GRID & covered_grid,
 const SCALAR_TYPE isovalue,
 const std::vector<NUM_TYPE> gcube_index_list,
 const SHARP_ISOVERT_PARAM & isovert_param,
 const OFFSET_VOXEL & voxel,
 const bool flag_min_offset,
 ISOVERT & isovert);

/// Recompute using adjacent isosurface vertex locations.
void recompute_using_adjacent
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid, ISOVERT & isovert);

/// Recompute isovert in cube cube_index
///   using adjacent isosurface vertex locations.
void recompute_using_adjacent
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid, 
 const VERTEX_INDEX cube_index, ISOVERT & isovert,
 bool & flag_loc_recomputed);

/// Set cover type of all covered cubes.
void set_cover_type(ISOVERT & isovert);

/// Set grid containing locations of edges in edgeI_coord[].
void set_edge_index(const std::vector<COORD_TYPE> & edgeI_coord,
                    SHARPISO_EDGE_INDEX_GRID & edge_index);

/// Count number of vertices on sharp corners or sharp edges.
/// Count number of smooth vertices.
void count_vertices
(const ISOVERT & isovert, ISOVERT_INFO & isovert_info);

/// Select all grid cubes which are not smooth.
void select_non_smooth(ISOVERT & isovert);

/// Get list of grid cubes from isovert.
void get_cube_list
  (const ISOVERT & isovert, std::vector<VERTEX_INDEX> & cube_list);

/// Transform GRID_CUBE_FLAG into a string
void convert2string(const GRID_CUBE_FLAG & flag, std::string & s);

// Transform CUBE_ADJACENCY_TYPE into a string
void convert2string
(const CUBE_ADJACENCY_TYPE & adj_type, std::string & s);

/// Copy isovert position from from_gcube to to_gcube.
void copy_isovert_position
(const SHARPISO_GRID & grid, 
 const NUM_TYPE from_gcube, const NUM_TYPE to_gcube, ISOVERT & isovert);

/// Compute overlap of 3x3x3 regions
bool find_3x3x3_overlap
(const GRID_COORD_TYPE cubeA_coord[DIM3],
 const GRID_COORD_TYPE cubeB_coord[DIM3],
 GRID_COORD_TYPE rmin[DIM3], GRID_COORD_TYPE rmax[DIM3],
 int & overlap_dim);

/// Compute overlap of 3x3x3 regions.
/// Version with grid and cube indices input.
template <typename GRID_TYPE>
bool find_3x3x3_overlap
(const GRID_TYPE & grid,
 const VERTEX_INDEX cubeA_index, const VERTEX_INDEX cubeB_index,
 GRID_COORD_TYPE rmin[DIM3], GRID_COORD_TYPE rmax[DIM3],
 int & overlap_dim);

/// Add to list selected cubes whose vertices are "connected" 
///   to vertex in cube0_index
/// @param[out] connected_sharp List of selected cubes.
void add2list_connected_sharp
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid, 
 const SCALAR_TYPE isovalue,
 const VERTEX_INDEX cube0_index,
 const SHREC::ISOVERT & isovert, 
 const std::vector<SHARPISO::VERTEX_INDEX> & gcube_map,
 CUBE_CONNECTED_ARRAY & connected_sharp);

// Find selected cubes whose vertices are "connected" to vertex in cube0_index
// @param[out] connected_sharp List of selected cubes.
void find_connected_sharp
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid, 
 const SCALAR_TYPE isovalue,
 const VERTEX_INDEX cube0_index,
 const SHREC::ISOVERT & isovert, 
 const std::vector<SHARPISO::VERTEX_INDEX> & gcube_map,
 CUBE_CONNECTED_ARRAY & connected_sharp);

// Compute the cosine of the angle between (v2,v1) and (v2,v3)
void compute_cos_angle
(const ISOVERT & isovert,
 const VERTEX_INDEX gcube_list_index_v1,
 const VERTEX_INDEX gcube_list_index_v2,
 const VERTEX_INDEX gcube_list_index_v3,
 SCALAR_TYPE & cos_angle);


// **************************************************
// BIN_GRID ROUTINES
// **************************************************

/// Initialize bin_grid.
/// @param bin_width = number of cubes along each axis.
void init_bin_grid
(const SHARPISO_GRID & grid, const AXIS_SIZE_TYPE bin_width,
 BIN_GRID<VERTEX_INDEX> & bin_grid);

/// Insert cube cube_index into the bin_grid.
void bin_grid_insert
(const SHARPISO_GRID & grid, const AXIS_SIZE_TYPE bin_width,
 const VERTEX_INDEX cube_index, BIN_GRID<int> & bin_grid);

/// Remove cube cube_index into the bin_grid.
void bin_grid_remove
(const SHARPISO_GRID & grid, const AXIS_SIZE_TYPE bin_width,
 const VERTEX_INDEX cube_index, BIN_GRID<int> & bin_grid);

/// Get the selected vertices around iv.
void get_selected
(const SHARPISO_GRID & grid,
 const VERTEX_INDEX iv,
 const BIN_GRID<VERTEX_INDEX> & bin_grid,
 const AXIS_SIZE_TYPE bin_width,
 std::vector<VERTEX_INDEX> & selected_list);


// **************************************************
// SUBROUTINES
// **************************************************

/// Return true if this vertex creates a triangle with a large angle.
/// @param check_triangle_angle If true, check it triangle has large angles.
/// @param bin_grid Contains the already selected vertices.
/// @param[out] v1,v2 vertex indices which form a triangle with iv.
bool creates_triangle
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
 const bool check_triangle_angle,
 const ISOVERT & isovertData,
 const VERTEX_INDEX iv,
 const SCALAR_TYPE isovalue,
 const BIN_GRID<VERTEX_INDEX> & bin_grid,
 const AXIS_SIZE_TYPE bin_width,
 VERTEX_INDEX & v1,
 VERTEX_INDEX & v2);

/// Return true if selecting this vertex creates a triangle with a large angle.
/// @param bin_grid Contains the already selected vertices.
/// @param[out] iv1, iv2 vertex indices which form a triangle with iv0
bool creates_triangle_new
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
 const ISOVERT & isovert,
 const VERTEX_INDEX iv0,
 const SCALAR_TYPE isovalue,
 const BIN_GRID<VERTEX_INDEX> & bin_grid,
 const AXIS_SIZE_TYPE bin_width,
 VERTEX_INDEX & iv1,
 VERTEX_INDEX & iv2);

/// Select and sort cubes with more than one eigenvalue.
///   Store references to cubes sorted by number of large eigenvalues
///     and then by increasing distance from isovert_coord to cube center.
/// @param gcube_index_list List of references to sharp cubes
///    sorted by number of large eigenvalues and by distance 
///    of sharp coord from cube center.


/// @param[out] gcube_index_list Sorted list of references to grid cubes
///      with more than one eigenvalue.
///    List is sorted by decreasing number of eigenvalues and then by
///      increasing linf_dist.
void get_corner_or_edge_cubes
(const std::vector<GRID_CUBE_DATA> & gcube_list,
 std::vector<NUM_TYPE> & gcube_index_list);

/// Get selected cubes.
///   Store references to cubes sorted by number of large eigenvalues
///     and then by increasing distance from isovert_coord to cube center.
/// @param gcube_index_list List of references to selected cubes
///    sorted by number of large eigenvalues and then by distance 
///    of sharp coord from cube center.
void get_selected_cubes
(const std::vector<GRID_CUBE_DATA> & gcube_list,
 std::vector<NUM_TYPE> & gcube_index_list);

/// Get selected corner cubes.
///   Store references to cubes sorted by number of large eigenvalues
///     and then by increasing distance from isovert_coord to cube center.
/// @param gcube_index_list List of references to selected corner cubes
///    sorted by distance of sharp coord from cube center.
void get_selected_corner_cubes
(const std::vector<GRID_CUBE_DATA> & gcube_list,
 std::vector<NUM_TYPE> & gcube_index_list);

/// Store boundary bits for each cube in gcube_list.
void store_boundary_bits
(const SHARPISO_GRID & grid, GRID_CUBE_DATA_ARRAY & gcube_list);

/// Store isosurface lookup table index in gcube_list.
void store_table_index
(const std::vector<IJKDUALTABLE::TABLE_INDEX> & table_index,
 GRID_CUBE_DATA_ARRAY & gcube_list);

/// Return true if line through sharp edge in cube0 passes near cube1.
/// Return false if cube0 has no sharp edge.
/// @param max_distance Line points to cube if distance between line and cube
///    is at most max_distance.
bool does_sharp_edge_point_to_cube
(const SHARPISO_GRID & grid, const ISOVERT & isovert,
 const VERTEX_INDEX cube0_index, const VERTEX_INDEX cube1_index,
 const COORD_TYPE max_distance, COORD_TYPE & distance);

/// Return true if line through sharp edge in cube0 passes near cube1.
/// Version which does not return distance.
bool does_sharp_edge_point_to_cube
(const SHARPISO_GRID & grid, const ISOVERT & isovert,
 const VERTEX_INDEX cube0_index, const VERTEX_INDEX cube1_index,
 const COORD_TYPE max_distance);

/// Return true if line through sharp edge in cube0 passes near cube1.
/// Use linf metric.
/// Return false if cube0 has no sharp edge.
/// @param max_distance Line points to cube if distance between line and cube
///    is at most max_distance.
bool does_sharp_edge_point_to_cube_linf
(const SHARPISO_GRID & grid, const ISOVERT & isovert,
 const VERTEX_INDEX cube0_index, const VERTEX_INDEX cube1_index,
 const COORD_TYPE max_distance,
 const SHARP_ISOVERT_PARAM & isovert_param);

}

#endif /* _SHREC_ISOVERT_H_ */
