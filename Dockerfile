# Usar una imagen base de Linux
FROM ubuntu:latest

# Instalar las herramientas y dependencias necesarias
RUN apt-get update && \
    apt-get install -y gcc \
                       make \
                       libc6-dev \
                       && \
    rm -rf /var/lib/apt/lists/*

# Copiar tu código fuente al contenedor
COPY serverTry.c /app/server.c

# Compilar tu aplicación dentro del contenedor
RUN gcc -o /app/server /app/server.c -pthread

# Exponer el puerto en el que el servidor escucha
EXPOSE 8000

# Comando para ejecutar tu aplicación al iniciar el contenedor
CMD ["/app/server"]
