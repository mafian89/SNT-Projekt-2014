#include "common.h"

struct mySortFuncDistFromDepotDown
{
	bool operator()(const tCustomer& a, const tCustomer& b) const
	{
		return a.distanceFromDepot > b.distanceFromDepot;
	}
} distance_sort_down;

struct mySortFuncDistFromDepotUp
{
	bool operator()(const tCustomer& a, const tCustomer& b) const
	{
		return a.distanceFromDepot < b.distanceFromDepot;
	}
} distance_sort_up;

struct mySortFuncSavings
{
	bool operator()(const tVertexPair & a, const tVertexPair & b) const
	{
		return a.savings > b.savings;
	}
} saving_sort_dec;

double distance(tNode from, tNode to) {
	return sqrt(pow((to.x - from.x), 2) + pow((to.y - from.y), 2));
}

int processFile(char * name, std::vector<tCustomer> &customers, tDepot &dep) {
	std::ifstream f(name);
	tCustomer tmp;
	std::string line;
	tNode depCoords,custCoords;
	if (f.is_open()) {
		while (std::getline(f, line)) {
			std::istringstream buffer(line);
			int stop, ot1, ct1, ot2, ct2, st;

			buffer >> stop;
			if (stop == 0) {
				dep.id = stop;

				buffer >> dep.coords.x;	//x
				buffer >> dep.coords.y;	//y

				depCoords.x = dep.coords.x;
				depCoords.y = dep.coords.y;
			}
			else {
				tmp.id = stop;

				buffer >> tmp.coords.x;	//x
				buffer >> tmp.coords.y;	//y

				custCoords.x = tmp.coords.x;
				custCoords.y = tmp.coords.y;
			}

			buffer >> ot1;
			buffer >> ct1;
			buffer >> ot2;
			buffer >> ct2;

			//demand
			buffer >> tmp.demand;

			buffer >> st;

			//type of service
			buffer >> tmp.typeOfService;

			tmp.parent = NULL;

			if (stop != 0) {
				tmp.distanceFromDepot = distance(depCoords,custCoords);
				customers.push_back(tmp);
			}

		}
		f.close();
	}
	else {
		std::cerr << "Cannot open file: " << name << std::endl;
		return -1;
	}
	return 0;
}

void printStructures(std::vector<tCustomer> &customers, tDepot &dep) {
	std::cout <<dep.id<<" "<< dep.coords.x << " " << dep.coords.y << std::endl;
	for (unsigned i = 0; i < customers.size(); i++)
	{
		std::cout << customers[i].id<< " " <<customers[i].coords.x << " " << customers[i].coords.y << " " << customers[i].demand << " " << customers[i].typeOfService << " " << customers[i].distanceFromDepot << std::endl;
	}
}

void geometricalCenter(tCluster &c) {
	int sumX = 0;
	int sumY = 0;
	for (unsigned i = 0; i < c.members.size(); i++)
	{
		sumX += c.members[i].coords.x;
		sumY += c.members[i].coords.y;
	}
	c.gc.x = sumX / c.members.size();
	c.gc.y = sumY / c.members.size();
	//c.gc.y =
}

tCustomer closestNode(tNode from, std::vector<tCustomer> &ref) {
	tCustomer tmp;
	double minDist = 9999999.0;
	int indexMax = 0;

	for (unsigned i = 0; i < ref.size(); i++)
	{
		ref[i].distanceFromGC = distance(from, ref[i].coords);
		//std::cout << "[" << ref[i].coords.x << ", " << ref[i].coords.y << "] distance: " << ref[i].distanceFromGC << " ";
		if (ref[i].distanceFromGC < minDist) {
			minDist = ref[i].distanceFromGC;
			indexMax = i;
		}
	}
	//std::cout << std::endl;
	//std::cout << "GC = [" << from.x << ", " << from.y << "] -> ";
	//std::cout << "[" << ref[indexMax].coords.x << ", " << ref[indexMax].coords.y << "] distance: " << ref[indexMax].distanceFromGC << "\n";

	/*std::cout << indexMax << " size: " << ref.size() << "\n";*/

	tmp = ref[indexMax];
	ref.erase(ref.begin() + indexMax);

	return tmp;
}


