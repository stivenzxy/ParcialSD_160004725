# 📃 Parcial Corte #2 - Sistemas Distribuidos  
## Simulador de Minería Distribuida (Leader / Worker) utilizando C

Este proyecto implementa un sistema de **minería distribuida** en C, donde un proceso **leader (servidor)** reparte tareas de búsqueda de hashes MD5 a varios procesos **worker (clientes)** conectados por TCP/IP utilizando sockets y procesos para dividir la utilización de recursos, cada worker explora un rango distinto de combinaciones hasta encontrar un hash que cumpla la condición especificada (empieza, termina o contiene un patrón específico).

---

## 🧩 1. Requisitos previos

Antes de compilar, asegúrese de tener instalado el compilador GCC y las librerías necesarias:

### Instalar GCC

```bash
sudo apt update
sudo apt install gcc
```

### Instalar Librerías adicionales

```bash
sudo apt update
sudo apt install build-essential libssl-dev
```

#### *⚠️ La librería libssl-dev es necesaria para usar las funciones de hash MD5 (OpenSSL).*


## 📂 2. Estructura del proyecto

```
.
├── client
│   ├── include
│   │   ├── mining_utils.h
│   │   ├── socket_utils.h
│   │   ├── string_utils.h
│   │   └── worker.h
│   └── src
│       ├── main.c
│       ├── mining_utils.c
│       ├── socket_utils.c
│       ├── string_utils.c
│       └── worker.c
└── server
    ├── include
    │   ├── mining_utils.h
    │   ├── socket_utils.h
    │   └── worker_utils.h
    └── src
        ├── main.c
        ├── mining_utils.c
        ├── socket_utils.c
        └── worker_utils.c
```

## ⚙️ 3. Compilación

### 🔱 Compilar el Leader (Servidor)

1. Ubicarse en el directorio server/src:

```bash
cd server/src
```
2. Ejecutar el siguiente comando para compilar el código utilizando gcc:

```bash
gcc main.c socket_utils.c worker_utils.c mining_utils.c -o leader -lm
```
#### *Esto genera el ejecutable leader.*
#### *⚠️ Es importante añadir el flag -lm al final, ya que indica al compilador que debe enlazar la biblioteca matemática estándar (libm) - se utiliza para realizar algunas operaciones dentro de código.*


### ⚒️ Compilar el Worker (Cliente)

1. Ubicarse en el directorio client/src:

```bash
cd client/src
```
2. Ejecutar el siguiente comando para compilar el código utilizando gcc:

```bash
gcc -o worker main.c worker.c socket_utils.c string_utils.c mining_utils.c -lssl -lcrypto
```
#### *Esto genera el ejecutable worker.*
#### *⚠️ Es importante añadir los flags -lssl y -lcrypto al final, ya que indica al compilador que dependencias debe utilizar para el uso de hash MD5 en este caso.*

## 🚀 4. Ejecución

### Paso 1 - Iniciar el Leader (servidor)

Ejecute en la terminal especificando los parámetros indicados:

```bash
./leader [PUERTO] [CANTIDAD_DE_WORKERS]
```
El servidor solicitará la configuración de minería:

#### Ejemplo propuesto:

```bash
Ingrese el texto base a minar: hola como estas, este es un buen dia para aprender blockchain
Ingrese la longitud del relleno [1-6]: 3
Tipo de condición:
 1. Empieza con
 2. Termina con
 3. Contiene
Seleccione: 2
Ingrese el patrón de condición (ejemplo '00'): 0f
```

Luego mostrará la configuración establecida:

```bash
--- Configuración ---
Texto base: hola como estas, este es un buen dia para aprender blockchain
Relleno: 3 caracteres (0-9, a-z)
Condición: endswith:0f
----------------------
Socket creado en puerto 9090
Leader activo en puerto 9090. Esperando hasta 2 workers...
```
### ⚙️ Paso 2 — Iniciar los Workers (clientes)

Abra una nueva terminal por cada worker y ejecute:

```bash
./worker [IP_DEL_LEADER] [PUERTO_DEL_LEADER]
```

#### Ejemplo en red local:

```bash
./worker 10.0.2.15 9090
```
#### *⚠️ Reemplace 10.0.2.15 por la dirección IPv4 local del servidor(puede obtenerla con ip a o hostname -I).*

El leader mostrará mensajes como:

```bash
[OK] Worker aceptado desde 10.0.2.15 [pid=9912] (1/2)
[OK] Worker aceptado desde 10.0.2.15 [pid=9974] (2/2)
[OK] Iniciando distribución de tareas ...
Rango de busqueda enviado al worker 1: 0000 - hzzz
Rango de busqueda enviado al worker 2: i000 - zzzz
```

## 🧩 5. Ejemplo de salida completa

```bash
["Worker #2 | PID=9974 | 10.0.2.15"]: FOUND:hola a todosij:8c14228b0a32bb87f225a0f5c9c47a0f
Hash encontrado por worker #2 (PID=9974, IP=10.0.2.15)
Enviando orden de detención [STOP] a worker: 10.0.2.15 [pid=9912]
[X] Worker #1 desconectado (PID=9912, IP=10.0.2.15)
Todos los workers han finalizado. Cerrando leader...
```
## 🔍 6. Comprobar si los procesos se ejecutan correctamente y con los pids indicados en el programa

Ver procesos activos (para verificar los workers corriendo):

```bash
ps -ef | grep worker
```
## 💡 Consideraciones importantes

- Los workers dividen el rango de búsqueda alfabéticamente (0000-hzzz, i000-zzzz, etc.) para evitar solapamientos / se utilizan las posibilidades de combinación de 0-9 + a-z.

## 👨‍💻 Autor

### Jesús Pérez
#### Código estudiantil: 160004725
#### Universidad de los Llanos

## 📚 Referencias importantes

- Códigos de ejemplo suministrados por el docente
- Documentación en internet sobre sockets en C
- Materiales de apoyo consultados para entender y aplicar conceptos de implementación de sockets.
