#pragma once
/**
 * ������ �ڵ��� �ڵ����
 */

#include <vector>
#include <memory>
#include <tuple>
#include <queue>
#include <string>
#include <unordered_map>

using std::vector;
using std::shared_ptr;

class Node;
class NodePool;

typedef shared_ptr<Node> ptrNode;
extern const int kPlaceOfMaxTokens;

/* ��Žڵ��к���token���ĵ�����������Ϣ */
class Place {
public:
	int8_t tokens_;
	int8_t row_;
	vector<int16_t> v_;
	// nums of tokens, id of place , waiting time of place
	Place(int8_t t, int8_t r, vector<int16_t> v) :tokens_(t), row_(r), v_(v) {};
};

/* ��ǰ����һ���ڵ�� �ɻ����ظ����� */
class NodePool : public std::enable_shared_from_this<NodePool> {  // std::enable_shared_from_this<NodePool> ���ڽ�thisָ��������ָ������
public:
	int maxNode;  // ���ڵ���
	std::queue<ptrNode> buf;    // ���˶��д洢�����ýڵ�
	NodePool(int n = 10000000);
	ptrNode getNode();   // ��ȡ�ڵ㣻��bufΪ�գ�����չ�ռ�
	void recycling(ptrNode node);  // ���վɽڵ㣬����ʼ��
private:
	NodePool() = delete;
};

/* �ڵ��� */
class Node {
public:
	/* ��¼��ǰȡ�����Ľڵ��� */
	static unsigned int num;
	/* �Ƿ����� */
	bool discarded_;
	/* �Ƿ����� */
	bool is_deadlock_;
	/* �Ƿ���open���� */
	bool is_open_;
	/* ��ʼ->��ǰ�Ĵ��� */
	unsigned short g_;
	/* ��ǰ->�յ��ʵ�ʴ��� */
	float h_;
	/* ��ǰ->�յ�Ĺ��ƴ��� */
	float h_pre_;
	/* �ӽڵ���� */
	short son_;
	/* �ڵ��� */
	unsigned int id_;
	/* �ڵ�� */
	NodePool* pool_;
	/* ���ڵ㣨������Ǩ��id������������ۡ����ڵ�ָ�룩 */
	std::vector<std::tuple<unsigned short, unsigned int, unsigned int, ptrNode>> fathers;
	/* �ӽڵ� */
	std::unordered_map<unsigned int, ptrNode> sons_;
	/* ��ʶ��ֻ����token��place�� */
	vector<Place> state_;
	Node(NodePool* pool) : discarded_(false), is_deadlock_(false), is_open_(false),
		g_(0), son_(0), h_(FLT_MAX), h_pre_(FLT_MAX), id_(Node::num), pool_(pool) { ++Node::num; }

	/* �жϽڵ��Ƿ���ͬ */
	bool operator==(Node& n) {
		if (n.state_.size() != this->state_.size())
			return false;
		for (int i = 0; i < n.state_.size(); ++i) {
			if (n.state_[i].row_ != this->state_[i].row_ ||
				n.state_[i].tokens_ != this->state_[i].tokens_ ||
				n.state_[i].v_ != this->state_[i].v_)
				return false;
		}
		return true;
	}

	/* ��ʶ���� */
	ptrNode operator+(const vector<int>& v) {
		auto newnode = pool_->getNode();
		int n = v.size();
		int j = 0;  // ��ǰ��ʶ��ĳ������
		int i = 0;  // ���ڹ��������ĳ������
		while (i <= state_.back().row_) {
			if (i < state_[j].row_) {
				if (v[i] > 0) {
					// ���ڹ��������е�Ԫ��Ϊ-1/0/1 ��v[i] = 1
					newnode->state_.emplace_back(Place(v[i], i, vector<int16_t>(kPlaceOfMaxTokens, 0)));
				}
			} else {  
				// ����v[i] = -1/0/1 �Ƚ���ǰ״̬��v�������״̬ ������updateVk���޸�
				if (v[i] + state_[j].tokens_ != 0) {
					newnode->state_.emplace_back(Place(v[i] + state_[j].tokens_, i, state_[j].v_));
				}
				++j;
			}
			++i;
		}
		while (i < n) {
			if (v[i] > 0) {
				newnode->state_.emplace_back(Place(v[i], i, vector<int16_t>(kPlaceOfMaxTokens, 0)));
			}
			++i;
		}
		return newnode;
	}

	/* ��Ǩʹ���жϡ�Ŀ���ж� */
	bool operator>(const vector<int>& obj) {
		int m = state_.size();
		int n = obj.size();
		int i = 0, j = 0;
		/* state_��������֮�ڲ��� */
		while (i < m && j < n) {
			if (state_[i].row_ < j) {
				++i;
			}
			else if (state_[i].row_ == j) {
				if (state_[i].tokens_ < obj[j])
					return false;
				++i; ++j;
			}
			else if (state_[i].row_ > j && obj[j] == 0) {
				++j;
			}
			else {
				return false;
			}
		}
		/* ʣ�ಿ�� */
		while (j < n) {
			if (obj[j] > 0)
				return false;
			++j;
		}
		return true;
	}

	/* ����ָ��������place */
	Place& operator[](int i) {
		Place temp(0, -1, vector<int16_t>(kPlaceOfMaxTokens, 0));
		for (auto& p : state_) {
			if (p.row_ < i)
				continue;
			else if (p.row_ == i)
				return p;
			else
				return temp;
		}
		return temp;
	}

	/* ��ʶת��Ϊ�ַ��� */
	std::string toString() {
		std::string str;
		for (auto p : state_) {
			str.append(std::to_string(p.row_));
			str += '/';
			str.append(std::to_string(p.tokens_));
			str += '/';
		}
		return str;
	}

private:
	Node() = delete;
};