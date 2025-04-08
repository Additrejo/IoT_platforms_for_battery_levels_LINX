% Código para obtener y graficar la taza de cambio en el voltaje.
% consulte el repositorio para su funcionamiento.
% Autor: Addi Trejo.
% Ult.Act: 08 Abril,2025.

% Configuración del canal ThingSpeak
channelID = 2805725; % Reemplaza con tu Channel ID
fieldID = 1; % Campo que quieres analizar (ejemplo: 1)
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % Reemplaza con tu API Key (si el canal es privado)

% Leer datos desde ThingSpeak
[data, timestamps] = thingSpeakRead(channelID, ...
    'Fields', fieldID, ...
    'NumPoints', 7000, ... % Número de puntos a analizar
    'ReadKey', readAPIKey); 

% Verificar si hay datos disponibles
if isempty(data)
    error('No se encontraron datos en el canal especificado.');
end

% Calcular estadísticas básicas
promedio = mean(data); % Promedio
maximo = max(data); % Máximo
minimo = min(data); % Mínimo

% Calcular la tasa de cambio (diferencia entre datos consecutivos)
tasaCambio = diff(data); % Calcula la diferencia entre puntos consecutivos
tasaCambioPromedio = mean(tasaCambio); % Promedio de la tasa de cambio

% Mostrar resultados en la consola
disp('Estadísticas de los datos:');
disp(['Promedio: ', num2str(promedio)]);
disp(['Máximo: ', num2str(maximo)]);
disp(['Mínimo: ', num2str(minimo)]);
disp(['Tasa de Cambio Promedio: ', num2str(tasaCambioPromedio)]);

% Crear la visualización
figure;

% Gráfico de los datos
subplot(2, 1, 1); % Subgráfico 1
plot(timestamps, data, '-o');
title('Voltaje');
xlabel('Tiempo');
ylabel('Voltaje');
grid on;

% Gráfico de la tasa de cambio
subplot(2, 1, 2); % Subgráfico 2
plot(timestamps(2:end), tasaCambio, '-o', 'Color', 'r');
title('Tasa de Cambio');
xlabel('Tiempo');
ylabel('Diferencia entre Datos');
grid on;