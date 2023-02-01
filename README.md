# Preparación del ambiente de trabajo.

Antes de realizar cualquier operación dentro del workspace es necesario realizar una configuración las varialbes de ambiente. Esto con el proposito de contar con las herramientas de compilación y depuración en la consola o el ambiente de trabajo, para esto se cuenta con dos posibilidades:

1. Setear las variables de ambiente para la consola actual. Ejecutar el siguiente comando:

```
export PATH=$PATH:${install_dir}/ISO-I/gcc-arm-none-eabi/bin
```

2. Configuración de las variables de ambiente en el sistema operativo. Modificar el archivo **.bashrc** en el home con la siguiente línea.

```
PATH=$PATH:${install_dir}/ISO-I/gcc-arm-none-eabi/bin
```


*Note: Estas instrucciones solo deben usarse si no se desea utilizar el IDE del fabricante del Kit utilizadó en la matería.*
