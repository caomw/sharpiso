/*
 *  sh_point_find.cxx
 *  SHARPPOINT
 *
 *  Created by arindam bhattacharya on 11/6/11.
 *  Copyright 2011 Ohio State University. All rights reserved.
 *
 */

#include <iostream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdlib.h>
#include "sharpiso_svd.h"
#include "sharpiso_feature.h"
#include "sh_point_find.h"
#include "sh_point_datastruct.h"
#include "sh_point_svdcal.h"



using namespace std;
using namespace sh_cube;
using namespace SHARPISO;

// Get gradients at edge intersection points determined by edge endpoints.
// Use sharp formula for computing gradient at intersection.
void get_edgeI_sharp_gradients
(const SHARPISO_SCALAR_GRID_BASE & scalar_grid,
 const GRADIENT_GRID_BASE & gradient_grid,
 const VERTEX_INDEX cube_index,
 const SCALAR_TYPE isovalue,
 std::vector<COORD_TYPE> & point_coord,
 std::vector<GRADIENT_COORD_TYPE> & gradient_coord,
 std::vector<SCALAR_TYPE> & scalar,
 NUM_TYPE & num_gradients)
{
  CUBE cube;
  GRADIENT_COORD_TYPE cube_gradient[NUM_CUBE_VERTICES3D*DIM3];
  SCALAR_TYPE cube_scalar[NUM_CUBE_VERTICES3D];
  static COORD_TYPE cube_coord[DIM3];

  point_coord.clear();
  gradient_coord.clear();
  scalar.clear();
  num_gradients = 0;

  get_cube_gradients
    (scalar_grid, gradient_grid, cube_index,
     cube_gradient, cube_scalar);

  bool setup_flag = sh_cube::setup_shCube
    (cube, cube_gradient, isovalue, cube_scalar);

  if (!setup_flag) { 
    // Isosurface does not intersect cube.
    return; 
  }

  setup_edgeIntercepts(cube, isovalue, true);

  scalar_grid.ComputeCoord(cube_index, cube_coord);

  for (int i = 0; i < cube.num_edges; i++) {
    if (cube.edges[i].is_intersect) {
      scalar.push_back(cube.edges[i].pt_intersect.scalar);
      for (int d = 0; d < 3; d++) {
        COORD_TYPE c = cube.edges[i].pt_intersect.pos[d] + cube_coord[d];
        point_coord.push_back(c);
        gradient_coord.push_back(cube.edges[i].pt_intersect.grads[d]);
      }
      num_gradients++;
    }
  }
}

void sh_normalize(double intial[],double normalized[])
{
	double sum(0.0),mag(0.0);
	for (int i=0; i<3; i++) {
		sum = sum + intial[i]*intial[i];
	}
	if (sum>0) {
		mag = sqrt(sum);
		for (int j=0; j<3; j++) {
			normalized[j] = intial[j] / mag;
		}
	}
};

void interpolate_positions( EDGE &e, const POINT& a1, const POINT & a2,const double k1)
{
	double k2 = 1.0 - k1;
	for(int j=0; j<3; j++)
	{
		e.pt_intersect.pos[j]=k2*a1.pos[j] + k1*a2.pos[j];
	}
};

/*
set gradients
 */
void set_gradients(EDGE &e, const double lambda){
	for (int d=0; d<3; d++) {
		e.pt_intersect.grads[d] = (1.0-lambda)*e.p1.grads[d]+lambda*e.p2.grads[d];
	}
};

/*
set_position of the edge intersect.
 */
void set_positions(EDGE &e,const double  lambda){
	for (int d=0; d<3; d++) {
		e.pt_intersect.pos[d] = (1.0-lambda)*e.p1.pos[d]+lambda*e.p2.pos[d];
	}
};

/*
assign gradients to the edge intersect.
 */
void assign_gradients(EDGE & e, POINT &p)
{
	for (int i=0;i<3;i++){
		e.pt_intersect.grads[i]=p.grads[i];
	}
}



/*
Function to set the point intersects on each edge
 */
