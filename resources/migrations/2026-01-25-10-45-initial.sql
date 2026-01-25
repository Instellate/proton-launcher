CREATE TABLE games
(
    id                  TEXT    NOT NULL PRIMARY KEY,
    name                TEXT    NOT NULL,
    executable_location TEXT    NOT NULL,
    prefix_location     TEXT    NOT NULL,
    banner_location     TEXT             DEFAULT null,
    launch_arguments    TEXT    NOT NULL DEFAULT '',
    play_time           BIGINT  NOT NULL DEFAULT 0,
    last_played         DATEIME NOT NULL DEFAULT 0,
);
