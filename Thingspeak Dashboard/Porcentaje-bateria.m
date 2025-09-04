% Visualización en tiempo real del porcentaje de batería con área rellena
% ThingSpeak MATLAB Visualization
% Autor: Addi Trejo
% Ult Actu: 03-Sep-2025

channelID = 2805725;
fieldID = 1; % Campo con el voltaje
readAPIKey = 'PWMRQ0HEW7NBTBBP';

% Leer últimas 8000 muestras
[voltData, time] = thingSpeakRead(channelID, ...
    'Fields', fieldID, ...
    'NumPoints', 8000, ...
    'ReadKey', readAPIKey);

if isempty(time)
    disp('⚠️ No se obtuvieron datos de ThingSpeak.');
else
    % Ajustar zona horaria a Ciudad de México (GMT-6, sin horario de verano)
    time.TimeZone = 'America/Mexico_City';

    % Conversión Voltaje -> Porcentaje
    Vmin = 3.00;
    Vmax = 4.19;
    percentData = (voltData - Vmin) / (Vmax - Vmin) * 100;
    percentData(percentData > 100) = 100;
    percentData(percentData < 0) = 0;

    % Crear gráfica de área
    area(time, percentData, 'FaceColor', [0 0.6 0.6], 'EdgeColor', 'k');

    % Etiquetas y estilo
    title('Porcentaje de batería 18650');
    xlabel('Tiempo (CDMX)');
    ylabel('Porcentaje [%]');
    ylim([0 110]);
    grid on;
end