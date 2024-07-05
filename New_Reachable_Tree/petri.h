#pragma once
/**
 * �ɴ�ͼ�㷨 + ��������
 */

#include <queue>
#include <map>
#include <string>
#include <algorithm>
#include <ctime>
#include <mutex>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include "node.h"

using std::string;
using std::list;

/* �������token */
const int kPlaceOfMaxTokens = 2;

extern std::unordered_set<int> ignore_m, ignore_v;

/* Dijkstra �������� */
class Dijkstra {
public:
	bool operator()(const ptrNode n1, const ptrNode n2) {
		return n1->g_ > n2->g_;
	}
};

/* A* �������� */
class AStar {
public:
	bool operator()(const ptrNode n1, const ptrNode n2) {
		return n1->g_ + n1->h_pre_ > n2->g_ + n2->h_pre_;
	}
};

class PetriNet {
public:
	/* �������� */
	int num_place_;
	/* ��Ǩ���� */
	int num_transition_;
	/* �ڵ�� */
	NodePool pool_;
	/* ���ڵ� */
	ptrNode root_;
	/* Ŀ��ڵ� */
	ptrNode goal_node_;
	/* ǰ�þ���ת�� */
	vector<vector<int>> Tpre_;
	/* ���þ���ת�� */
	vector<vector<int>> Tpost_;
	/* ��������ת�� */
	vector<vector<int>> C;
	/* ��ʼ��ʶ */
	vector<int> m0_;
	/* ��ʱ */
	vector<int> delay_;
	/* Ŀ���ʶ */
	vector<int> goal_;
	/* �洢Ŀ��ڵ� */
	vector<ptrNode> goal_nodes_;
	/* �洢�����ڵ� */
	std::unordered_map<unsigned, ptrNode> deadlock_nodes_;
	/* �洢Ҷ�ӽڵ� */
	std::unordered_map<unsigned, ptrNode> leaf_nodes_;
	/* �洢�½ڵ� open�� */
	std::priority_queue<ptrNode, vector<ptrNode>, AStar> open_list_;
	/* �洢��չ���Ľڵ� close �� */
	std::map<string, list<ptrNode>> entire_list_;
	/* ����·���Ķ������� */
	vector<vector<int>> paths_;

	/* ��ʼ������ */
	PetriNet(vector<int>& m, vector<int>& d, vector<int>& goal,
			 vector<vector<int>>& p, vector<vector<int>>& q)
		: pool_(10000000), m0_(m), delay_(d), goal_(goal) {
		if (p.size() == 0 || m0_.size() == 0 || delay_.size() == 0 || goal_.size() == 0 || q.size() == 0) {
			printf("input files error!\n");
			exit(-1);
		}

		num_place_ = m.size();
		num_transition_ = p[0].size();

		Tpre_.resize(num_transition_, vector<int>(num_place_, 0));
		Tpost_.resize(num_transition_, vector<int>(num_place_, 0));
		C.resize(num_transition_, vector<int>(num_place_, 0));

		for (int i = 0; i < num_place_; ++i) {
			int num_of_place = 0;
			for (int j = 0; j < num_transition_; ++j) {
				Tpre_[j][i] = p[i][j];
				Tpost_[j][i] = q[i][j];
				C[j][i] = q[i][j] - p[i][j];
				if (p[i][j] != 0) {
					++num_of_place;
				}
			}
		}

		root_ = pool_.getNode();
		for (int i = 0; i < num_place_; ++i) {
			if (m0_[i] != 0)
				root_->state_.emplace_back(Place(m0_[i], i, vector<int16_t>(kPlaceOfMaxTokens, 0)));
		}

		goal_node_ = pool_.getNode();
		for (int i = 0; i < num_place_; ++i) {
			if (goal_[i] != 0)
				goal_node_->state_.emplace_back(Place(goal_[i], i, vector<int16_t>(kPlaceOfMaxTokens, 0)));
		}

		root_->is_open_ = true;
		open_list_.push(root_);
		std::list<ptrNode> temp = { root_ };
		entire_list_.emplace(root_->toString(), std::move(temp));  // move�ƶ���Դ��ԭ��ַ��ŵ���Դ������

		/* ֱ��·���������� */
		paths_.assign(4, {});
		/* ·��1�еĶ������� */
		paths_[0] = { 0, 1, 2, 3, 26 };
		/* ·��2�еĶ������� */
		paths_[1] = { 4, 5, 10, 11, 12, 9, 27 };
		/* ·��3�еĶ������� */
		paths_[2] = { 4, 5, 6, 7, 8, 9, 27 };
		/* ·��4�еĶ������� */
		paths_[3] = { 13, 14, 15, 16, 17, 18, 28 };

		return;
	}

