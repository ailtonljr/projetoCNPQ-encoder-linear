# Sistema IoT para Validação de Robô SCARA

Este projeto implementa uma pipeline de dados IoT completa, projetada para monitorar o deslocamento de uma régua digital de alta precisão (SINO KA-300) em tempo real. O objetivo principal do sistema é servir como uma ferramenta para a validação do sistema de controle e da precisão de movimentos de um robô SCARA construído em impressora 3D.

## Arquitetura do Sistema

O fluxo de dados segue uma arquitetura moderna e desacoplada, utilizando o protocolo MQTT como intermediário:

`Arduino (Sensor) -> MQTT Broker (HiveMQ) -> Node-RED -> InfluxDB -> Grafana (Dashboard)`

Todo o ambiente de backend (Node-RED, InfluxDB e Grafana) é orquestrado com Docker e Docker Compose, garantindo portabilidade e facilidade de implantação.

## Tecnologias Utilizadas

* **Hardware:** Arduino Mega, Ethernet Shield, Encoder Óptico SINO KA-300.
* **Backend:** Docker, Docker Compose, Node-RED, InfluxDB v2, Grafana.
* **Protocolo de Comunicação:** MQTT.

---
## Como Executar o Projeto

Siga estes passos para configurar e executar o ambiente completo do zero.

### Pré-requisitos
* [Git](https://git-scm.com/)
* [Docker](https://www.docker.com/products/docker-desktop/) e Docker Compose

### 1. Baixar o Projeto
Abra um terminal e clone este repositório:

```bash
git clone https://github.com/RodrigoShig/projetoCNPQ-encoder-linear.git
cd projetoCNPQ-encoder-linear
```

### 2. Setup Inicial do InfluxDB (Execução Única)
Esta etapa cria o banco de dados e as credenciais necessárias.

1.  Inicie **apenas** o serviço do InfluxDB:
    ```bash
    docker-compose up -d influxdb
    ```

2.  Aguarde um minuto e acesse `http://localhost:8086` no seu navegador. Siga o assistente "Get Started" para criar seu **usuário**, **senha**, **Organização** e **Bucket**.

3.  Gere um **Token de API** em `Load Data > API Tokens > Generate API Token`. Selecione "All Access Token".

4.  **Anote** o nome da sua Organização, seu Bucket e o Token gerado.

5.  Pare o serviço com o comando:
    ```bash
    docker-compose down
    ```

### 3. Configurar o Projeto
1.  Na pasta `grafana_config/provisioning/datasources/`, faça uma cópia do arquivo `datasource.yml.example` e renomeie a cópia para `datasource.yml`.
2.  Abra o novo arquivo `datasource.yml` e preencha os campos `organization`, `defaultBucket` e `token` com as credenciais que você anotou.

### 4. Iniciar o Ambiente Completo
Execute o comando abaixo para iniciar todos os serviços (InfluxDB, Grafana e Node-RED).
```bash
docker-compose up -d
```

### 5. Configurar o Node-RED
1.  Acesse a interface do Node-RED em `http://localhost:1880`.
2.  Importe o fluxo: Menu **☰ > Importar >** selecionar um arquivo para upload e escolha o arquivo `flows.json`.
3.  No fluxo importado, dê um duplo clique no nó `influxdb out`, edite a configuração do servidor e atualize o **Token** com o que você criou no InfluxDB.
4.  Clique em **"Implantar" (Deploy)**.

### 6. Carregar o Firmware do Arduino
1.  Abra o arquivo `.ino` da pasta `firmware_arduino` na IDE do Arduino.
2.  Verifique se o fator de calibração está correto para o seu hardware.
3.  Carregue o código para a sua placa Arduino e ligue-a.

### Pronto!
Seu dashboard estará disponível e sendo atualizado em tempo real em `http://localhost:3000`.

---
## Gerenciamento do Ambiente

* **Para parar todos os serviços:** `docker-compose down`
* **Para ver o status dos serviços:** `docker-compose ps`
* **Para ver os logs em tempo real:** `docker-compose logs -f`