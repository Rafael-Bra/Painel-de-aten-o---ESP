# Painel de Atenção da Coordenação

## Descrição

Este projeto realiza o monitoramento do nível de atenção dos alunos em cinco laboratórios, utilizando um ESP32 central que recebe dados em tempo real via MQTT.

As informações são enviadas por câmeras inteligentes no formato:

```
LabX/AlunoYY
```

O sistema calcula a média de atenção por laboratório e indica o resultado por meio de LEDs.

---

## Funcionamento

Os dados recebidos são armazenados em uma matriz bidimensional (laboratório x aluno). A cada nova mensagem:

1. O laboratório e o aluno são identificados a partir do tópico
2. O valor de atenção é atualizado
3. A média do laboratório é recalculada
4. O LED correspondente é atualizado

A média considera apenas alunos que já enviaram dados.

---

## Regra de decisão

* Média < 0.5: LED ligado
* Média ≥ 0.5: LED desligado

---

## Hardware

| Laboratório | GPIO |
| ----------- | ---- |
| Lab1        | 2    |
| Lab2        | 4    |
| Lab3        | 5    |
| Lab4        | 18   |
| Lab5        | 19   |

---

## Configuração MQTT

* Broker: escola-mesquita.cloud.shiftr.io
* Porta: 1883
* Usuário: escola-mesquita
* Senha: blcAkgBumM26K61q

Tópicos monitorados:

```
Lab1/#
Lab2/#
Lab3/#
Lab4/#
Lab5/#
```

---

## Estrutura de dados

```cpp
float atencao[5][10];
bool chegou[5][10];
```

---

## Execução

1. Conectar ao Wi-Fi
2. Conectar ao broker MQTT
3. Inscrever-se nos tópicos dos laboratórios
4. Receber e processar mensagens
5. Atualizar média e LEDs

---

## Validação

Os testes podem ser feitos publicando valores diretamente no broker:

https://escola-mesquita.cloud.shiftr.io/

Exemplo:

```
Lab1/Aluno01 → 1.0
Lab1/Aluno02 → 0.0

---
