CREATE TABLE CustomerInfo (
    id INT(16) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    email VARCHAR(100) NOT NULL,
    gender VARCHAR(6) NOT NULL,
    deviceOrder INT(5) NOT NULL,
    checkOutTime VARCHAR(19)
)