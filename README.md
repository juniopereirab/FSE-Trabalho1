# Trabalho 1 - FSE

Controle de sinal de trânsito em cruzamentos com Servidor Central.

Nome: Dâmaso Júnio Pereira Brasileo
Matrícula: 17/0031438

## Instalação

Para instalação, basta executar:

```bash
cd Central
make
```

Para instalar o servidor Central.

E:

```bash
cd Distributed
make
```

Para instalar o servidor Distribuído.


## Execução

Para executar, é necessário executar o Servidor Central + Duas instâncias do servidor distribuído na RASP 44, seguindo os comandos:

```bash
cd Central
bin/bin
```

```bash
cd Distributed
bin/bin cross1.txt
```

```bash
cd Distributed
bin/bin cross2.txt
```

Sendo `cross1.txt` ou `cross2.txt` os arquivos de configuração das portas da RASP.
Assim, executando na RASP 43 ou na RASP 44, os 4 cruzamentos serão executados.

## Modo de Uso

O servidor Central aceita até 4 conexões simultâneas, contemplando os 4 cruzamentos. O teste que foi feito, foi executando 
RASP 44 => Servidor Central + 2 instâncias do Servidor Distribuído
RASP 43 => 2 instâncias do Servidor Distribuído

Foram todas conectadas e enviando informações para o Servidor Central.

Acionando os sensores e botões através da plataforma online, as entradas foram identificadas e executadas.