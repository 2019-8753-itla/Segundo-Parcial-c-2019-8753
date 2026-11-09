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

### Comandos UART implementados

| Comando | Funcion | Respuesta |
|---|---|---|
| STATUS | Consultar el estado del sistema | t=...;ADC=...;PCT=...;THR=...;MODE=...;ALARM=... |
| THR n | Configurar umbral entre 10 y 95 | OK THR=n |
| START | Activar modo RUN | OK RUN |
| STOP | Activar modo PAUSE | OK PAUSE |
| Comando invalido | Rechazar comando | ERR |

Caracteristicas:

* Comunicacion USART2 a 115200 baudios, 8N1.
* Recepcion UART mediante interrupciones.
* Transmision UART mediante interrupciones.
* Colas FIFO para recepcion y transmision.
* Comandos terminados en LF o CRLF.
* Longitud maxima de comando: 32 caracteres.
* Telemetria automatica cada 1000 ms.
* Telemetria activa durante RUN y PAUSE.
* Comandos sensibles a mayusculas y minusculas.

## Estado actual


* Proyecto integrado en la placa NUCLEO-F302R8.
* TIM6 configurado como base temporal de 1 ms.
* ADC1 funcionando con adquisicion cada 100 ms.
* LED de actividad PB13 funcionando cada 500 ms.
* Boton USER PC13 con antirrebote de 30 ms.
* Modos RUN y PAUSE implementados.
* Alarma fisica PB12 funcionando con umbral configurable e histeresis de 5 puntos porcentuales.
* USART2 funcionando a 115200 baudios, 8N1.
* Comandos STATUS, THR n, START y STOP implementados.
* Telemetria automatica cada 1000 ms.
* Colas FIFO UART RX y TX implementadas.
* Compilacion verificada con 0 errores y 0 warnings.
* Pruebas funcionales de ADC, GPIO, alarma y UART realizadas satisfactoriamente.
* Prueba de estres UART de 30 segundos ejecutada: 142 comandos enviados y 172 lineas completas recibidas.
* P10 pendiente de cierre formal: investigar un ERR aislado y verificar latencia de respuesta menor o igual a 100 ms.
* Firmware respaldado en GitHub hasta el commit 15.
* Pendiente: documentacion final, evidencias y verificaciones instrumentales.

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



## Día 2 — Implementación de TIMER, ADC y GPIO



**Fecha:** 03/11/2026



### Funcionalidades implementadas



**1. Base temporal TIM6**



- Configuración del TIMER con frecuencia de entrada de 8 MHz.

- Prescaler: 79.

- Auto-reload (ARR): 99.

- Período calculado: 1 ms.

- Contador global `g_msTicks`, actualizado mediante interrupciones.



**2. Adquisición ADC1**



- Canal: ADC1_IN1 (PA0).

- Resolución: 12 bits.

- Rango digital: 0–4095.

- Adquisición programada cada 100 ms.

- Conversión mediante interrupciones.

- Calibración al iniciar el programa.

- Conversión a porcentaje entero mediante redondeo.



Fórmula:



`PCT = (100 × ADC + 2047) / 4095`



**3. LED de actividad**



- Pin: PB13.

- Cambio de estado cada 500 ms.

- Implementación no bloqueante mediante el contador TIM6.

- Funcionamiento simultáneo con ADC1.



### Pruebas realizadas



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



### Verificaciones pendientes



- Medición instrumental de los períodos de 1 ms, 100 ms y 500 ms.

- Comprobación de precisión ADC con voltajes medidos.

- Pruebas formales P01–P10 del examen.

- Implementación de alarma, botón y comunicación UART.


## Día 3 — Botón USER, modos RUN/PAUSE y alarma

**Fecha:** 04/11/2026

### 1. Botón USER con antirrebote

- Pin utilizado: PC13.
- Lógica activa en bajo: suelto = 1, presionado = 0.
- Antirrebote por software de 30 ms.
- Implementación no bloqueante utilizando `g_msTicks`.
- Una sola acción por pulsación válida.
- Contador de pruebas: `g_buttonPressCount`.

### 2. Modos de funcionamiento

El sistema dispone de dos modos:

- **RUN (0):** funcionamiento normal, con evaluación de la alarma.
- **PAUSE (1):** alarma desactivada, manteniendo la adquisición ADC y el LED de actividad.

El sistema inicia en RUN. Cada pulsación válida del botón USER alterna entre RUN y PAUSE.

### 3. Alarma con histéresis

- Salida: PB12.
- Umbral inicial: THR = 70 %.
- Histéresis: H = 5 puntos porcentuales.
- Encendido: PCT >= THR.
- Apagado: PCT < THR - H.
- Entre ambos límites se conserva el estado anterior.
- En PAUSE la alarma permanece apagada.
- Al regresar a RUN se evalúa el último porcentaje ADC válido utilizando THR, sin recuperar automáticamente el estado anterior de la alarma.

### 4. Pruebas funcionales realizadas

