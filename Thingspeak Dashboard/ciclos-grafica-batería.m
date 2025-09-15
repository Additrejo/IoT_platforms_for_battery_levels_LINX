% Título: Contador de Ciclos a lo largo del Tiempo (Field 8) con Duración
% Descripción: Grafica el número de ciclos y añade una etiqueta con la
% duración de cada ciclo en horas y minutos.


% --- Configuración del Canal y Campo ---
channelID = 2805725;
readAPIKey = 'PWMRQ0HEW7NBTBBP'; 


% El campo que contiene el contador de ciclos.
fieldID = 8; 


% --- Lectura de Datos de ThingSpeak ---
% Se leen los datos de los últimos 3650 días (10 años) para obtener todo el historial.
[ciclosData, time] = thingSpeakRead(channelID, ...
                                    'ReadKey', readAPIKey, ...
                                    'Fields', fieldID, ...
                                    'NumDays', 9000);


% --- Creación de la Gráfica ---
if isempty(time) || numel(time) < 2
    % Mensaje si no se encuentran datos.
    text(0.5, 0.5, 'No se encontraron datos en el Field 8.', 'HorizontalAlignment', 'center');
else
    % Limpieza de datos por si hay valores no numéricos
    if iscell(ciclosData)
        ciclosData = cellfun(@str2double, ciclosData);
    end
    ciclosData(isnan(ciclosData)) = 0; % Asegura que no haya NaNs


    % Se utiliza 'stairs' con el color 'k' (negro)
    stairs(time, ciclosData, 'k', 'LineWidth', 2);
    hold on; % Permite añadir las etiquetas de tiempo


    % --- NUEVO: CÁLCULO Y VISUALIZACIÓN DE LA DURACIÓN DE CADA CICLO ---

    % 1. Encontrar los puntos donde el ciclo aumenta
    change_indices = find(diff(ciclosData) > 0) + 1;
    event_indices = [1; change_indices]; % Incluye el primer punto como inicio

    % 2. Itera a través de cada ciclo para calcular y mostrar su duración
    for i = 1:numel(event_indices)-1
        % Obtiene el tiempo de inicio y fin del ciclo actual
        startTime = time(event_indices(i));
        endTime = time(event_indices(i+1));
        
        % Calcula la duración
        duration = endTime - startTime;
        
        % Formatea la duración a un texto "Xh Ym"
        totalHours = floor(hours(duration));
        remainingMinutes = round(minutes(duration - hours(totalHours)));
        duration_text = sprintf('%dh %dm', totalHours, remainingMinutes);
        
        % Calcula la posición para la etiqueta de texto
        % X: A la mitad del escalón del ciclo
        text_x_pos = startTime + duration/2;
        % Y: Un poco por encima del escalón
        text_y_pos = ciclosData(event_indices(i)) + 0.2; 
        
        % Dibuja el texto en la gráfica
        text(text_x_pos, text_y_pos, duration_text, ...
             'HorizontalAlignment', 'center', 'Color', 'blue', ...
             'FontSize', 8, 'FontWeight', 'bold');
    end


    % --- Formato y Estilo de la Gráfica ---
    grid on;
    title('Evolución y Duración de Ciclos en el Tiempo (Field 8)');
    xlabel('Fecha y Hora');
    ylabel('Número de Ciclo');
    
    % Asegura que el eje Y comience en 0.
    if ~all(isnan(ciclosData))
        upperLimit = max(ciclosData) + 1;
        ylim([0 upperLimit]);
    end
    
    hold off;
end