void set_pt_intersect
(EDGE &e,
		const double isovalue,
		const bool use_cmplx_interp)
{
	//set the scalar
	//set the grads (normalize _grads)
	//set the pos
	if (!use_cmplx_interp) {
		//set scalar
		e.pt_intersect.scalar = isovalue;
		double lambda = (isovalue-e.p1.scalar)/(e.p2.scalar-e.p1.scalar);
		set_gradients(e,lambda);
		set_positions(e,lambda);
	}
	else
	{
		bool use_simple_interp = false;// default value set to false.
		//set up u1
		float  u1[3]={0.0};
		for (int k=0; k<3; k++)
		{
			u1[k] = e.p2.pos[k]-e.p1.pos[k];
		}
		// normalize u1
		normalize(u1, u1);
		// add the normalization routine here
		double grads_p1[] ={e.p1.grads[0],e.p1.grads[1],e.p1.grads[2]};
		double grads_p2[] ={e.p2.grads[0],e.p2.grads[1],e.p2.grads[2]};

		//Check if {u1 dot g1 - u1 dot g2} == 0 then donot proceed.
		double dot_pdt_u1_g1 = inner_product(u1, u1+3, grads_p1, 0.0);
		double dot_pdt_u1_g2 = inner_product(u1, u1+3, grads_p2, 0.0);

		double denominator = (dot_pdt_u1_g1 - dot_pdt_u1_g2);
		double numerator = ((e.p2.scalar - e.p1.scalar) - dot_pdt_u1_g2 );

		if ( denominator != 0.0) {
			//calculate lambda
			double lambda = numerator/denominator;
			if (lambda>1.0 || lambda < 0.0) {
				// *** NOT CORRECT ***
				// *** lambda could be > 1.0 or < 0.0 but one gradient dominates ***
				use_simple_interp = true;
			}
			else
			{
				e.pt_intersect.scalar = isovalue;
				//calculate the s_lambda
				double s_lambda = e.p1.scalar + dot_pdt_u1_g1*lambda;
				POINT p_lambda;
				//p_lambda scalar
				p_lambda.scalar = s_lambda;
				//p_lambda positions
				for(int i=0; i<3; i++)
					p_lambda.pos[i] = lambda*e.p2.pos[i] + (1.0-lambda)*e.p1.pos[i];

				//set default gradients [these are not used anywhere]
				for(int i=0;i<3;i++)
					p_lambda.grads[i]=0.0;
				// compute lambda2 based on lambda
				double lambda2 = 0.0;

				if (e.p1.scalar <isovalue && isovalue<=s_lambda)
				{
					lambda2=(isovalue-e.p1.scalar)/(s_lambda-e.p1.scalar);
					if (lambda2 > 1.0 || lambda2<0.0) {
						use_simple_interp = true;
					}
					else{
						interpolate_positions(e, e.p1, p_lambda, lambda2);
						assign_gradients(e, e.p1);
					}
				}
				else if (s_lambda<isovalue && isovalue<=e.p2.scalar) {
					lambda2=(isovalue-s_lambda)/(e.p2.scalar-s_lambda);
					if (lambda2 > 1.0 || lambda2<0.0) {
						use_simple_interp = true;
					}
					else{
						interpolate_positions(e, p_lambda, e.p2, lambda2);
						assign_gradients(e, e.p2);}
				}
				else if(e.p2.scalar<isovalue && isovalue<=s_lambda){
					lambda2=(isovalue-e.p2.scalar)/(s_lambda-e.p2.scalar);

					if (lambda2 > 1.0 || lambda2 < 0.0) {
						use_simple_interp = true;
					}
					else{
						interpolate_positions(e, e.p2, p_lambda, lambda2);
						assign_gradients(e, e.p2);
					}
				}
				else if(s_lambda<isovalue && isovalue<=e.p1.scalar){
					lambda2 = (isovalue-s_lambda)/(e.p1.scalar-s_lambda);
					if (lambda2 > 1.0 || lambda2 < 0.0) {
						use_simple_interp = true;
					}
					else
					{
						interpolate_positions(e, p_lambda, e.p1, lambda2);
						assign_gradients(e, e.p1);
					}
				}
			}
		}
		if (use_simple_interp || denominator == 0.0) {
			//set scalar
			e.pt_intersect.scalar = isovalue;
			double lambda = (isovalue-e.p1.scalar)/(e.p2.scalar-e.p1.scalar);

			set_gradients(e,lambda);
			set_positions(e,lambda);
		}
	}
	e.pt_intersect.normalize_grads();
};


// Setup edge_intercepts.
// Find the intersect points of edges of the cube and the isosuraface.

void setup_edgeIntercepts
(CUBE &cb,
		const double isovalue,
		const bool use_cmplx_interp)
{

	cb.ne_intersect = 0;
	int ne = cb.num_edges;
	int dim = cb.dim;
	for (int i = 0; i < ne; i++) {

		if(((cb.edges[i].p1.scalar >= isovalue) &&
				( isovalue > cb.edges[i].p2.scalar)) ||
				(cb.edges[i].p1.scalar < isovalue)&&
				(isovalue <= cb.edges[i].p2.scalar))
		{
			cb.ne_intersect++;
			cb.edges[i].is_intersect = true;
			cb.intersects_ind[i]=1;
			// set the pt_intersect
			set_pt_intersect(cb.edges[i], isovalue, use_cmplx_interp);
		}
		else
		{
			cb.edges[i].is_intersect = false;
			cb.intersects_ind[0];
		}
	}
};



/*
For each cube this function returns a double[3] 'point'
which the dual-vertex for this cube
input :
gradients,
scalar_values,
isovalue,
EIGEN_VALUE_CUTOFF for svd calculation
point(which is also its out put)
 */

bool shFindPoint
(const GRADIENT_COORD_TYPE gradients[],
		const  SCALAR_TYPE  scalar_vals[],
		const  SCALAR_TYPE  isovalue,
		const  bool use_cmplx_interp,
		const  SCALAR_TYPE   EIGEN_VALUE_CUTOFF,
		float eigenvalues[DIM3],
		int &num_large_eigenvalues,
		SVD_INFO &svd_info,
		COORD_TYPE *shpoint)
{
	//setup sh_cube.
	CUBE cb;
	bool cubeSetup(true);
	cubeSetup = sh_cube::setup_shCube
			(cb, gradients, isovalue, scalar_vals);

	for (int i = 0; i < DIM3; i++)
	{ eigenvalues[i] = 0; }

	num_large_eigenvalues = 0;

	if (!cubeSetup) { return false; }

	//setup edge_intercepts.
	setup_edgeIntercepts
	(cb, isovalue, use_cmplx_interp);

	//find point calculations.
	findPoint
	(cb, EIGEN_VALUE_CUTOFF, eigenvalues, num_large_eigenvalues,
			svd_info, shpoint);
	return true;
}

