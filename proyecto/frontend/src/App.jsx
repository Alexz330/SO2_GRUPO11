import React, { useEffect, useState } from "react";
import io from "socket.io-client";
import { Pie } from "react-chartjs-2";
import "chart.js/auto";
import "./App.css";

const socket = io("http://localhost:3000"); // Asegúrate de que la URL coincide con tu servidor

const TOTAL_MEMORY_SYSTEM = 4 * 1024 * 1024 * 1024; // 4GB en bytes

function App() {
  const [processes, setProcesses] = useState([]);
  const [memoryRequests, setMemoryRequests] = useState([]);
  const [pieChartData, setPieChartData] = useState([]);

  // Estado para la paginación
  const [processPage, setProcessPage] = useState(0);
  const [requestPage, setRequestPage] = useState(0);
  const itemsPerPage = 10;

  useEffect(() => {
    socket.on("update", (data) => {
      setProcesses(data.processes);
      setMemoryRequests(data.memoryRequests);
      console.log(data.pieChartData, "hola");
      // Calcular el porcentaje de memoria usada
      const processedPieChartData = data.pieChartData.map((item) => ({
        ...item,
        percentage: (item.total_memory / TOTAL_MEMORY_SYSTEM) * 100,
      }));

      // Ordenar por porcentaje de memoria usada
      processedPieChartData.sort((a, b) => b.percentage - a.percentage);

      // Agrupar en "Otros" si hay más de 10 procesos
      if (processedPieChartData.length > 10) {
        const topTen = processedPieChartData.slice(0, 10);
        const others = processedPieChartData.slice(10);

        const otherTotalMemory = others.reduce(
          (sum, item) => sum + item.total_memory,
          0
        );
        const otherPercentage = (otherTotalMemory / TOTAL_MEMORY_SYSTEM) * 100;

        topTen.push({
          process_name: "Otros procesos",
          total_memory: otherTotalMemory,
          percentage: otherPercentage,
        });

        setPieChartData(topTen);
      } else {
        setPieChartData(processedPieChartData);
      }
    });

    return () => {
      socket.off("update");
    };
  }, []);

  const pieData = {
    labels: pieChartData.map((item) => item.process_name),
    datasets: [
      {
        label: "Memory Usage",
        data: pieChartData.map((item) => item.percentage),
        backgroundColor: [
          "#2ECC71",
          "#27AE60",
          "#665191",
          "rgba(100, 102, 255, 0.7)",
          "#FF5733",
          "#f95d6a",
          "#36A2EB",
          "#FFCE56",
          "#FF6384",
          "rgba(153, 102, 255, 0.7)",
          "#AAAAAA", // Color para "Others"
        ],
      },
    ],
  };

  const handleNextProcessPage = () => {
    setProcessPage((prevPage) => prevPage + 1);
  };

  const handlePrevProcessPage = () => {
    setProcessPage((prevPage) => Math.max(prevPage - 1, 0));
  };

  const handleNextRequestPage = () => {
    setRequestPage((prevPage) => prevPage + 1);
  };

  const handlePrevRequestPage = () => {
    setRequestPage((prevPage) => Math.max(prevPage - 1, 0));
  };

  const paginatedProcesses = processes.slice(
    processPage * itemsPerPage,
    (processPage + 1) * itemsPerPage
  );
  const paginatedRequests = memoryRequests.slice(
    requestPage * itemsPerPage,
    (requestPage + 1) * itemsPerPage
  );

  return (
    <div className="App p-8 bg-gray-100 min-h-screen">
      <h1 className="text-4xl font-bold mb-8 text-center">Dashboard</h1>

      <div className="flex flex-col lg:flex-row justify-between mb-12">
        <div className="w-full lg:w-1/2 flex justify-center mb-8 lg:mb-0">
          <div className="w-96 h-96">
            <Pie data={pieData} />
          </div>
        </div>
        <div className="w-full lg:w-1/2">
          <h2 className="text-2xl font-semibold mb-4">Processes</h2>
          <table className="min-w-full bg-white shadow-md rounded">
            <thead>
              <tr>
                <th className="py-2 px-4 border-b">#</th>
                <th className="py-2 px-4 border-b">PID</th>
                <th className="py-2 px-4 border-b">Nombre</th>
                <th className="py-2 px-4 border-b">Memoria</th>
                <th className="py-2 px-4 border-b">Porcentaje</th>
              </tr>
            </thead>
            <tbody>
              {paginatedProcesses.map((process, index) => (
                <tr key={process.pid} className="hover:bg-gray-100">
                  <td className="py-2 px-4 border-b">
                    {processPage * itemsPerPage + index + 1}
                  </td>
                  <td className="py-2 px-4 border-b">{process.pid}</td>
                  <td className="py-2 px-4 border-b">{process.process_name}</td>
                  <td className="py-2 px-4 border-b">{process.memory_used}</td>
                  <td className="py-2 px-4 border-b">
                    {process.memory_percentage}%
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
          <div className="flex justify-between mt-4">
            <button
              onClick={handlePrevProcessPage}
              disabled={processPage === 0}
              className="py-2 px-4 bg-blue-500 text-white rounded disabled:opacity-50"
            >
              Anterior
            </button>
            <button
              onClick={handleNextProcessPage}
              disabled={(processPage + 1) * itemsPerPage >= processes.length}
              className="py-2 px-4 bg-blue-500 text-white rounded disabled:opacity-50"
            >
              Siguiente
            </button>
          </div>
        </div>
      </div>

      <div className="mb-6">
        <h2 className="text-2xl font-semibold mb-4">Solicitudes</h2>
        <table className="min-w-full bg-white shadow-md rounded">
          <thead>
            <tr>
              <th className="py-2 px-4 border-b">#</th>
              <th className="py-2 px-4 border-b">PID</th>
              <th className="py-2 px-4 border-b">Llamada</th>
              <th className="py-2 px-4 border-b">Tamaño</th>
              <th className="py-2 px-4 border-b">Fecha</th>
            </tr>
          </thead>
          <tbody>
            {paginatedRequests.map((request, index) => (
              <tr
                key={`${request.pid}-${request.timestamp}-${request.my_call}-${index}`}
                className="hover:bg-gray-100"
              >
                <td className="py-2 px-4 border-b">
                  {requestPage * itemsPerPage + index + 1}
                </td>
                <td className="py-2 px-4 border-b">{request.pid}</td>
                <td className="py-2 px-4 border-b">{request.my_call}</td>
                <td className="py-2 px-4 border-b">{request.memory_size}</td>
                <td className="py-2 px-4 border-b">
                  {new Date(request.timestamp).toLocaleString()}
                </td>
              </tr>
            ))}
          </tbody>
        </table>
        <div className="flex justify-between mt-4">
          <button
            onClick={handlePrevRequestPage}
            disabled={requestPage === 0}
            className="py-2 px-4 bg-blue-500 text-white rounded disabled:opacity-50"
          >
            Anterior
          </button>
          <button
            onClick={handleNextRequestPage}
            disabled={(requestPage + 1) * itemsPerPage >= memoryRequests.length}
            className="py-2 px-4 bg-blue-500 text-white rounded disabled:opacity-50"
          >
            Siguiente
          </button>
        </div>
      </div>
    </div>
  );
}

export default App;
