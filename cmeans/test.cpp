#include "fuzzy_clustering.hpp"

using namespace Clustering;

int main(){

  static unsigned int number_points = 17;
  static unsigned int dimension_point = 4;
  static unsigned int number_clusters = 3;
  Matrix dataset(number_points, dimension_point);

  for (unsigned int i=0; i < number_points; i++)
    for (unsigned int j=0; j < dimension_point; j++)
      dataset(i, j) = int(rand() % 100);

  std::cout << "Points (n_points X dim_point)"
	    <<std::endl<< " " <<dataset << std::endl;
 
  Fuzzy f(dataset, number_clusters);

  f.clustering();
}
