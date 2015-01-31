-- Create all tables

CREATE TABLE wiki (id INTEGER PRIMARY KEY, name TEXT, url TEXT, uw , uapis TEXT, ssl NUMERIC);
CREATE TABLE text (id INTEGER PRIMARY KEY, text TEXT, html TEXT);
CREATE TABLE page (id INTEGER PRIMARY KEY, name TEXT, text_id NUMERIC, space NUMERIC, last_edited NUMERIC, last_updated NUMERIC, wiki NUMERIC);
