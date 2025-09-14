% Título: Gráfica de Corriente de Carga con Picos y Ciclos
% Descripción: Grafica la corriente de carga (Field 4) y añade una etiqueta
% con el número de ciclo y el pico máximo de corriente para cada ciclo diario.


% --- Configuración del Canal y Campo ---
channelID = 2805725;
readAPIKey = 'PWMRQ0HEW7NBTBBP'; 

% --- MODIFICADO: Se especifican ambos campos a leer ---
fieldID_Corriente = 4;
fieldID_Ciclos = 8;


% --- Lectura de Datos de ThingSpeak ---
% Se leen ambos campos (Corriente y Ciclos) en una sola llamada
[channelData, time] = thingSpeakRead(channelID, ...
                                     'ReadKey', readAPIKey, ...
                                     'Fields', [fieldID_Corriente, fieldID_Ciclos], ...
                                     'NumPoints', 8000);


% --- Creación de la Gráfica ---
if isempty(time) || numel(time) < 2
    text(0.5, 0.5, 'No se encontraron datos para graficar.', 'HorizontalAlignment', 'center');
else
    % --- MODIFICADO: Se separan los datos en variables distintas ---
    data_mA = channelData(:, 1) * 1000; % Columna 1 es Corriente
    ciclosData = channelData(:, 2);     % Columna 2 es Ciclos
    
    data_mA(isnan(data_mA)) = 0; % Limpieza de datos
    % Rellena valores faltantes en el contador de ciclos para asegurar consistencia
    ciclosData = fillmissing(ciclosData, 'previous');


    % --- Graficar el Resultado Principal ---
    area(time, data_mA, 0, 'FaceColor', [0.85 0.95 0.85], 'EdgeColor', 'none');
    hold on;
    plot(time, data_mA, 'Color', [0 1 0], 'LineWidth', 2);


    % --- CÁLCULO Y GRÁFICA DE LÍNEAS DE VALOR MÁXIMO ---
    
    threshold = 50; % Umbral para detectar actividad de carga
    start_indices = find(data_mA(2:end) > threshold & data_mA(1:end-1) <= threshold) + 1;
    
    if data_mA(1) > threshold
        start_indices = [1; start_indices];
    end


    % Itera a través de cada ciclo detectado para dibujar su línea y texto
    for k = 1:numel(start_indices)
        start_idx = start_indices(k);
        
        if k < numel(start_indices)
            end_idx = start_indices(k+1) - 1;
        else
            end_idx = numel(data_mA);
        end
        
        current_cycle_data = data_mA(start_idx:end_idx);
        current_cycle_time = time(start_idx:end_idx);
        
        peak_current = max(current_cycle_data);
        
        if peak_current > threshold
            plot(current_cycle_time, ones(size(current_cycle_time)) * peak_current, '--r', 'LineWidth', 1.5);
            
            % --- MODIFICADO: Se añade el número de ciclo a la etiqueta ---
            cycle_num = ciclosData(start_idx); % Obtiene el ciclo al inicio del pico
            % Se crea una etiqueta de dos líneas
            text_label = sprintf('Ciclo %d\n%.0f mA', cycle_num, peak_current);
            
            text_x_position = current_cycle_time(floor(end/2));
            text_y_position = peak_current + 100;
            
            text(text_x_position, text_y_position, text_label, 'Color', 'r', 'HorizontalAlignment', 'center', 'FontSize', 8, 'FontWeight', 'bold');
        end
    end


    % --- Formato y Estilo Final ---
    grid on;
    title('Corriente de Carga (Máximos por Ciclo)');
    xlabel('Fecha y Hora');
    ylabel('Corriente (mA)'); 
    
    ylim([0 2200]);
    yticks(0:200:2200);
    
    hold off;
end