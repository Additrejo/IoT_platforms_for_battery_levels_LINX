% Porcentaje con áreas por rango (Field 2) – ThingSpeak MATLAB Visualization

channelID   = 2805725;
fieldID     = 2;                         % <-- porcentaje en Field 2
readAPIKey  = 'PWMRQ0HEW7NBTBBP';
numPoints   = 800;                       % ajusta si quieres más/menos historia

% Leer datos (dos salidas devuelve valores y tiempos)
[pct, time] = thingSpeakRead(channelID, ...
    'Fields', fieldID, ...
    'NumPoints', numPoints, ...
    'ReadKey', readAPIKey);

% Filtrar NaN por si hay huecos
valid = ~isnan(pct);
pct   = pct(valid);
time  = time(valid);

% (Opcional) Etiquetar como hora CDMX sin convertir
% time.TimeZone = 'America/Mexico_City';

% Asegurar límites 0–100
pct(pct < 0)   = 0;
pct(pct > 100) = 100;

% --- Construir capas apiladas (área) ---
% low:  0–20   | mid: 21–79 (60 pts) | high: 80–100
low  = min(pct, 20);               % altura de la capa roja
mid  = max(min(pct, 80) - 20, 0);  % altura de la capa verde (encima de low)
high = max(pct - 80, 0);           % altura de la capa amarilla (encima de mid)

Y = [low(:), mid(:), high(:)];     % n x 3 para área apilada

% --- Graficar ---
h = area(time, Y);                  % área apilada
set(h(1), 'FaceColor', [1   0.7 0.7], 'EdgeColor','none'); % rojo tenue   (0–20%)
set(h(2), 'FaceColor', [0.7 1   0.7], 'EdgeColor','none'); % verde tenue  (21–79%)
set(h(3), 'FaceColor', [1   1   0.6], 'EdgeColor','none'); % amarillo tenue (80–100%)

hold on;
plot(time, sum(Y,2), 'k', 'LineWidth', 1.2);               % línea negra del % real

title('Porcentaje de batería 18650');
xlabel('Tiempo (CDMX)');
ylabel('Porcentaje [%]');
ylim([0 110]);
grid on;
hold off;