#include "dynamic.h"
#include <algorithm>
#include <tuple>

using namespace std;
using namespace cv;

Match::Match(int rows, int cols) :rows_(rows), cols_(cols), table_(vector<vector<Node>>(rows))
{
	for (auto& e:table_)
	{
		e = vector<Node>(cols);
	}
}


tuple <vector<pair<int, int>>,double> Match::match(EnergyMap* map)
{

	if (find(map))
	{
		vector<pair<int, int>> path;

		auto it = std::min_element(begin(table_[rows_ - 1]), end(table_[rows_ - 1]), [](Node& a, Node& b){return a.value < b.value; });

		int col = it - begin(table_[rows_ - 1]);
		path.push_back(make_pair(rows_ - 1, col));

		for (int r = rows_ - 2; r >= 0;--r)
		{
			int d = table_[r+1][col].path;
			col += d;
			path.push_back(make_pair(r, col));
		}

		return make_tuple(path,it->value);
	}

	else return{};
}



std::vector<std::vector<std::pair<int, int>>> Match::match_multi(EnergyMap* map, int k)
{
	if (find(map))
	{
		vector<vector<pair<int, int>>> k_paths;
		vector<pair<int, int>> path;

		auto rank = table_[rows_ - 1];
		sort(begin(rank), end(rank), [](Node& a, Node& b){return a.value < b.value; });

		for (int i = 0; i < k; ++i)
		{
			double value = rank[i].value;
			auto it = find_if(begin(table_[rows_ - 1]), end(table_[rows_ - 1]), [value](Node& a){return a.value == value; });
			int col = it - begin(table_[rows_ - 1]);
			path.push_back(make_pair(rows_ - 1, col));
			for (int r = rows_ - 2; r >= 0; --r)
			{
				int d = table_[r + 1][col].path;
				col += d;
				path.push_back(make_pair(r, col));
			}
			k_paths.push_back(path);
		}
		return k_paths;
	}
		
	else return{};
}


bool Match::find(EnergyMap* map)
{
	if (rows_ <=0 || cols_ <= 0) return false;
	
	for (int r = 0; r < rows_;++r)
	for (int c = 0; c < cols_; ++c)
	{
		if (r == 0)
		{
			table_[r][c].value = map->get_energy(r, c);
			table_[r][c].path = Direction::MIDDLE;
		}
		else
		{
			if (c == 0)
			{
				int t = transform(table_[r - 1][c].value, table_[r - 1][c + 1].value).first;
				table_[r][c].value = map->get_energy(r, c) + table_[r - 1][c + t].value;
				table_[r][c].path = t;
			}
			else if (c == cols_ - 1)
			{
				int t = transform(table_[r - 1][c-1].value, table_[r - 1][c].value).first;
				t -= 1;
				table_[r][c].value = map->get_energy(r, c) + table_[r - 1][c + t].value;
				table_[r][c].path = t;
			}
			else
			{
				int t = transform(table_[r - 1][c - 1].value, table_[r - 1][c].value, table_[r - 1][c + 1].value).first;

				table_[r][c].value = map->get_energy(r, c) + table_[r - 1][c + t].value;
				table_[r][c].path = t;
			}
		}
	}

	return true;
}

bool Match::find2(EnergyMap* map)
{
	if (rows_ <= 0 || cols_ <= 0) return false;

	for (int r = 0; r < rows_; ++r)
	for (int c = 0; c < cols_; ++c)
	{
		auto intensity = map->get_energy(r, c, EnergyMap::Forward::NONE);
		if (r == 0)
		{
			table_[r][c].value = intensity.first;
			table_[r][c].path = Direction::MIDDLE;
		}
		else
		{
			if (c == 0)
			{
				auto a1 = map->get_energy(r, c, EnergyMap::Forward::VERTICAL);
				auto a2 = map->get_energy(r, c, EnergyMap::Forward::RIGHT);

				auto t = transform(table_[r - 1][c].value + a1.second, table_[r - 1][c + 1].value + a2.second);

				table_[r][c].value = intensity.first + t.second;
				table_[r][c].path = t.first;

			}
			else if (c == cols_ - 1)
			{
				auto a1 = map->get_energy(r, c, EnergyMap::Forward::LEFT);
				auto a2 = map->get_energy(r, c, EnergyMap::Forward::VERTICAL);

				auto t = transform(table_[r - 1][c - 1].value + a1.second, table_[r - 1][c].value + a2.second);
				table_[r][c].value = intensity.first + t.second;
				table_[r][c].path = t.first - 1;
			}
			else
			{
				auto a1 = map->get_energy(r, c, EnergyMap::Forward::LEFT);
				auto a2 = map->get_energy(r, c, EnergyMap::Forward::VERTICAL);
				auto a3 = map->get_energy(r, c, EnergyMap::Forward::RIGHT);

				auto t = transform(table_[r - 1][c - 1].value + a1.second, table_[r - 1][c].value + a2.second, table_[r - 1][c + 1].value + a3.second);

				table_[r][c].value = intensity.first + t.second;
				table_[r][c].path = t.first;
			}
		}
	}

	return true;
}



