% Título: Gráfica de Temperatura de la Batería
% Descripción: Lee datos de temperatura de un canal de ThingSpeak y los 
% muestra en una gráfica de tiempo.

% --- Configuración del Canal y Campo ---
channelID = 2805725;
readAPIKey = 'PWMRQ0HEW7NBTBBP';

% !!! IMPORTANTE: Cambia '7' al número de campo donde envías la temperatura.
fieldID_Temperatura = 3; 

% --- Lectura de Datos de ThingSpeak ---
% Leemos los últimos 4500 puntos (aprox. 1 día de datos si envías cada 20s)
[temperaturaData, time] = thingSpeakRead(channelID, ...
                                         'ReadKey', readAPIKey, ...
                                         'Fields', fieldID_Temperatura, ...
                                         'NumPoints', 8000);

% --- Creación de la Gráfica ---
if isempty(time)
    % Mensaje si no se encuentran datos
    text(0.5, 0.5, 'No se encontraron datos de temperatura para graficar.', 'HorizontalAlignment', 'center');
else
    % Si se encuentran datos, se crea la gráfica de líneas
    plot(time, temperaturaData, 'LineWidth', 1.5, 'Color', '#FF5733'); % Color naranja/rojo
    
    % --- Formato y Estilo de la Gráfica ---
    grid on;
    title('Temperatura de la Batería');
    xlabel('Fecha y Hora');
    ylabel('Temperatura (°C)'); % Puedes cambiar a °F si usas esa unidad
end