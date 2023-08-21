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
  dispose() {
    dblib.symbols.deinit(this.#db);
  }
}

const db = new DB();

console.log("Listening on http://127.0.0.1:8080");

for await (const conn of Deno.listen({ port: 8080 })) {
  (async () => {
    for await (const e of Deno.serveHttp(conn)) {
      const cookie = e.request.headers.get("Cookie");
      if (typeof cookie !== "string") {
        e.respondWith(new Response("no cookie"));
        continue;
      }
      const user = auth.checkCookie(db, cookie);
      if (!user) {
        e.respondWith(new Response("not logged in"));
        continue;
      }
      e.respondWith(new Response("Hello, " + user));
    }
  })();
}

db.dispose();
dblib.close();
