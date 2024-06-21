import express from 'express';
import mysql from 'mysql2/promise';
import dotenv from 'dotenv';
import http from 'http';
import { Server } from 'socket.io';
import cors from 'cors';

dotenv.config();

const app = express();
const port = process.env.PORT || 3000;

// Crear servidor HTTP
const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: 'http://localhost:5173', // Asegúrate de que coincida con la URL de tu aplicación React
    methods: ['GET', 'POST'],
  },
});

// Middleware para analizar el cuerpo de las solicitudes JSON
app.use(express.json());
app.use(cors({
  origin: 'http://localhost:5173', // Asegúrate de que coincida con la URL de tu aplicación React
  methods: ['GET', 'POST'],
  allowedHeaders: ['Content-Type'],
}));

// Configuración de la conexión a la base de datos
const dbConfig = {
  host: process.env.DB_HOST,
  user: process.env.DB_USER,
  password: process.env.DB_PASS,
  database: process.env.DB_NAME,
};

const TOTAL_MEMORY_SYSTEM = 4 * 1024 * 1024 * 1024; // 4GB en bytes

let connection;

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

// Función para emitir actualizaciones en tiempo real
async function emitUpdates() {
  try {
    const [processes] = await connection.execute(`
      SELECT 
        pid, 
        process_name, 
        GREATEST(SUM(CASE WHEN my_call = 'mmap' THEN memory_size ELSE 0 END) - SUM(CASE WHEN my_call = 'munmap' THEN memory_size ELSE 0 END), 0) AS memory_used,
        (GREATEST(SUM(CASE WHEN my_call = 'mmap' THEN memory_size ELSE 0 END) - SUM(CASE WHEN my_call = 'munmap' THEN memory_size ELSE 0 END), 0) / ${TOTAL_MEMORY_SYSTEM}) * 100 AS memory_percentage
      FROM 
        memory_usage
      GROUP BY 
        pid, process_name
    `);

    const [memoryRequests] = await connection.execute(`
      SELECT 
        pid, 
        process_name, 
        my_call, 
        memory_size, 
        timestamp 
      FROM 
        memory_usage
    `);

    const [pieChartData] = await connection.execute(`
      SELECT 
        process_name, 
        SUM(memory_size) AS total_memory
      FROM 
        memory_usage
      WHERE 
        my_call = 'mmap'
      GROUP BY 
        process_name
      ORDER BY 
        total_memory DESC
  
    `);

    const totalMemoryQuery = await connection.execute(`
      SELECT SUM(memory_size) AS total_memory
      FROM memory_usage
      WHERE my_call = 'mmap'
    `);

    const totalMemory = totalMemoryQuery[0][0].total_memory;
    const otherMemory = totalMemory - pieChartData.reduce((sum, row) => sum + row.total_memory, 0);

    if (pieChartData.length === 10 && otherMemory > 0) {
      pieChartData.push({ process_name: 'Others', total_memory: otherMemory });
    }

    io.emit('update', { processes, memoryRequests, pieChartData });
  } catch (error) {
    console.error('Error al emitir actualizaciones:', error);
  }
}

// Configurar la conexión de Socket.IO
io.on('connection', (socket) => {
  console.log('Cliente conectado');
  emitUpdates();

  socket.on('disconnect', () => {
    console.log('Cliente desconectado');
  });
});

// Configurar el intervalo para emitir actualizaciones cada segundo
setInterval(emitUpdates, 1000);



// Iniciar el servidor
server.listen(port, () => {
  console.log(`Servidor escuchando en http://localhost:${port}`);
});
