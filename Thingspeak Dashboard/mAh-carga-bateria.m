% Título: Gráfica de Corriente de Carga (Field 4 en mA)
% Descripción: Lee y grafica los datos del Field 4 de un canal de ThingSpeak,
% con el área rellena en un degradado de verdes.


% --- Configuración del Canal y Campo ---
channelID = 2805725;
readAPIKey = 'PWMRQ0HEW7NBTBBP'; 
fieldID = 4; 


% --- Lectura de Datos de ThingSpeak ---
[data_A, time] = thingSpeakRead(channelID, ...
                                'ReadKey', readAPIKey, ...
                                'Fields', fieldID, ...
                                'NumPoints', 8000);


% --- Creación de la Gráfica ---
if isempty(time)
    text(0.5, 0.5, 'No se encontraron datos en el Field 4.', 'HorizontalAlignment', 'center');
else
    data_mA = data_A * 1000;


    % --- NUEVO: Creación de la gráfica con degradado ---
    % 1. Crea un área base con el color verde claro para el fondo.
    area(time, data_mA, 0, 'FaceColor', [0.85 0.95 0.85], 'EdgeColor', 'none'); % Verde muy claro


    hold on; % Mantiene la gráfica actual activa para añadir más elementos


    % 2. Dibuja la línea principal de los datos con el verde brillante.
    plot(time, data_mA, 'Color', [0 1 0], 'LineWidth', 2); % Verde brillante puro [R G B]


    hold off; % Libera la gráfica


    % --- Formato y Estilo de la Gráfica ---
    grid on;
    title('Corriente de Carga (mAh)');
    xlabel('Fecha y Hora');
    ylabel('Corriente (mAh)');
    
    % Se ajusta el eje Y para que siempre vaya de 0 a 2200 mA.
    ylim([0 2200]);

    % --- NUEVO: Se define la escala del eje Y en intervalos de 200 ---
    yticks(0:200:2200);

end