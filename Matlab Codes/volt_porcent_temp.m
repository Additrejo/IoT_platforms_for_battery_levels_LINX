% Configuración de ThingSpeak
channelID = 2805725; % Reemplaza con tu canal
readAPIKey = 'SC8TJFLKBEKIPCX7'; % Reemplaza con tu API Key de lectura
filename = 'datos_baterias.xlsx'; % Archivo Excel

% Leer datos desde ThingSpeak
data = thingSpeakRead(channelID, 'Fields', [1,2,3,4], 'NumPoints', 100, 'ReadKey', readAPIKey);

% Extraer variables
voltaje = data(:,1);
porcentaje = data(:,2);
temperatura = data(:,3);
ciclos = data(:,4);

% Guardar en Excel
T = table(voltaje, porcentaje, temperatura, ciclos);
if isfile(filename)
    writetable(T, filename, 'WriteMode', 'append');
else
    writetable(T, filename);
end

% Graficar datos
figure;
subplot(3,1,1);
plot(voltaje, 'b'); title('Voltaje');
subplot(3,1,2);
plot(porcentaje, 'r'); title('Porcentaje Batería');
subplot(3,1,3);
plot(temperatura, 'g'); title('Temperatura');