	/* ʹ�ܱ�Ǩ�ж� */
	vector<int> enableTrans(ptrNode node) {
		vector<int> ans;
		for (int i = 0; i < num_transition_; ++i) {
			if (*node > Tpre_[i])
				ans.push_back(i);
		}
		node->son_ = ans.size();
		return ans;
	}

	/* Ŀ��ڵ��ж� */
	bool isGoalNode(ptrNode curnode) {
		if (*(curnode) > goal_) {
			return 1;
		}
		return 0;
	}

	/* �����ѵȴ�ʱ��v������ȴ��ʱ��� */
	int updateVk(ptrNode newnode, ptrNode curnode, int t) {
		int lambda = 0;
		for (int i = 0; i < num_place_; ++i) {
			if (Tpre_[t][i] != 0) {
				// [i]ΪNode���еķ�������
				// ��Ϊ�鿴��i�������Ƿ�Ϊ��
				// ��Ϊ�գ�����place->row_ = -1
				// �������i��״̬
				if ((*curnode)[i].row_ == -1) {
					printf("something wrong in 110 rows\n");
					exit(-1);
				}
				// ��ȡ����������ѵȴ�ʱ������
				auto cur_v = (*curnode)[i].v_;
				// v��ֵ����ָ��
				auto v_max = std::max_element(cur_v.begin(), cur_v.end());
				// ����ȴ�ʱ��
				int diff = delay_[i] - *v_max;
				// �����������������token 
				if ((*newnode)[i].row_ != -1) {
					// ���ֵ��v�е�����
					int index = std::distance(cur_v.begin(), v_max);
					// ����token���ѵȴ�ʱ������
					(*newnode)[i].v_[index] = 0;
					// �ж�v�Ƿ�Ӵ�С
					bool is_update = std::is_sorted((*newnode)[i].v_.begin(), (*newnode)[i].v_.end(),
						[](int16_t& i1, int16_t& i2) {
							if (i1 == i2) return false;
							return i1 > i2;
						});
					// �������������
					if (!is_update) {
						std::sort((*newnode)[i].v_.begin(), (*newnode)[i].v_.end(), std::greater_equal<int16_t>());
					}
				};
				// ����ȴ������ʱ��
				lambda = std::max(lambda, diff);
			}
		}
		// ����newnode�е�v
		for (int i = 0; i < newnode->state_.size(); ++i) {
			/**
			 * ������Ǩʱ��Place�����������
			 * ���һ: ��̬����(�����е�token�ڼ��������в�������)
			 *         Tpost_[t][newnode->state_[i].row_] = 0
			 * ���������̬����(�����е�token�����仯)
			 *         Tpost_[t][newnode->state_[i].row_] = 1
			 */
			 // ����µ�token����
			int num = newnode->state_[i].tokens_ - Tpost_[t][newnode->state_[i].row_];
			for (int j = 0; j < std::min(num, kPlaceOfMaxTokens); ++j) {
				auto& v = newnode->state_[i].v_;
				v[j] = std::min(delay_[newnode->state_[i].row_], v[j] + lambda);
			}
		}
		return lambda;
	}

	/* ����ʽ(1) */
	float heuristicsOne(ptrNode& node, float idle_time = 0, int ER = 11) {
		// ER:��Դ����  time:��ʱ��
		float time = 0;

		// ���ҿ���id����·��λ��
		auto findPosition = [&](int8_t id) {
			for (int i = 0; i < paths_.size(); ++i) {
				for (int j = 0; j < paths_[i].size(); ++j) {
					if (paths_[i][j] == id) {
						return std::make_pair(i, j);
					}
				}
			}
			return std::make_pair(-1, -1);
		};

		// �ۼӸ���·���ϵĹ����������Ŀ�������ʱ��
		for (auto& p : node->state_) {
			// ��ѯ��������ڷ���(·��i, ����j) �����ڷ���(-1, -1)
			std::pair<int, int> res = findPosition(p.row_);
			if (res.first == -1) {
				continue;
			}
			// �ۼӵ�ǰ��������ȴ���ʱ��
			std::for_each(p.v_.begin(), p.v_.begin() + std::min(2, (int)p.tokens_), [&](int16_t& t) noexcept {
				time += std::max(0, delay_[p.row_] - t);
				});

			// �ۼӺ�����������ȴ���ʱ��
			for (int i = res.second + 1; i < paths_[res.first].size(); ++i) {
				time += delay_[paths_[res.first][i]] * p.tokens_;
			}
		}
		return (time + idle_time) / ER;
	}

