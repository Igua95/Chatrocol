## Chatrocol

Protocolo de chat implementado en C, usando TCP, BSD Socket, y threads.

#### How to run

Compilar server
` gcc server.c -o server `

Correr server
` ./server `

Compilar cliente
` gcc client.c -o client `

Correr cliente
` ./client localhost`

#### Listado de codigos

  |Request client   | Code protocol        |     Response server                            |
  | --------------- | -------------------- | -----------------------------------------------|
  |/login username  |Cod: 100 (login)      |     101 Registrado 102 no hay espacio(anonimo) |
  |/online 		      |Cod: 110 (online)     |     111 Usuarios conectados, 121 nada conectado|
  |(enter) 		      |Cod: 120 (refresh)    |     121 Lista de mensajes                      |
  |/quit   		      |Cod: 130 quit			   |     131 Usuario removido                       |
  |(write and enter)|Cod: 140 new message  |     141 Lista de mensajes                      |

#### Demo
![Alt Text](http://g.recordit.co/L5vcySugu1.gif)

