#!/bin/bash
# Prueba automatica: levanta el servidor y le envia 5 clientes
# (2 binarios, 2 de texto, 1 con tipo invalido). Uso: bash prueba.sh
cd "$(dirname "$0")" || exit 1
make -s || exit 1

./bin/SMTI 5 &
SRV=$!
sleep 1

./bin/cliente_sensor 101 TEMP 92.5
./bin/cliente_sensor 102 PRESION 12.3 texto
./bin/cliente_sensor 103 CAUDAL 3.2
./bin/cliente_sensor 104 XYZ 1.0          # tipo invalido -> NACK
./bin/cliente_sensor 105 TEMP 40 texto

wait $SRV
echo "Servidor termino con codigo $?"