	/* ��Ǩ�������� */
	void fire(ptrNode curnode, int t) {
		auto newnode = *curnode + C[t];
		newnode->discarded_ = false;
		int waiting_time = updateVk(newnode, curnode, t);
		newnode->g_ = curnode->g_ + waiting_time;
		newnode->fathers.emplace_back(std::make_tuple(t, curnode->id_, waiting_time, curnode));
		if (isGoalNode(newnode)) {
			newnode->h_ = 0;
			goal_nodes_.emplace_back(newnode);
			newnode->h_pre_ = 0;
			open_list_.push(newnode);
			curnode->sons_.emplace(newnode->id_, newnode);
			return;
		}
		newnode->h_pre_ = heuristicsOne(newnode);
		auto str = newnode->toString();
		auto pair = isNewNode(newnode);   // return <bool,list<ptrNode>>  bool = 1 or Ϊ�½ڵ�  bool = 0 �ɽڵ�
		if (pair.second == nullptr) {
			list<ptrNode> temp;
			temp.push_back(newnode);
			curnode->sons_.emplace(newnode->id_, newnode);
			entire_list_.emplace(str, std::move(temp));
			open_list_.push(newnode);
			newnode->is_open_ = true;
		}
		else {
			/* �����½ڵ㣬���� */
			if (!pair.first) {
				for (auto f : newnode->fathers) {
					std::get<3>(f)->sons_.erase(newnode->id_);
					if ((--std::get<3>(f)->son_) == 0)
						leaf_nodes_.emplace(std::get<3>(f)->id_, std::get<3>(f));
				}
				pool_.recycling(newnode);
				return;
			}
			/* ���½ڵ㣬����open_list��entire_list */
			else {
				curnode->sons_.emplace(newnode->id_, newnode);
				pair.second->emplace_back(newnode);
				open_list_.push(newnode);
				newnode->is_open_ = true;
			}
		}
	}

	/* G����(�Ƿ�Ϊ�ɽڵ�) */
	bool isSame(ptrNode& newnode, ptrNode& oldnode) {
		// �ж�v�Ƿ����
		for (int i = 0; i < newnode->state_.size(); ++i) {
			if (newnode->state_[i].v_ != oldnode->state_[i].v_)
				return false;
		}
		// gֵ�Ƚ�
		if (newnode->g_ < oldnode->g_) {
			return false;
		}
		// ��״̬��ͬ�ڵ�ϲ���һ��(m = m', v = v')
		for (auto f : newnode->fathers) {
			if (!std::count(oldnode->fathers.begin(), oldnode->fathers.end(), f)) {
				oldnode->fathers.push_back(f);
				std::get<3>(f)->sons_.emplace(oldnode->id_, oldnode);
				++std::get<3>(f)->son_;
			}
		}
		return true;
	}

	/* GV����(�Ƿ�Ϊ�½ڵ�) */
	bool isNew(ptrNode& newnode, ptrNode& oldnode) {
		unsigned int equal = 0;
		// gֵ�Ƚ�
		if (newnode->g_ > oldnode->g_) {
			return false;
		}
		// v������С�Ƚ�
		for (int i = 0; i < newnode->state_.size(); ++i) {
			if (newnode->state_[i].v_ == oldnode->state_[i].v_) {
				++equal;
				continue;
			}
			for (int j = 0; j < newnode->state_[i].v_.size(); ++j) {
				if (newnode->state_[i].v_[j] < oldnode->state_[i].v_[j]) {
					return false;
				}
			}
		}

		// ����ʶ��ͬ�ڵ�ϲ���һ��(m = m')
		if (equal == newnode->state_.size()) {
			for (auto f : oldnode->fathers) {
				// �ɽڵ�ĸ��ڵ��������½ڵ�
				if (!std::count(newnode->fathers.begin(), newnode->fathers.end(), f)) {
					newnode->fathers.push_back(f);
					std::get<3>(f)->sons_.emplace(newnode->id_, newnode);
					++std::get<3>(f)->son_;
				}
			}
		}

		return true;
	}

