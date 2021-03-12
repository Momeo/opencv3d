#include <iostream>  
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include <numeric>
#include <direct.h>

#include <opencv2/opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/calib3d.hpp>

namespace opencv3d {

	// Point Types:
	// PointXYZ - float x, y, z
	// PointXYZI - float x, y, z, intensity
	// PointXYZRGB - float x, y, z, rgb
	enum PointType {
		PointXYZ = 0,
		PointXYZI,
		PointXYZRGB
	};
	class PointCloud {
	private:
		PointType m_PointType;
	public:
		PointCloud(PointType);
		cv::Mat points;
		int NumPoints() {
			return points.rows;
		}
		int NumCols() {
			return points.cols;
		}
		PointType Type() {
			return m_PointType;
		}
	};
	//template <typename PointT>
	//PointCloud<PointT>::PointCloud()
	//{
	//	if(std::is_same<PointT, int>::value)
	//}
	PointCloud::PointCloud(PointType pType)
	{
		m_PointType = pType;
		int features = 0;
		switch (pType)
		{
		case PointXYZ:
			features = 3;
			break;
		case PointXYZI:
			features = 4;
			break;
		case PointXYZRGB:
			features = 6;
			break;
		default:
			break;
		}
		points = cv::Mat::zeros(0, features, CV_32F);
	}


}
