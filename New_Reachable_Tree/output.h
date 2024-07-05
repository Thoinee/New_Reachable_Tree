#pragma once
/**
 * ��������txt�����ӻ��ļ�gv
 */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <queue>
#include <iomanip>
#include <functional>
#include <mutex>
#include "petri.h"
#include "input.h"

static std::mutex mtx;

/* ԭģ��������ʡ�ԵĿ��� */
std::unordered_set<int> ignore_m = {
	26, 27, 28
};

std::unordered_set<int> ignore_v = {
	0, 4, 13, 19, 20, 21, 22, 23,
	24, 25, 26, 27, 28
};

/* �޸ĺ�ģ��������ʡ�ԵĿ��� */
//static std::unordered_set<int> ignore_m = { 
//	26, 27, 28, 29, 30, 31, 32, 33,
//	34, 35, 36, 37, 38, 39, 40, 41,
//	42, 43, 44, 45, 46 
//};
//static std::unordered_set<int> ignore_v = {
//	0, 4, 13, 19, 20, 21, 22, 23,
//	24, 25, 26, 27, 28, 29, 30, 31,
//	32, 33, 34, 35, 36, 37, 38, 39,
//	40, 41, 42, 43, 44, 45, 46
//};

/* ���������Ʊ��� */
static float max_accept_multiple = 1;
/* tree_nodes_num: �ɴ�ͼ�ڵ�����  total_nodes_num: ���������� */
static unsigned long tree_nodes_num, total_nodes_num;

