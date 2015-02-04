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
    "wiki_init" NUMERIC NOT NULL
);

CREATE INDEX idx_wiki_name ON wiki(name);

CREATE TABLE revision
(
    "id" INTEGER PRIMARY KEY,
    "rev_id" INTEGER,
    "rev_page" INTEGER,
    "rev_text_id" INTEGER,
    "rev_comment" TEXT,
    "rev_user_text" TEXT,
    "rev_timestamp" INTEGER,
    "rev_minor_edit" INTEGER,
    "rev_deleted" INTEGER,
    "rev_len" INTEGER,
    "rev_parent_id" INTEGER,
    "rev_sha1" TEXT,
    "rev_content_model" TEXT,
    "rev_content_format" TEXT,
    "rev_wiki" INTEGER
);

CREATE INDEX idx_revision_rev_id ON revision(rev_id);

CREATE TABLE text
(
    "id" INTEGER PRIMARY KEY,
    "text" TEXT,
    "html" TEXT
);

CREATE TABLE page
(
    "id" INTEGER PRIMARY KEY,
    "name" TEXT,
    "preferred_rev_id" NUMERIC,
    "last_rev_id" NUMERIC,
    "namespace" NUMERIC,
    "last_edited" NUMERIC,
    "last_updated" NUMERIC,
    "wiki" NUMERIC,
    "deleted" NUMERIC,
    "downloaded" NUMERIC,
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
    "wiki" NUMERIC,
    "namespace_id" NUMERIC,
    "name" TEXT,
    "is_talk" NUMERIC
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
