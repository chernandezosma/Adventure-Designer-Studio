# Adventure Designer Studio — UI Design Specification
> Generado desde la sesión de diseño en Claude.ai  
> Rama objetivo: `DataObjects` — repo: `chernandezosma/Adventure-Designer-Studio`  
> Backend: **ImGui + SDL3**, multiplataforma (Windows / macOS / Linux)

---

## 1. Layout general del IDE

El IDE se divide en **3 columnas** dentro de un layout principal de tipo dockspace:

```
┌─────────────────────────────────────────────────────────────────┐
│  TITLEBAR (30px) — Logo · Menús · Botones ventana               │
├──────────────┬──────────────────────────────┬───────────────────┤
│              │  GRAPH VIEW (flex)            │                   │
│  PROJECT     │  Grafo de escenas (SVG-like)  │  PROPERTIES       │
│  TREE        ├──────────────────────────────┤  PANEL            │
│  PANEL       │  CODE EDITOR                  │  (240px)          │
│  (220px)     │  + OUTPUT STRIP               │                   │
├──────────────┴──────────────────────────────┴───────────────────┤
│  STATUSBAR (22px)                                                │
└─────────────────────────────────────────────────────────────────┘
```

### Proporciones
| Panel | Ancho | Notas |
|---|---|---|
| ProjectTreePanel | 220px fijo | Redimensionable con splitter |
| CenterArea | flex (resto) | Grid rows: grafo 60% / editor 40% |
| PropertiesPanel | 240px fijo | Redimensionable con splitter |
| Titlebar | 100% × 30px | |
| Statusbar | 100% × 22px | |

---

## 2. Paleta de colores

```cpp
// Backgrounds
ImVec4 BG0  = {0.059f, 0.067f, 0.090f, 1.0f}; // #0f1117 — fondo base
ImVec4 BG1  = {0.086f, 0.106f, 0.141f, 1.0f}; // #161b24 — paneles
ImVec4 BG2  = {0.118f, 0.145f, 0.200f, 1.0f}; // #1e2533 — hover
ImVec4 BG3  = {0.145f, 0.176f, 0.243f, 1.0f}; // #252d3e — selección activa

// Bordes
ImVec4 BORDER    = {0.180f, 0.227f, 0.314f, 1.0f}; // #2e3a50
ImVec4 BORDER_HI = {0.239f, 0.310f, 0.431f, 1.0f}; // #3d4f6e

// Textos
ImVec4 TEXT0 = {0.910f, 0.902f, 0.875f, 1.0f}; // #e8e6df — primario
ImVec4 TEXT1 = {0.722f, 0.706f, 0.659f, 1.0f}; // #b8b4a8 — secundario
ImVec4 TEXT2 = {0.353f, 0.384f, 0.459f, 1.0f}; // #5a6275 — muted/labels

// Acentos por tipo de elemento
ImVec4 C_SCENE  = {0.482f, 0.616f, 1.000f, 1.0f}; // #7b9dff — Escenas
ImVec4 C_NPC    = {0.831f, 0.400f, 0.478f, 1.0f}; // #d4667a — Personajes
ImVec4 C_ITEM   = {0.357f, 0.769f, 0.627f, 1.0f}; // #5bc4a0 — Objetos
ImVec4 C_PUZZLE = {0.910f, 0.643f, 0.290f, 1.0f}; // #e8a44a — Puzzles
ImVec4 C_VAR    = {0.690f, 0.486f, 1.000f, 1.0f}; // #b07cff — Variables
ImVec4 C_AUDIO  = {0.941f, 0.541f, 0.365f, 1.0f}; // #f08a5d — Audio

// Estados
ImVec4 C_ERROR  = {0.878f, 0.322f, 0.322f, 1.0f}; // #e05252
ImVec4 C_WARN   = {0.910f, 0.643f, 0.290f, 1.0f}; // #e8a44a (igual que puzzle)
ImVec4 C_OK     = {0.357f, 0.769f, 0.627f, 1.0f}; // #5bc4a0 (igual que item)
```

---

## 3. ProjectTreePanel — Especificación completa

### 3.1 Estructura de la clase

