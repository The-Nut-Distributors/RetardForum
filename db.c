#include <Python.h>
#include <assert.h>
#include <dlfcn.h>
#include <unistd.h>

// TODO: Figure out how to print exceptions from called Python functions

typedef struct {
  PyObject *module;
  PyObject *connection;
} DB;

DB *init(void) {
  // Let sqlite3 in Python find the Python library
  dlopen("/usr/lib/python3.9/config-3.9-x86_64-linux-gnu/libpython3.9.so",
         RTLD_GLOBAL | RTLD_NOW);
  char cwd_buf[PATH_MAX];
  char *cwd = getwd(&cwd_buf);
  Py_InitializeEx(0);
  PyObject *sys_path = PySys_GetObject("path");
  PyObject *cwdPy = PyUnicode_FromString(cwd);
  int status = PyList_Append(sys_path, cwdPy);
  assert(status == 0);
  Py_DECREF(cwdPy);
  PyObject *module = PyImport_ImportModule("db");
  if (module == NULL) {
    PyErr_Print();
    return NULL;
  }
  PyObject *connect = PyObject_GetAttrString(module, "connect");
  PyObject *connection = PyObject_CallObject(connect, NULL);
  DB *db = malloc(sizeof(DB));
  db->module = module;
  db->connection = connection;
  return db;
}

void deinit(DB *db) {
  Py_DECREF(db->connection);
  Py_DECREF(db->module);
  free(db);
  Py_FinalizeEx();
}

char *findSession(DB *db, const char *id) {
  PyObject *fs = PyObject_GetAttrString(db->module, "findSession");
  PyObject *args = PyTuple_New(2);
  int status;
  status = PyTuple_SetItem(args, 0, db->connection);
  assert(status == 0);
  PyObject *idPy = PyUnicode_FromString(id);
  status = PyTuple_SetItem(args, 1, idPy);
  assert(status == 0);
  PyObject *result = PyObject_CallObject(fs, args);
  if (result == Py_None) {
    return NULL;
  }
  ssize_t size;
  const char *user_ = PyUnicode_AsUTF8AndSize(result, &size);
  char *user = malloc(size + 1);
  for (ssize_t i = 0; i < size; i++) {
    user[i] = user_[i];
  }
  user[size] = 0;
  return user;
}

char *createSession(DB *db) {
  PyObject *cs = PyObject_GetAttrString(db->module, "createSession");
  PyObject *result = PyObject_CallOneArg(cs, db->connection);
  ssize_t size;
  const char *id_ = PyUnicode_AsUTF8AndSize(result, &size);
  char *id = malloc(size + 1);
  for (ssize_t i = 0; i < size; i++) {
    id[i] = id_[i];
  }
  id[size] = 0;
  return id;
}
