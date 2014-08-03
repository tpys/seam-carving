// seam carving.cpp : Defines the entry point for the console application.
//


#include "dynamic.h"

using namespace cv;
using namespace std;


void vertical_seam_carving(const Mat& src, Mat& dst, int col);
void horizontal_seam_carving(const Mat& src, Mat& dst, int row);
void manual_order_seam_carving(const Mat& src, Mat& dst, int row, int col);
void content_amplification(const Mat& src, Mat& dst);


int main(int argc, char** argv)
{

	if (argc != 3) return -1;

	const string image_name = argv[1];
	Mat src = imread(image_name);
	if (!src.data) return -1;
	
	Mat dst;
	const string mode = argv[2];

	if (mode == "-v")
		vertical_seam_carving(src, dst, 0.5*src.cols); // perform vertical seam carving
	else if (mode == "-h")
		horizontal_seam_carving(src, dst, 0.5*src.cols);
	else if (mode == "-b")
		manual_order_seam_carving(src, dst, 0.5*src.cols, 0.5*src.cols);
	else if (mode=="-a")
		content_amplification(src,dst);

	imwrite(mode+".png",dst);

	namedWindow("before");
	imshow("before", src);
	namedWindow("after");
	imshow("after", dst);
	waitKey(0);

	return 0;
}


/**vertical seam carving
	-input: image 'src', target column size 'col'
	-output: 'dst'
*/
void vertical_seam_carving(const Mat& src, Mat& dst, int col)
{
	if (col == src.cols) return;

	int remove_num = src.cols - col;
	src.copyTo(dst);
	Mat display;
	src.copyTo(display);

	for (int i = 0; i < remove_num;++i)
	{
		Seam seam(dst);
		//seam.show_energy();
		EnergyMap* energy = &seam;
		Match dynamic_match(dst.rows, dst.cols);
		auto path = get<0>(dynamic_match.match(energy));
		//seam.show(path, dst);

		for (auto& e : path)
		{
			for (int c = e.second; c < dst.cols - 1; ++c)
			{
				dst.at<Vec3b>(e.first, c) = dst.at<Vec3b>(e.first, c + 1);
			}
		}

		dst = dst(Rect(0, 0, dst.cols - 1, dst.rows));
	}

}


/**horizontal seam carving
	-input: image 'src', target row size 'row'
	-output: 'dst'
*/
void horizontal_seam_carving(const Mat& src, Mat& dst, int row)
{
	transpose(src, dst);
	flip(dst, dst, 1);

	vertical_seam_carving(dst, dst, row);

	flip(dst, dst, 1);
	transpose(dst, dst);
}


/**seam carving on both direction
	-input: image 'src', target image size 'row' and 'col'
	-output: 'dst'
*/
void manual_order_seam_carving(const Mat& src, Mat& dst, int row, int col)
{
	vertical_seam_carving(src, dst, col);
	horizontal_seam_carving(dst, dst, row);
}


/**content amplification
	-input: image 'src'
	-output: 'dst'
*/
void content_amplification(const Mat& src, Mat& dst)
{
	resize(src, dst, Size(1.5*src.cols, 1.5*src.rows));
	manual_order_seam_carving(dst, dst, src.rows, src.cols);
}


////////////////////////not finished function ///////////////////////////////////////////////////

/**enlarging image
	-input: image 'src', increased column 'add_cols'
	-output: 'dst'
*/
void enlarging(const Mat& src, Mat& dst, int add_cols)
{
	src.copyTo(dst);
	vector<vector<pair<int, int>>>  all_path;

	// find first k seams
	for (int i = 0; i < add_cols; ++i)
	{
		Seam seam(dst);
		EnergyMap* energy = &seam;
		Match dynamic_match(dst.rows, dst.cols);
		auto path = get<0>(dynamic_match.match(energy));
		all_path.push_back(path);

		for (auto& e : path)
		{
			for (int c = e.second; c < dst.cols - 1; ++c)
			{
				dst.at<Vec3b>(e.first, c) = dst.at<Vec3b>(e.first, c + 1);
			}
		}
		dst = dst(Rect(0, 0, dst.cols - 1, dst.rows));
	}


	// duplicate them by order
	sort(begin(all_path), end(all_path), [](vector<pair<int, int>>& a, vector<pair<int, int>>& b){return a[0].second == b[0].second; });
	Mat large(src.rows,src.cols+add_cols,CV_8UC3);

	for (int i = 0; i < all_path.size();++i)
	{


	}
}


/**same functional as manual_order_seam_carving, but with optimal choose removed seam order.
	-input: image 'src', increased column 'add_cols'
	-output: 'dst'
*/
void optimal_order_seam_carving(const Mat& src, Mat& dst, int row, int col)
{
	src.copyTo(dst);

	int r = dst.rows - row;
	int c = dst.cols - col;

	enum Order
	{
		START = -1,
		LEFT, // represent column remove operation
		UP,  // represent row remove operation
	};

	vector<vector<pair<double, int>>>  order_map(r);
	for (auto& e : order_map)
	{
		e = vector<pair<double, int>>(c);
	}

	Mat col_reduced;
	Mat row_reduced;

	// fill in borders
	//auto it
	//for (int j = 0; j < col;++j)
	//{
	//	order_map[0][j].first=
	//}

	// fill in internal
	for (int i = 0; i < r; ++i)
	{
		for (int j = 0; j < c; ++j)
		{


		}
	}

}


// helper function 
tuple<vector<pair<int, int>>, double> vertical_match(const Mat& src)
{
	Seam seam(src);
	EnergyMap* energy = &seam;
	Match dynamic_match(src.rows, src.cols);
	return dynamic_match.match(energy);
}


// helper function
tuple<vector<pair<int, int>>, double>horizontal_match(const Mat& src)
{
	Mat dst;
	src.copyTo(dst);

	transpose(dst, dst);
	flip(dst, dst, 1);

	Seam seam(dst);
	EnergyMap* energy = &seam;
	Match dynamic_match(dst.rows, dst.cols);

	return dynamic_match.match(energy);
}
