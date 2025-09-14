% Título: Gráfrica de Temperatura de la Batería con Picos y Valles
% Descripción: Lee datos de temperatura, los grafica y etiqueta los valores
% máximos y mínimos de cada ciclo, evitando la superposición de etiquetas.


% --- Configuración del Canal y Campo ---
channelID = 2805725;
readAPIKey = 'PWMRQ0HEW7NBTBBP';


% El campo donde envías la temperatura.
fieldID_Temperatura = 3; 


% --- Lectura de Datos de ThingSpeak ---
[temperaturaData, time] = thingSpeakRead(channelID, ...
                                         'ReadKey', readAPIKey, ...
                                         'Fields', fieldID_Temperatura, ...
                                         'NumPoints', 8000);


% --- Creación de la Gráfica ---
if isempty(time) || numel(time) < 2
    text(0.5, 0.5, 'No se encontraron datos de temperatura para graficar.', 'HorizontalAlignment', 'center');
else
    plot(time, temperaturaData, 'LineWidth', 1.5, 'Color', '#FF5733'); 
    hold on;


    % --- ENCONTRAR Y ETIQUETAR PICOS Y VALLES (CON AJUSTE) ---
    
    % Se define una distancia mínima entre picos para evitar superposiciones
    min_distancia_picos = 1000; % Ajusta este valor si es necesario
    
    % 1. Encontrar los picos máximos
    % --- MODIFICADO: Se añade 'MinPeakDistance' ---
    [pks_max, locs_max] = findpeaks(temperaturaData, ...
                                    'MinPeakProminence', 1.5, ...
                                    'MinPeakDistance', min_distancia_picos);
    
    % 2. Encontrar los picos mínimos
    % --- MODIFICADO: Se añade 'MinPeakDistance' ---
    [pks_min_inv, locs_min] = findpeaks(-temperaturaData, ...
                                        'MinPeakProminence', 1.5, ...
                                        'MinPeakDistance', min_distancia_picos);
    pks_min = -pks_min_inv;


    % 3. Dibuja el texto para cada pico máximo encontrado
    for i = 1:numel(pks_max)
        text_label = sprintf('%.1f°C', pks_max(i));
        text(time(locs_max(i)), pks_max(i), text_label, ...
             'VerticalAlignment', 'bottom', 'HorizontalAlignment', 'center', ...
             'Color', 'red', 'FontSize', 8, 'FontWeight', 'bold');
    end


    % 4. Dibuja el texto para cada pico mínimo encontrado
    for i = 1:numel(pks_min)
        text_label = sprintf('%.1f°C', pks_min(i));
        text(time(locs_min(i)), pks_min(i), text_label, ...
             'VerticalAlignment', 'top', 'HorizontalAlignment', 'center', ...
             'Color', 'blue', 'FontSize', 8, 'FontWeight', 'bold');
    end
    

    % --- Formato y Estilo Final de la Gráfica ---
    grid on;
    title('Temperatura de la Batería (Máximos y Mínimos)');
    xlabel('Fecha y Hora');
    ylabel('Temperatura (°C)');
    
    hold off;
end