# Inspetor HTTP

Projeto final de Teleinformática e Redes 2 (1/2018)

Consiste em um programa com GUI (GTK+ 3.0) com os submódulos descritos abaixo.
A tela inicial deve ser o Interceptor mas também deve contar com um botão para abrir um spider e um client, que abrirão em uma nova janela.

O executável será chamado de aracne e usará as bibliotecas POSIX C.

---

## Interceptor

Permite interceptar e editar requests HTTP dos navegador antes de permitir que sigam para seu destino final.

## Spider

O spider cria recursivamente uma árvore hipertextual das URLs adjacentes à uma URL fornecida.
URLs fora do domínio raiz não devem ser perseguidas.

## Client

Comportamento similar ao wget porém somente para HTTP. Deve corrigir as referências para que o site possa ser usado localmente.
