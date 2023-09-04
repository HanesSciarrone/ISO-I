# Preparación del ambiente de trabajo.

Antes de realizar cualquier operación dentro del workspace es necesario configurar las varialbes de ambiente con el proposito de contar con las herramientas de compilación y depuración en la consola o el ambiente de trabajo. Para esto se cuenta con dos posibilidades:

1. Setear las variables de ambiente para la consola actual. Ejecutar el siguiente comando:

```
export PATH=$PATH:${install_dir}/ISO-I/gcc-arm-none-eabi/bin
```

2. Configuración de las variables de ambiente en el sistema operativo. Modificar el archivo **.bashrc** en el home con la siguiente línea.

```
PATH=$PATH:${install_dir}/ISO-I/gcc-arm-none-eabi/bin
```

## Preparación de OpenOCD

1. Clonarse el código de [OpenOCD](https://git.code.sf.net/p/openocd/code openocd-code) desde el repositorio oficial.

2. Hacer un checkout al tag de la última versión disponible.

3. Dentro del repositorio realizar la compilación/instalación del OpenOCD a travez de los siguientes comandos.

```
./bootstrap
./configure --prefix=${install_dir}
make
make install
```

Puede ser necesario correr la última instrucción con permisos de administrador si seteo un directorio de instalación que lo requiera.

*Note: Estas instrucciones solo deben usarse si no se desea utilizar el IDE del fabricante del Kit utilizadó en la matería.*

# Depuración de aplicación

Para poder hacer debugging de la aplicación es necesario primero ejecutar OpenOCD para la _interfaz_ y _target_ de la placa a utilizar. Luege ejecutar GDB y conectarse a OpenOCD para estár empezar el proceso de debugging.

1. Ejecutar OpenOCD

```
./${install_dir}/bin/openocd -f ${install_dir}/share/openocd/scripts/board/stm32f429discovery.cfg
```

2. Ejecutar arm-none-eabi-gdb con el programa que desea hacer debugging.

```
arm-none-eabi-gdb RTOS/build/RTOS.elf
```

3. Dentro de la sesión de GDB debemos conectarnos con OpenOCD para hacer el debugging.

```
target extended-remote localhost:${OpenOCD_port}
```




