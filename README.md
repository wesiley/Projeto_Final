# Detector de Poluição Sonora com Feedback Visual

Este projeto consiste em um detector de poluição sonora que monitora o nível de ruído ambiente e fornece feedback visual através de um display OLED e LEDs RGB. O sistema é capaz de classificar o ruído em três níveis: baixo, médio e alto, e alertar o usuário sobre possíveis níveis prejudiciais de poluição sonora.

# Funcionalidades
Monitoramento de Ruído: Captura e classifica o ruído ambiente em três níveis: baixo, médio e alto.

# Feedback Visual:

Display OLED: Exibe o nível de ruído e o estado do sistema.

LEDs RGB: Mudam de cor conforme o nível de ruído:

Verde: Ruído baixo.

Azul: Ruído médio.

Vermelho: Ruído alto.

Interação com Botões: Dois botões permitem ativar e desativar o sistema.

# Componentes Utilizados
Microcontrolador: Raspberry Pi Pico.

Display: OLED SSD1306 (128x64 pixels, comunicação I2C).

Sensores: Microfone com saída analógica.

LEDs: RGB (vermelho, verde e azul).

Botões: Dois botões para controle do sistema.

Outros: Resistores, jumpers e protoboard.

# Configuração do Software
Pré-requisitos
IDE: Visual Studio Code com PlatformIO ou Thonny.

# Bibliotecas:

ssd1306.h para controle do display OLED.

pico/stdlib.h para acesso ao hardware do Raspberry Pi Pico.

# Passos para Configuração
Clone este repositório:

bash
Copy
git clone https://github.com/wesiley/Projeto_Final

Compile e faça o upload do código para o Raspberry Pi Pico.

# Video Demonstrativo


# Como Usar
Inicialização: Ao ligar o sistema, o display exibirá a mensagem "Detector de Ruído - Aguardando...".

Ativação: Pressione o Botão A para ativar o sistema.

Monitoramento: O sistema começará a monitorar o ruído ambiente e exibir o nível no display e nos LEDs.

Desativação: Pressione o Botão B para desativar o sistema.

Contribuição
Contribuições são bem-vindas! Siga os passos abaixo:

Faça um fork do projeto.

Crie uma branch para sua feature (git checkout -b feature/nova-feature).

Commit suas mudanças (git commit -m 'Adiciona nova feature').

Push para a branch (git push origin feature/nova-feature).

Abra um Pull Request.

Licença
Este projeto está licenciado sob a licença MIT. Veja o arquivo LICENSE para mais detalhes.
