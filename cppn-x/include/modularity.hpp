

#ifndef _MODULARITY_HPP_
#define _MODULARITY_HPP_

#include "CE_Cppn.h"

#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include <set>

// Community Structure in Directed Networks
// EA Leicht, MEJ Newman - Physical Review Letters, 2008 - APS
// http://arxiv.org/pdf/0709.4500
namespace mod
{


void convert_graph(Cppn* src, Cppn* g,
		std::map<Node*,Node*>& rmap)
{
	typedef Node* vertex_desc_t;
	std::map<Node*, vertex_desc_t> neuron_map;

	foreach(Node* node1, src->getNodes()){
		Node* node2 = new Node();
		g->addNode(node2);
		neuron_map[node1]=node2;
		rmap[node2] = node1;
	}

	foreach(Edge* edge1, src->getEdges()){
		//    	Edge* edge2 = new Edge(neuron_map[edge1->sourceNode()], neuron_map[edge1->destNode()]); //FIXME This constructor does not exist
		g->addConnection(edge1);
	}
}

inline bool _linked(
		const Node* node1,
		const Node* node2)
{
	foreach(Edge* edge, node1->outgoingEdges()){
		if(edge->destNode() == node2) return true;
	}
	return false;
}

inline void _b_matrix(const Cppn* g, gsl_matrix*& b)
{
	size_t nrOfNodes = g->getNrOfNodes();
	b = gsl_matrix_alloc(nrOfNodes, nrOfNodes);

	size_t m = g->getNrOfEdges();
	size_t i = 0;

	foreach(Node* node1, g->getNodes()){
		size_t j = 0;
		foreach(Node* node2, g->getNodes()){
			size_t k_i_in = node1->incomingEdges().size();
			size_t k_j_out = node2->outgoingEdges().size();
			double b_ij = _linked(node1, node2) - k_i_in * k_j_out / (double) m;
			gsl_matrix_set (b, i, j, b_ij);
			++j;
		}
		++i;
	}
}

// res = b^T + b, allocate res
static void _bbt(gsl_matrix* b, gsl_matrix*& res)
{
	gsl_matrix* bt = gsl_matrix_alloc(b->size1, b->size2);
	res = gsl_matrix_alloc(b->size1, b->size2);
	gsl_matrix_memcpy(res, b);

	gsl_matrix_transpose_memcpy(bt, b);
	gsl_matrix_add(res, bt);
	gsl_matrix_free(bt);
}

// split in two according to b
static void _split_eigen(gsl_matrix* b, size_t n, std::vector<int>& s)
{
	//    assert(n);
	s.resize(n);
	gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(n);
	gsl_vector *eval = gsl_vector_alloc(n);
	gsl_matrix *evec = gsl_matrix_alloc(n, n);
	gsl_matrix *b_copy = gsl_matrix_alloc(n, n);
	gsl_matrix_memcpy(b_copy, b);
	gsl_eigen_symmv(b_copy, eval, evec, w);

	gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_VAL_DESC);
	for (size_t i = 0; i < n; ++i)
		s[i] = (gsl_matrix_get(evec, i, 0) > 0 ? 1 : -1);

	gsl_eigen_symmv_free(w);
	gsl_vector_free(eval);
	gsl_matrix_free(evec);
	gsl_matrix_free(b_copy);
	//    assert(s.size() == n);
}

static void _bg(gsl_matrix* b, const std::vector<size_t>& sg1,
		gsl_matrix*& bg)
{
	size_t n1 = sg1.size();
	//    assert(n1);
	bg = gsl_matrix_alloc(n1, n1);
	for (size_t i = 0; i < n1; ++i)
		for (size_t j = 0; j < n1; ++j)
		{
			double b_ij = gsl_matrix_get(b, sg1[i], sg1[j]);
			if (i == j)
				for (size_t k = 0; k < n1; ++k)
					b_ij -= gsl_matrix_get(b, sg1[i], sg1[k]);
			gsl_matrix_set(bg, i, j, b_ij);
		}
}