```cpp
// ProjectTreePanel.h
#pragma once
#include "imgui.h"
#include <string>
#include <vector>
#include <functional>

// Tipos de nodo del árbol
enum class NodeType {
    Scene, NPC, Item, Puzzle, Variable, Audio,
    // Sub-nodos
    SceneDescription, SceneOptions,
    NPCDialogs, NPCStats,
    PuzzleClues, PuzzleSolution
};

// Badge visual sobre un nodo
enum class NodeBadge {
    None, Start, End, Win, Conditional,
    Locked, Key, Usable, Error, Warning, Mission, Secret
};

struct TreeNode {
    std::string id;           // Identificador único (ej. "sala_entrada")
    std::string label;        // Texto mostrado en el árbol
    NodeType    type;
    NodeBadge   badge        = NodeBadge::None;
    NodeBadge   badge2       = NodeBadge::None;   // segundo badge opcional
    std::string typeTag;      // etiqueta de tipo pequeña (ej. "bool", "sfx", "opt")
    std::string typeTagValue; // valor para variables (ej. "false", "0")
    bool        hasChildren  = false;
    bool        isExpanded   = false;
    bool        hasError     = false;
    bool        hasWarning   = false;
    int         depth        = 0;     // nivel de indentación (0=raíz, 1=hijo, etc.)
    std::vector<TreeNode> children;
};

class ProjectTreePanel {
public:
    ProjectTreePanel();
    ~ProjectTreePanel() = default;

    void Render();                        // Llamar cada frame en el ImGui loop
    void SetProject(/* ProjectData* */);  // Inyectar modelo de datos

    // Callbacks — conectar al resto del IDE
    std::function<void(const std::string& nodeId, NodeType)> OnNodeSelected;
    std::function<void(const std::string& nodeId, NodeType)> OnNodeDoubleClicked;
    std::function<void(NodeType)>                            OnAddNode;
    std::function<void(const std::string& nodeId)>           OnDeleteNode;
    std::function<void(const std::string& nodeId)>           OnDuplicateNode;

    const std::string& GetSelectedNodeId() const { return m_selectedId; }

private:
    // Render helpers
    void RenderTopBar();
    void RenderSearchBar();
    void RenderStatsStrip();
    void RenderSection(const char* label, ImVec4 color,
                       const char* icon, std::vector<TreeNode>& nodes,
                       bool& expanded, int count);
    void RenderNode(TreeNode& node);
    void RenderBadge(NodeBadge badge);
    void RenderContextMenu(TreeNode& node);
    void RenderFooter();

    // State
    std::string m_selectedId;
    char        m_searchBuf[256] = {};
    bool        m_secScenes   = true;
    bool        m_secNPCs     = true;
    bool        m_secItems    = true;
    bool        m_secPuzzles  = true;
    bool        m_secVars     = true;
    bool        m_secAudio    = true;

    // Data (poblar desde el modelo real del proyecto)
    std::vector<TreeNode> m_scenes;
    std::vector<TreeNode> m_npcs;
    std::vector<TreeNode> m_items;
    std::vector<TreeNode> m_puzzles;
    std::vector<TreeNode> m_variables;
    std::vector<TreeNode> m_audio;

    void RebuildFromProject();  // Sincroniza m_scenes etc. desde el modelo
};
```

### 3.2 Estructura visual de cada nodo

```
[indent_lines][expander_arrow][icon] label [badge1] [badge2] [type_tag] [value_tag]
                                                                         ^(hover) [✎] [⊕] [🗑]
```

- **indent_lines**: líneas verticales/horizontales que dibujan el árbol (ver sección 3.4)
- **expander_arrow**: `▶` / `▼` solo si `hasChildren == true`; espacio vacío si no
- **icon**: icono de Tabler Icons (ver tabla sección 3.3)
- **badge**: píldoras de colores con texto (ver sección 3.5)
- **type_tag**: texto monospace pequeño en `TEXT2` (ej. `bool`, `sfx`)
- **value_tag**: valor actual de la variable en badge `b-lock`
- **hover actions**: botones de acción que solo aparecen en hover

### 3.3 Iconos por tipo de nodo

Usar Tabler Icons o FontAwesome embebidos como font. Si el proyecto ya usa una icon font, adaptarlos:

