#include "fuzzy_clustering.hpp"
#include <boost/foreach.hpp>

namespace Clustering{

  void Fuzzy::computeCentroids(){

      //  compute the centroid on random assignment above
      //    the centroid of a cluster is the mean of all the points,
      //    weighted by their degree of belonging to the cluster
      //
      *p_centroids_ = prod (*p_membership_,        rows_);
      //   n_clusters   =      n_clusters          rows.size1()
      // X [rows.size2()=      X [rows.size1()=    X [rows.size2=
      //    =size_of_a_point_]    =number_points_]    size_of_a_point]  

      //std::cout << "Centroids (n_clusters X dim_point)" 
      //		<<std::endl << " " << (*p_centroids_) << std::endl;
      
      Vector sum_uk = ZeroVector(number_clusters_);
      for (Index j = 0; j < number_clusters_; j++)
	for (Index i = 0 ; i < number_points_; i++)
	  sum_uk[j] += (*p_membership_)(j, i);
      
      for (Index j = 0; j < number_clusters_; j++)
	for (Index f = 0 ; f < size_of_a_point_; f++)
	  (*p_centroids_)(j, f) /= sum_uk[j];

      //std::cout << "normalizing for " << sum_uk << std::endl;      
      std::cout << "Centroids (n_clusters X dim_point)" 
		<<std::endl << " " << (*p_centroids_) << std::endl;
  }


  void Fuzzy::computeCentroids2(){

      //  compute the centroid on random assignment above
      //    the centroid of a cluster is the mean of all the points,
      //    weighted by their degree of belonging to the cluster
      //
    
    // compute sum_i=1...N  u_ij^m 
    //
    Matrix u_ji_m(number_clusters_, number_points_);
    Value normalization;

    for (Index j = 0; j < number_clusters_; j++)
      for (Index f = 0; f < size_of_a_point_; f++)
	(*p_centroids_)(j, f) = 0.0;	

    // for each cluster
    for (Index j = 0; j < number_clusters_; j++)

      // for each point
      for (Index i = 0 ; i < number_points_; i++)
	
	// compute u_ij_m
	u_ji_m(j, i) = pow ( (*p_membership_)(j, i), m_);


    for (Index j = 0; j < number_clusters_; j++)
      for (Index i = 0 ; i < number_points_; i++)	
	for (Index f = 0; f < size_of_a_point_; f++)
	  (*p_centroids_)(j, f) += u_ji_m(j, i) * rows_(i, f);
	
    for (Index j = 0; j < number_clusters_; j++){
      
      normalization = 0.0;
      for (Index i = 0 ; i < number_points_; i++) 
	normalization += u_ji_m(j,i);
	
      for (Index f = 0; f < size_of_a_point_; f++)
	(*p_centroids_)(j, f) /= normalization;
    }

    std::cout << "normalizing for " << normalization << std::endl;      
    std::cout << "Centroids (n_clusters X dim_point)" 
	      <<std::endl << " " << (*p_centroids_) << std::endl;
  }


  // update membership  
  //  
  //     
  bool Fuzzy::updateMembership(){

    Matrix matrix_norm_one_xi_minus_cj(number_points_, number_clusters_);

    for (unsigned int i = 0 ; i < number_points_; i++)
      for (unsigned int j = 0; j < number_clusters_; j++)
	matrix_norm_one_xi_minus_cj(i, j) = 0.0;

    // for each point
    for (unsigned int i = 0 ; i < number_points_; i++)
      // for each cluster
      for (unsigned int j = 0; j < number_clusters_; j++)
	// for each feature
	for (unsigned int f = 0; f < size_of_a_point_; f++){

	  // x_i-c_j              x_i           c_j
	  matrix_norm_one_xi_minus_cj(i, j) += 
	    abs(rows_(i, f) - (*p_centroids_)(j, f));

	  //std::cout << "matrix_norm_one_xi_minus_cj("<<i<<","<<j<<")="
	  //	    << "rows_("<<i<<", "<<f<<")=" << rows_(i, f) 
	  //    << " (*p_centroids_)("<<j<<", "<<f
	  //    <<")=" << (*p_centroids_)(j, f)
	  //    << std::endl;
	}

    float coeff;
    // for each point
    for (unsigned int i = 0 ; i < number_points_; i++)
      // for each cluster
      for (unsigned int j = 0; j < number_clusters_; j++){
	// for each cluster
	coeff = 0.0;
	for (unsigned int k = 0; k < number_clusters_; k++)
	  
	  coeff+= 
	    pow ( (matrix_norm_one_xi_minus_cj(i, j) /
		   matrix_norm_one_xi_minus_cj(i, k)) , 
		  2 / (m_ - 1) );

	(*p_new_membership_)(j, i) = 1 / coeff;
      }

    std::cout << "New membership " << (*p_new_membership_) << std::endl;
	
    if (!can_stop()){
      (*p_membership_) = (*p_new_membership_);  // switch matrices
      return false; 
    }
    
    return true;
  }
};
