#include "../argon2/include/argon2.h"
#include "../include/node_api/node_api.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h>

void hash_password(uint8_t *pwd, size_t pwd_len, uint8_t *out_buf) {
#define salt_len 16
  uint8_t salt[salt_len];
  ssize_t written = getrandom(salt, salt_len, 0);
  assert(written != -1);
#define hash_len 32
  uint8_t hash[hash_len];
  int status = argon2id_hash_raw(2, 1 << 16, 1, pwd, pwd_len, salt, salt_len,
                                 hash, hash_len);
  assert(status == ARGON2_OK);
  for (size_t i = 0; i < hash_len; i++)
    out_buf[i] = hash[i];
  for (size_t i = 0; i < salt_len; i++)
    out_buf[hash_len + i] = salt[i];
}

napi_value hash_password_napi(napi_env env, uint8_t *pwd, size_t pwd_len) {
  void *data;
  napi_value js_hash_value;
  napi_status status = napi_create_arraybuffer(env, 48, &data, &js_hash_value);
  assert(status == napi_ok);
  hash_password(pwd, pwd_len, data);
  return js_hash_value;
}

napi_value signup(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_value argv[3];
  napi_status status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  assert(status == napi_ok);
  napi_value ensureUserExists;
  status = napi_get_named_property(env, argv[0], "ensureUserExists",
                                   &ensureUserExists);
  assert(status == napi_ok);
  napi_value js_hash_value = hash_password_napi(env, (uint8_t *)"password", 8);
  // napi_call_function(env, argv[0], ensureUserExists, 2);
  return js_hash_value;
}

napi_value login(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_status status = napi_get_cb_info(env, info, &argc, &argv, NULL, NULL);
  assert(status == napi_ok);
  printf("fart\n");
}

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

  napi_value signupFunc;
  status = napi_create_function(env, "signup", 2, signup, NULL, &signupFunc);
  assert(status == napi_ok);
  status = napi_set_named_property(env, exports, "signup", signupFunc);
  assert(status == napi_ok);
  return 0;
}