	/* ǿʱ�����ж�(�Ƿ�Ϊ�ɽڵ�) */
	bool isOld(ptrNode& newnode, ptrNode& oldnode) {
		unsigned int equal = 0;
		/* gֵС��Ϊ�ɽڵ� */
		//if (newnode->g_ < oldnode->g_) {
		//	return false;
		//}
		/* gֵ�� */
		int diff = newnode->g_ - oldnode->g_;

		// ��v��v'ת�Ƶ�ͬһʱ���Ƚ�
		for (int i = 0; i < newnode->state_.size(); ++i) {
			for (int j = 0; j < newnode->state_[i].v_.size(); ++j) {
				if (newnode->state_[i].v_[j] > oldnode->state_[i].v_[j] + diff) {
					return false;
				}
			}
			if (newnode->state_[i].v_ == oldnode->state_[i].v_) {
				++equal;
			}
		}
		// ���ɽڵ�ĸ��ڵ���ӵ��½ڵ�ĸ��ڵ���
		if (equal == newnode->state_.size()) {
			for (auto f : newnode->fathers) {
				if (!std::count(oldnode->fathers.begin(), oldnode->fathers.end(), f)) {
					oldnode->fathers.push_back(f);
					std::get<3>(f)->sons_.emplace(oldnode->id_, oldnode);
					++std::get<3>(f)->son_;
				}
			}
		}
		return true;
	}

	/* �¾ɽڵ��ж� */
	std::pair<bool, list<ptrNode>*> isNewNode(ptrNode newnode) {
		auto str = newnode->toString();
		if (entire_list_.count(str) <= 0) {
			return std::make_pair(1, nullptr);
		}
		/* �ҵ���ͬ��ʶ������ */
		auto it = entire_list_.find(str);
		/* �����������ϵĽڵ���бȽ� */
		for (auto itor = it->second.begin(); itor != it->second.end();) {
			auto oldnode = *itor;
			/* �ж�����չ�����ڵ���¾��� */
			if (isOld(newnode, oldnode)) {
				return std::make_pair(0, &(it->second));
			}
			/* �ж��Ƿ�ɾ���ɽڵ� */
			if (isOld(oldnode, newnode)) {
				/* ֻ����û���ӽڵ��Ҳ���open���е�״̬ */
				if (!oldnode->son_) {
					/* ���ڵ�ΪҶ�ӽڵ���� */
					short res = 0;
					/* ����son_ */
					for (auto f : oldnode->fathers) {
						auto f_node = std::get<3>(f);
						if ((--f_node->son_) == 0) {
							leaf_nodes_.emplace(f_node->id_, f_node);
							++res;
						}
					}
					/* ��������Ҷ�ӽڵ����ɾ��oldnode */
					leaf_nodes_.erase(oldnode->id_); deadlock_nodes_.erase(oldnode->id_);
					/* close����ɾ�������µ����� */
					itor = it->second.erase(itor);

					if (!oldnode->is_open_ && res == oldnode->fathers.size()) {
						pool_.recycling(oldnode);
					}
					else {
						oldnode->discarded_ = true;
					}
				}
				else ++itor;
			}
			else ++itor;
		}
		return std::make_pair(1, &(it->second));
	}

	/* ������ */
	void forwardTree() {
		std::cout << "\nBegin forward tree -> ";
		clock_t start = clock();
		while (!open_list_.empty()) {
			auto curnode = open_list_.top();
			curnode->is_open_ = false;
			open_list_.pop();

			/* ����������� */

			if (isGoalNode(curnode)) {
				break;
			}


			if (curnode->discarded_) {
				continue;
			}

			auto enables = enableTrans(curnode);

			if (enables.empty()) {
				curnode->is_deadlock_ = true;
				deadlock_nodes_.emplace(curnode->id_, curnode);
				continue;
			}
			std::cout << "  b E " << std::endl;
			for (auto t : enables) {
				std::cout << t << "    ghg  " << std::endl;
				fire(curnode, t);
			}
			std::cout << "  E D " << std::endl << std::endl;
		}
		clock_t end = clock();
		auto programTimes = end - start;
		std::cout << "Forward tree finish��" << programTimes << "ms��->";
	}

	/* ������ */
	void backTree() {
		std::cout << "Begin back tree -> ";
		clock_t start = clock();
		vector<ptrNode> back_node = goal_nodes_;

		for (auto& node : deadlock_nodes_) {
			back_node.insert(back_node.end(), node.second);
		}
		for (auto& node : leaf_nodes_) {
			back_node.insert(back_node.end(), node.second);
		}

		/* ���������л�ȡ���ݼ� */
		while (!open_list_.empty()) {
			auto node = open_list_.top();
			open_list_.pop();
			if (!isGoalNode(node) && !node->discarded_) {
				back_node.push_back(node);
			}
		}

		while (!back_node.empty()) {
			auto node = back_node.back();
			back_node.pop_back();
			for (auto f : node->fathers) {
				auto fnode = std::get<3>(f);
				fnode->h_ = std::min(node->h_ + std::get<2>(f), fnode->h_);

				if ((--fnode->son_) == 0) {
					back_node.push_back(fnode);
				}
			}
		}
		clock_t end = clock();
		auto programTimes = end - start;
		std::cout << "Back tree finish��" << programTimes << "ms��\n" << std::endl;
	}

};