pair<int, double> Match::transform(double a, double b) const
{
	return a < b ? make_pair(MIDDLE, a) : make_pair(RIGHT, b);
}

pair<int, double> Match::transform(double a, double b, double c) const
{
	return a < b ? (a < c ? make_pair(LEFT,a) : make_pair(RIGHT,c)) : (b < c ? make_pair(MIDDLE,b) : make_pair(RIGHT,c));
}


Seam::Seam(const cv::Mat& src) :rows_(src.rows), cols_(src.cols)
{
	create_energy(src);
}


double Seam::get_energy(int r, int c) const
{
	return gradient_.at<uchar>(r,c);
}


pair<double, double> Seam::get_energy(int r, int c, Forward f) const
{
	double cost = 0;

	if (f == Forward::LEFT)
	{
		if (c == cols_ - 1)
			cost = abs(intensity_.at<uchar>(r - 1, c) - intensity_.at<uchar>(r, c - 1)) + intensity_.at<uchar>(r, c - 1);
		else
			cost = abs(intensity_.at<uchar>(r, c + 1) - intensity_.at<uchar>(r, c - 1)) +
				abs(intensity_.at<uchar>(r - 1, c) - intensity_.at<uchar>(r, c - 1));
	}

	else if (f == Forward::VERTICAL)
	{
		if (c == 0)
			cost = intensity_.at<uchar>(r, c + 1);
		else if (c == cols_ - 1)
			cost = intensity_.at<uchar>(r, c - 1);
		else 
			cost = abs(intensity_.at<uchar>(r, c + 1) - intensity_.at<uchar>(r, c - 1));
	}

	else if (f == Forward::RIGHT)
	{
		if (c == 0)
			cost = abs(intensity_.at<uchar>(r - 1, c) - intensity_.at<uchar>(r, c + 1)) + intensity_.at<uchar>(r, c + 1);
		else
			cost = abs(intensity_.at<uchar>(r, c + 1) - intensity_.at<uchar>(r, c - 1)) +
				abs(intensity_.at<uchar>(r - 1, c) - intensity_.at<uchar>(r, c + 1));
	}
	else
	{
		;
	}

	return make_pair(intensity_.at<uchar>(r, c), cost);
}

void Seam::create_energy(const cv::Mat& src)
{
	int depth = CV_16S;

	GaussianBlur(src, intensity_, Size(3, 3), 0, 0);
	cvtColor(src, intensity_, COLOR_BGR2GRAY);

	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	Sobel(intensity_, grad_x, depth, 1, 0); // dx
	convertScaleAbs(grad_x, abs_grad_x); // |dx|
	Sobel(intensity_, grad_y, depth, 0, 1); // dy
	convertScaleAbs(grad_y, abs_grad_y); // |dy|
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradient_);
}

void Seam::show(const std::vector<std::pair<int, int>>& path, Mat& src) const
{
	for (auto& e:path)
	{
		circle(src, Point(e.second, e.first), 0.5, Scalar(0, 0, 255));
	}

	namedWindow("seam");
	imshow("seam", src);
	waitKey(0);
}

void Seam::show(const std::vector<std::vector<std::pair<int, int>>>& path, cv::Mat& src) const
{
	for (auto& a:path)
	{
		for (auto& b:a)
		{
			circle(src, Point(b.second, b.first), 0.5, Scalar(0, 0, 255));
		}
	}

	namedWindow("seam");
	imshow("seam", src);
	waitKey(0);
}



void Seam::show_energy() const
{
	namedWindow("energy");
	imshow("energy", gradient_);
	waitKey(0);
}