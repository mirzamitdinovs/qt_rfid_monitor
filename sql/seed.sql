-- sql/seed.sql
USE rfid_demo;

-- =========================================
-- Create tables if they don't exist
-- =========================================

CREATE TABLE IF NOT EXISTS current_workers (
  id INT PRIMARY KEY,
  count_value INT NOT NULL,
  updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
    ON UPDATE CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS area_workers (
  id INT AUTO_INCREMENT PRIMARY KEY,
  check_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  name VARCHAR(64) NOT NULL,
  phone_masked VARCHAR(32) NOT NULL,
  memo TEXT
);

-- =========================================
-- Seed demo data
-- =========================================

-- Insert or update total count
INSERT INTO current_workers (id, count_value)
VALUES (1, 5)
ON DUPLICATE KEY UPDATE count_value = VALUES(count_value);

-- Replace all worker rows with sample data
DELETE FROM area_workers;
INSERT INTO area_workers (check_time, name, phone_masked, memo) VALUES
  (NOW(),                   '김민수', '010-****-****', ''),
  (NOW() - INTERVAL 3 MINUTE,  '이세종', '010-****-****', ''),
  (NOW() - INTERVAL 10 MINUTE, '이수연', '010-****-****', ''),
  (NOW() - INTERVAL 16 MINUTE, '박이산', '010-****-****', ''),
  (NOW() - INTERVAL 21 MINUTE, '이순신', '010-****-****', '');
