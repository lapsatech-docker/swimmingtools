-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema swtdb
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `swtdb` ;

-- -----------------------------------------------------
-- Schema swtdb
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `swtdb` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci ;
USE `swtdb` ;

-- -----------------------------------------------------
-- Table `swtdb`.`users`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `swtdb`.`users` ;

CREATE TABLE IF NOT EXISTS `swtdb`.`users` (
  `user_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `username` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`user_id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `swtdb`.`files`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `swtdb`.`files` ;

CREATE TABLE IF NOT EXISTS `swtdb`.`files` (
  `file_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(256) NOT NULL,
  `uploaded` TIMESTAMP NOT NULL,
  PRIMARY KEY (`file_id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `swtdb`.`products`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `swtdb`.`products` ;

CREATE TABLE IF NOT EXISTS `swtdb`.`products` (
  `product_id` SMALLINT NOT NULL AUTO_INCREMENT,
  `decription` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`product_id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `swtdb`.`activities`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `swtdb`.`activities` ;

CREATE TABLE IF NOT EXISTS `swtdb`.`activities` (
  `activity_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `user_id` INT UNSIGNED NULL,
  `product_id` SMALLINT NOT NULL,
  `software_version` SMALLINT UNSIGNED NOT NULL,
  `serial_number` INT UNSIGNED NOT NULL,
  `date` DATETIME NOT NULL,
  `pool_size_meters` FLOAT NOT NULL,
  `pool_size_units` TINYINT NOT NULL,
  PRIMARY KEY (`activity_id`),
  INDEX `fk_activities_users_idx` (`user_id` ASC),
  INDEX `fk_activities_products_idx` (`product_id` ASC),
  CONSTRAINT `fk_activities_users`
    FOREIGN KEY (`user_id`)
    REFERENCES `swtdb`.`users` (`user_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_activities_files`
    FOREIGN KEY (`activity_id`)
    REFERENCES `swtdb`.`files` (`file_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_activities_products`
    FOREIGN KEY (`product_id`)
    REFERENCES `swtdb`.`products` (`product_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `swtdb`.`editor_actions`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `swtdb`.`editor_actions` ;

CREATE TABLE IF NOT EXISTS `swtdb`.`editor_actions` (
  `editor_action_id` TINYINT NOT NULL,
  `description` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`editor_action_id`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `swtdb`.`editor_log`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `swtdb`.`editor_log` ;

CREATE TABLE IF NOT EXISTS `swtdb`.`editor_log` (
  `editor_log_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `activity_id` INT UNSIGNED NOT NULL,
  `timestamp` TIMESTAMP NOT NULL,
  `action_id` TINYINT NOT NULL,
  `length_index` SMALLINT NULL,
  `change_stroke_from` TINYINT NULL,
  `change_stroke_to` TINYINT NULL,
  `change_stroke_option` TINYINT NULL,
  `new_pool_size` FLOAT NULL,
  `new_pool_size_units` TINYINT NULL,
  PRIMARY KEY (`editor_log_id`),
  INDEX `fk_editor_log_activities_idx` (`activity_id` ASC),
  INDEX `fk_editor_log_actions_idx` (`action_id` ASC),
  CONSTRAINT `fk_editor_log_activities`
    FOREIGN KEY (`activity_id`)
    REFERENCES `swtdb`.`activities` (`activity_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_editor_log_actions`
    FOREIGN KEY (`action_id`)
    REFERENCES `swtdb`.`editor_actions` (`editor_action_id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `swtdb`.`error_log`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `swtdb`.`error_log` ;

CREATE TABLE IF NOT EXISTS `swtdb`.`error_log` (
  `error_log_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `timestamp` TIMESTAMP NOT NULL,
  `script` VARCHAR(256) NULL,
  `file_id` INT NULL,
  `message` VARCHAR(1024) NULL,
  PRIMARY KEY (`error_log_id`))
ENGINE = InnoDB;

SET SQL_MODE = '';
GRANT USAGE ON *.* TO apache;
 DROP USER apache;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'apache' IDENTIFIED BY 'b1ozvc30';

GRANT SELECT, INSERT, TRIGGER, UPDATE, DELETE ON TABLE `swtdb`.* TO 'apache';
SET SQL_MODE = '';
GRANT USAGE ON *.* TO swtreader;
 DROP USER swtreader;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'swtreader' IDENTIFIED BY 'b1ozvc30';

GRANT SELECT ON TABLE `swtdb`.* TO 'swtreader';
SET SQL_MODE = '';
GRANT USAGE ON *.* TO swtadmin;
 DROP USER swtadmin;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'swtadmin' IDENTIFIED BY 'O97nn}8d1';

GRANT ALL ON `swtdb`.* TO 'swtadmin';

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
-- begin attached script 'script'
INSERT INTO editor_actions VALUES (1, 'Merge'), (2, 'Split'), (3, 'Change stroke'), (4, 'Delete'), (5, 'Change Pool Size'), (6, 'Undo All'), (7, 'Download');
INSERT INTO products VALUES (1, 'Garmin Swim'), (2, 'Garmin Forerunner 910'), (3, 'Garmin Fénix 2'), (4, 'Garmin Forerunner 920'), (5, 'Tomtom Multisport'), (6, 'Garmin VivoActive'), (7, 'Garmin Fénix 3'), (8, 'Garmin Epix'), (9, 'Garmin Forerunner 735'), (10, 'Garmin Fénix 5'), (11, 'Garmin Forerunner 935");
-- end attached script 'script'
