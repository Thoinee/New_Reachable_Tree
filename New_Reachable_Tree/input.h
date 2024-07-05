#pragma once
/**
 * ��ȡ��ʼ��ʶm0, ��ʱdelay, ǰ/���ù�������pre/post
 */

 /* ���������� */
constexpr auto kInitMark = "./data/basic/899/m0.txt";
constexpr auto kGoalMarking = "./data/basic/899/GoalMarking.txt";

/* ���ݼ�txt ���ӻ�gv ��Ϣinfo�ļ� */
constexpr auto kOutputTxtPath = "./data/Search/899/Dataset.txt";
constexpr auto kOutputGvPath = "./data/Search/899/Reachable_Graph.gv";
constexpr auto kInfoPath = "./data/899/Info.txt";

/* �������������� */
constexpr auto kPrePath = "./data/basic/pre.txt";
constexpr auto kPostPath = "./data/basic/post.txt";
constexpr auto kTimePath = "./data/basic/delay.txt";
constexpr auto kGoalPlace = "./data/basic/GoalPlace.txt";

/* ������������ */
//constexpr auto kPrePath = "./data/controller/pre.txt";
//constexpr auto kPostPath = "./data/controller/post.txt";
//constexpr auto kTimePath = "./data/controller/delay.txt";
//constexpr auto kGoalPlace = "./data/basic/GoalPlace.txt";

#include <iostream>
#include <fstream>
#include <vector> 
#include <string>

using std::vector;
template<class T>
/* ��ȡ��ʼ��ʶ����ʱ */
vector<T> readVector(const char* path) {
	T num;
	vector<T> result;
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cout << "fail to oepn file :" << path << std::endl;
		return {};
	}

	while (file >> num) {
		result.push_back(num);
	}

	file.close();
	return result;
}

/* ��ȡ�������� */
vector<vector<int>> readMatrix(const char* path, int m) {
	vector<vector<int>> matrix;
	std::ifstream file(path, std::ios::in);

	if (!file.is_open()) {
		std::cout << "can not open file:" << path << std::endl;
		exit(1);
	}

	int num = 0;
	vector<int> ans;

	while (file >> num) {
		ans.push_back(num);
	}

	int n = ans.size() / m;
	matrix.resize(m, vector<int>(n, 0));

	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			matrix[i][j] = ans[i*n + j];
		}
	}

	file.close();
	return matrix;
}

/* ��ȡĿ��ڵ��ʶ */
vector<int> getGoalMark(vector<int>& goalMarking, vector<int>& goalPlace, int nums) {
	vector<int> ans(nums, 0);

	if (goalMarking.size() != goalPlace.size()) {
		std::cout << "The dimension of goal marking and goal place is not consistent!" << std::endl;
		exit(-1);
	}

	for (int i = 0; i < goalPlace.size(); ++i) {
		ans[goalPlace[i]] = goalMarking[i];
	}

	return ans;
}
