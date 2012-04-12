// Test ijkIO.txx

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>
#include <iostream>

#include "findEdgeCountTypes.h"
#include "findEdgeCountIO.h"
#include "findEdgeCount.h"

// global variables
char * input_filename(NULL);
string output_fn;
COORD_TYPE * vertex_coord(NULL);
VERTEX_INDEX * edge_endpoint(NULL);
int dimension(3);
int num_vertices(0);
int num_edges(0);
bool  flag_op_to_file = false;
// miscellaneous routines
void usage_error();
void parse_command_line(int argc, char **argv);
void compute_output_fn ();

int main(int argc, char **argv)
{
  try {
    parse_command_line(argc, argv);

    ifstream in(input_filename, ios::in);
    if (!in.good()) {
      cerr << "Unable to open file " << input_filename << "." << endl;
      exit(30);
    };

    ijkinLINE(in, dimension, vertex_coord, num_vertices, 
      edge_endpoint, num_edges);
    in.close();
    // count the degree of each vertex
    vector <int> vert_degree(num_vertices,0);
    // count the degrees of the different edge points 
    count_edge_degrees(dimension, vertex_coord, num_vertices,
      edge_endpoint, num_edges, vert_degree);
    if ( !flag_op_to_file){
      // output the edge information 
      output_vert_degree( num_vertices, vert_degree);
    }
    else 
    {
      //compute the output file name
      //compute_output_fn ();
      output_vert_degree_2_file (num_vertices, vert_degree);
    }

  }
  catch (ERROR & error) {
    if (error.NumMessages() == 0) {
      cerr << "Unknown error." << endl;
    }
    else { error.Print(cerr); }
    cerr << "Exiting." << endl;
    exit(20);
  }
  catch (...) {
    cerr << "Unknown error." << endl;
    exit(50);
  };

  return 0;
}



// **************************************************
// Miscellaneous routines
// **************************************************

void usage_msg()
{
  cerr << "Usage: findedgecount -fp <.line file>" << endl;
}

void usage_error()
{
  usage_msg();
  exit(10);
}

void parse_command_line(int argc, char **argv)
{
  /*
  int iarg = 1;

  if (argc != 2) { usage_error(); }

  input_filename = argv[1];
  */
  if (argc == 1)  {usage_error();}
  int iarg=1;
  while (iarg<argc && argv[iarg][0]=='-')
  {
    string s = argv[iarg];
    if (s=="-fp")
    {
      flag_op_to_file = true;
      iarg++;   
    }
  }
  input_filename = argv[iarg];
}




void compute_output_fn ()
{
  size_t found;
  string infile = input_filename;
  found=infile.find_last_of(".");
  output_fn = infile.substr(0,found);
  output_fn += ".txt";
}