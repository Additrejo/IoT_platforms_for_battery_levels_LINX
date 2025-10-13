% Título: Gráfica de Corriente de Descarga (Field 5 en mA) con Picos por Ciclo
% Descripción: Lee y grafica los datos del Field 5 y añade una línea
% punteada mostrando el pico máximo de corriente para cada ciclo.
% Modificado para incluir el tiempo entre ciclos.

% --- Configuración del Canal y Campo ---
channelID = 2805725;
readAPIKey = 'PWMRQ0HEW7NBTBBP'; 

% El campo que quieres graficar. Corresponde a la corriente de descarga.
fieldID = 5; 

% --- Lectura de Datos de ThingSpeak ---
% Leemos los últimos 8000 puntos para tener un buen rango de datos.
[data_A, time] = thingSpeakRead(channelID, ...
                                'ReadKey', readAPIKey, ...
                                'Fields', fieldID, ...
                                'NumPoints', 8000);

% --- Creación de la Gráfica ---
if isempty(time) || numel(time) < 2
    text(0.5, 0.5, 'No se encontraron datos en el Field 5.', 'HorizontalAlignment', 'center');
else
    % Conversión de Amperios (A) a miliamperios (mA)
    data_mA = data_A * 1000;
    data_mA(isnan(data_mA)) = 0; % Limpieza de datos

    % --- Graficar el Resultado Principal ---
    area(time, data_mA, 0, 'FaceColor', [0.95 0.85 0.85], 'EdgeColor', 'none');
    hold on; % Activa el modo para añadir más elementos
    plot(time, data_mA, 'Color', [1 0 0], 'LineWidth', 2);

    % --- CÁLCULO Y GRÁFICA DE LÍNEAS DE PICO MÁXIMO Y TIEMPOS ---
    
    threshold = 50; % Umbral para detectar actividad de descarga
    
    % Encuentra los índices donde empiezan los ciclos de descarga
    start_indices = find(data_mA(2:end) > threshold & data_mA(1:end-1) <= threshold) + 1;
    
    if data_mA(1) > threshold
        start_indices = [1; start_indices];
    end

    % Itera a través de cada ciclo detectado para dibujar su línea y texto
    for k = 1:numel(start_indices)
        start_idx = start_indices(k);
        
        % Determina el fin del ciclo actual
        if k < numel(start_indices)
            end_idx = start_indices(k+1) - 1;
        else
            end_idx = numel(data_mA); % Es el último ciclo
        end
        
        % --- NUEVO: CÁLCULO DEL TIEMPO ENTRE CICLOS ---
        if k > 1
            % El fin del ciclo anterior es el índice justo antes del inicio de este.
            prev_cycle_end_idx = start_idx - 1;
            prev_cycle_end_time = time(prev_cycle_end_idx);
            
            % Calcula la duración entre ciclos
            time_between_cycles = time(start_idx) - prev_cycle_end_time;
            time_between_cycles.Format = 'hh:mm:ss';
            
            % Calcula la posición para la etiqueta (en medio del tiempo inactivo)
            text_x_position = prev_cycle_end_time + (time(start_idx) - prev_cycle_end_time) / 2;
            
            % Dibuja la etiqueta
            text(text_x_position, 150, string(time_between_cycles), 'Color', [0.3 0.3 0.3], ...
                 'HorizontalAlignment', 'center', 'FontSize', 8, 'FontWeight', 'bold');
        end
        
        % Obtiene los datos solo para el ciclo actual
        current_cycle_data = data_mA(start_idx:end_idx);
        current_cycle_time = time(start_idx:end_idx);
        
        % Encuentra el pico de corriente en el ciclo
        peak_current = max(current_cycle_data);
        
        % Dibuja la línea punteada azul si el pico es significativo
        if peak_current > threshold
            plot(current_cycle_time, ones(size(current_cycle_time)) * peak_current, '--b', 'LineWidth', 1.5);
            
            % Escribe el valor numérico sobre la línea
            text_label = sprintf('%.0f mA', peak_current);
            text_x_position = current_cycle_time(floor(end/2));
            text_y_position = peak_current + 100;
            
            text(text_x_position, text_y_position, text_label, 'Color', 'b', 'HorizontalAlignment', 'center', 'FontSize', 8, 'FontWeight', 'bold');
        end
    end

    % --- Formato y Estilo Final ---
    grid on;
    title('Corriente de Descarga (Máximos por Ciclo)');
    xlabel('Fecha y Hora');
    ylabel('Corriente (mA)');
    
    ylim([0 2200]);
    yticks(0:200:2200);

    hold off; % Finaliza el modo de añadir elementos
end
