#ifndef FCM_DEFINE_H
#define FCM_DEFINE_H

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/smart_ptr.hpp>
//#include <boost/fusion/include/deque.hpp>
//#include <boost/fusion/include/deque_fwd.hpp>
//#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/container.hpp>

// Data type.
typedef boost::numeric::ublas::vector<double> Vector;
typedef boost::numeric::ublas::zero_vector<double> ZeroVector;

typedef boost::numeric::ublas::matrix<double> Matrix;
typedef boost::numeric::ublas::zero_matrix<double> ZeroMatrix;

typedef boost::shared_ptr<Matrix> MatrixPtr;

typedef CArray<double, double&>		DoubleArray;

#define SUBCLUS_PROPORTION(a,b,c,d)		((a/b + c/d - 1) < 0.0)

enum DistanceFunction
{
	MANHATTAN_DIS = 0,
	EUCLIDEAN_DIS
};

static TCHAR BASED_CODE g_szFilter[]	= _T("Text Files (*.txt;*.dat)|*.txt;*.dat|")
										  _T("Worksheet Files (*.xls;*.csv)|*.xls;*.csv|")
										  _T("All Files (*.*)|*.*||");
const CString FILE_DATA_INFO			= _T("Instances : %d, Attributes : %d");

const CString FCM_DELIMITER				= _T(" ,");
const std::string FCM_LOGPATH			= "D:\\log.txt";

typedef struct _Distance2Objects
{
	int		iObj;
	int		jObj;
	double	distance;
} Distance2Objects;

#endif //FCM_DEFINE_H