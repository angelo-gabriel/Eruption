# Eruption

Aplicação dos meus estudos de **Vulkan** através da biblioteca **LightweightVK**, com janela gerenciada por **GLFW** e shaders escritos em **GLSL**.

## Stack

| Componente | Descrição |
|---|---|
| **C++20** | Padrão da linguagem |
| **Vulkan** | API gráfica de baixo nível |
| **LightweightVK (LVK)** | Abstração simplificada sobre Vulkan |
| **GLFW** | Criação de janela e gerenciamento de eventos |
| **Assimp** | Importação de modelos 3D |
| **GLM** | Matrizes, transformações e projeção 3D |
| **GLSL** | Linguagem de shaders (vertex + fragment) |
| **CMake + Ninja** | Sistema de build |
| **MSVC** | Toolchain de compilação (Visual Studio) |

## Pré-requisitos

- **Git**
- **CMake** 3.10+
- **Ninja** (build system)
- **Visual Studio 2022+** (com workload C++) — fornece o MSVC e o Windows SDK
- **Vulkan SDK** — [baixar em vulkan.lunarg.com](https://vulkan.lunarg.com/)

## Clonando o repositório

O projeto usa **Git submodules** para o LightweightVK, Assimp e GLM. Clone com submódulos:

```powershell
git clone --recurse-submodules https://github.com/angelo-gabriel/Eruption.git
cd Eruption
```

Se já clonou sem submódulos, inicialize-os:

```powershell
git submodule update --init --recursive
```

## Build

### Pelo Visual Studio

1. Abra a pasta do projeto no Visual Studio (`File > Open > Folder`).
2. O CMake detectará automaticamente os presets em `CMakePresets.json`.
3. Selecione o preset desejado (`x64-debug`, `x64-release`, etc.).
4. Compile com `Ctrl+Shift+B` ou `Build > Build All`.
5. Execute com `F5` ou selecione o alvo `Eruption.exe` e clique em **Run**.

### Pelo terminal (PowerShell)

Abra um **Developer PowerShell for VS** (para que o MSVC esteja no PATH) e execute:

```powershell
# Configurar
cmake --preset x64-debug

# Compilar
cmake --build out/build/x64-debug

# Executar
.\out\build\x64-debug\Eruption.exe
```

Para release:

```powershell
cmake --preset x64-release
cmake --build out/build/x64-release
.\out\build\x64-release\Eruption.exe
```

## Estrutura do projeto

```
Eruption/
├── data/		  # Arquivos .gltf e texturas
├── source/
│   ├── main.cpp          # Ponto de entrada da aplicação
│   ├── Utils.h           # Declarações utilitárias
│   └── Utils.cpp         # Leitura e compilação de shaders
├── resources/
│   ├── main.vert         # Vertex shader (GLSL)
│   └── main.frag         # Fragment shader (GLSL)
├── third_party/
│   ├── lightweightvk/    # Submódulo Git — biblioteca LVK
│   ├── assimp/           # Submódulo Git — biblioteca Assimp
│   └── glm/              # Submódulo Git — biblioteca GLM
├── CMakeLists.txt        # Configuração CMake do projeto
├── CMakePresets.json     # Presets de build (x64-debug, x64-release, ...)
└── README.md
```

## Como funciona

1. **GLFW** cria a janela e gerencia o loop de eventos.
2. **LightweightVK** inicializa o contexto Vulkan com swapchain.
3. **Assimp** carrega o modelo `data/rubber_duck/scene.gltf`, incluindo sua malha e os dados associados ao asset.
4. Os vértices e índices da malha são enviados para buffers Vulkan de dispositivo e renderizados com `cmdDrawIndexed`.
5. **GLM** calcula as matrizes de modelo, visão e projeção. O modelo gira continuamente com base no tempo da aplicação.
6. Um depth buffer é utilizado para garantir a ordenação correta da geometria 3D.
7. Cada frame renderiza a malha em duas passagens: uma superfície sólida e uma sobreposição em wireframe.
8. Os shaders GLSL (`main.vert` e `main.frag`) são lidos do disco e compilados em runtime via `glslang` (integrado ao LVK).
9. Os caminhos absolutos dos shaders e dos modelos são resolvidos pelas definições de compilação `SHADER_DIR` e `MODEL_DIR`, configuradas no `CMakeLists.txt`.

## Recursos atuais

- Janela Vulkan em resolução inicial de 1280 × 720.
- Importação de modelos `.gltf` usando Assimp.
- Renderização de geometria 3D com vertex buffer e index buffer.
- Projeção em perspectiva e animação de rotação do modelo.
- Depth testing com escrita de profundidade.
- Visualização simultânea da superfície sólida e da malha em wireframe.
- Asset de exemplo em `data/rubber_duck/`, com textura PNG e arquivo binário da cena.