| NodeType | Icono sugerido | Color |
|---|---|---|
| Scene (normal) | `ti-map-pin` | C_SCENE |
| Scene (inicio) | `ti-map-pin` + badge START | C_SCENE |
| Scene (fin) | `ti-flag` | C_ERROR |
| NPC (genérico) | `ti-user` | C_NPC |
| NPC (guardián) | `ti-shield` | C_NPC |
| NPC (rey/jefe) | `ti-crown` | C_NPC |
| Item (llave) | `ti-key` | C_ITEM |
| Item (antorcha) | `ti-flame` | C_ITEM |
| Item (mapa) | `ti-map` | C_ITEM |
| Item (poción) | `ti-flask` | C_ITEM |
| Item (espada/daga) | `ti-sword` | C_ITEM |
| Item (pergamino) | `ti-scroll` | C_ITEM |
| Item (moneda) | `ti-coin` | C_ITEM |
| Item (gema) | `ti-diamond` | C_ITEM |
| Puzzle (puerta) | `ti-door` | C_PUZZLE |
| Puzzle (trampa) | `ti-alert-triangle` | C_PUZZLE |
| Puzzle (genérico) | `ti-puzzle` | C_PUZZLE |
| Variable (bool) | `ti-toggle-right` | C_VAR |
| Variable (int) | `ti-123` | C_VAR |
| Variable (float) | `ti-123` | C_VAR |
| Variable (string) | `ti-abc` | C_VAR |
| Variable (list) | `ti-list-check` | C_VAR |
| Variable (enum) | `ti-adjustments` | C_VAR |
| Variable (time) | `ti-clock` | C_VAR |
| Audio (ambient) | `ti-volume` | C_AUDIO |
| Audio (bgm) | `ti-music` | C_AUDIO |
| Audio (sfx) | `ti-speakerphone` | C_AUDIO |
| Sub: descripción | `ti-align-left` | TEXT2 |
| Sub: opciones | `ti-list` | TEXT2 |
| Sub: diálogos | `ti-message-2` | TEXT2 |
| Sub: stats NPC | `ti-chart-bar` | TEXT2 |
| Sub: pistas | `ti-eye` | TEXT2 |
| Sub: solución | `ti-check` | TEXT2 |

### 3.4 Líneas de indentación del árbol

Para cada nodo con `depth > 0`, dibujar líneas usando `ImDrawList`:

```cpp
// Pseudocódigo ImGui para las líneas del árbol
// Por cada nivel de indentación:
//   - Si no es el último hijo: línea vertical completa
//   - Si es el último hijo: línea vertical hasta la mitad + línea horizontal

void ProjectTreePanel::DrawIndentLines(int depth, bool isLast, ImVec2 nodePos) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float lineColor = IM_COL32(46, 58, 80, 255); // BORDER color
    float indentW = 18.0f;  // ancho por nivel
    float nodeH   = 22.0f;  // altura de fila

    for (int i = 0; i < depth; i++) {
        float x = nodePos.x + i * indentW + indentW * 0.5f;
        bool lastLevel = (i == depth - 1);

        if (!lastLevel) {
            // Línea vertical continua
            dl->AddLine({x, nodePos.y}, {x, nodePos.y + nodeH}, lineColor, 1.0f);
        } else if (isLast) {
            // L invertida: vertical hasta mitad + horizontal
            dl->AddLine({x, nodePos.y}, {x, nodePos.y + nodeH * 0.5f}, lineColor, 1.0f);
            dl->AddLine({x, nodePos.y + nodeH * 0.5f},
                        {x + indentW * 0.5f, nodePos.y + nodeH * 0.5f}, lineColor, 1.0f);
        } else {
            // T: vertical completa + horizontal en mitad
            dl->AddLine({x, nodePos.y}, {x, nodePos.y + nodeH}, lineColor, 1.0f);
            dl->AddLine({x, nodePos.y + nodeH * 0.5f},
                        {x + indentW * 0.5f, nodePos.y + nodeH * 0.5f}, lineColor, 1.0f);
        }
    }
}
```

### 3.5 Badges (píldoras de estado)

Renderizar como rectángulos redondeados con texto pequeño:

