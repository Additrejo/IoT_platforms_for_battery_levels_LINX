% Código para obtener y graficar datos del voltaje de una batería 18650 en ThingSpeak
% mostrando el tiempo en el eje X con formato dinámico
% Autor: Addi Trejo, 08 Abril 2025
% Ult Actu: 25 Agosto 2025

channelID = 2805725; % Reemplaza con tu Channel ID
fieldID = 1; % Campo a visualizar
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % Reemplaza con tu API Key (si el canal es privado)

% Leer datos junto con la marca de tiempo
[data, time] = thingSpeakRead(channelID, ...
    'Fields', fieldID, ...
    'NumPoints', 800, ... % Ajusta el número de puntos
    'ReadKey', readAPIKey);

% Crear un gráfico con tiempo en el eje X
figure;
plot(time, data, '-o');
title('Voltaje de la batería 18650');
xlabel('Tiempo');
ylabel('Voltaje [V]');
grid on;

% ---- Ajuste automático del formato del eje X ----
if isempty(time)
    disp('⚠️ No se obtuvieron datos de ThingSpeak.');
else
    durationHours = hours(max(time) - min(time));

    if durationHours < 24
        datetick('x', 'HH:MM', 'keepticks'); % Si es menos de 1 día -> horas:minutos
    elseif durationHours < 7*24
        datetick('x', 'dd-mmm HH:MM', 'keepticks'); % Si es menos de 1 semana -> día y hora
    else
        datetick('x', 'dd-mmm', 'keepticks'); % Si es más de 1 semana -> solo día
    end
end
