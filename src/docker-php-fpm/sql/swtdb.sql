CREATE TABLE `activities` (
  `activity_id` integer  NOT NULL PRIMARY KEY AUTOINCREMENT
,  `user_id` integer  DEFAULT NULL
,  `product_id` integer NOT NULL
,  `software_version` integer  NOT NULL
,  `serial_number` integer  NOT NULL
,  `date` datetime NOT NULL
,  `pool_size_meters` float NOT NULL
,  `pool_size_units` integer NOT NULL
,  CONSTRAINT `fk_activities_files` FOREIGN KEY (`activity_id`) REFERENCES `files` (`file_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
,  CONSTRAINT `fk_activities_users` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
);
CREATE TABLE `editor_actions` (
  `editor_action_id` integer NOT NULL
,  `description` varchar(45) NOT NULL
,  PRIMARY KEY (`editor_action_id`)
);
CREATE TABLE `editor_log` (
  `editor_log_id` integer  NOT NULL PRIMARY KEY AUTOINCREMENT
,  `activity_id` integer  NOT NULL
,  `timestamp` timestamp NOT NULL DEFAULT current_timestamp 
,  `action_id` integer NOT NULL
,  `length_index` integer DEFAULT NULL
,  `change_stroke_from` integer DEFAULT NULL
,  `change_stroke_to` integer DEFAULT NULL
,  `change_stroke_option` integer DEFAULT NULL
,  `new_pool_size` float DEFAULT NULL
,  `new_pool_size_units` integer DEFAULT NULL
,  CONSTRAINT `fk_editor_log_actions` FOREIGN KEY (`action_id`) REFERENCES `editor_actions` (`editor_action_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
,  CONSTRAINT `fk_editor_log_activities` FOREIGN KEY (`activity_id`) REFERENCES `activities` (`activity_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
);
CREATE TABLE `error_log` (
  `error_log_id` integer  NOT NULL PRIMARY KEY AUTOINCREMENT
,  `timestamp` timestamp NOT NULL DEFAULT current_timestamp 
,  `script` varchar(256) DEFAULT NULL
,  `file_id` integer DEFAULT NULL
,  `message` varchar(1024) DEFAULT NULL
);
CREATE TABLE `files` (
  `file_id` integer  NOT NULL PRIMARY KEY AUTOINCREMENT
,  `name` varchar(256) NOT NULL
,  `uploaded` timestamp NOT NULL DEFAULT current_timestamp 
);
CREATE TABLE `products` (
  `product_id` integer NOT NULL PRIMARY KEY AUTOINCREMENT
,  `decription` varchar(45) NOT NULL
);
CREATE TABLE `users` (
  `user_id` integer  NOT NULL PRIMARY KEY AUTOINCREMENT
,  `username` varchar(45) NOT NULL
);
CREATE INDEX "idx_editor_log_fk_editor_log_activities_idx" ON "editor_log" (`activity_id`);
CREATE INDEX "idx_editor_log_fk_editor_log_actions_idx" ON "editor_log" (`action_id`);
CREATE INDEX "idx_activities_fk_activities_users_idx" ON "activities" (`user_id`);
CREATE INDEX "idx_activities_fk_activities_products_idx" ON "activities" (`product_id`);
INSERT INTO editor_actions VALUES (1, 'Merge'), (2, 'Split'), (3, 'Change stroke'), (4, 'Delete'), (5, 'Change Pool Size'), (6, 'Undo All'), (7, 'Download');
INSERT INTO products VALUES (1, 'Garmin Swim'), (2, 'Garmin Forerunner 910'), (3, 'Garmin Fénix 2'), (4, 'Garmin Forerunner 920'), (5, 'Tomtom Multisport'), (6, 'Garmin VivoActive'), (7, 'Garmin Fénix 3'), (8, 'Garmin Epix'), (9, 'Garmin Forerunner 735'), (10, 'Garmin Fénix 5'), (11, 'Garmin Forerunner 935'), (12, 'Garmin Vivoactive 3'), (13, 'Garmin D2 Charlie'), (14, 'Garmin Descent MK1'), (15, 'Garmin Approach S60'), (16, 'Garmin Forerunner 645'), (17,'Garmin Fenix 5 Plus') ;