```cpp
struct BadgeStyle {
    ImVec4 textColor;
    ImVec4 bgColor;   // textColor con alpha ~0.15
    const char* label;
};

// Tabla de estilos de badges
BadgeStyle GetBadgeStyle(NodeBadge badge) {
    switch (badge) {
        case NodeBadge::Start:       return {{0.35f,0.77f,0.63f,1}, {0.35f,0.77f,0.63f,0.15f}, "inicio"};
        case NodeBadge::End:         return {{0.88f,0.32f,0.32f,1}, {0.88f,0.32f,0.32f,0.15f}, "fin"};
        case NodeBadge::Win:         return {{0.35f,0.77f,0.63f,1}, {0.35f,0.77f,0.63f,0.20f}, "victoria"};
        case NodeBadge::Conditional: return {{0.91f,0.64f,0.29f,1}, {0.91f,0.64f,0.29f,0.15f}, "condicional"};
        case NodeBadge::Locked:      return {{0.69f,0.49f,1.00f,1}, {0.69f,0.49f,1.00f,0.15f}, "bloqueado"};
        case NodeBadge::Key:         return {{0.43f,0.77f,0.91f,1}, {0.43f,0.77f,0.91f,0.15f}, "llave"};
        case NodeBadge::Usable:      return {{0.91f,0.64f,0.29f,1}, {0.91f,0.64f,0.29f,0.12f}, "usable"};
        case NodeBadge::Mission:     return {{0.91f,0.64f,0.29f,1}, {0.91f,0.64f,0.29f,0.15f}, "misión"};
        case NodeBadge::Secret:      return {{0.69f,0.49f,1.00f,1}, {0.69f,0.49f,1.00f,0.15f}, "secreto"};
        case NodeBadge::Error:       return {{0.88f,0.32f,0.32f,1}, {0.88f,0.32f,0.32f,0.15f}, "err"};
        case NodeBadge::Warning:     return {{0.91f,0.64f,0.29f,1}, {0.91f,0.64f,0.29f,0.15f}, "warn"};
        default:                     return {{}, {}, nullptr};
    }
}

void ProjectTreePanel::RenderBadge(NodeBadge badge) {
    auto style = GetBadgeStyle(badge);
    if (!style.label) return;

    ImGui::SameLine();
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImVec2 textSz = ImGui::CalcTextSize(style.label);
    float padX = 4.0f, padY = 1.0f, rounding = 7.0f;
    ImVec2 bMin = p;
    ImVec2 bMax = {p.x + textSz.x + padX*2, p.y + textSz.y + padY*2};

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(bMin, bMax,
        ImGui::ColorConvertFloat4ToU32(style.bgColor), rounding);
    dl->AddText({bMin.x + padX, bMin.y + padY},
        ImGui::ColorConvertFloat4ToU32(style.textColor), style.label);

    // Avanzar cursor
    ImGui::SetCursorScreenPos({bMax.x + 3.0f, p.y});
    ImGui::Dummy({0, textSz.y + padY*2});
}
```

### 3.6 Barra de búsqueda

```cpp
void ProjectTreePanel::RenderSearchBar() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, BG1);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, BG3);
    ImGui::PushStyleColor(ImGuiCol_Border, BORDER);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 8.0f);
    if (ImGui::InputTextWithHint("##search", "Buscar...", m_searchBuf, sizeof(m_searchBuf))) {
        // El filtrado se aplica en RenderNode() comparando m_searchBuf con node.label
    }
    ImGui::PopStyleColor(3);
}
```

**Lógica de filtrado:** en `RenderNode()`, si `m_searchBuf` no está vacío y `node.label` no contiene el texto (case-insensitive), hacer `return` sin renderizar el nodo.

### 3.7 Strip de estadísticas

Fila compacta debajo del buscador con puntos de color + conteos:

```
● 6 esc   ● 3 npc   ● 8 obj   ● 3 puz   ● 9 var   ● 4 aud
```

```cpp
void ProjectTreePanel::RenderStatsStrip() {
    struct Stat { ImVec4 color; int count; const char* label; };
    Stat stats[] = {
        {C_SCENE,  (int)m_scenes.size(),    "esc"},
        {C_NPC,    (int)m_npcs.size(),      "npc"},
        {C_ITEM,   (int)m_items.size(),     "obj"},
        {C_PUZZLE, (int)m_puzzles.size(),   "puz"},
        {C_VAR,    (int)m_variables.size(), "var"},
        {C_AUDIO,  (int)m_audio.size(),     "aud"},
    };
    ImGui::PushStyleColor(ImGuiCol_ChildBg, BG0);
    for (auto& s : stats) {
        // Punto de color
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddCircleFilled(
            {p.x+4, p.y+7}, 3.0f, ImGui::ColorConvertFloat4ToU32(s.color));
        ImGui::Dummy({10, 14});
        ImGui::SameLine();
        // Número en blanco + label muted
        ImGui::TextColored(TEXT0, "%d", s.count);
        ImGui::SameLine(0, 2);
        ImGui::TextColored(TEXT2, "%s", s.label);
        ImGui::SameLine(0, 8);
    }
    ImGui::NewLine();
    ImGui::PopStyleColor();
}
```

### 3.8 Secciones colapsables

Cada categoría (Escenas, Personajes, etc.) es una sección colapsable con:
- Flecha `▶` / `▼` animada
- Icono de color
- Label en mayúsculas + color de acento
- Badge con conteo de elementos

