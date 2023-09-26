import sqlite3

con = sqlite3.connect("db/retard_forum.db")
print("sessions", con.execute("SELECT COUNT(*) FROM sessions").fetchone()[0])
cur = con.execute("SELECT * FROM sessions")
for row in cur:
    print(row)
print("users", con.execute("SELECT COUNT(*) FROM users").fetchone()[0])
cur = con.execute("SELECT * FROM users")
for row in cur:
    print(row)
