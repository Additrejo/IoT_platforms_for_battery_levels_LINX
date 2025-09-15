% Configura los detalles del canal
channelID = 12345;                                        % Identificador único del canal en ThingSpeak donde se almacenan los datos.
readAPIKey = 'YOUR_READ_API_KEY';                            % Clave de lectura del canal (solo necesaria si el canal es privado).

% Leer el historial completo de datos del canal
[data, timestamps] = thingSpeakRead(channelID, ...          % Función para leer datos de ThingSpeak.
    'Fields', 1, ...                                        % Especifica el campo que se desea leer (en este caso, el campo 1).
    'DateRange', [datetime(2023,1,1), datetime('now')], ... % Define el rango de fechas desde el 1 de enero de 2023 hasta el momento actual.
    'ReadKey', readAPIKey);                                 % Incluye la clave de lectura necesaria para canales privados.

% Mostrar los datos en la consola
disp(table(timestamps, data)); % Convierte los datos y marcas de tiempo en una tabla y la muestra en la consola.

                                                       % Graficar los datos históricos
figure;                                                % Crea una nueva ventana para la figura.
plot(timestamps, data, '-o');                          % Grafica los datos frente a sus marcas de tiempo con líneas y marcadores circulares.
title('Historial Completo de Datos de ThingSpeak');    % Título del gráfico.
xlabel('Tiempo');                                      % Etiqueta del eje X.
ylabel('Valor');                                       % Etiqueta del eje Y.
grid on;                                               % Activa la cuadrícula en el gráfico para mejorar la visualización.