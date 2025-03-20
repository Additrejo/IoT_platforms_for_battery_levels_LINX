% Configuración del canal ThingSpeak
channelID = 2805725; % Reemplaza con tu Channel ID
fieldID = 1; % Campo que quieres analizar
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % Reemplaza con tu API Key si el canal es privado

% Leer datos desde ThingSpeak
[data, timestamps] = thingSpeakRead(channelID, ...
    'Fields', fieldID, ...
    'NumPoints', 7000, ... % Número de puntos a analizar
    'ReadKey', readAPIKey); 

% Verificar si hay datos disponibles
if isempty(data)
    error('No se encontraron datos en el canal especificado.');
end

% Calcular la tasa de cambio (diferencia entre datos consecutivos)
tasaCambio = diff(data); % Calcula la diferencia entre puntos consecutivos

% Gráfico de la tasa de cambio con color café
figure;
plot(timestamps(2:end), tasaCambio, '-o', 'Color', [0.545, 0.271, 0.075]); % Código RGB para café
title('Puntos de Carga y descarga máximos');
xlabel('Tiempo');
ylabel('Diferencia entre Datos');
grid on;
