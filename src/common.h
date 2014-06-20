#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>

enum direction {
	CLOCKWISE = 1,
	COUNTERCLOCKWIS
};

enum color {
	DEFAULT = 1,
	GRAY,
	BLACK
};

typedef struct node {
	int x, y;
} tNode;

typedef struct depot {
	int id;
	tNode coords;
} tDepot;

typedef struct customer {
	int id;
	tNode coords;
	int demand;
	int typeOfService;	//delivery/pickup
	double distanceFromDepot;
	double distanceFromGC;
	int degree;
	struct customer * parent = NULL;
} tCustomer;

typedef struct vehicle {
	int capacity = 100;
} tVehicle;

typedef struct vertexPair {
	tCustomer * from;
	tCustomer * to;
	double savings;
	int color;
} tVertexPair;

typedef struct cluster {
	tNode gc;
	std::vector<tCustomer> members;
	std::vector<tVertexPair> route;
	int c_Delivery;
	int c_Pickup;
	int id;
	double cost = 0.0;
} tCluster;
