#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include "classes/AdvancedTimer.cpp"
#include "classes/GraphData.cpp"
#include "classes/RouteSolution.cpp"
#include "classes/ChartData.cpp"
#include "classes/InterfaceGate.cpp"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

using namespace std;

enum State { Waiting, Ready, Solving, Paused };

const int SYNC_WAIT_DELAY = 200;
const int UPDATE_DELAY = 1000;

void cpsleep(int m) {
	#ifdef _WIN32
		Sleep(m);
	#else
		usleep(m * 1000);
	#endif
}

int main() {
	srand(time(0));
	InterfaceGate::init();
	GraphData *graph;
	ChartData *chart;
	State state = Waiting;
	int iteration_delay = 0;
	AdvancedTimer syncTimer, iterationTimer, updateTimer;
	syncTimer.run();
	while (true) {
		ll syncTimeLeft = SYNC_WAIT_DELAY - syncTimer.value();
		ll iterationTimeLeft = (state == Solving) ? iteration_delay - iterationTimer.value() : 1e18;
		ll updateTimeLeft = (state == Solving) ? UPDATE_DELAY - updateTimer.value() : 1e18;
		ll timeLeft = max(min(syncTimeLeft, min(iterationTimeLeft, updateTimeLeft)), 0ll);
		if (timeLeft > 0)
			cpsleep(timeLeft);
 
		if (syncTimeLeft <= timeLeft) {
			PyObject *event;
			InterfaceGate::pylock();
			while ((event = InterfaceGate::extract_event()) != Py_None) {
				string event_type(PyUnicode_AsUTF8(PyDict_GetItemString(event, "type")));
				if (event_type == "PROBLEM_LOADING") {
					graph = new GraphData(PyDict_GetItemString(event, "problem"));
					AdvancedTimer opTimer;
					opTimer.run();
					RouteSolution *cur = new RouteSolution(graph);
					double badCost = cur->cost * 2;
					for (int i = 0; i < 10000; ++i) {
						RouteSolution *prev = new RouteSolution(cur);
						delete cur;
						cur = prev;
					}
					delete cur;
					double avg = opTimer.value() / double(10000);
					double tStart = -badCost / log(0.5);
					PyObject *update = PyDict_New();
					PyDict_SetItemString(update, "averageOpTime", PyFloat_FromDouble(avg));
					PyDict_SetItemString(update, "tStartRecommended", PyFloat_FromDouble(tStart));
					InterfaceGate::sendAppEvent("PROBLEM_READY", update);
					state = Ready;
				}
			}
			InterfaceGate::pyunlock();
			syncTimer.clear();
			syncTimer.run();
		}

		if (iterationTimeLeft <= timeLeft) {
			// Iteration
			iterationTimer.clear();
			iterationTimer.run();
		}

		if (updateTimeLeft <= timeLeft) {
			// Update
			updateTimer.clear();
			updateTimer.run();
		}
	}
	return 0;
}