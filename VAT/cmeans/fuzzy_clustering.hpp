#ifndef FUZZY_CLUSTERING 
#define FUZZY_CLUSTERING 1
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <stdlib.h>

namespace Clustering{
  
  typedef float Value;
  typedef unsigned int Index;
  typedef boost::numeric::ublas::vector<Value> Vector;
  typedef boost::numeric::ublas::zero_vector<Value> ZeroVector;
  typedef boost::numeric::ublas::matrix<Value> Matrix;  
  typedef boost::numeric::ublas::zero_matrix<Value> ZeroMatrix;  

  class Fuzzy{  
  public:

    //
    // Notation 
    // 
    //   i  -> is used for point 
    //   j  -> is used for cluster
    //   f  -> is used for dimension (feature)
    //
    Fuzzy(Matrix& rows, Index number_clusters, 
	  Value fuzziness = 1.1, Value epsilon=0.01):

      // fuzzyness
      m_(fuzziness),

      // epsislon
      epsilon_(epsilon),

      // number clusters
      number_clusters_(number_clusters),
      
      // number points
      number_points_(rows.size1()),

      // dimension of a point
      size_of_a_point_(rows.size2()),

      // we have num_clusters centroids 
      p_centroids_(new Matrix(number_clusters, size_of_a_point_)),
      //                      number_clusters X rows.size2()

      // each row is member of a cluster to some extent
      p_membership_(new Matrix(number_clusters_, number_points_)),
      //                       number_clusters X rows.size1() 

      // each row is member of a cluster to some extent
      p_new_membership_(new Matrix(number_clusters_, number_points_)),
      //                       number_clusters X rows.size1() 

      // preserve the dataset
      rows_(rows)
    {
      // random membership for each row 
      //
      float normalization_factor;

      for (Index i = 0 ; i < number_points_; i++){
	normalization_factor = 0.0;
	for (Index j = 0; j < number_clusters; j++)	
	  normalization_factor += 
	    (*p_membership_)(j, i) = (rand() / (RAND_MAX + 0.0));
	for (Index j = 0; j < number_clusters; j++)
	  (*p_membership_)(j, i) /= normalization_factor;
      }
      std::cout << "Fuzzy membership (n_clusters X n_points)" << std::endl 
		<< " " << (*p_membership_) << std::endl;

      computeCentroids();
    };

    // compute centroids
    //
    void computeCentroids();
    void computeCentroids2();


    // update membership  
    //  
    //  @param  Value fuzziness
    //
    bool updateMembership();

    // stop criteria
    //  
    //  @param epsilon
    // 
    inline bool can_stop()
    { 
      Value t = norm_1( (*p_membership_)-(*p_new_membership_) );
      std::cout << " norm t = " << t << std::endl;
      return  t < epsilon_; 
    }

    //
    // clustering
    // 
    inline void clustering(unsigned int num_iteration=100){

      unsigned int iteration = 0;
      while (!updateMembership() && iteration++ < num_iteration)
	computeCentroids2();
    }

  private:
    
    Value m_;                // fuzzyness
    Value epsilon_;                 // epsilon convergence

    Index number_clusters_;         // number clusters
    Index number_points_;           // number points in dataset
    Index size_of_a_point_;         // size of a single point

    Matrix * p_centroids_;          // centroids
    Matrix * p_membership_;         // membership
    Matrix * p_new_membership_;      // new membership
    Matrix& rows_;                  // dataset

  };
};
#endif

////     

