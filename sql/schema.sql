-- schema.sql
CREATE TABLE IF NOT EXISTS current_workers (
  id INT PRIMARY KEY AUTO_INCREMENT,
  count_value INT NOT NULL
);

CREATE TABLE IF NOT EXISTS area_workers (
  id INT PRIMARY KEY AUTO_INCREMENT,
  check_time DATETIME NOT NULL,
  name VARCHAR(100) NOT NULL,
  phone_masked VARCHAR(32),
  memo VARCHAR(255) DEFAULT ''
);

-- seed
INSERT INTO current_workers (count_value) VALUES (27);

INSERT INTO area_workers (check_time, name, phone_masked, memo) VALUES
  (NOW(),                      '김민수', '010-****-****', ''),
  (NOW() - INTERVAL 3 MINUTE,  '이세종', '010-****-****', ''),
  (NOW() - INTERVAL 10 MINUTE, '이수연', '010-****-****', ''),
  (NOW() - INTERVAL 16 MINUTE, '박이산', '010-****-****', ''),
  (NOW() - INTERVAL 21 MINUTE, '이순신', '010-****-****', '');