```cpp
void ProjectTreePanel::RenderSection(
    const char* label, ImVec4 color, const char* icon,
    std::vector<TreeNode>& nodes, bool& expanded, int count)
{
    ImGui::PushStyleColor(ImGuiCol_Header,        BG2);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, BG2);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,  BG3);

    bool open = ImGui::CollapsingHeader(
        ("##sec_" + std::string(label)).c_str(),
        expanded ? ImGuiTreeNodeFlags_DefaultOpen : 0);
    expanded = open;

    // Overlay: icono + label + badge
    // (Usar ImGui::SameLine + GetItemRectMin para posicionar sobre el header)

    if (open) {
        for (auto& node : nodes)
            RenderNode(node);
    }
    ImGui::PopStyleColor(3);
}
```

### 3.9 Hover actions (editar / duplicar / eliminar)

Los botones solo son visibles cuando el ratón está sobre el nodo:

```cpp
// Al final de RenderNode(), antes de cerrar el selectable:
if (ImGui::IsItemHovered()) {
    float btnW = 18.0f;
    ImVec2 rowMax = ImGui::GetItemRectMax();
    // Posicionar botones a la derecha
    float x = rowMax.x - btnW * 3 - 6.0f;
    float y = ImGui::GetItemRectMin().y + 2.0f;

    ImGui::SetCursorScreenPos({x, y});
    if (ImGui::SmallButton(("✎##e" + node.id).c_str()))
        if (OnNodeSelected) OnNodeSelected(node.id, node.type);
    ImGui::SameLine(0,2);
    if (ImGui::SmallButton(("⊕##d" + node.id).c_str()))
        if (OnDuplicateNode) OnDuplicateNode(node.id);
    ImGui::SameLine(0,2);
    if (ImGui::SmallButton(("🗑##x" + node.id).c_str()))
        if (OnDeleteNode) OnDeleteNode(node.id);
}
```

### 3.10 Barra inferior (footer)

```cpp
void ProjectTreePanel::RenderFooter() {
    ImGui::Separator();
    float halfW = (ImGui::GetContentRegionAvail().x - 4.0f) * 0.5f;

    if (ImGui::Button("+ Nueva escena", {halfW, 22}))
        if (OnAddNode) OnAddNode(NodeType::Scene);

    ImGui::SameLine(0, 4);

    if (ImGui::Button("Añadir...", {halfW, 22})) {
        ImGui::OpenPopup("add_element_popup");
    }

    if (ImGui::BeginPopup("add_element_popup")) {
        if (ImGui::MenuItem("Escena"))     OnAddNode(NodeType::Scene);
        if (ImGui::MenuItem("Personaje"))  OnAddNode(NodeType::NPC);
        if (ImGui::MenuItem("Objeto"))     OnAddNode(NodeType::Item);
        if (ImGui::MenuItem("Puzzle"))     OnAddNode(NodeType::Puzzle);
        if (ImGui::MenuItem("Variable"))   OnAddNode(NodeType::Variable);
        if (ImGui::MenuItem("Audio"))      OnAddNode(NodeType::Audio);
        ImGui::EndPopup();
    }
}
```

---

## 4. Statusbar

