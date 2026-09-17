# Vulkano

Aplicação dos meus estudos de **Vulkan** através da biblioteca **LightweightVK**, com janela gerenciada por **GLFW** e shaders escritos em **GLSL**.

## Stack

| Componente | Descrição |
|---|---|
| **C++20** | Padrão da linguagem |
| **Vulkan** | API gráfica de baixo nível |
| **LightweightVK (LVK)** | Abstração simplificada sobre Vulkan |
| **GLFW** | Criação de janela e gerenciamento de eventos |
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

O projeto usa **Git submodules** para o LightweightVK. Clone com submódulos:

```powershell
git clone --recurse-submodules https://github.com/angelo-gabriel/Vulkano.git
cd Vulkano
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
5. Execute com `F5` ou selecione o alvo `Vulkano.exe` e clique em **Run**.

### Pelo terminal (PowerShell)

Abra um **Developer PowerShell for VS** (para que o MSVC esteja no PATH) e execute:

```powershell
# Configurar
cmake --preset x64-debug

# Compilar
cmake --build out/build/x64-debug

# Executar
.\out\build\x64-debug\Vulkano.exe
```

Para release:

```powershell
cmake --preset x64-release
cmake --build out/build/x64-release
.\out\build\x64-release\Vulkano.exe
```

## Estrutura do projeto

```
Vulkano/
├── source/
│   ├── main.cpp          # Ponto de entrada da aplicação
│   ├── Utils.h           # Declarações utilitárias
│   └── Utils.cpp         # Leitura e compilação de shaders
├── resources/
│   ├── main.vert         # Vertex shader (GLSL)
│   └── main.frag         # Fragment shader (GLSL)
├── third_party/
│   └── lightweightvk/    # Submódulo Git — biblioteca LVK
├── CMakeLists.txt        # Configuração CMake do projeto
├── CMakePresets.json     # Presets de build (x64-debug, x64-release, ...)
└── README.md
```

## Como funciona

1. **GLFW** cria a janela e gerencia o loop de eventos.
2. **LightweightVK** inicializa o contexto Vulkan com swapchain.
3. Os shaders GLSL (`main.vert` e `main.frag`) são lidos do disco e compilados em runtime via `glslang` (integrado ao LVK).
4. O caminho absoluto dos shaders é resolvido via a definição de compilação `SHADER_DIR` (configurada no `CMakeLists.txt`), garantindo que o executável encontre os arquivos independentemente do diretório de trabalho.
5. A cada frame, um command buffer é adquirido, o pipeline de renderização é vinculado, e `cmdDraw(3)` desenha o triângulo.