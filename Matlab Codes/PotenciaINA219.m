% Configuración del canal ThingSpeak
channelID = 2805725; % Reemplaza con tu Channel ID
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % API Key si el canal es privado

% Leer datos de Voltaje (Campo 1) y Corriente (Campo 5)
[voltajeData, voltajeTimestamps] = thingSpeakRead(channelID, ...
    'Fields', 1, ...
    'NumPoints', 8000, ...
    'ReadKey', readAPIKey);

[corrienteData, corrienteTimestamps] = thingSpeakRead(channelID, ...
    'Fields', 5, ...
    'NumPoints', 8000, ...
    'ReadKey', readAPIKey);

% Verificar si hay datos disponibles
if isempty(voltajeData) || isempty(corrienteData)
    error('No se encontraron datos suficientes en ThingSpeak.');
end

% Calcular potencia (P = V * I)
potenciaData = voltajeData .* corrienteData;
potenciaTimestamps = min(voltajeTimestamps, corrienteTimestamps);

% Crear la figura y graficar la potencia
figure;
plot(potenciaTimestamps, potenciaData, '-m', 'LineWidth', 1.5);
ylabel('Potencia (W)');
xlabel('Tiempo');
title('Potencia desde ThingSpeak');
grid on;
legend('Potencia');
