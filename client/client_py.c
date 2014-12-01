#include "Python.h"

#include "client.h"
#include "../controller/ds4.h"

static ds4_client_t *client = NULL;
static PyObject *ControllerError;

static PyObject* init(PyObject *self, PyObject *args) {
    ds4_client_destroy(&client);
    client = ds4_client_new();

    Py_RETURN_NONE;
}

static PyObject* connected(PyObject *self, PyObject *args) {
    if (!client) {
        PyErr_SetString(ControllerError, "Controller connection not initialized");
        return NULL;
    }
    return Py_BuildValue("i", ds4_client_connected(client));
}

static PyObject* getState(PyObject *self, PyObject *args) {
    if (!client) {
        PyErr_SetString(ControllerError, "Controller connection not initialized");
        return NULL;
    }

    ds4_controls_t *packed = ds4_client_controls(client);

    if (!packed) {
        PyErr_SetString(ControllerError, "Could not receive data from controller");
        return NULL;
    }

    PyObject *dict = PyDict_New();

    PyDict_SetItemString(dict, "left_analog_x", Py_BuildValue("i", packed->left_analog_x));
    PyDict_SetItemString(dict, "left_analog_y", Py_BuildValue("i", packed->left_analog_y));
    PyDict_SetItemString(dict, "right_analog_x", Py_BuildValue("i", packed->right_analog_x));
    PyDict_SetItemString(dict, "right_analog_y", Py_BuildValue("i", packed->right_analog_y));
    PyDict_SetItemString(dict, "dpad", Py_BuildValue("i", packed->dpad));
    PyDict_SetItemString(dict, "square", Py_BuildValue("i", packed->square));
    PyDict_SetItemString(dict, "cross", Py_BuildValue("i", packed->cross));
    PyDict_SetItemString(dict, "triangle", Py_BuildValue("i", packed->triangle));
    PyDict_SetItemString(dict, "l1", Py_BuildValue("i", packed->l1));
    PyDict_SetItemString(dict, "r1", Py_BuildValue("i", packed->r1));
    PyDict_SetItemString(dict, "l2", Py_BuildValue("i", packed->l2));
    PyDict_SetItemString(dict, "r2", Py_BuildValue("i", packed->r2));
    PyDict_SetItemString(dict, "share", Py_BuildValue("i", packed->share));
    PyDict_SetItemString(dict, "options", Py_BuildValue("i", packed->options));
    PyDict_SetItemString(dict, "l3", Py_BuildValue("i", packed->l3));
    PyDict_SetItemString(dict, "r3", Py_BuildValue("i", packed->r3));
    PyDict_SetItemString(dict, "ps_btn", Py_BuildValue("i", packed->ps_btn));
    PyDict_SetItemString(dict, "track_pad_btn", Py_BuildValue("i", packed->track_pad_btn));
    PyDict_SetItemString(dict, "timestamp", Py_BuildValue("i", packed->timestamp));
    PyDict_SetItemString(dict, "l2_analog", Py_BuildValue("i", packed->l2_analog));
    PyDict_SetItemString(dict, "r2_analog", Py_BuildValue("i", packed->r2_analog));
    PyDict_SetItemString(dict, "counter", Py_BuildValue("i", packed->counter));
    PyDict_SetItemString(dict, "battery", Py_BuildValue("i", packed->battery));
    PyDict_SetItemString(dict, "roll", Py_BuildValue("i", packed->roll));
    PyDict_SetItemString(dict, "pitch", Py_BuildValue("i", packed->pitch));
    PyDict_SetItemString(dict, "yaw", Py_BuildValue("i", packed->yaw));
    PyDict_SetItemString(dict, "accel_x", Py_BuildValue("i", packed->accel_x));
    PyDict_SetItemString(dict, "accel_y", Py_BuildValue("i", packed->accel_y));
    PyDict_SetItemString(dict, "accel_z", Py_BuildValue("i", packed->accel_z));

    return dict;
}

static PyObject* rumble(PyObject *self, PyObject *args) {
    if (!client) {
        PyErr_SetString(ControllerError, "Controller connection not initialized");
        return NULL;
    }

    ds4_client_rumble(client);

    Py_RETURN_NONE;
}

static PyObject* setColor(PyObject *self, PyObject *args) {
    if (!client) {
        PyErr_SetString(ControllerError, "Controller connection not initialized");
        return NULL;
    }

    int r, g, b;
    if (!PyArg_ParseTuple(args, "iii", &r, &g, &b)) {
        PyErr_SetString(ControllerError, "Invalid color arguments, takes three ints");
        return NULL;
    }

    ds4_client_rgb(client, r, g, b);

    Py_RETURN_NONE;
}


static PyMethodDef ControllerMethods[] = {
    {"init", init, METH_VARARGS, "Initialize the controller connection."},
    {"connected", connected, METH_VARARGS, "Check if a controller is connected."},
    {"getState", getState, METH_VARARGS, "Get the current controller state."},
    {"rumble", rumble, METH_VARARGS, "Rumble the controller."},
    {"setColor", setColor, METH_VARARGS, "Set the controller RGB LED color."}
};

PyMODINIT_FUNC initcontroller(void) {
    PyObject *module = Py_InitModule("controller", ControllerMethods);

    ControllerError = PyErr_NewException("Controller.Error", NULL, NULL);
    Py_INCREF(ControllerError);
    PyModule_AddObject(module, "error", ControllerError);
}