double computeSavings(tCustomer *i, tCustomer *j,tDepot dep) {
	//tVertexPair pair;

	double a = distance(i->coords,dep.coords);
	double b = distance(dep.coords, j->coords);
	double c = distance(i->coords, j->coords);

	//pair.from = i;
	//pair.to = j;

	//pair.from->degree = 0;
	//pair.to->degree = 0;

	//pair.from->parent->coords = dep.coords;
	//pair.to->parent->coords = dep.coords;

	//std::cout << i->demand << " " << j->demand << std::endl;

	/*pair.savings = a + b - c;*/

	return (a + b - c);

	//return pair;
}


bool cycleDetected(tCustomer *from, tCustomer *to, tDepot dep) {

	tCustomer *current = from;

	//std::cout << "from: " << from << " to: " << to << "\n";
	//while ((current->parent->coords.x != dep.coords.x) && (current->parent->coords.y != dep.coords.y)) {
	//int i = 0;
	while ((current->parent != NULL) && (current != current->parent)) {
		//std::cout << "current->parent: " <<current->parent << "\n";
		if (current->parent == to) {
			return true;
		}
		//if ((current->parent->coords.x == to->parent->coords.x) && (current->parent->coords.y == to->parent->coords.y))
		//{
		//	return true;
		//}
		current = current->parent;
		//i++;
		//if (i > 5) {
		//	break;
		//}
	}

	return false;
}

//the Clarke-Wright heuristic
void ClarkeWright(tCluster *cluster, tDepot dep, direction dir) {
	//std::cout << "Tu som\n";
	//1.   Identify a hub vertex h
	//2.   VH = V - {h}
	tNode hub = dep.coords;
	//Should i compute starting cost here?
	//Starting cost

	std::vector<tVertexPair> sortlist;
	unsigned size = cluster->members.size();
	for (unsigned i = 0; i < size; i++)
	{
		for (unsigned j = 0; j < size; j++) {
			if (j == i) { // j <= i
				continue;
			}
			tVertexPair  tmp;
			tmp.savings = computeSavings(&cluster->members[i], &cluster->members[j], dep);

			tmp.from = &cluster->members[i];
			tmp.to = &cluster->members[j];

			tmp.from->degree = 0;
			tmp.to->degree = 0;
			//std::cout << "[" <<tmp.from->coords.x << ", " << tmp.from->coords.y << "]->";
			//std::cout << "[" << tmp.to->coords.x << ", " << tmp.to->coords.y << "]\n";
			sortlist.push_back(tmp);
		}
	}

	
	std::sort(sortlist.begin(),sortlist.end(),saving_sort_dec);
	//for (unsigned i = 0; i < sortlist.size(); i++)
	//{
	//	std::cout << i << ": " << sortlist[i].savings << "\n";
	//}

	unsigned i = 0;
	tCustomer * actual_node = sortlist.front().to;
	sortlist.erase(sortlist.begin());

	tCustomer  *depot = new tCustomer;
	depot->coords = dep.coords;
	depot->parent = NULL;
	depot->demand = 0; //depot only
	depot->typeOfService = -1;//depot only

	tVertexPair start;
	start.from = depot;
	start.to = actual_node;
	cluster->route.push_back(start);

	while (size > 2) {
		//try vertex pair(i, j) in sortlist order
		//if (i < sortlist.size()) {
		for (i = 0; i < sortlist.size(); i++)
		{
			tVertexPair pair = sortlist[i];
			if (pair.from == actual_node) {

				sortlist.erase(sortlist.begin() + i);

				if (!cycleDetected(pair.from, pair.to, dep)) {

					//std::cout << "!detected\n";

					pair.from->degree += 1;
					pair.to->degree += 1;

					double cost = distance(pair.from->coords, pair.to->coords);
					//std::cout << "Add: [" << pair.from->coords.x << ", " << pair.from->coords.y << "]->";
					//std::cout << "[" << pair.to->coords.x << ", " << pair.to->coords.y << "] cost: "<<cost<<"\n";

					cluster->cost += cost;
					cluster->route.push_back(pair);

					//pair.from->parent->coords = dep.coords;
					//pair.to->parent->coords = pair.from->coords;
					pair.to->parent = pair.from;

					if (pair.from->degree == 2) {
						//std::cout << size << " pair.from->degree == 2\n";
						size--;
					}

					if (pair.to->degree == 2){
						//std::cout << size << " pair.to->degree == 2\n";
						size--;
					}
					actual_node = pair.to;
					break; //break for
				}
			}
		}
		//std::cout << "Kontrola i++\n";
	}
	if (!cluster->route.empty()) {
		tVertexPair end;
		end.to = depot;
		end.from = cluster->route.back().to;

		double cost = distance(end.from->coords, end.to->coords);
		
		//std::cout << "Add: [" << end.from->coords.x << ", " << end.from->coords.y << "]->";
		//std::cout << "[" << end.to->coords.x << ", " << end.to->coords.y << "] cost: "<<cost<<"\n";
		
		cluster->cost += cost;

		cost = distance(start.from->coords, start.to->coords);
		
		//std::cout << "Add: [" << start.from->coords.x << ", " << start.from->coords.y << "]->";
		//std::cout << "[" << start.to->coords.x << ", " << start.to->coords.y << "] cost: " << cost << "\n";
		
		cluster->cost += cost;

		cluster->route.push_back(end);

		//tVertexPair *current = &end;
		//tVertexPair *first = &start;


		//std::cout << cluster->route.size() << std::endl;


		//std::cout << "---------------\nBackward:\n";
		//for (int i = cluster->route.size()-1; i >= 0; i--)
		//{
		//	tVertexPair *current = &cluster->route[i];
		//	std::cout << "[" << current->to->coords.x << ", " << current->to->coords.y << "]";
		//	std::cout << "->[" << current->from->coords.x << ", " << current->from->coords.y << "]\n";
		//}

		//std::cout << "---------------\nFoward:\n";
		//for (int i = 0; i < cluster->route.size(); i++)
		//{
		//	tVertexPair *current = &cluster->route[i];
		//	std::cout << "[" << current->from->coords.x << ", " << current->from->coords.y << "]";
		//	std::cout << "->[" << current->to->coords.x << ", " << current->to->coords.y << "]\n";
		//}

		//std::cout << "---------------\n";
	}

}