/* ����ȫ�ֿɴ�ͼ */
void globalGraphCreate(PetriNet& tree) {
	/* ���� */
	mtx.lock();
	/* ���ݱ�����ֵ */
	auto threshold_value = max_accept_multiple * tree.root_->h_;
	/* ͼ�ڵ������ */
	std::ofstream file(kOutputGvPath);

	//std::unordered_set<int> goals;
	//std::for_each(tree.goal_nodes_.begin(), tree.goal_nodes_.end(), [&](ptrNode& node) {
	//	if (goals.find(node->g_) == goals.end()) {
	//		goals.emplace(node->g_);
	//	}
	//	});

	file << "digraph {" << '\n';
	/* ���ýڵ� */
	//for (auto& node : tree.goal_nodes_) {
	//	//if (goals.count(node->g_ + node->h_)) {
	//	if (node->g_ + node->h_ <= threshold_value) {
	//		file << "\""
	//			<< "m" << node->id_
	//			<< "_g" << node->g_
	//			<< "_h" << node->h_
	//			<< "\""
	//			<< " [color=red style=filled]"
	//			<< "\n";
	//	}
	//}
	//for (auto& nodes : tree.entire_list_) {
	//	for (auto& node : nodes.second) {
	//		//if(goals.count(node->g_ + node->h_)) {
	//		if (node->g_ + node->h_ <= threshold_value) {
	//			if (tree.deadlock_nodes_.count(node->id_) > 0) {
	//				file << "\""
	//					<< "m" << node->id_
	//					<< "_g" << node->g_
	//					<< "_h" << node->h_
	//					<< "\""
	//					<< " [color=blue]"
	//					<< "\n";
	//			}
	//			else if (tree.leaf_nodes_.count(node->id_) > 0) {
	//				file << "\""
	//					<< "m" << node->id_
	//					<< "_g" << node->g_
	//					<< "_h" << node->h_
	//					<< "\""
	//					<< " [color=green]"
	//					<< "\n";
	//			}
	//			else {
	//				file << "\""
	//					<< "m" << node->id_
	//					<< "_g" << node->g_
	//					<< "_h" << node->h_
	//					<< "\""
	//					<< '\n';
	//			}
	//			for (auto& n1 : node->sons_) {
	//				if (tree.leaf_nodes_.count(n1.second->id_) > 0) {
	//					file << "\""
	//						<< "m" << n1.second->id_
	//						<< "_g" << n1.second->g_
	//						<< "_h" << (n1.second->h_ < 9999 ?
	//							n1.second->h_ : tree.root_->h_ * 1.05 - n1.second->g_)
	//						<< "\""
	//						<< " [color=green]"
	//						<< "\n";
	//				}
	//				else {
	//					file << "\""
	//						<< "m" << n1.second->id_
	//						<< "_g" << n1.second->g_
	//						<< "_h" << (n1.second->h_ < 9999 ?
	//							n1.second->h_ : tree.root_->h_ * 1.05 - n1.second->g_)
	//						<< "\""
	//						<< '\n';
	//				}
	//				file << "\""
	//					<< "m" << node->id_
	//					<< "_g" << node->g_
	//					<< "_h" << node->h_
	//					<< "\""
	//					<< " -> "
	//					<< "\""
	//					<< "m" << n1.second->id_
	//					<< "_g" << n1.second->g_
	//					<< "_h" << (n1.second->h_ < 9999 ?
	//						n1.second->h_ : tree.root_->h_ * 1.05 - n1.second->g_)
	//					<< "\""
	//					<< " [label=t" << std::get<0>(n1.second->fathers[0]) << "]\n";
	//			}
	//		}
	//	}
	//}


	/* ���ýڵ������ӻ� */
	/* ˼�룺�Ӹýڵ��ҵ��丸�ڵ� ���������� */
	for (auto& node : tree.goal_nodes_) {
		if (node->h_ < 9999) {
			for (auto& father : node->fathers) {
				auto f_node = std::get<3>(father);
				file << "\""
					<< "m" << f_node->id_
					<< "_g" << f_node->g_
					<< "_h" << f_node->h_
					<< "\""
					<< " -> "
					<< "\""
					<< "m" << node->id_
					<< "_g" << node->g_
					<< "_h" << node->h_
					<< "\""
					<< " [label=t" << std::get<0>(father) << "]\n";
			}
		}
	}
	for (auto& nodes : tree.entire_list_) {
		for (auto& node : nodes.second) {
			if (node->h_ < 9999) {
				for (auto& father : node->fathers) {
					auto f_node = std::get<3>(father);
					if (f_node->h_ < 9999) {
						file << "\""
							<< "m" << f_node->id_
							<< "_g" << f_node->g_
							<< "_h" << f_node->h_
							<< "\""
							<< " -> "
							<< "\""
							<< "m" << node->id_
							<< "_g" << node->g_
							<< "_h" << node->h_
							<< "\""
							<< " [label=t" << std::get<0>(father) << "]\n";
					}
				}
			}
		}
	}
	file << "}";
	file.close();
	/* ���� */
	mtx.unlock();
}

/* �������Ŀ��·�� */
void graphCreate(PetriNet& tree) {
	std::queue<ptrNode> back_node;
	std::unordered_set<unsigned short> already;
	short num_of_goals = 0;
	std::ofstream ofs(kOutputGvPath, std::ios::out);
	ofs << "digraph {" << '\n';
	for (auto& node : tree.goal_nodes_) {
		back_node.push(node);
	}
	while (!back_node.empty()) {
		auto node = back_node.front();
		back_node.pop();

		if (num_of_goals < tree.goal_nodes_.size()) {
			num_of_goals++;
			ofs << "m" << node->id_
				<< "_g" << node->g_
				<< "_h" << node->h_
				<< " [color=red, style=filled]"
				<< "\n";
		}
		else {
			ofs << "m" << node->id_
				<< "_g" << node->g_
				<< "_h" << node->h_
				<< "\n";
		}

		if (node->fathers.empty()) {
			continue;
		}

		auto f_node = std::get<3>(node->fathers[0]);
		ofs << "m" << f_node->id_
			<< "_g" << f_node->g_
			<< "_h" << f_node->h_
			<< " -> "
			<< "m" << node->id_
			<< "_g" << node->g_
			<< "_h" << node->h_
			<< " [label=t" << std::get<0>(node->fathers[0])
			<< "]\n";
		if (already.count(f_node->id_) == 0) {
			back_node.push(f_node);
			already.emplace(f_node->id_);
		}
	}
	ofs << "}";
	ofs.close();
}

