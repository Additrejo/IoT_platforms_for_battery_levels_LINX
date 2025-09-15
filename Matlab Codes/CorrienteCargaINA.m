% Código para graficar la corriente de carga que da el BMS TP4050.
% Basado en el proyecto de monitoreo de batería 18650
% Autor: Addi Trejo, 2025
% Ult.Act: 08/Abril/2025

% Configuración de ThingSpeak
channelID = 2805725;       % ID de tu canal (el mismo que usaste para voltaje)
fieldID = 5;               % Campo 5 = Corriente de carga (INA219 en 0x41)
readAPIKey = 'PWMRQ0HEW7NBTBBP'; % API Key (privada)

% Obtener datos
data = thingSpeakRead(channelID, 'Fields', fieldID, 'NumPoints', 500, 'ReadKey', readAPIKey);

% Crear figura
figure;
set(gcf, 'Color', 'w'); % Fondo blanco para la figura

% Graficar datos en rojo
plot(data, '-r', 'LineWidth', 1.5); % '-r' = línea sólida roja
title('Corriente de Carga de Batería 18650 (INA219 @ 0x41)', 'FontSize', 14);
xlabel('Número de Muestra', 'FontSize', 12);
ylabel('Corriente (A)', 'FontSize', 12);
grid on;

% Línea horizontal para el valor promedio (en negro)
hold on;
yline(mean(data), '--k', ['Promedio: ', num2str(mean(data), '%.3f'), ' A'], ...
    'LabelHorizontalAlignment', 'right', 'FontSize', 10);
hold off;

% Añadir leyenda
legend('Corriente de carga', 'Location', 'best', 'FontSize', 10);

% Ajustar estilo de ejes
set(gca, 'FontSize', 12, 'Box', 'on');

% Mostrar estadísticas en consola
disp('=== Estadísticas de Corriente de Carga ===');
disp(['Máximo: ', num2str(max(data)), ' A']);
disp(['Mínimo: ', num2str(min(data)), ' A']);
disp(['Promedio: ', num2str(mean(data)), ' A']);
