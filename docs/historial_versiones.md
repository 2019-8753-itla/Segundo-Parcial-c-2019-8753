# Historial de versiones

## Dia 1 - Configuracion inicial

### Commit 01

Hash corto: bc564a1

Tipo: chore

Descripcion:
Creacion del proyecto STM32 y primera compilacion
sin errores.

### Commit 02

Hash corto: 9ca20cd

Tipo: chore

Descripcion:
Configuracion inicial de GPIO, ADC1, TIM6 y USART2.

Configuracion compilada sin errores.

### Commit 03

Tipo: docs

Descripcion prevista:
Documentacion inicial del proyecto, calculos
del TIMER y registro de versiones.

## Versiones previstas

|Version|Objetivo|
|-|-|
|v0.1.0|Configuracion inicial|
|v0.2.0|TIMER y ADC|
|v0.3.0|Boton y alarma|
|v0.4.0|UART e integracion|
|v0.5.0|Pruebas y correcciones|
|v1.0.0|Entrega final|

Las etiquetas se crearan cuando cada hito
este realmente verificado.



DIA 2 - 03/11/2026



Commit 04:

feat(timer): implementar base temporal de TIM6



Commit 05:

feat(adc): implementar adquisicion periodica y porcentaje



Commit 06:

feat(gpio): implementar LED de actividad periodico



Commit 07:

docs: registrar implementacion TIMER ADC y GPIO



Version objetivo: v0.2.0



Estado:

Base temporal, adquisicion ADC y LED de actividad implementados.

Pruebas instrumentales pendientes.



DIA 3 - 04/11/2026



Commit 08: 2687731

feat(button): implementar antirrebote no bloqueante



Commit 09: 30a8450

feat(mode): implementar estados RUN y PAUSE



Commit 10: d87321c

feat(alarm): implementar alarma con histeresis



Commit 11:

docs: registrar boton modos y alarma



Version objetivo: v0.3.0



Pruebas:

\- Compilacion: 0 errores, 0 warnings.

\- Boton USER PC13 activo en bajo.

\- Alternancia RUN/PAUSE correcta.

\- ADC y LED siguen funcionando durante PAUSE.

\- Alarma con THR=70 y H=5 verificada funcionalmente.

\- Regreso PAUSE a RUN con PCT=68: alarma apagada.



Pendiente:

\- Verificacion de limites exactos de histeresis.

\- Medicion fisica de PB12.

\- Pruebas instrumentales.

\- Integracion de comandos UART.