template <typename T>
/* ת��Ϊ���ݼ���ʽ */
vector<T> toVector(const ptrNode node, const int num_place, const vector<int>& delay) {   // �ڵ�ָ��  PetriNet��������
	auto places = node->state_;
	int j = 0;   // ���Ѹ�ֵ�Ŀ������м���
	std::vector<T> ans;
	for (int i = 0; i < num_place; ++i) {   //  �������п��������и�ֵ
		if (ignore_m.count(i) >= 1)    // ���鵽��ignore_m�д洢�����ÿ���������������ֵ
			continue;
		if (j < places.size()) {       // ����ֵδ�����踳ֵ�Ŀ�����
			if (i < places[j].row_) {  // ��state_��δ��¼�Ŀ�������0
				ans.push_back(0);
			}
			else if (i == places[j].row_) {      // ����state_�а�����place����token�����и�ֵ
				ans.push_back(places[j].tokens_);
				++j;   // ����ֵ+1
			}
			else {     // ����������continue��������޸�ֵ��i�Զ���1������iλ�ÿճ���ignore_m�а�������������state_�У�
				--i;   // ��������--i������λ�ã�++j���Ϻ��е�place����ֵ������j�޷�==����token�Ŀ�����
				++j;
			}
		}
		else {         // ������ֵ=����token�Ŀ���������ʣ��Ŀ���ȫ��0
			ans.push_back(0);
		}
	}
	j = 0;

	/* ������ͬ�� */
	for (int i = 0; i < num_place; ++i) {
		if (ignore_v.count(i) >= 1)
			continue;
		if (j < places.size()) {
			if (i < places[j].row_) {
				ans.insert(ans.end(), 2, 0);
			}
			else if (i == places[j].row_) {
				ans.insert(ans.end(), places[j].v_.begin(), places[j].v_.end());
				++j;
			}
			else {
				--i;
				++j;
			}
		}
		else {
			ans.insert(ans.end(), 2, 0);
		}
	}
	j = 0;

	/* ������ʱ��Ϣ */
	for (int i = 0; i < num_place; ++i) {
		if (ignore_v.count(i) >= 1)
			continue;
		if (j < places.size()) {
			if (i < places[j].row_) {
				ans.push_back(0);
			}
			else if (i == places[j].row_) {
				ans.push_back(delay[i]);
				++j;
			}
			else {
				--i;
				++j;
			}
		}
		else {
			ans.push_back(0);
		}
	}

	return ans;
}

/* ����Ŀ��·�����丽�������� */
void dataCreateTxt(PetriNet& tree, const int num_place) {
	/* ���� */
	mtx.lock();
	/* ���ݼ������ */
	std::ofstream file(kOutputTxtPath);
	/* ������λС����ʽ */
	file << std::setiosflags(std::ios::fixed) << std::setprecision(2);

	// ���������Ľڵ���
	std::unordered_set<unsigned int> uset;
	// �������·���ϵĽڵ�
	std::queue<ptrNode> datas;
	datas.push(tree.root_);

	auto Generate = [&](ptrNode n) {
		static bool flag = false;
		vector<float> vec = toVector<float>(n, num_place, tree.delay_);

		if (!flag) {
			flag = true;
		}
		else {
			file << '\n';
		}

		for (auto& v : vec) {
			file << v << '\t';
		}

		file << (n->h_ < 9999 ?
			n->h_ : tree.root_->h_ * 1.05 - n->g_);

		++total_nodes_num;
	};

	// ��ʼ��
	Generate(tree.root_);

	while (!datas.empty()) {
		// ȡ���ڵ�
		auto node = datas.front();
		datas.pop();

		for (auto son : node->sons_) {
			if (uset.count(son.second->id_)) {
				continue;
			}

			if (son.second->g_ + son.second->h_ == tree.root_->h_) {
				datas.push(son.second);
			}

			// ���һ������
			Generate(son.second);
			uset.insert(son.second->id_);
		}
	}

	file.close();
	/* ���� */
	mtx.unlock();
}

