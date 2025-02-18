% Configuración del canal ThingSpeak
channelID = 2805725; % Reemplaza con tu Channel ID
fieldID = 1; % Campo donde se registra el voltaje de la batería
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % Reemplaza con tu API Key si el canal es privado

% Leer hasta 8000 puntos de voltaje desde ThingSpeak
[data, timestamps] = thingSpeakRead(channelID, ...
    'Fields', fieldID, ...
    'NumPoints', 8000, ... % Límite máximo permitido
    'ReadKey', readAPIKey);

% Verificar si hay datos disponibles
numDatos = length(data);
disp(['Datos recuperados: ', num2str(numDatos)]);

if numDatos < 2
    error('No hay suficientes datos en ThingSpeak para graficar.');
end

% Especificar los voltajes de inicio y fin
voltajeInicio = 3.04; % Voltaje de inicio
voltajeFin = 4.13;    % Voltaje de fin

% Encontrar el índice del primer punto donde el voltaje cruza el inicio
indiceInicio = find(data >= voltajeInicio, 1, 'first');
% Encontrar el índice del primer punto donde el voltaje cruza el fin
indiceFin = find(data >= voltajeFin, 1, 'first');

% Verificar que se hayan encontrado ambos índices
if isempty(indiceInicio)
    warning('No se encontró un punto con voltaje >= %.2fV. Se usará el primer dato.', voltajeInicio);
    indiceInicio = 1; % Usar el primer dato disponible
end
if isempty(indiceFin)
    warning('No se encontró un punto con voltaje >= %.2fV. Se usará el último dato.', voltajeFin);
    indiceFin = numDatos; % Usar el último dato disponible
end

% Calcular el tiempo transcurrido entre ambos puntos
tiempoTranscurrido = minutes(timestamps(indiceFin) - timestamps(indiceInicio));

% Mostrar resultados en la consola
disp(['El tiempo aproximado para cargar de ', num2str(voltajeInicio), 'V a ', ...
    num2str(voltajeFin), 'V es de ', num2str(tiempoTranscurrido), ' minutos.']);

% Graficar el voltaje y marcar los puntos de inicio y fin
figure;
plot(timestamps, data, '-o', 'DisplayName', 'Voltaje');
hold on;
plot(timestamps(indiceInicio), data(indiceInicio), 'go', 'MarkerSize', 10, 'DisplayName', 'Inicio');
plot(timestamps(indiceFin), data(indiceFin), 'ro', 'MarkerSize', 10, 'DisplayName', 'Fin');
hold off;

% Personalizar el gráfico
title(['Carga de la batería: ', num2str(voltajeInicio), 'V a ', num2str(voltajeFin), 'V']);
xlabel('Tiempo');
ylabel('Voltaje (V)');
legend('show');
grid on;
