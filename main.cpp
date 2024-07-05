/**
 * ���������
 */

#include <iostream>
#include <thread>
#include "input.h"
#include "node.h"
#include "petri.h"
#include "output.h"

int main(int argc, const char* argv[]) {
	/* ������ */
	std::vector<int> m0 = readVector<int>(kInitMark);
	std::vector<int> d0 = readVector<int>(kTimePath);
	std::vector<int> goals = readVector<int>(kGoalPlace);
	std::vector<int> goal_marking = readVector<int>(kGoalMarking);
	std::vector<int> goal_vector = getGoalMark(goal_marking, goals, m0.size());
	std::vector<vector<int>> pre = readMatrix(kPrePath, m0.size());
	std::vector<vector<int>> post = readMatrix(kPostPath, m0.size());

	/* �������뷴���� */
	PetriNet petri(m0, d0, goal_vector, pre, post);
	petri.forwardTree();
	petri.backTree();
	       
	/* t1: ���ɿɴ�ͼ */
	//std::thread t1(globalGraphCreate, std::ref(petri));
	/* t2: ��ȡ����·��������һ����״̬ */
	std::thread t2(dataCreateTxt, std::ref(petri), m0.size());
	/* t3: ��ȡȫ��״̬ */
	//std::thread t3(dataCreateAllTxt, std::ref(petri), m0.size());
	/* t4: ��Ϣ��� */
	std::thread t4(infoCreate, std::ref(petri));

	//t1.join();
	t2.join();
	//t3.join();
	t4.join();

	return 0;
}
