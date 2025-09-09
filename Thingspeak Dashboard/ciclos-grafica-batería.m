% Título: Contador de Ciclos a lo largo del Tiempo (Field 8)
% Descripción: Grafica el número de ciclos de la batería en función del tiempo,
% mostrando cuándo se completa cada ciclo.


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
if isempty(time)
    % Mensaje si no se encuentran datos.
    text(0.5, 0.5, 'No se encontraron datos en el Field 8.', 'HorizontalAlignment', 'center');
else
    % Limpieza de datos por si hay valores no numéricos
    if iscell(ciclosData)
        ciclosData = cellfun(@str2double, ciclosData);
    end


    % Se utiliza 'stairs' con el color 'k' (negro)
    stairs(time, ciclosData, 'k', 'LineWidth', 2);
    
    % --- Formato y Estilo de la Gráfica ---
    grid on;
    title('Evolución de Ciclos en el Tiempo (Field 8)');
    xlabel('Fecha y Hora');
    ylabel('Número de Ciclo');
    
    % Asegura que el eje Y comience en 0.
    if ~all(isnan(ciclosData))
        upperLimit = max(ciclosData) + 1;
        ylim([0 upperLimit]);
    end
end