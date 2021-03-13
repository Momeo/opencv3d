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
		void translatePointCloud(cv::Mat R, cv::Mat T = cv::Mat::zeros(1,3,CV_32F))
		{
			// check R and T
			if (T.cols == 1 && T.rows == 3)
				T = T.t();
			cv::Size sz = points.size();
			cv::Mat TExpend;
			cv::repeat(T, NumPoints(), 1 , TExpend);
			cv::Mat P = points.colRange(0, 3);
			cv::Mat Pt = P * R + TExpend;
			points.colRange(0, 3) = Pt;
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
	//bool computeRT(vector<Point3f> A, vector<Point3f> B, Mat& R, Mat& T)
	//{
	//	if (A.size() != B.size())
	//		return false;
	//	Mat P_A(A);
	//	P_A = P_A.reshape(1, A.size());
	//	Mat P_B(B);
	//	P_B = P_B.reshape(1, B.size());
	//	Mat H = Mat::zeros(3, 3, CV_32F);
	//	/******* 中心化 ******/
	//	vector<float> centroid_A(3);
	//	vector<float> centroid_B(3);
	//	for (size_t i = 0; i < P_A.cols; i++)
	//	{
	//		Mat dataCol_A = P_A.colRange(i, i + 1);
	//		Scalar tmp_A = cv::mean(dataCol_A);
	//		centroid_A[i] = tmp_A[0];
	//		Mat dataCol_B = P_B.colRange(i, i + 1);
	//		Scalar tmp_B = cv::mean(dataCol_B);
	//		centroid_B[i] = tmp_B[0];
	//	}

	//	Mat Q_A(P_A.size(), CV_32F);
	//	Mat Q_B(P_B.size(), CV_32F);
	//	for (int i = 0; i < P_A.rows; i++)
	//	{
	//		Q_A.row(i) = P_A.row(i) - Mat(centroid_A).reshape(0, 1);
	//		Q_B.row(i) = P_B.row(i) - Mat(centroid_B).reshape(0, 1);
	//		H += Q_B.row(i).t() * Q_A.row(i);
	//	}
	//	/******* R ******/
	//	Mat U, W, V;
	//	SVD::compute(H, W, U, V);
	//	R = V.t() * U.t();
	//	T = Mat(centroid_A) - R * Mat(centroid_B);
	//	return true;
	//}


}