std::vector<tCluster> makeClusters(std::vector<tCustomer> customers, int capacity) {
	std::vector<tCluster> result;
	tCluster clust;
	tCustomer accCust;

	int id = 1;
	int Q = capacity;

	while (!customers.empty()){
		accCust = customers.back();
		customers.pop_back();

		clust.id = id;
		clust.c_Delivery = Q;
		clust.c_Pickup = Q;
		clust.members.push_back(accCust);

		
		//0 - delivery, 1-pickup
		if (accCust.typeOfService == 0) {
			clust.c_Delivery -= accCust.demand;
			//std::cout <<"c_Delivery: " <<  clust.c_Delivery << "\n";
		}
		else {
			clust.c_Pickup -= accCust.demand;
			//std::cout << "c_Pickup: "<<clust.c_Pickup << "\n";
		}

		geometricalCenter(clust);

		if (!customers.empty()) {
			accCust = closestNode(clust.gc, customers);
		}
		while (1){
			clust.members.push_back(accCust);
			geometricalCenter(clust);

			if ((accCust.typeOfService == 0)) {
				if ((clust.c_Delivery - accCust.demand) > 0) {
					clust.c_Delivery -= accCust.demand;
				}
				else {
					break;
				}
			}
			else {
				if ((clust.c_Pickup - accCust.demand) > 0) {
					clust.c_Pickup -= accCust.demand;
				}
				else {
					break;
				}
			}

			if (!customers.empty()) {
				accCust = closestNode(clust.gc, customers);
			}

		}

		//std::cout << customers.size() << "\n";
		id++;
		result.push_back(clust);
		clust.members.clear();
	}


	return result;
}

bool isLoadFeasibilityViolated(int ToS, int demand, int Q, int capacity) {
	if (ToS == 1 && ((Q + demand) <= capacity)) {
		//Q += demand;
		return false;
	}
	else if (ToS == 0 && ((Q - demand) >= 0)) {
		//Q -= demand;
		return false;
	}
	return true;
}

bool checkSumOfDemands(std::vector<tCustomer> * skippedList, int Q, int capacity) {
	int sumPickUp = 0;
	int sumDelivery = 0;

	for (unsigned i = 0; i < skippedList->size(); i++)
	{
		tCustomer * node = &(*skippedList)[i];
		//0 - delivery, 1-pickup
		if (node->typeOfService == 0) {
			sumDelivery += node->demand;
		} else {
			sumPickUp += node->demand;
		}
	}

	if (Q + sumPickUp <= capacity && Q - sumDelivery >= 0) {
		return true;
	}

	return false;
}

