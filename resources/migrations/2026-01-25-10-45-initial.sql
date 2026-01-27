CREATE TABLE games
(
    id                  TEXT NOT NULL PRIMARY KEY,
    name                TEXT NOT NULL,
    executable_location TEXT NOT NULL,
    prefix_location     TEXT NOT NULL,
    banner_location     TEXT    DEFAULT NULL,
    launch_arguments    TEXT    DEFAULT NULL,
    proton_path         TEXT    DEFAULT NULL,
    play_time           BIGINT  DEFAULT NULL,
    last_played         DATEIME DEFAULT NULL
)