Barra de 22px en la parte inferior, con fondo `C_SCENE` (#7b9dff):

```
ADS 0.4.2 | La Cripta del Rey Olvidado | sala_entrada.ads | Línea 12, Col 4     6 escenas | 3 NPC | 2 warns · 1 err
```

```cpp
void RenderStatusBar() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, io.DisplaySize.y - 22});
    ImGui::SetNextWindowSize({io.DisplaySize.x, 22});
    ImGui::PushStyleColor(ImGuiCol_WindowBg, C_SCENE);

    ImGui::Begin("##statusbar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    ImGui::TextColored(BG0, "ADS 0.4.2");
    ImGui::SameLine(); ImGui::TextColored(BG0, "|");
    ImGui::SameLine(); ImGui::TextColored(BG0, "%s", projectName.c_str());
    // ... resto de items
    ImGui::End();
    ImGui::PopStyleColor();
}
```

---

## 5. Datos de demo (para poblar el árbol)

Los siguientes datos se usaron en el diseño. Conectar con el modelo real del proyecto:

### Escenas
| id | label | badge1 | badge2 | error/warn |
|---|---|---|---|---|
| intro | intro | Start | — | — |
| sala_entrada | sala_entrada | — | — | — |
| pasillo_oscuro | pasillo_oscuro | Conditional | Warning | warn |
| camara_trampa | camara_trampa | — | Error | err |
| altar_final | altar_final | End | Win | — |

Sub-nodos de `sala_entrada`: descripción (`txt`), opciones (`opt` × 4)

### Personajes (NPC)
| id | label | badge | sub-nodos |
|---|---|---|---|
| guardian_sombra | guardian_sombra | Conditional (hostil) | diálogos ×6, stats |
| espectro_rey | espectro_rey | Locked | — |
| mercader_sombras | mercader_sombras | Conditional (neutral) | — |

### Objetos
| id | label | badge |
|---|---|---|
| llave_antigua | llave_antigua | Key |
| antorcha | antorcha | Usable |
| mapa_cripta | mapa_cripta | Mission |
| pocion_salud | pocion_salud | Usable |
| daga_oxidada | daga_oxidada | Usable (equipable) |
| pergamino_runa | pergamino_runa | Secret |
| moneda_rara | moneda_rara | — |
| calavera_cristal | calavera_cristal | Mission |

### Puzzles
| id | label | badge | sub-nodos |
|---|---|---|---|
| acertijo_puertas | acertijo_puertas | Key (→pasillo) | pistas ×3, solución |
| secuencia_altar | secuencia_altar | Error | — |
| trampa_suelo | trampa_suelo | Conditional (mortal) | — |

### Variables
| id | tipo | valor |
|---|---|---|
| antorcha_encendida | bool | false |
| llave_obtenida | bool | false |
| rey_apaciguado | bool | false |
| habilidad_percepcion | int | 0 |
| puntos_honor | int | 0 |
| escenas_visitadas | list | [] |
| nombre_jugador | str | "" |
| dificultad | enum | normal |
| tiempo_partida | float | 0.0 |

### Audio
| id | label | tipo |
|---|---|---|
| cripta_amb.ogg | cripta_amb.ogg | amb |
| altar_tema.ogg | altar_tema.ogg | bgm |
| puerta_sfx.ogg | puerta_sfx.ogg | sfx |
| muerte_sfx.ogg | muerte_sfx.ogg | sfx |

---

## 6. Comportamientos interactivos

| Acción | Comportamiento |
|---|---|
| Click en nodo | Selecciona el nodo, dispara `OnNodeSelected` |
| Click en sección header | Colapsa / expande la sección |
| Click en flecha `▶` | Expande sub-nodos del nodo |
| Hover en nodo | Muestra botones ✎ ⊕ 🗑 a la derecha |
| Escribir en buscador | Filtra todos los nodos en tiempo real (case-insensitive) |
| Click en `+ Nueva escena` | Dispara `OnAddNode(NodeType::Scene)` |
| Click en `Añadir...` | Abre popup con todos los tipos |
| Nodo seleccionado | Fondo `BG3`, borde izquierdo 2px en `C_SCENE` |
| Nodo con error | Badge rojo `err` visible siempre |
| Nodo con warning | Badge ámbar `warn` visible siempre |

---

## 7. Integración en el main loop

```cpp
// En tu main loop / dockspace, después de BeginDockSpace:

// Instanciar una vez
static ProjectTreePanel treePanel;

// Conectar callbacks (una vez, en init)
treePanel.OnNodeSelected = [&](const std::string& id, NodeType type) {
    // Abrir el fichero/objeto correspondiente en el editor central
    editorPanel.OpenNode(id, type);
    propertiesPanel.ShowNode(id, type);
};
treePanel.OnDeleteNode = [&](const std::string& id) {
    // Confirmar y eliminar del modelo
};
treePanel.OnAddNode = [&](NodeType type) {
    // Crear nuevo objeto en el modelo y refrescar el árbol
    treePanel.RebuildFromProject();  // llamar tras modificar el modelo
};

// Cada frame:
ImGui::SetNextWindowSize({220, -1}, ImGuiCond_FirstUseEver);
ImGui::Begin("Proyecto##tree");
treePanel.Render();
ImGui::End();
```

---

## 8. Convenciones del proyecto a respetar

- Backend: **ImGui + SDL3**, multiplataforma
- Estructura: clases separadas por fichero `.h` + `.cpp`, con patrón `Panel`
- El árbol **no debe** manejar estado del modelo directamente — solo leerlo y disparar callbacks
- Reconstruir el árbol (`RebuildFromProject()`) solo cuando el modelo cambia, no cada frame
- El panel tiene ancho fijo de 220px pero debe ser **redimensionable** con el splitter del dockspace

---

*Fin de la especificación — generado el 2026-05-08*