void makeLink(std::vector<tVertexPair> * path, tCustomer * from, tCustomer * to) {
	tVertexPair * pair = new tVertexPair;
	pair->from = from;
	pair->to = to;

	//Savings mean distance (cost) :D :-/ >:-/ :-O 
	pair->savings = distance(from->coords, to->coords);

	path->push_back(*pair);
}

void step2(std::vector<tCluster>& cluster, tDepot dep, int capacity) {
	double cost = 0.0;
	int Q = 100;

	std::vector<tCustomer> skippedList;
	std::vector<tVertexPair> path;

	for (unsigned i = 0; i < cluster.size(); i++)
	{
		ClarkeWright(&cluster[i], dep, CLOCKWISE);
		Q = 100;
		//std::cout<< "Cluster cost: " <<cluster[i].cost <<std::endl;
		//cost += cluster[i].cost;

		//1 = skip depo
		if (cluster[i].route.size() > 0){
			tCustomer * current = cluster[i].route.front().from;

			for (unsigned j = 0; j < cluster[i].route.size(); j++)
			{
				tVertexPair * pair = &cluster[i].route[j];
				//0 - delivery, 1-pickup
				int ToS = pair->to->typeOfService;

				if (isLoadFeasibilityViolated(ToS, pair->to->demand, Q, capacity)) {
					skippedList.push_back(*pair->to);
				}
				else {
					if (checkSumOfDemands(&skippedList, Q, capacity)) {

						for (unsigned j = 0; j < skippedList.size(); j++)
						{
							makeLink(&path, current, &skippedList[j]);
							current = &skippedList[j];
						}
						skippedList.clear();
					}
					else {
						Q = ToS > 0 ? Q += pair->to->demand : Q -= pair->to->demand;
					}
					makeLink(&path, current, pair->to);
					current = pair->to;
				}

				

					//std::cout << j << " to demand: " << curr->to->demand << " & ";
					//std::cout << j << ": "<< curr->from->demand << std::endl;
					//std::cout << "[" << curr->from->coords.x << ", " << curr->from->coords.y << "]";
					//std::cout << "->[" << curr->to->coords.x << ", " << curr->to->coords.y << "]\n";
			}
		}
	}

	tCustomer * depot = new tCustomer;
	depot->coords = dep.coords;
	depot->parent = NULL;
	depot->typeOfService = -1;
	depot->demand = 0;

	tCustomer * from = depot;
	Q = 100;

	while (!skippedList.empty())
	{
		tCustomer current = skippedList.front();
		skippedList.erase(skippedList.begin());

		if (isLoadFeasibilityViolated(current.typeOfService, current.demand, Q, capacity)){
			makeLink(&path, from, depot);
			Q = 100;
			makeLink(&path, depot, &current);
		}
		else {
			makeLink(&path, from, &current);
		}
		Q = current.typeOfService > 0 ? Q += current.demand : Q -= current.demand;

		from = &current;
	}

	for (unsigned i = 0; i < path.size(); i++)
	{
		cost += path[i].savings;
	}
	std::cout << cost << std::endl;
	std::cout << skippedList.size() << "\n";

	//std::cout << "Total cost: "<< cost << std::endl;
	//ClarkeWright(cluster[0], dep, CLOCKWISE);
}

int main(int argc, char ** argv) {
	std::vector<tCustomer> customers;
	tDepot dep;
	std::vector<tCluster> clusters;

	if (argc < 2){
		std::cerr << "Need filename!\n";
	}

	if (processFile(argv[1], customers, dep) < 0){
		return EXIT_FAILURE;
	}

	/*std::cout << customers.size()<<"\n";*/

	//printStructures(customers, dep);
	std::sort(customers.begin(),customers.end(),distance_sort_up);
	//printStructures(customers, dep);

	clusters = makeClusters(customers,100);

	step2(clusters, dep, 100);

	//for (unsigned i = 0; i < clusters.size(); i++)
	//{
	//	std::cout << clusters[i].id <<" "<< clusters[i].members.size() << "\n";
	//}

	return EXIT_SUCCESS;
}