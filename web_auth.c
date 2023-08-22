#include "include/node_api/node_api.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

napi_value signup(napi_env env, napi_callback_info info) {}

napi_value check_cookie(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value ok;
  size_t argc = 2;
  napi_value argv[2];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  assert(status == napi_ok);

  napi_value findSession;
  status = napi_get_named_property(env, argv[0], "findSession", &findSession);
  assert(status == napi_ok);
  napi_value user;
  status = napi_call_function(env, argv[0], findSession, 1, argv + 1, &user);
  assert(status == napi_ok);
  napi_valuetype user_typeof;
  status = napi_typeof(env, user, &user_typeof);
  assert(status == napi_ok);
  if (user_typeof == napi_null) {
    napi_value createSession;
    status =
        napi_get_named_property(env, argv[0], "createSession", &createSession);
    assert(status == napi_ok);
    napi_value id;
    status = napi_call_function(env, argv[0], createSession, 0, NULL, &id);
    assert(status == napi_ok);
    return id;
  }
  return user;
}

napi_value napi_register_module_v1(napi_env env, napi_value exports) {
  napi_status status;
  napi_value checkCookie;
  status = napi_create_function(env, "checkCookie", 11, check_cookie, NULL,
                                &checkCookie);
  assert(status == napi_ok);
  status = napi_set_named_property(env, exports, "checkCookie", checkCookie);
  assert(status == napi_ok);
  return 0;
}
