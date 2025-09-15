% Configuración de ThingSpeak
channelID = 2805725; % Reemplaza con tu Channel ID
fieldVoltaje = 1;    % Campo donde se registra el voltaje de la batería
fieldCiclos = 4;     % Campo para contar ciclos (asegúrate de crearlo en ThingSpeak)
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % API Key para lectura
writeAPIKey = 'SC8TJFLKBEKIPCX7'; % API Key para escritura

% Leer los datos desde ThingSpeak (máximo permitido: 8000 puntos)
[data, timestamps] = thingSpeakRead(channelID, ...
    'Fields', fieldVoltaje, ...
    'NumPoints', 8000, ...
    'ReadKey', readAPIKey);

% Verificar si hay datos disponibles
if isempty(data)
    error('No se encontraron datos en el campo especificado.');
end

% Definir umbrales de voltaje para detectar ciclos
voltajeBajo = 3.2;  
voltajeAlto = 3.88;  

% Detectar ciclos (transiciones de 3V a 4.2V y de regreso)
ciclos = 0;
subida = false; % Indica si el voltaje ha subido por encima de 4.2V

for i = 2:length(data)
    if data(i) >= voltajeAlto
        subida = true; % Detecta que el voltaje subió por encima de 4.2V
    elseif subida && data(i) <= voltajeBajo
        ciclos = ciclos + 1; % Contar un ciclo cuando baja nuevamente a 3V
        subida = false; % Reiniciar el estado
    end
end

% Mostrar el número de ciclos detectados
disp(['Número de ciclos detectados: ', num2str(ciclos)]);

% Leer el último valor registrado en ThingSpeak para evitar actualizaciones innecesarias
ultimoCiclo = thingSpeakRead(channelID, 'Fields', fieldCiclos, 'NumPoints', 1, 'ReadKey', readAPIKey);

% Solo actualizar si el valor cambió y respetar el límite de 15 segundos
if isempty(ultimoCiclo) || ciclos ~= ultimoCiclo
    pause(20); % Espera obligatoria para cumplir con el límite de ThingSpeak
    thingSpeakWrite(channelID, ciclos, 'Fields', fieldCiclos, 'WriteKey', writeAPIKey);
    disp('Contador de ciclos actualizado en ThingSpeak.');
else
    disp('No es necesario actualizar el contador.');
end
