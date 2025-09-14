% Dashboard en vivo con timer, etiquetas de ciclo y picos de voltaje
% Hora corregida a GMT-6 (Ciudad de México)
% Autor: Addi Trejo
% Ult Actu: 14-Sep-2025


channelID = 2805725;
readAPIKey = 'PWMRQ0HEW7NBTBBP';

% Se definen los IDs de ambos campos
fieldID_Voltaje = 1;
fieldID_Ciclos = 8;


timeWindowHours = 72; % Ventana de tiempo (últimos 3 días)
updateInterval = 60;  % Intervalo de actualización en segundos


figure;
hPlot = plot(NaN, NaN, '-o');
title(sprintf('Voltaje Batería (Últimas %d horas)', timeWindowHours));
xlabel('Tiempo (CDMX GMT-6)');
ylabel('Voltaje [V]');
grid on;
hold on;


% ---- Función de actualización ----
function updatePlot(~, ~, channelID, readAPIKey, fieldIDs, timeWindowHours, hPlot)
    [channelData, time] = thingSpeakRead(channelID, ...
        'Fields', fieldIDs, ...
        'ReadKey', readAPIKey, ...
        'DateRange', [datetime('now','TimeZone','UTC')-hours(timeWindowHours), ...
                      datetime('now','TimeZone','UTC')]);


    if ~isempty(time) && size(channelData,2) == 2
        % Ajuste correcto a GMT-6 (CDMX)
        time = time - hours(6);


        % Separar los datos
        voltajeData = channelData(:, 1);
        ciclosData = channelData(:, 2);
        ciclosData = fillmissing(ciclosData, 'previous');


        timeNum = datenum(time);


        % Actualizar datos en la gráfica de voltaje
        set(hPlot, 'XData', timeNum, 'YData', voltajeData);


        % --- Borra las etiquetas viejas para la nueva actualización ---
        delete(findobj(gca, 'Tag', 'cycle_label'));
        delete(findobj(gca, 'Tag', 'peak_label')); % Borra etiquetas de picos anteriores


        % --- LÓGICA PARA DIBUJAR ETIQUETAS DE CICLO Y PICOS ---
        
        % Encuentra los índices donde empiezan los ciclos
        start_indices = find(diff(ciclosData) > 0) + 1;
        % Añade el primer punto para poder analizar el primer ciclo en la ventana
        if ~isempty(ciclosData)
            start_indices = [1; start_indices];
        end
        % Elimina duplicados si el primer punto ya era un cambio
        start_indices = unique(start_indices, 'stable');


        % Itera a través de cada ciclo para dibujar sus etiquetas
        for k = 1:numel(start_indices)
            start_idx = start_indices(k);
            
            % Dibuja la etiqueta de inicio de ciclo
            new_cycle_num = ciclosData(start_idx);
            label_time = timeNum(start_idx);
            line([label_time label_time], ylim, 'Color', 'r', 'LineStyle', '--', 'Tag', 'cycle_label');
            text(label_time, max(ylim)-0.05, sprintf(' Ciclo %d', new_cycle_num), 'Color', 'r', ...
                 'FontWeight', 'bold', 'Tag', 'cycle_label');
            
            % --- NUEVO: LÓGICA PARA ETIQUETAR PICOS DE VOLTAJE ---
            
            % Determina el fin del ciclo actual en la ventana de datos
            if k < numel(start_indices)
                end_idx = start_indices(k+1) - 1;
            else
                end_idx = numel(voltajeData);
            end
            
            % Encuentra el pico de voltaje solo en el segmento del ciclo actual
            if start_idx <= end_idx
                [peak_value, rel_peak_idx] = max(voltajeData(start_idx:end_idx));
                abs_peak_idx = start_idx + rel_peak_idx - 1; % Índice absoluto
                
                % Dibuja la etiqueta del pico
                peak_label_str = sprintf('%.2f V', peak_value);
                text(timeNum(abs_peak_idx), peak_value, peak_label_str, ...
                    'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'center', ...
                    'Color', [0 0.5 0], 'FontWeight', 'bold', 'Tag', 'peak_label');
            end
        end


        % Ajustar formato del eje X
        datetick('x', 'dd-mmm HH:MM', 'keepticks');
        xtickangle(45);


        drawnow;
    else
        disp('⚠️ No se obtuvieron datos (o datos incompletos) de ThingSpeak.');
    end
end


% ---- Configurar timer ----
t = timer('ExecutionMode','fixedRate','Period',updateInterval, ...
    'TimerFcn', @(~,~)updatePlot([],[],channelID,readAPIKey,[fieldID_Voltaje, fieldID_Ciclos],timeWindowHours,hPlot));


start(t);