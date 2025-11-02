CREATE DATABASE IF NOT EXISTS rfid_demo CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE rfid_demo;

-- Table 1: current number of workers (store one row)
CREATE TABLE IF NOT EXISTS current_workers (
  id          INT PRIMARY KEY DEFAULT 1,
  count_value INT NOT NULL
);

-- Table 2: list of workers inside the area
CREATE TABLE IF NOT EXISTS area_workers (
  id           INT AUTO_INCREMENT PRIMARY KEY,
  check_time   DATETIME NOT NULL,  -- when observed inside area
  name         VARCHAR(100) NOT NULL,
  phone_masked VARCHAR(32)  NOT NULL,
  memo         VARCHAR(255) NULL
);
