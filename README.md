# TP1
TP1 taller de programacion

* Alumno: Alasino Franco
* Padron: 102165
* URL: https://github.com/alasinofranco3/TP1

## Descripcion de la resolucion del trabajo:
Para el protocolo de comunicacion uso los TDAS de socket,cliente(common_client) y servidor(common_server).
Claramente el cliente tendra dentro un socket mientras que el servidor cuenta con dos en este caso,uno sera el socket aceptador y el otro el socket que se utiliza para conectarse con el cliente.

El TDA socket implementa todas las funciones basicas del socket(connect,bind_and_listen,accept entre otras).Este TDA solo nos servira para conexiones de protocolo TCP y usa direcciones IPV4.Mi idea en un principio era pasar por parametro unas hints al socket para fuese mas abstracto,pero esto me generaba un error que era complicado de resolver y por cuestiones de tiempo debi recurrir a esta solucion mas concreta.

Los archivos client.c y server.c corresponden a tanto un servidor como a un cliente especificos para el protocolo usado en este trabajo(dbus_server y dbus_client).En ellos encontramos funciones especificas para el procesamiento y armado de mensajes acordes al protocolo.

Para el procesamiento del archivo de entrada tuve varios problemas y me costo bastante poder encontrar una solucion.Finalmente logre resolverlo de la siguiente manera: el dbus_client llama a su funcion dbus_client_get_call .Aqui se crea un buffer dinamico en donde se almacenara la linea o llamada leida(resazible_buffer),se pasa este buffer a un parser (common_parser) que ,mediante lecturas de 32 bytes, se encarga de guardar  en el la informacion del llamado.El parser lo que hace es:primero agrega al buffer lo que quedo sin procesar de la linea anterior(guardado en el parametro r),luego lee y va agregando lo leido al buffer hasta que encuentre un fin de linea (\n) usando strchr.Una vez que encontro el fin de linea,coloco un \0 en la posicion del \n y agrego el aux_buffer(donde se alamcena el resultado del fread).Finalmente actualizo r,con el resto de la linea que os quedo sin procesar para tomarlo en cuenta en el proceso de la siguiente linea.

Una vez procesada la linea,debiamos crear un mensaje con el formato apropiado y mandarlo a traves del socket al servidor.Para esto tenemos el dbus_message_t que tiene un buffer de 16 bytes para almacenar la informacion del mensaje(endianness,tamanio del header,longitud del size,etc) y dos buffers dinamicos uno para el body y otro para el header(Ambos son cadenas de bytes que se envian con el formato pedido).La funcion que se encarga de armar el mensaje es dbus_message_set,dentro de esta funcion tenemos otras 3 ,una que se encarga de setear el body otra el header y otra los primeros 16 bytes del mensaje (lo que decidi llamar info del mensaje).

Una vez armado el mensaje, procedemos a enviarlo a traves del socket.El dbus_client envia el mensaje al servidor y ademas se queda en espera hasta que el servidor le mande un OK para indicarle que el mensaje fue recibido.Al igual que el cliente el servidor recibe el mensaje en 3 partes:info,header y body.A medida que el dbus_server va recibiendo informacion ,la va convirtiendo e imprimiendo en la pantalla.Finalmente le ewnvia al cliente el mensaje  que notifica la recepciion correcta del mensaje.

Este proceso se repite para todas las lineas del archivo del dbus_client.En caso de error se liberan los recursos utilizados y se destruyen los TDAs creados,decidi tomar -1 como codigo de error.

## Diagramas:
![diagrama_clases_tp1](https://user-images.githubusercontent.com/50004705/81158734-a53a9880-8f5e-11ea-901e-9caad4037874.png)

* Diagrama de clases,que muestra las relaciones de los TDAs del protocolo TCP

## Aclaraciones:

* En el TDA parser decidi crear un tipo de dato para que fuese mas prolijo y en caso de que en algun momento quisiese agregarle algun "atributo" pudiese hacerlo facilmente sin necesidad de cambiar mucho codigo.Por cuestiones de estandares de codificacion,no podia dejar este struct vacio por eso le coloque una variable "dummie" que no tiene importancia a usos practicos.

* En algunos archivos hay funciones privadas que decidi no declararlas en .h ya que son de uso iterno. 

* La funcion set_first_4_bytes se ocupa de setear los primeros 4 bytes de cada parametro del header del mensaje.Recordando que el primero indica el tipo (ej: 06 para destino, 01 para path, etc), el segundo es un 1 que indica la cantidad (en este caso es igual para todos los parmetros), el tercero indica el tipo de dato (ej: 'o' para el path, 'g' para la firma y 's' para el resto de los parametros) y finalmente en todos los casos debemos colocar un 0.El parametro type representa el tipo de parametro que vamos a usar.

* La funcion  dbus_message_header_add se ocupa de agregar un parametro al header del mensaje dbus. Str representa la cadena de texto del parametro y type el tipo de parametro ej:  taller.dbus.params, 6 indica que estamos agregando el destino(porque el tipo 6 corresponde al destino) al header y dicho destino es " taller.dbus.params".Por otro lado, el tamanio del buffer utilizado en esta funcion es igual a 9 + strlen(str) + el padding correspondiente porque para enviar cada parametro se necesita ,ademas de enviar la cadena y los bytes de padding, enviar 9 bytes mas para cumplir con el formato del protocolo dbus(1 para el tipo,1 para la cantidad, 1 para el tipo de dato,1 para poner un 0, 4 para enviar el tamanio de la cadena a enviar,1 correspondiente al \0 que debemos colocar al final del string).
Una vez creado el buffer con el tamanio adecuado se comienza a llenar con la informacion correspondiente:primero se setean los primeros 4 bytes (como esta explicado aqui arriba), luego de estos 4 bytes colocamos 4 bytes mas correspondiente al int del tamanio del string,luego van los caracteres del string y su \0 y finalmente los ceros del padding.Para terminar, se agrega el buffer con el parametro con el formato adecuado al header del mensaje.

* La funcion  dbus_message_header_add_args se ocupa de agregar la firma de la funcion al header del mensaje.El parametro args guarda todos los argumentos de la llamada a ser enviada.El tamanio del buffer corresponde a 6 + args_num porque la firma se debe enviar con el siguiente formato: 4 bytes correspondientes al formato de la firma (1 para el tipo 08,1 para la cantidad,1 para el tipo de dato 'g' 1 para colocar un 0),1 byte para la cantidad de parametros(en este caso usamos 1 byte para guardar este numero),args_num bytes para indicar el tipo de cada parametro de nuestra llamaday finalmente 1 byte para poner un 0.En este caso no se agrega padding al final.

