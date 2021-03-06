#pragma once
#include <Python.h>
#include <vector>
#include "ChartData.cpp"

using namespace std;

typedef long long ll;

class InterfaceGate {
private:
	static PyObject *bridge_dict;
	static PyGILState_STATE state;
public:
	static void pylock() {
		state = PyGILState_Ensure();
	}

	static void pyunlock() {
		PyGILState_Release(state);
	}

	static void init() {
		Py_Initialize();
		PyEval_InitThreads();
		PyEval_SaveThread();

		pylock();
		bridge_dict = PyModule_GetDict(PyImport_ImportModule("bridge"));
		call("run_browser_async", PyTuple_New(0));
		pyunlock();
	}

	static PyObject* extract_event() {
		return call("extract_event", PyTuple_New(0));
	}

	static void sendAppEvent(string newState, PyObject *update) {
		PyObject *event = PyDict_New();
		if (newState != "")
			PyDict_SetItemString(event, "newState", PyUnicode_FromString(newState.c_str()));
		if (update != nullptr)
			PyDict_SetItemString(event, "update", update);
		call("sendAppEvent", PyTuple_Pack(1, event));
	}
private:
	InterfaceGate() {}

	static PyObject* call(string func, PyObject *args) {
		return PyObject_CallObject(PyDict_GetItemString(bridge_dict, func.c_str()), args);
	}
};

PyObject* InterfaceGate::bridge_dict;
PyGILState_STATE InterfaceGate::state;