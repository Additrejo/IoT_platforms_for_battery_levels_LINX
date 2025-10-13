%% Configuración del canal ThingSpeak
channelID = 2805725; % Reemplaza con tu Channel ID
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % Reemplaza con tu API Key de lectura


% Número de puntos a leer
numPoints = 8000;


%% Leer datos de Corriente de Carga (Campo 4)
[corrienteCargaData, corrienteCargaTimestamps] = thingSpeakRead(channelID, ...
    'Fields', 4, ...
    'NumPoints', numPoints, ...
    'ReadKey', readAPIKey);


%% Leer datos de Corriente de Descarga (Campo 5)
[corrienteDescargaData, corrienteDescargaTimestamps] = thingSpeakRead(channelID, ...
    'Fields', 5, ...
    'NumPoints', numPoints, ...
    'ReadKey', readAPIKey);


%% Convertir corrientes a mA y limpiar datos
corrienteCargaData_mA = corrienteCargaData * 1000;
corrienteCargaData_mA(isnan(corrienteCargaData_mA)) = 0;

corrienteDescargaData_mA = corrienteDescargaData * 1000;
corrienteDescargaData_mA(isnan(corrienteDescargaData_mA)) = 0;


%% Verificar si hay datos
if isempty(corrienteCargaData) || isempty(corrienteDescargaData)
    error('No se encontraron datos suficientes en ThingSpeak.');
end


%% Crear figura
figure('Color','w');
hold on;


% Relleno verde para corriente de carga
area(corrienteCargaTimestamps, corrienteCargaData_mA, 'FaceColor', [0.3 0.8 0.3], 'FaceAlpha',0.3,'EdgeColor','g','LineWidth',1.5);


% Relleno rojo para corriente de descarga
area(corrienteDescargaTimestamps, corrienteDescargaData_mA, 'FaceColor', [0.9 0.3 0.3], 'FaceAlpha',0.3,'EdgeColor','r','LineWidth',1.5);


% --- NUEVO: CÁLCULO Y GRÁFICA DE LA DURACIÓN PARA CICLOS DE CARGA ---

threshold = 50; % Umbral en mA para detectar actividad de carga
start_indices = find(corrienteCargaData_mA(2:end) > threshold & corrienteCargaData_mA(1:end-1) <= threshold) + 1;

if corrienteCargaData_mA(1) > threshold
    start_indices = [1; start_indices];
end

for k = 1:numel(start_indices)
    start_idx = start_indices(k);
    
    if k < numel(start_indices)
        end_idx = start_indices(k+1) - 1;
    else
        end_idx = numel(corrienteCargaData_mA);
    end
    
    % Solo procesa si el ciclo tiene una duración real
    if start_idx < end_idx
        startTime = corrienteCargaTimestamps(start_idx);
        endTime = corrienteCargaTimestamps(end_idx);
        
        % 1. Calcula la duración
        duration = endTime - startTime;
        
        % 2. Formatea la duración a "Xh Ym"
        totalHours = floor(hours(duration));
        remainingMinutes = round(minutes(duration - hours(totalHours)));
        duration_text = sprintf('%dh %dm', totalHours, remainingMinutes);
        
        % 3. Dibuja el indicador en la parte inferior (usando un color oscuro para contraste)
        indicator_y_pos = 100; 
        plot([startTime, endTime], [indicator_y_pos, indicator_y_pos], '-k', 'LineWidth', 1); % Línea horizontal
        plot([startTime, startTime], [0, indicator_y_pos], '-k', 'LineWidth', 1); % Tick vertical inicial
        plot([endTime, endTime], [0, indicator_y_pos], '-k', 'LineWidth', 1); % Tick vertical final
        
        % 4. Escribe el texto de la duración
        text(startTime + hours(1), indicator_y_pos + 50, duration_text, 'Color', 'k', 'FontSize', 8, 'FontWeight', 'bold');
    end
end

% --- FIN DE LA SECCIÓN NUEVA ---


% Configuración del eje Y sin decimales
ylabel('Corriente (mA)');
yticks_vals = get(gca, 'YTick');
yticks_vals = round(yticks_vals);
set(gca, 'YTick', yticks_vals);


% Configuración de eje X y título
xlabel('Tiempo');
title('Corrientes de Carga y Descarga (mA)');


% Leyenda y cuadrícula
legend('Corriente de Carga','Corriente de Descarga','Location','best');
grid on;

hold off;