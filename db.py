import sqlite3
import uuid

# TODO: users Table, signup and login functions to be called from web_auth.c through web_server.ts


def connect():
    con = sqlite3.connect("retard_forum.db")
    con.execute(
        """CREATE TABLE IF NOT EXISTS sessions (
            id TEXT NOT NULL,
            user TEXT
        )"""
    )
    con.execute(
        """CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL
        )"""
    )
    con.commit()
    return con


def ensureUserExists(con: sqlite3.Connection, username: str, password_hash: str) -> int:
    con.execute(
        "INSERT OR IGNORE INTO users (username, password_hash) VALUES (?, ?)",
        (username, password_hash),
    )
    con.commit()
    cur = con.execute("SELECT id FROM users WHERE username = ?", (username,))
    res = cur.fetchone()
    return res[0]


def findSession(con: sqlite3.Connection, id: str) -> str:
    cur = con.execute("SELECT user FROM sessions WHERE id = ?", (id,))
    res = cur.fetchone()
    if res == None:
        return None
    return res[0]


def createSession(con: sqlite3.Connection) -> str:
    id = str(uuid.uuid4())
    con.execute("INSERT INTO sessions (id, user) VALUES (?, ?)", (id, None))
    con.commit()
    return id