double _delta_q(gsl_matrix* bbt, const std::vector<int>& sg, size_t m)
{
	// 1/(4 * m) * s^T * bbt * s
	//    assert(sg.size());
	gsl_matrix* sv = gsl_matrix_alloc(sg.size(), 1);
	gsl_matrix* svt = gsl_matrix_alloc(1, sg.size());
	gsl_matrix* tmp1 = gsl_matrix_alloc(sg.size(), 1);
	gsl_matrix* tmp2 = gsl_matrix_alloc(1, 1);

	for (size_t i = 0; i < sg.size(); ++i)
	{
		gsl_matrix_set(sv, i, 0, sg[i]);
		gsl_matrix_set(svt, 0, i, sg[i]);
	}
	gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
			1.0, bbt, sv,
			0.0, tmp1);
	gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
			1.0, svt, tmp1,
			0.0, tmp2);
	double delta_q = 1.0 / (4.0 * m) * gsl_matrix_get(tmp2, 0, 0);

	gsl_matrix_free(sv);
	gsl_matrix_free(svt);
	gsl_matrix_free(tmp1);
	gsl_matrix_free(tmp2);

	return delta_q;
}

void _split(gsl_matrix* b, const std::vector<size_t>& sg, size_t m,
		std::vector<std::string>& modules)
{
	//    assert(sg.size());

	gsl_matrix* bbt, *bg;

	_bg(b, sg, bg);
	_bbt(bg, bbt);
	std::vector<int> s;
	_split_eigen(bbt, sg.size(), s);

	double delta_q = _delta_q(bbt, s, m);
	if (delta_q <= 1e-5)
	{
		gsl_matrix_free(bbt);
		gsl_matrix_free(bg);
		return;
	}

	// new sg
	std::vector<size_t> sg1, sg2;
	//    assert(sg.size() == s.size());
	for (size_t i = 0; i < s.size(); ++i)
		if (s[i] == -1)
		{
			modules[sg[i]] += '0';
			sg1.push_back(sg[i]);
		}
		else
		{
			modules[sg[i]] += '1';
			sg2.push_back(sg[i]);
		}

	gsl_matrix_free(bbt);
	gsl_matrix_free(bg);

	if (sg1.size() > 0 && sg2.size() > 0)
	{
		_split(b, sg1, m, modules);
		_split(b, sg2, m, modules);
	}

}



float compute_modularity(Cppn* g, const std::vector<std::string>& modules)
{
	double m = g->getNrOfEdges();
	double q = 0.0;
	size_t v1_i = 0;

	foreach(Node* node1, g->getNodes()){
		size_t v2_i = 0;
		foreach(Node* node2, g->getNodes()){
			if (modules[v1_i] == modules[v2_i]){
				size_t k_i_in = node1->incomingEdges().size();
				size_t k_j_out = node2->outgoingEdges().size();
				double b_ij = _linked(node1, node2) - k_i_in * k_j_out / (double) m;
				q += b_ij;
			}
			++v2_i;
		}
		++v1_i;
	}

	return q / m;
}

inline void split(Cppn* g, std::vector<std::string>& modules)
{
	double m = g->getNrOfEdges();
	gsl_matrix* b, *bbt;

	_b_matrix(g, b);
	_bbt(b, bbt);

	size_t n = g->getNrOfNodes();
	std::vector<int> s;
	_split_eigen(bbt, n, s);

	size_t n1 = std::count(s.begin(), s.end(), -1);
	size_t n2 = n - n1;

	modules.resize(g->getNrOfNodes());
	std::vector<size_t> sg1(n1), sg2(n2);
	size_t j1 = 0, j2 = 0;
	for (size_t i = 0; i < n; ++i){
		if (s[i] == -1)
		{
			modules[i] += '0';
			//    		assert(j1 < sg1.size());
			sg1[j1] = i;
			++j1;
		}
		else
		{
			modules[i] += '1';
			//    		assert(j2 < sg2.size());
			sg2[j2] = i;
			++j2;
		}
	}

	if (sg1.size() > 0 && sg2.size() > 0)
	{
		_split(b, sg1, m, modules);
		_split(b, sg2, m, modules);
	}
	gsl_matrix_free(b);
	gsl_matrix_free(bbt);
}





//void _h_modules(const std::set<Node*>& nodes,
//		const std::vector<std::string>& ids,
//		std::vector<std::set<Node*> >& mods,
//		size_t n)
//{
//	//    assert(nodes.size());
//	typedef Node* vertex_desc_t;
//	std::set<vertex_desc_t> s1, s2;
//	for(std::set<Node*>::iterator it = nodes.begin(); it != nodes.end(); it++){
//		if (n >= ids[(*it)->getIndex()].size()) //TODO This is probably not the correct way to get the correct id for each node.
//		{
//			return;
//		}
//
//		if (ids[(*it)->getIndex()][n] == '0') //TODO This is probably not the correct way to get the correct id for each node.
//			s1.insert((*it));
//		else
//			s2.insert((*it));
//	}
//
//	mods.push_back(s1);
//	mods.push_back(s2);
//
//	if (s1.size())
//		_h_modules(s1, ids, mods, n + 1);
//	if (s2.size())
//		_h_modules(s2, ids, mods, n + 1);
//}

