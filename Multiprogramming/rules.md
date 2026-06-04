# 1. Granularidad de Archivos (Estructura Atómica)
Regla 1:1: Un archivo, una función. El nombre del archivo debe coincidir exactamente con el nombre de la función (ej. video_clear_screen.c contiene void video_clear_screen()).
Prototipos Centralizados: 
Cada carpeta de módulo debe tener un único archivo de cabecera (.h) que recoja los prototipos de todos los archivos de esa carpeta.Cero Lógica en Main: El archivo main.c solo puede contener llamadas a funciones externas. Está prohibido definir bucles for, while o condicionales complejos dentro de main.
# 2. Restricciones de Extensión (Límites de Líneas)
Máximo en C: Ninguna función puede superar las 25 líneas de código real.
Máximo en ASM: Ninguna función puede superar las 40 líneas de instrucciones.
Punto de Quiebre: Si una función requiere más líneas, debe dividirse forzosamente en una "función de orquestación" y múltiples "funciones de utilidad" en archivos separados.
# 3. Nomenclatura y Trazabilidad (Regla del Prefijo)
Nombres Basados en Contexto: Las variables globales y funciones deben empezar con el prefijo del módulo/archivo para encontrarlas con un grep.
Mal: init();
Bien: gdt_init();, vga_write_char();, mem_alloc();
Variables de Archivo: No se permiten variables globales fuera de los archivos de configuración. Si una función necesita estado, debe recibirlos por argumento o usar static dentro de su archivo único.
# 4. Principio de No Repetición (DRY)
Cero Duplicación: Antes de escribir una lógica de manipulación de bits o registros, el LLM debe verificar si ya existe en la carpetas existentes.
Abstracción de Hardware: No se permite escribir instrucciones asm directamente en archivos .c de lógica de negocio.
Deben envolverse en una función dentro de la carpeta y llamarse desde ahí.
# 5. Documentación y Firmas (Cabeceras Obligatorias)
Cada archivo debe empezar con un comentario de una sola línea que describa:
Qué hace la función.
Qué registros o memoria afecta.
Dependencias de otras funciones.
# 6. Reglas Específicas para el LLM
Contexto Incremental: "Antes de generar código nuevo, revisa la lista de archivos existentes para asegurar que no estás duplicando funcionalidad".
Salida Modular: "Si te pido una funcionalidad compleja, entrégame el código dividido en múltiples bloques de código, indicando el nombre de archivo para cada uno".
Verifica antes de modificar: Antes de cambiar algo del código fuente siempre detalla donde se va a hacer el cambio, como y por qué. Espera confirmación del programador.