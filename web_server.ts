import { createRequire } from "node:module";

const require = createRequire(import.meta.url);

const auth = require("./web_auth.node");

const dblib = Deno.dlopen(
  "./libdb.so",
  {
    init: { parameters: [], result: "pointer" },
    deinit: { parameters: ["pointer"], result: "void" },
    findSession: { parameters: ["pointer", "buffer"], result: "pointer" },
    createSession: { parameters: ["pointer"], result: "pointer" },
    ensureUserExists: {
      parameters: ["pointer", "buffer", "buffer"],
      result: "i32",
    },
  },
);

const cstr = (s: string) => {
  const b = new Uint8Array(s.length + 1);
  const { written } = new TextEncoder().encodeInto(s, b);
  return b.subarray(0, written);
};

class DB {
  #db: Deno.PointerValue;
  constructor() {
    this.#db = dblib.symbols.init();
  }
  findSession(id: string) {
    const user = dblib.symbols.findSession(this.#db, cstr(id));
    if (user) {
      const view = new Deno.UnsafePointerView(user);
      return view.getCString();
    }
    return null;
  }
  createSession() {
    const user = dblib.symbols.createSession(this.#db)!;
    const view = new Deno.UnsafePointerView(user);
    return view.getCString();
  }
  ensureUserExists(username: string, password_hash: string) {
    const username_cstr = cstr(username);
    const password_hash_cstr = cstr(password_hash);
    return dblib.symbols.ensureUserExists(
      this.#db,
      username_cstr,
      password_hash_cstr,
    );
  }
  dispose() {
    dblib.symbols.deinit(this.#db);
  }
}

const db = new DB();

console.log("Listening on http://127.0.0.1:8080");

for await (const conn of Deno.listen({ port: 8080 })) {
  (async () => {
    for await (const e of Deno.serveHttp(conn)) {
      // const cookie = e.request.headers.get("Cookie");
      // if (typeof cookie !== "string") {
      //   e.respondWith(new Response("no cookie"));
      //   continue;
      // }
      // const user = auth.checkCookie(db, cookie);
      // if (!user) {
      //   e.respondWith(new Response("not logged in"));
      //   continue;
      // }
      // e.respondWith(new Response("Hello, " + user));
      const url = new URL(e.request.url);
      if (
        url.pathname === "/login" && e.request.method === "POST" &&
        e.request.body
      ) {
        let body = "";
        for await (
          const chunk of e.request.body.pipeThrough(new TextDecoderStream())
        ) {
          body += chunk;
        }
        const loginInfo = new URLSearchParams(body);
        console.log(Object.fromEntries(loginInfo.entries()));
        // TODO: login logic (web_auth.c must handle login)
        continue;
      }
      if (
        url.pathname === "/signup" && e.request.method === "POST" &&
        e.request.body
      ) {
        let body = "";
        for await (
          const chunk of e.request.body.pipeThrough(new TextDecoderStream())
        ) {
          body += chunk;
        }
        const signupInfo = Object.fromEntries(
          new URLSearchParams(body).entries(),
        );
        console.log(signupInfo);
        if (
          "username" in signupInfo &&
          "password" in signupInfo &&
          typeof signupInfo.username === "string" &&
          typeof signupInfo.password === "string"
        ) {
          e.respondWith(
            new Response(
              `User ID: ${
                db.ensureUserExists(signupInfo.username, signupInfo.password)
              }`,
            ),
          );
          continue;
        }
      }
      e.respondWith(
        new Response(
          `Login<form action="/login" method="POST">
<input type="text" name="username" />
<input type="password" name="password" />
<input type="submit" value="Login" />
</form><br />
Signup<form action="/signup" method="POST">
<input type="text" name="username" />
<input type="password" name="password" />
<input type="submit" value="Signup" />
</form>`,
          {
            headers: {
              "Content-Type": "text/html",
            },
          },
        ),
      );
    }
  })();
}

db.dispose();
dblib.close();
