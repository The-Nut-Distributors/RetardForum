import sqlite3
import uuid

def connect():
    con = sqlite3.connect("furom.db")
    con.execute(
        """CREATE TABLE IF NOT EXISTS sessions (
        id TEXT NOT NULL,
        user TEXT
    )"""
    )
    return con


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