/* ����ȫ������ */
void dataCreateAllTxt(PetriNet& tree, const int num_place) {
	/* ���� */
	mtx.lock();
	/* ���ݼ������ */
	std::ofstream file(kOutputTxtPath);
	/* ������λС����ʽ */
	file << std::setiosflags(std::ios::fixed) << std::setprecision(2);

	auto OutputData = [&](ptrNode node, bool flag = true) {
		/* ת��Ϊ���ݸ�ʽ */
		auto vec = toVector<float>(node, num_place, tree.delay_);
		for (auto v : vec) {
			file << v << '\t';
		}
		file << (flag ? node->h_ : tree.root_->h_ * 1.05 - node->g_);
		/* ���� */
		++total_nodes_num;
	};

	bool begin = true;
	/* Ŀ��ڵ� */
	for (auto& node : tree.goal_nodes_) {
		/* ��ʼ����Ҫ���� */
		if (!begin) {
			file << '\n';
		}
		else {
			begin = false;
		}
		/* ������� */
		OutputData(node);
	}

	/* ����ڵ� */
	for (auto& nodes : tree.entire_list_) {
		for (auto& node : nodes.second) {
			if (node->h_ > 9999) {
				/* �鿴�丸�ڵ��Ƿ�������·���� */
				bool flag = std::any_of(
					node->fathers.begin(), node->fathers.end(),
					[&](decltype(node->fathers[0])& t) {
						ptrNode fnode = std::get<3>(t);
						return fnode->h_ + fnode->g_ == tree.root_->h_;
					});

				if (flag) {
					file << '\n';
					OutputData(node, false);
				}
				continue;
			}
			/* ���� */
			file << '\n';
			/* ������� */
			OutputData(node);
		}
	}

	file.close();
	/* ���� */
	mtx.unlock();
}

/* ��Ϣ��� */
void infoCreate(PetriNet& tree) {
	/* ���� */
	mtx.lock();
	/* ��Ϣ�ļ������ */
	std::ofstream file(kInfoPath, std::ios::out);

	tree_nodes_num += tree.goal_nodes_.size();
	for (auto nodes : tree.entire_list_) {
		tree_nodes_num += nodes.second.size();
	}

	if (!file.is_open()) {
		std::cerr << "File open error!" << std::endl;
		exit(-1);
	}
	else {
		file << "Total nodes = " << tree_nodes_num << '\n';
		file << "Target nodes  = " << tree.goal_nodes_.size() << '\n';
		file << "Dataset = " << total_nodes_num << '\n';
		file << "The minimum time = " << tree.root_->h_ << "s\n";
		file << "Node size = " << sizeof(Node);
	}

	for (auto nodes : tree.entire_list_) {
		for (auto node : nodes.second) {
			if (node->son_) {
				std::cout << node->id_ << '\t' << node->son_ << "\tBackTree exception\n";
			}
		}
	}

	std::cout << "deadlock nodes = " << tree.deadlock_nodes_.size() << "   ";
	std::cout << "leaf nodes = " << tree.leaf_nodes_.size() << "   ";
	std::cout << "Goal nodes = " << tree.goal_nodes_.size() << '\n' << std::endl;
	for (auto &goal_ : tree.goal_nodes_) {
		std::cout << "Mark��" << goal_->toString() << "  g = " << goal_->g_ << '\n';
	}

	file.close();
	/* ���� */
	mtx.unlock();
}