# SEMARD

Implementación de un sistema de monitoreo periódico y en tiempo real para cuerpos de agua superficial continental, con sensores remotos, base de datos y procedimientos de diseño propio para el Departamento de Bolívar

## INTRODUCCION

La importancia de desarrollar un dispositivo electrónico de alto desempeño radica en el aseguramiento y establecimiento de buenas pautas en el proceso de pruebas, por lo tanto, es significativa la implementación de métodos y procedimiento de pruebas de ensayo y error.
En relación con lo anterior, se deben tener en cuenta una serie de pasos:

- Planeación de la prueba 
- Implementación de la prueba
- Análisis de resultados
- Corrección de errores

Este repositorio tiene como objetivo documentar la informacion necesaria respecto a los dispositivos utilizados en la construccion del equipo de medicion de caidad de agua. Ademas que contiene diferentes codigos de pruebas para asegurar el funcionamiento del dispositivo electrónico de medición de calidad del agua.

## OBJETIVO GENERAL

Realizar pruebas de integración, compatibilidad, rendimiento entre el equipo Core y elementos que componen el dispositivo para medición de calidad del agua.

## OBJETIVOS ESPECIFICOS
 
-Realizar pruebas de integración del equipo Core con sensores.
-Realizar pruebas de desempeño de los diferentes elementos que componen el dispositivo. 
-Realizar programación del equipo Core que permita maximizar el rendimiento del dispositivo.

## MATERIALES
-TTGO LORA32
-SEN0189
-SEN0161
-SEN0237
-SEN0244
-DFR0300
-DS18B20
-ADS1115

##FUNCIONAMIENTO

El código implementado realiza una tarea específica que consiste en leer las señales de los sensores utilizando el ADS1115 cada 7,5 segundos. Cada minuto, se calcula un valor promedio de las lecturas tomadas y se realiza la conversión a valores digitales utilizando fórmulas predefinidas. Estos valores se corresponden con unidades específicas para cada sensor.

Este proceso de lectura y conversión se repite durante 15 minutos, lo que permite obtener una serie de datos representativos. Después de esta etapa, el sistema entra en el modo de sueño profundo durante 45 minutos. Esta característica de bajo consumo de energía es extremadamente útil durante períodos de inactividad, ya que ayuda a lograr un ahorro significativo de energía.
