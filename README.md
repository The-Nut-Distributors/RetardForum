# RetardForum

| Feature                           | Language         | Status |
| --------------------------------- | ---------------- | ------ |
| Web server                        | Deno             | ✅     |
| Authentication                    | C                | ✅     |
| User database                     | Python (sqlite3) | ✅     |
| Websocket realtime chat           | Nim              | ❌     |
| Forum posts                       | Zig              | ❌     |
| Emoji picker                      | Rust             | ❌     |
| Utf8 validator                    | Golang           | ❌     |
| Freeing all malloc'd memory       | C++              | ❌     |
| Loader / startup code / unpacking | Java             | ❌     |

## Dependencies required

-  [Curl](https://curl.se/)
-  [Zig](https://ziglang.org/)
-  [Deno](https://deno.land/)
-  [Python](https://python.org/)

## Build and run

```sh
zig run -lc build.c
```

## Contributing

3 space indent, see [`.editorconfig`](.editorconfig)
