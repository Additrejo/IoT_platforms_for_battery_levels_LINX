% Código para obtener y graficar 500 muestras del voltaje de una batería 18650 en thingspeak ,
% consulte el repositorio para su funcionamiento.
% Autor: Addi Trejo, 08 Abril,2025
% Ult Actu: 08 Abril 2025


channelID = 2805725; % Reemplaza con tu Channel ID
fieldID = 1; % Campo a visualizar
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % Reemplaza con tu API Key (si el canal es privado)
data = thingSpeakRead(channelID, 'Fields', fieldID, 'NumPoints', 500, 'ReadKey', readAPIKey);

% Crear un gráfico
figure;
plot(data, '-o');
title('Voltaje en 500 muestras');
xlabel('Número de Muestra');
ylabel('Volts [V]');
grid on;