// ALL the modules (high and low levels)
//double h_modules(Cppn* g_origin, std::vector<std::set<Node*> >& mods_r)
//{
//	Cppn* g;
//	std::vector<std::set<Node*> > mods;
//	std::map<Node*, Node*> rmap;
//	convert_graph(g_origin, g, rmap);
//
//	std::vector<std::string> ids;
//	split(g, ids);
//	typedef std::set<Node*> vset_t;
//	vset_t nodes;
//
//	foreach(Node* node, g->getNodes()){
//		nodes.insert(node);
//	}
//
//	_h_modules(nodes, ids, mods, 0);
//
//	float m = compute_modularity(g, ids);
//	// create mod_r
//	mods_r.resize(mods.size());
//	for (size_t i = 0; i < mods.size(); ++i){
//		for(std::set<Node*>::iterator it = mods[i].begin(); it != mods[i].end(); it++){
//			mods_r[i].insert(rmap[(*it)]);
//		}
//	}
//
//	return m;
//}

// true if set is included in Vm
template<typename S>
bool _includes(const S& s, const std::vector<S>& vm)
{
	for (size_t i = 0; i < vm.size(); ++i)
		if (s != vm[i] &&
				std::includes(vm[i].begin(), vm[i].end(), s.begin(), s.end()))
			return true;
	return false;
}

template<typename Vm>
void _write_modules(Cppn* g, const Vm& vm, std::ostream &os)
{
	Vm vv;
	// find the sets that are not included in another set
	for (size_t i = 0; i < vm.size(); ++i)
		if (!_includes(vm[i], vm))
			vv.push_back(vm[i]);
	// split the vm list
	std::vector<Vm> n_vm(vv.size());
	for (size_t k = 0; k < vv.size(); ++k)
		for (size_t i = 0; i < vm.size(); ++i)
			if (vm[i] != vv[k] &&
					std::includes(vv[k].begin(), vv[k].end(),
							vm[i].begin(), vm[i].end()))
				n_vm[k].push_back(vm[i]);

	// recurse
	for (size_t i = 0; i < n_vm.size(); ++i)
	{
		os << "subgraph cluster_" << rand()<< " {" << std::endl;
		if (!n_vm[i].empty())
			_write_modules(g, n_vm[i], os);
		else
			for (typename Vm::value_type::const_iterator it = vv[i].begin();
					it != vv[i].end(); ++it)
				os << (*it)->getName() << std::endl;
		os << "}" << std::endl;
	}
}


// hierachical dot file (slow version)
//void write_modules(Cppn* g, std::ostream &os)
//{
//	std::vector<std::set<Node*> > mods_r;
//	h_modules(g, mods_r);
//	os << "digraph G {" << std::endl;
//	_write_modules(g, mods_r, os);
//	foreach(Edge* edge, g->getEdges()){
//		os << edge->sourceNode()->getName()
//    			  << " -> " << edge->destNode()->getName()
//    			  << "[label=\"" << edge->getWeight() << "\"]" << std::endl;
//	}
//	os << "}" <<std::endl;
//}


// post-processing : modules in a more convenient form (list of sets)
// ONLY leaf-modules (not the 'higher-level' modules'
inline double modules(Cppn* g, std::vector<std::vector<Node*> > &mods)
{
	std::vector<std::string> modules;
	split(g, modules);

	typedef std::map<std::string, std::vector<Node*> > m_map_t;
	m_map_t m_map;
	size_t i = 0;

	foreach(Node* node, g->getNodes()){
		m_map[modules[i++]].push_back(node);
	}

	mods.resize(m_map.size());
	size_t k = 0;
	for (m_map_t::const_iterator it = m_map.begin(); it != m_map.end();++it)
	{
		for(std::vector<Node*>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
			mods[k].push_back((*it2));
		}
		++k;
	}

	float q = compute_modularity(g, modules);
	return q;
}

inline double modularity(Cppn* g)
{
	std::vector<std::vector<Node*> > mods;
	return modules(g, mods);
}
}

#endif
