-- Create all tables

-- Wiki table contains information about all wikis that are in this datafile
CREATE TABLE wiki
(
    "id" INTEGER PRIMARY KEY NOT NULL,
    "name" TEXT NOT NULL,
    "url" TEXT NOT NULL,
    "uw" NULL NOT NULL,
    "uapis" TEXT NOT NULL,
    "ssl" NUMERIC NOT NULL,
    "wiki_init" NUMERIC NOT NULL,
    "time_init" NUMERIC
);

CREATE INDEX idx_wiki_name ON wiki(name);

CREATE TABLE revision
(
    "id" INTEGER PRIMARY KEY,
    "rev_id" INTEGER NOT NULL,
    "rev_page" INTEGER NOT NULL,
    "rev_text_id" INTEGER NOT NULL,
    "rev_comment" TEXT NOT NULL,
    "rev_user_text" TEXT NOT NULL,
    "rev_timestamp" INTEGER NOT NULL,
    "rev_minor_edit" INTEGER NOT NULL,
    "rev_deleted" INTEGER NOT NULL,
    "rev_len" INTEGER NOT NULL,
    "rev_parent_id" INTEGER NOT NULL,
    "rev_sha1" TEXT,
    "rev_content_model" TEXT,
    "rev_content_format" TEXT,
    "wiki" INTEGER,
    "downloaded" INTEGER
);

CREATE INDEX idx_revision_rev_id ON revision(rev_id);

CREATE TABLE text
(
    "id" INTEGER PRIMARY KEY,
    "wiki" INTEGER NOT NULL,
    "text" TEXT NOT NULL,
    "html" TEXT NOT NULL
);

CREATE INDEX idx_text_wiki ON text(wiki);

CREATE TABLE page
(
    "id" INTEGER PRIMARY KEY,
    "name" TEXT UNIQUE NOT NULL,
    "preferred_rev_id" NUMERIC,
    "latest" NUMERIC,
    "content_model" TEXT,
    "lang" TEXT,
    "namespace" NUMERIC NOT NULL,
    "last_edited" NUMERIC,
    "last_updated" NUMERIC,
    "wiki" NUMERIC NOT NULL,
    "deleted" NUMERIC,
    "downloaded" NUMERIC NOT NULL,
    "counter" NUMERIC,
    "is_redirect" NUMERIC,
    "is_new" NUMERIC,
    "current_revision" NUMERIC,
    "last_revision" NUMERIC
);

CREATE INDEX idx_page_wiki ON page(wiki);
CREATE INDEX idx_page_deleted ON page(deleted);
CREATE INDEX idx_page_name ON page(name);
CREATE INDEX idx_page_namespace ON page(namespace);

CREATE TABLE namespaces
(
    "id" INTEGER PRIMARY KEY,
    "wiki" NUMERIC NOT NULL,
    "namespace_id" NUMERIC NOT NULL,
    "name" TEXT NOT NULL,
    "canonical" TEXT NOT NULL,
    "is_talk" NUMERIC NOT NULL
);

CREATE TABLE metadata
(
    "id" INTEGER PRIMARY KEY,
    "name" TEXT,
    "value" TEXT
);

BEGIN;
INSERT INTO metadata (name, value) VALUES ('version', '1.0.0');
INSERT INTO metadata (name, value) VALUES ('created', CURRENT_TIMESTAMP);
COMMIT;
