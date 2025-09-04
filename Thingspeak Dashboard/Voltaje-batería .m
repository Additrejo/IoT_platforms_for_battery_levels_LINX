% Dashboard en vivo con timer (MATLAB Online / ThingSpeak)
% Hora ajustada manualmente a GMT-6 (Ciudad de México)
% Autor: Addi Trejo
% Ult Actu: 03-Sep-2025

channelID = 2805725;
fieldID = 1;
readAPIKey = 'PWMRQ0HEW7NBTBBP';

timeWindowHours = 24; % Ventana de tiempo (últimas 24h)
updateInterval = 60;  % Intervalo de actualización en segundos

figure;
hPlot = plot(NaN, NaN, '-o');
title(sprintf('Voltaje Batería', timeWindowHours));
xlabel('Tiempo (CDMX GMT-6)');
ylabel('Voltaje [V]');
grid on;
hold on;

% ---- Función de actualización ----
function updatePlot(~, ~, channelID, fieldID, readAPIKey, timeWindowHours, hPlot)
    [data, time] = thingSpeakRead(channelID, ...
        'Fields', fieldID, ...
        'ReadKey', readAPIKey, ...
        'DateRange', [datetime('now','TimeZone','UTC')-hours(timeWindowHours), ...
                      datetime('now','TimeZone','UTC')]);

    if ~isempty(time)
        % Ajuste manual a GMT-6 (CDMX)
        time = time - hours(2);

        % Convertir datetime -> número para graficar
        timeNum = datenum(time);

        % Actualizar datos en la gráfica
        set(hPlot, 'XData', timeNum, 'YData', data);

        % Ajustar formato del eje X
        durationHours = hours(max(time) - min(time));
        if durationHours < 24
            datetick('x', 'dd-mmm HH:MM', 'keepticks');
        elseif durationHours < 7*24
            datetick('x', 'dd-mmm HH:MM', 'keepticks');
        else
            datetick('x', 'dd-mmm', 'keepticks');
        end

        drawnow;
    else
        disp('⚠️ No se obtuvieron datos de ThingSpeak.');
    end
end

% ---- Configurar timer ----
t = timer('ExecutionMode','fixedRate','Period',updateInterval, ...
    'TimerFcn', @(~,~)updatePlot([],[],channelID,fieldID,readAPIKey,timeWindowHours,hPlot));

start(t);
