#ifndef _OPT_PLACEMENT_HPP_
#define _OPT_PLACEMENT_HPP_


//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/graphviz.hpp>
//#include <boost/graph/iteration_macros.hpp>
//#include <boost/foreach.hpp>
//#include <Eigen/Core>
//#include <Eigen/LU>
//#include "mod2.hpp"
#include <modularity.hpp>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

namespace opt_placement
{

  inline gsl_matrix* _laplacian(const Cppn* g, const std::set<Node*>& verts)
  {
	  gsl_matrix* l = gsl_matrix_calloc (g->getNrOfNodes(), g->getNrOfNodes());

	  size_t i = 0;
	  foreach(Node* node1, g->getNodes()){
		  	size_t j = 0;
		    foreach(Node* node2, g->getNodes()){
		    	if (verts.find(node1) == verts.end()|| verts.find(node2) == verts.end()){
		    		gsl_matrix_set(l, i,j,0.0);
		    	} else if (i == j){
		    		gsl_matrix_set(l, i,j,node1->incomingEdges().count() + node1->outgoingEdges().count());
		    	} else if (mod::_linked(node1, node2) || mod::_linked(node2, node1)){
		    		gsl_matrix_set(l, i,j,-1.0);
		    	} else {
		    		gsl_matrix_set(l, i,j,0.0);
		    	}
		    	++j;
		    }
		    ++i;
	  }

	  return l;
  }

   // include in the adjacency iff source _or_ target is in verts
  inline gsl_matrix* _adjacency(const Cppn* g, const std::set<Node*>& verts)
  {
	  gsl_matrix* a = gsl_matrix_calloc (g->getNrOfNodes(), g->getNrOfNodes());

	  size_t i = 0;
	  foreach(Node* node1, g->getNodes()){
		  	size_t j = 0;
		    foreach(Node* node2, g->getNodes()){
				  if ((verts.find(node1) != verts.end() || verts.find(node2) != verts.end()) && (mod::_linked(node1, node2) || mod::_linked(node2, node1)))
					  gsl_matrix_set(a, i,j,1.0);
				  else
					  gsl_matrix_set(a, i,j,0.0);

		    	++j;
		    }
		    ++i;
	  }

	  return a;
  }

   // include in the degree matrix iff vertex is in verts
  inline gsl_matrix* _degree(const Cppn* g, const std::set<Node*>& verts)
  {
	  gsl_matrix* d = gsl_matrix_calloc (g->getNrOfNodes(), g->getNrOfNodes());

	  size_t i = 0;
	  foreach(Node* node1, g->getNodes()){
		  if (verts.find(node1) != verts.end())
			  gsl_matrix_set(d, i,i,node1->incomingEdges().count() + node1->outgoingEdges().count());

		  ++i;
	  }
	  return d;
  }


  template<typename V, typename T>
  inline int _find_index(const V& vect, const T& e)
  {
    for (int i = 0; i < vect.size(); ++i)
      if (e == vect[i]){
//    	  std::cout << i << std::endl;
          return i;
      }

    return -1;
  }



  inline gsl_vector* _f(const Cppn* g,
		  const QList<Node*>& inputs,
		  const QVector<double>& coords_inputs,
		  const QList<Node*>& outputs,
		  const QVector<double>& coords_outputs)
  {
	  gsl_vector* f = gsl_vector_calloc(g->getNrOfNodes());


	  size_t i = 0;


	  foreach(Node* node1, g->getNodes()){
		  int in = _find_index(inputs, node1);
		  if (in != -1){
		  	  gsl_vector_set(f, i, coords_inputs[in]);
		  }
		  int out = _find_index(outputs, node1);
		  if (out != -1){
		  	  gsl_vector_set(f, i, coords_outputs[out]);
		  }
		  ++i;
	  }

	  return f;
  }


  inline QVector<double> compute(const Cppn* g,
                                 const QList<Node*>& inputs,
                                 const QVector<double>& coords_inputs,
                                 const QList<Node*>& outputs,
                                 const QVector<double>& coords_outputs)
  {
    typedef Node* v_d_t;
    std::set<v_d_t> all_set, io_set, no_io_set;

    foreach(Node* node1, g->getNodes()) all_set.insert(node1);
    foreach(Node* node, inputs) io_set.insert(node);
    foreach(Node* node, outputs) io_set.insert(node);


    std::set_difference(all_set.begin(), all_set.end(),
                        io_set.begin(), io_set.end(),
                        std::insert_iterator<std::set<v_d_t> >(no_io_set,
                                                               no_io_set.begin()));
    gsl_matrix* l = _laplacian(g, no_io_set);
    gsl_matrix* b = _adjacency(g, io_set);
    gsl_matrix* d = _degree(g, io_set);
    gsl_vector* f = _f(g, inputs, coords_inputs, outputs, coords_outputs);

	// Define all the used matrices
	gsl_matrix_add (l, d);

	//Calculate inverse of l+d
    int s;
	gsl_matrix* temp1 = gsl_matrix_alloc (l->size1, l->size2);
	gsl_matrix* temp2 = gsl_matrix_alloc (l->size1, l->size2);

	gsl_vector* result = gsl_vector_alloc(l->size1);
	QVector<double> qresult(l->size1);

	gsl_permutation * perm = gsl_permutation_alloc (f->size);
	gsl_linalg_LU_decomp (l, perm, &s);
	gsl_linalg_LU_invert (l, perm, temp1);


	//Multiplying the inverse of l+d with b
	gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, temp1, b, 0.0, temp2);

	//Multiplying the inverse of l+d multiplied with b with f
	gsl_blas_dgemv (CblasNoTrans, 1.0, temp2, f, 0.0, result);

	//Fix positions of input nodes
	int i=0;
	foreach(Node* node1, g->getNodes()){
		if (io_set.find(node1) != io_set.end()){
			qresult[i] = gsl_vector_get(f, i);
		} else {
			qresult[i] = gsl_vector_get(result, i);
		}
		i++;
	}

	gsl_matrix_free(temp1);
	gsl_matrix_free(temp2);
	gsl_vector_free(result);

    return qresult;
  }
}
#endif
