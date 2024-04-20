# Usar una imagen base de Ubuntu
FROM ubuntu:latest

# Actualizar los repositorios e instalar las herramientas necesarias para compilar
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Copiar el código fuente del servidor al directorio de trabajo en el contenedor
COPY serverTry.c /usr/src/server/

# Establecer el directorio de trabajo
WORKDIR /usr/src/server/

# Compilar el programa del servidor
RUN gcc -o server_program serverTry.c

# Exponer el puerto 8000 para que sea accesible desde fuera del contenedor
EXPOSE 8000

# Comando por defecto para ejecutar el servidor cuando se inicie el contenedor
CMD ["./server_program"]

