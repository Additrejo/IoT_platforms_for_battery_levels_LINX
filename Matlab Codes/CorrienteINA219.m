% Configuración del canal ThingSpeak
channelID = 2805725; % Reemplaza con tu Channel ID
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % API Key si el canal es privado

% Leer datos de Corriente (Campo 5)
[corrienteData, corrienteTimestamps] = thingSpeakRead(channelID, ...
    'Fields', 5, ...
    'NumPoints', 8000, ...
    'ReadKey', readAPIKey);

% Verificar si hay datos disponibles
if isempty(corrienteData)
    error('No se encontraron datos suficientes en ThingSpeak.');
end

% Crear la figura y graficar la corriente
figure;
plot(corrienteTimestamps, corrienteData, '-b', 'LineWidth', 1.5);
ylabel('Corriente (A)');
xlabel('Tiempo');
title('Corriente desde ThingSpeak');
grid on;
legend('Corriente');
