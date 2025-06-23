CREATE DATABASE soil_monitoring;

USE soil_monitoring;

CREATE TABLE soil_data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    soil1 INT NOT NULL,
    soil2 INT NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);