#pragma once
#include <vector>
#include <opencv.hpp>


class EnergyMap;

class Match{
public:
	Match(int rows, int cols);

	enum Direction
	{
		LEFT = -1,
		MIDDLE,
		RIGHT
	};

	struct Node
	{
		int path;
		double value;
	};

	// return optimum seam coord and energy
	std::tuple<std::vector<std::pair<int, int>>,double> match(EnergyMap* map);

	// I didn't use this function, I thought it could be used in enlarging.
	std::vector<std::vector<std::pair<int, int>>> match_multi(EnergyMap* map, int k);


private:
	// find optimum seam with backward energy
	bool find(EnergyMap* map);

	// Try to implement forward energy, it doesn't seam to work.
	bool find2(EnergyMap* map);

	// decide orientation
	std::pair<int, double> transform(double a, double b, double c) const;
	std::pair<int, double> transform(double a, double b) const;

	std::vector<std::vector<Node>> table_;
	int rows_;
	int cols_;
};


/**abstract class as interface, with it we can apply this dynamic program to other similar problem

*/
class EnergyMap{
	friend class Match;
public:
	EnergyMap(){}
	virtual ~EnergyMap(){};

	enum class Forward
	{
		LEFT,
		VERTICAL,
		RIGHT,
		NONE,
	};

	double virtual get_energy(int i, int j) const=0;
	std::pair<double, double> virtual get_energy(int i, int j, Forward f) const = 0;
};



class Seam:public EnergyMap
{
public:
	Seam(const cv::Mat& src);
	double get_energy(int i, int j) const final;
	std::pair<double, double> get_energy(int i, int j, Forward f) const final;

	// useful in debugging 
	void show(const std::vector<std::pair<int, int>>& path, cv::Mat& src) const;
	void show(const std::vector<std::vector<std::pair<int, int>>>& path, cv::Mat& src) const;
	void show_energy() const;

private:

	// build energy map, concretly use image gradient magnitude.
	void create_energy(const cv::Mat& src);

	int rows_;
	int cols_;
	cv::Mat gradient_;
	cv::Mat intensity_;

};