| Prueba | Resultado |
|---|---|
| Compilación | 0 errores, 0 warnings |
| Alternancia RUN/PAUSE | Correcta |
| ADC durante PAUSE | Continúa funcionando |
| LED durante PAUSE | Continúa parpadeando |
| Alarma en RUN con 50 % | Apagada |
| Alarma en RUN con 75 % | Encendida |
| Descenso a 68 % desde alarma encendida | Permanece encendida |
| Descenso a 64 % | Apagada |
| Aumento a 68 % desde alarma apagada | Permanece apagada |
| Entrada a PAUSE con 80 % | Alarma apagada |
| Regreso a RUN con 80 % | Alarma encendida |
| Regreso de PAUSE a RUN con 68 % | Alarma apagada |

### 5. Verificaciones pendientes

- Comprobar los límites exactos de 65 %, 69 % y 70 % mediante pruebas controladas.
- Verificar físicamente el nivel eléctrico de PB12.
- Realizar las pruebas instrumentales de temporización.
- Integrar los comandos UART y comprobar la modificación del umbral mediante `THR n`.

### 6. Control de versiones

- Commit 08: `2687731` — Antirrebote del botón.
- Commit 09: `30a8450` — Modos RUN/PAUSE.
- Commit 10: `d87321c` — Alarma con histéresis.
- Commit 11: Documentación del Día 3.

**Versión objetivo:** `v0.3.0`.



## Dia 4 - Integracion UART, telemetria y pruebas

**Fecha:** 08/11/2026

### 1. Comunicacion UART

Se implemento USART2 a 115200 baudios, 8 bits de datos,
sin paridad y 1 bit de parada (8N1).

La recepcion se realiza mediante interrupciones y una
cola FIFO de eventos. Los comandos se interpretan fuera
de las interrupciones, dentro del ciclo principal.

La transmision utiliza interrupciones y una cola FIFO
de mensajes completos, para evitar que las respuestas
y la telemetria mezclen sus caracteres.

### 2. Comandos implementados

- STATUS: consulta inmediata del estado del sistema.
- THR n: configura el umbral entre 10 y 95.
- START: activa el modo RUN.
- STOP: activa el modo PAUSE.
- Comandos invalidos: respuesta ERR.

Los comandos distinguen mayusculas y minusculas y
terminan con LF o CRLF.

La longitud maxima permitida es de 32 caracteres.
Una linea demasiado larga se descarta hasta LF y
produce una respuesta ERR.

### 3. Formato de telemetria

Ejemplo:

`t=1200;ADC=2866;PCT=70;THR=70;MODE=RUN;ALARM=1`

Campos:

- t: tiempo transcurrido en milisegundos.
- ADC: lectura analogica de 0 a 4095.
- PCT: porcentaje entero de 0 a 100.
- THR: umbral configurado.
- MODE: RUN o PAUSE.
- ALARM: 0 (apagada) o 1 (encendida).

La telemetria se programa cada 1000 ms y continua
durante los modos RUN y PAUSE.

### 4. Pruebas funcionales realizadas

| Verificacion | Resultado |
|---|---|
| Compilacion | 0 errores y 0 warnings |
| Comando STATUS | Correcto |
| Comando START | Correcto |
| Comando STOP | Correcto |
| Comando THR n | Correcto |
| Rechazo de umbrales invalidos | Correcto |
| Rechazo de linea de 33 caracteres | Correcto en prueba manual |
| Telemetria automatica | Funcionando |
| Telemetria durante PAUSE | Funcionando |
| LED de alarma PB12 | Verificado fisicamente |
| Histeresis de 5 puntos | Verificada con potenciometro |
| Recuperacion tras prueba de carga | Correcta |

### 5. Prueba de estres UART

Se ejecuto una prueba automatizada de aproximadamente
30 segundos utilizando PowerShell y el puerto COM5.

Resultados registrados:

- Comandos STATUS enviados: 142.
- Lineas completas recibidas: 172.
- Lineas con formato de telemetria valido: 171.
- Respuestas ERR inesperadas: 1.

Posteriormente se enviaron cinco comandos STATUS
manualmente mediante PuTTY, todos con respuesta correcta.

La prueba demuestra continuidad de la comunicacion,
pero no certifica todavia ausencia total de respuestas
perdidas ni latencia maxima de 100 ms.

### 6. Verificaciones pendientes

- Investigar el ERR aislado durante la prueba de carga.
- Medir latencia de respuestas UART (limite: 100 ms).
- Completar P10 alternando RUN/PAUSE y variando el ADC.
- Verificar instrumentalmente las tolerancias temporales.
- Completar evidencias y revision final de P01 a P10.
- Utilizar potenciometro de 10 kohm conforme al requisito.

### 7. Control de versiones

- Commit 14: 68ba147 - Comandos UART, telemetria y FIFO.
- Commit 15: a9c2bd9 - Proteccion de cola TX ante interrupciones.

Repositorio:
https://github.com/2019-8753-itla/Segundo-Parcial-c-2019-8753