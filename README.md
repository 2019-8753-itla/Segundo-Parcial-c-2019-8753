# Segundo Parcial - Microcontroladores

## Datos del estudiante

* Nombre: Jose Amauris Palma Rosario
* Matricula: 2019-8753

## Objetivo

Desarrollar un monitor de senal analogica con alarma
configurable utilizando ADC, TIMER, GPIO y UART
en un unico firmware.

## Hardware

* Placa: NUCLEO-F302R8
* Microcontrolador: STM32F302R8Tx
* Potenciometro: 10 kohm
* Dos LED externos con resistencias limitadoras
* Pulsador USER B1 integrado
* Computadora con terminal serial

## Herramientas

* STM32CubeIDE 1.0.1
* Git Bash
* GitHub
* PuTTY

## Mapa de pines

|Funcion|Pin|Configuracion|
|-|-|-|
|Potenciometro|PA0|ADC1\_IN1 Single-ended|
|LED de actividad|PB13|GPIO Output|
|LED de alarma|PB12|GPIO Output|
|Pulsador USER B1|PC13|GPIO Input|
|UART TX|PA2|USART2\_TX|
|UART RX|PA3|USART2\_RX|

## Conexiones

* Potenciometro: extremos a 3.3 V y GND.
* Cursor del potenciometro: PA0.
* LED actividad: PB13 con resistencia limitadora.
* LED alarma: PB12 con resistencia limitadora.
* Pulsador: USER B1 integrado en la placa.
* UART: USART2 mediante conexion compatible con ST-LINK.

## ADC

* Periferico: ADC1
* Canal: IN1
* Resolucion: 12 bits
* Rango digital: 0 a 4095
* VREF nominal prevista: 3.3 V
* Rango de entrada previsto: 0 a VREF
* Sampling Time: 61.5 ciclos
* Conversion: individual, iniciada por software

La referencia real se verificara en el montaje.

Formula:
PCT = redondear (100 \* ADC / 4095)
redondear = llevamos los decimales al entero mas cercano

## TIMER

* Periferico: TIM6
* Reloj actual: 8 MHz
* Prescaler: 79
* Periodo ARR: 99
* Frecuencia calculada: 1000 Hz
* Tick calculado: 1 ms

Tareas previstas:

* ADC cada 100 ms
* LED actividad cada 500 ms
* Telemetria cada 1000 ms
* Antirrebote de 30 ms

## UART

* Periferico: USART2
* Baud rate: 115200
* Datos: 8 bits
* Paridad: ninguna
* Stop bits: 1
* Recepcion: interrupciones habilitadas

Comandos por implementar:

* STATUS
* THR n
* START
* STOP

## Estado actual

* Proyecto base compilado correctamente.
* Perifericos configurados en STM32CubeIDE.
* Firmware funcional pendiente de implementar.
* Pruebas P01 a P10 pendientes.

## Compilacion

1. Abrir STM32CubeIDE 1.0.1.
2. Importar el proyecto STM32 existente.
3. Comprobar la configuracion del archivo .ioc.
4. Seleccionar Project > Build Project.
5. Verificar que compile sin errores.

## Repositorio

https://github.com/2019-8753-itla/Segundo-Parcial-c-2019-8753

## Evidencias

Los resultados de P01 a P10 se documentaran
despues de ejecutar cada prueba.



\## Día 2 — Implementación de TIMER, ADC y GPIO



\*\*Fecha:\*\* 03/11/2026



\### Funcionalidades implementadas



\*\*1. Base temporal TIM6\*\*



\- Configuración del TIMER con frecuencia de entrada de 8 MHz.

\- Prescaler: 79.

\- Auto-reload (ARR): 99.

\- Período calculado: 1 ms.

\- Contador global `g\_msTicks`, actualizado mediante interrupciones.



\*\*2. Adquisición ADC1\*\*



\- Canal: ADC1\_IN1 (PA0).

\- Resolución: 12 bits.

\- Rango digital: 0–4095.

\- Adquisición programada cada 100 ms.

\- Conversión mediante interrupciones.

\- Calibración al iniciar el programa.

\- Conversión a porcentaje entero mediante redondeo.



Fórmula:



`PCT = (100 × ADC + 2047) / 4095`



\*\*3. LED de actividad\*\*



\- Pin: PB13.

\- Cambio de estado cada 500 ms.

\- Implementación no bloqueante mediante el contador TIM6.

\- Funcionamiento simultáneo con ADC1.



\### Pruebas realizadas



| Prueba | Resultado |

|---|---|

| Compilación del firmware | 0 errores |

| Interrupciones TIM6 | Contador incrementa |

| Intervalo ADC observado | 100 ms |

| Conversiones ADC observadas | 52 |

| LED PB13 | Parpadeo comprobado |

| ADC al mínimo | 0 (0 %) |

| ADC a mitad de recorrido | 2141 (52 %) |

| ADC al máximo | 4037 (99 %) |



Las pruebas analógicas se realizaron con un potenciómetro de 100 kΩ utilizado provisionalmente. El requisito original indica un potenciómetro de 10 kΩ.



\### Verificaciones pendientes



\- Medición instrumental de los períodos de 1 ms, 100 ms y 500 ms.

\- Comprobación de precisión ADC con voltajes medidos.

\- Pruebas formales P01–P10 del examen.

\- Implementación de alarma, botón y comunicación UART.

