<<<<<<< HEAD
% Dashboard en vivo con timer, etiquetas de ciclo y picos de voltaje
=======
% Dashboard en vivo con timer, etiquetas de ciclo, picos de voltaje y duración de ciclo
% Acota el inicio y fin de cada ciclo con líneas verticales.
>>>>>>> upstream/main
% Hora corregida a GMT-6 (Ciudad de México)
% Autor: Addi Trejo
% Ult Actu: 14-Sep-2025

<<<<<<< HEAD

=======
>>>>>>> upstream/main
channelID = 2805725;
readAPIKey = 'PWMRQ0HEW7NBTBBP';

% Se definen los IDs de ambos campos
fieldID_Voltaje = 1;
fieldID_Ciclos = 8;

<<<<<<< HEAD

timeWindowHours = 72; % Ventana de tiempo (últimos 3 días)
updateInterval = 60;  % Intervalo de actualización en segundos


=======
timeWindowHours = 72; % Ventana de tiempo (últimos 3 días)
updateInterval = 60;  % Intervalo de actualización en segundos

>>>>>>> upstream/main
figure;
hPlot = plot(NaN, NaN, '-o');
title(sprintf('Voltaje Batería (Últimas %d horas)', timeWindowHours));
xlabel('Tiempo (CDMX GMT-6)');
ylabel('Voltaje [V]');
grid on;
hold on;

<<<<<<< HEAD

=======
>>>>>>> upstream/main
% ---- Función de actualización ----
function updatePlot(~, ~, channelID, readAPIKey, fieldIDs, timeWindowHours, hPlot)
    [channelData, time] = thingSpeakRead(channelID, ...
        'Fields', fieldIDs, ...
        'ReadKey', readAPIKey, ...
        'DateRange', [datetime('now','TimeZone','UTC')-hours(timeWindowHours), ...
                      datetime('now','TimeZone','UTC')]);

<<<<<<< HEAD

=======
>>>>>>> upstream/main
    if ~isempty(time) && size(channelData,2) == 2
        % Ajuste correcto a GMT-6 (CDMX)
        time = time - hours(6);

<<<<<<< HEAD

=======
>>>>>>> upstream/main
        % Separar los datos
        voltajeData = channelData(:, 1);
        ciclosData = channelData(:, 2);
        ciclosData = fillmissing(ciclosData, 'previous');

<<<<<<< HEAD

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


=======
        timeNum = datenum(time);

        % Actualizar datos en la gráfica de voltaje
        set(hPlot, 'XData', timeNum, 'YData', voltajeData);

        % --- Borra las etiquetas y áreas viejas para la nueva actualización ---
        delete(findobj(gca, 'Tag', 'cycle_label'));
        delete(findobj(gca, 'Tag', 'peak_label')); 
        delete(findobj(gca, 'Tag', 'duration_label'));
        delete(findobj(gca, 'Tag', 'cycle_shade')); % Limpia cualquier sombreado residual

        % --- LÓGICA PARA DIBUJAR ETIQUETAS DE CICLO ---
        
        % Encuentra los índices donde empiezan los ciclos
        start_indices = find(diff(ciclosData) > 0) + 1;
        if ~isempty(ciclosData)
            start_indices = [1; start_indices];
        end
        start_indices = unique(start_indices, 'stable');

>>>>>>> upstream/main
        % Itera a través de cada ciclo para dibujar sus etiquetas
        for k = 1:numel(start_indices)
            start_idx = start_indices(k);
            
<<<<<<< HEAD
            % Dibuja la etiqueta de inicio de ciclo
=======
            % Dibuja la línea y etiqueta de inicio de ciclo
>>>>>>> upstream/main
            new_cycle_num = ciclosData(start_idx);
            label_time = timeNum(start_idx);
            line([label_time label_time], ylim, 'Color', 'r', 'LineStyle', '--', 'Tag', 'cycle_label');
            text(label_time, max(ylim)-0.05, sprintf(' Ciclo %d', new_cycle_num), 'Color', 'r', ...
                 'FontWeight', 'bold', 'Tag', 'cycle_label');
            
<<<<<<< HEAD
            % --- NUEVO: LÓGICA PARA ETIQUETAR PICOS DE VOLTAJE ---
            
=======
>>>>>>> upstream/main
            % Determina el fin del ciclo actual en la ventana de datos
            if k < numel(start_indices)
                end_idx = start_indices(k+1) - 1;
            else
                end_idx = numel(voltajeData);
            end
<<<<<<< HEAD
            
            % Encuentra el pico de voltaje solo en el segmento del ciclo actual
            if start_idx <= end_idx
                [peak_value, rel_peak_idx] = max(voltajeData(start_idx:end_idx));
                abs_peak_idx = start_idx + rel_peak_idx - 1; % Índice absoluto
                
                % Dibuja la etiqueta del pico
=======

            % --- LÓGICA PARA DURACIÓN, PICO Y LÍNEA FINAL DE CICLO ---
            if start_idx < end_idx
                % Dibuja la línea final para acotar el ciclo
                end_label_time = timeNum(end_idx);
                line([end_label_time end_label_time], ylim, 'Color', 'r', 'LineStyle', '--', 'Tag', 'cycle_label');

                % Calcula la duración
                cycleDuration = time(end_idx) - time(start_idx);
                cycleDuration.Format = 'hh:mm:ss'; % Formato HH:MM:SS
                durationStr = sprintf('Duración: %s', string(cycleDuration));

                % Calcula el punto medio del ciclo para posicionar la etiqueta
                midTime = timeNum(start_idx) + (timeNum(end_idx) - timeNum(start_idx)) / 2;

                % Dibuja la etiqueta de duración en el centro del ciclo
                text(midTime, min(ylim) + 0.1, durationStr, ...
                    'Color', [0.4 0.2 0.6], 'FontWeight', 'bold', 'Tag', 'duration_label', ...
                    'HorizontalAlignment', 'center', 'FontSize', 9);

                % Encuentra y etiqueta el pico de voltaje
                [peak_value, rel_peak_idx] = max(voltajeData(start_idx:end_idx));
                abs_peak_idx = start_idx + rel_peak_idx - 1; 
                
>>>>>>> upstream/main
                peak_label_str = sprintf('%.2f V', peak_value);
                text(timeNum(abs_peak_idx), peak_value, peak_label_str, ...
                    'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'center', ...
                    'Color', [0 0.5 0], 'FontWeight', 'bold', 'Tag', 'peak_label');
            end
        end

<<<<<<< HEAD

=======
>>>>>>> upstream/main
        % Ajustar formato del eje X
        datetick('x', 'dd-mmm HH:MM', 'keepticks');
        xtickangle(45);

<<<<<<< HEAD

=======
>>>>>>> upstream/main
        drawnow;
    else
        disp('⚠️ No se obtuvieron datos (o datos incompletos) de ThingSpeak.');
    end
end

<<<<<<< HEAD

=======
>>>>>>> upstream/main
% ---- Configurar timer ----
t = timer('ExecutionMode','fixedRate','Period',updateInterval, ...
    'TimerFcn', @(~,~)updatePlot([],[],channelID,readAPIKey,[fieldID_Voltaje, fieldID_Ciclos],timeWindowHours,hPlot));

<<<<<<< HEAD

start(t);
=======
start(t);


>>>>>>> upstream/main
