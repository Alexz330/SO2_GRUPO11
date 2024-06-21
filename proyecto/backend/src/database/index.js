import mysql from 'mysql2/promise';
import dotenv from 'dotenv';

dotenv.config();


let connection;

const dbConfig = {
    host: process.env.DB_HOST,
    user: process.env.DB_USER,
    password: process.env.DB_PASS,
    database: process.env.DB_NAME,
};
  
console.log(dbConfig);
  

// Conectar a la base de datos
async function connectToDatabase() {
  try {
    connection = await mysql.createConnection(dbConfig);
    console.log('Conectado a la base de datos MySQL');
  } catch (error) {
    console.error('Error al conectar a la base de datos:', error);
  }
}


connectToDatabase();

export default connection;