Aplicação Cliente-Servidor para alocação de memória em servidor. (local)

5 terminais(2 servidores e 3 clientes).

Para executar:

compilar servidor1: gcc servidorlocal1.c -o sv1 -lpthread

executar servidor1: ./sv1

compilar servidor2: gcc servidorlocal2.c -o sv2 -lpthread

executar servidor2: ./sv2

compilar cliente: gcc clientelocal.c -o cliente

executar cliente: ./cliente
