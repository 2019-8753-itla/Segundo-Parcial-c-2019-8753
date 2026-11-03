# Calculo de TIM6

## Configuracion de reloj
Segun Clock Configuration:

- SYSCLK: 8 MHz
- HCLK: 8 MHz
- APB1 Prescaler: 1
- PCLK1: 8 MHz
- TIM6 Clock: 8 MHz

## Objetivo
Generar un tick de hardware cada 1 ms.

Frecuencia requerida:

f_tick = 1 / 0.001 = 1000 Hz

## Formula

f_tick = f_TIM6 / ((PSC + 1) * (ARR + 1))

## Valores seleccionados

PSC = 79
ARR = 99

f_tick = 8000000 / (80 * 100)

f_tick = 1000 Hz

T_tick = 1 / 1000 = 0.001 s = 1 ms

## Planificacion de tareas

| Tarea | Intervalo | Ticks |
|---|---|---|
| Adquisicion ADC | 100 ms | 100 |
| LED actividad | 500 ms | 500 |
| Telemetria UART | 1000 ms | 1000 |
| Antirrebote | 30 ms | 30 |

## Implementacion prevista
TIM6 generara interrupciones periodicas.
Las interrupciones seran breves.

El procesamiento de comandos UART, las decisiones
de alarma y las tareas principales se ejecutaran
fuera de las interrupciones.

## Verificacion pendiente
Los intervalos deben medirse experimentalmente
y cumplir una tolerancia de +/- 5 %.
