/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *         Marek Filip, xfilip46@stud.fit.vutbr.cz
 * @date   2020-May-19
 */

#include <student/gpu.hpp>
/* TODO:
- delete this
- change date
- gpu and rabbit
- add valid source to code (stackOverflow or other)
- replace .width with getWidth etc.
*/


/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
  frameBuffer.colorBuffer = nullptr;
  frameBuffer.depthBuffer = nullptr;
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
  if (!buffers.empty()) {
    auto bufferIt = buffers.begin();
    while (bufferIt != buffers.end()) {
      deleteBuffer(bufferIt->first);
      bufferIt = buffers.begin();
    }
  }

  deleteFramebuffer();
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) {
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>
  BufferID bufferID = buffers.size() + 1;
  void *bufferData = malloc(size);
  buffers[bufferID] = bufferData;

  return bufferID;
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.
  auto bufferIt = buffers.find(buffer);
  if (bufferIt != buffers.end()) {
    free(bufferIt->second);
    buffers.erase(buffer);
  }
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>
  std::memmove((uint8_t *) buffers[buffer] + offset, data, size);
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes.
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned.
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>
  std::memmove(data, (uint8_t *) buffers[buffer] + offset, size);
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) {
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>
  if (buffers.find(buffer) != buffers.end()) {
    return true;
  } else {
    return false;
  }
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller(){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.
  VertexPullerID vao = vertexPullers.size() + 1;

  // Initialize the table as empty
  std::unique_ptr<VAO> emptyTable = std::make_unique<VAO>();
  // VAO *emptyTable = VAO;
  emptyTable->indexing.bufferID = emptyID;
  // emptyTable->indexing.indexType = IndexType::UINT8;
  for (auto head : emptyTable->heads) {
    head.bufferID = emptyID;
    head.offset = 0;
    head.stride = 0;
    head.type = AttributeType::EMPTY;
    head.enabled = false;
  }

  vertexPullers[vao] = std::move(emptyTable);  // FIXME: ignore error
  // vertexPullers.emplace(vao, std::move(emptyTable));

  return vao;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void GPU::deleteVertexPuller(VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
  vertexPullers.erase(vao);
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void GPU::setVertexPullerHead(VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>
  auto vertexPullerIt = vertexPullers.find(vao);
  if (vertexPullerIt != vertexPullers.end()) {
    vertexPullerIt->second->heads[head] = { buffer, offset, stride, type };
  }

}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
  auto vertexPullerIt = vertexPullers.find(vao);
  if (vertexPullerIt != vertexPullers.end()) {
    vertexPullerIt->second->indexing = { buffer, type };
  }
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller
 * @param head head id
 */
void GPU::enableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>
  auto vertexPullerIt = vertexPullers.find(vao);
  if (vertexPullerIt != vertexPullers.end()) {
    vertexPullerIt->second->heads[head].enabled = true;
  }
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>
  auto vertexPullerIt = vertexPullers.find(vao);
  if (vertexPullerIt != vertexPullers.end()) {
    vertexPullerIt->second->heads[head].enabled = false;
  }
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void GPU::bindVertexPuller(VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
  activeVP = vao;
}

/**
 * @brief This function deactivates vertex puller.
 */
void GPU::unbindVertexPuller(){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
  activeVP = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool GPU::isVertexPuller(VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.
  if (vertexPullers.find(vao) != vertexPullers.end()) {
    return true;
  } else {
    return false;
  }
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID GPU::createProgram(){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>
  auto program = std::make_unique<ProgramSettings>();
  // ProgramID id = (ProgramID) program.get();
  ProgramID id = programs.size() + 1;
  programs[id] = std::move(program);
  return id;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void GPU::deleteProgram(ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>
  programs.erase(prg);
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader
 * @param fs fragment shader
 */
void GPU::attachShaders(ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
  programs[prg]->vs = vs;
  programs[prg]->fs = fs;
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void GPU::setVS2FSType(ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>
  auto programIt = programs.find(prg);
  if (programIt != programs.end()) {
    programIt->second->vs2fs[attrib] = type;
  }
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void GPU::useProgram(ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
  activeShader = prg;
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool GPU::isProgram(ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>
  if (programs.find(prg) != programs.end()) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform1f(ProgramID prg,uint32_t uniformId,float     const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní promzěnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>
  auto programIt = programs.find(prg);
  if (programIt != programs.end()) {
    programIt->second->uniforms.uniform[uniformId].v1 = d;
  }
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform2f(ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.
  auto programIt = programs.find(prg);
  if (programIt != programs.end()) {
    programIt->second->uniforms.uniform[uniformId].v2 = d;
  }
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform3f(ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.
  auto programIt = programs.find(prg);
  if (programIt != programs.end()) {
    programIt->second->uniforms.uniform[uniformId].v3 = d;
  }
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform4f(ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.
  auto programIt = programs.find(prg);
  if (programIt != programs.end()) {
    programIt->second->uniforms.uniform[uniformId].v4 = d;
  }
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
  auto programIt = programs.find(prg);
  if (programIt != programs.end()) {
    programIt->second->uniforms.uniform[uniformId].m4 = d;
  }
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>
  frameBuffer.width = width;
  frameBuffer.height = height;
  const uint32_t size = width * height;
  frameBuffer.colorBuffer = new uint8_t[size*4];
  frameBuffer.depthBuffer = new float[size];
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer(){
  /// \todo tato funkce by měla dealokovat framebuffer.
  frameBuffer.width = 0;
  frameBuffer.height = 0;
  if (frameBuffer.colorBuffer != nullptr) {
    delete[] frameBuffer.colorBuffer;
    frameBuffer.colorBuffer = nullptr;
  }
  if (frameBuffer.depthBuffer != nullptr) {
    delete[] frameBuffer.depthBuffer;
    frameBuffer.depthBuffer = nullptr;
  }
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.
  // set the new width and height
  deleteFramebuffer();
  createFramebuffer(width, height);
  // const auto size = width * height;
  // frameBuffer.width = width;
  // frameBuffer.height = height;
  // // reallocated space for color and depth buffers
  // delete[] frameBuffer.colorBuffer;
  // delete[] frameBuffer.depthBuffer;
  // frameBuffer.colorBuffer = new uint8_t[size*4];
  // frameBuffer.depthBuffer = new float[size];
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
  return frameBuffer.colorBuffer;
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
  return frameBuffer.depthBuffer;
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
  return frameBuffer.width;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
  return frameBuffer.height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void GPU::clear(float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>
  const auto size = frameBuffer.width * frameBuffer.height;
  // clear the color buffer
  for (size_t y = 0; y < frameBuffer.height; y++) {
    for (size_t x = 0; x < frameBuffer.width; x++) {
      putPixel({(uint8_t) (r * 255),
                (uint8_t) (g * 255),
                (uint8_t) (b * 255),
                (uint8_t) (a * 255)}, x, y);
    }
  }
  for (size_t i = 0; i < size; i++) {
    frameBuffer.depthBuffer[i] = 2.0f;
  }
}

void GPU::drawTriangles(uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>

  // for (size_t i = 0; i < frameBuffer.height * frameBuffer.height; i++) {
  //   frameBuffer.depthBuffer[i] = 1.0f;
  // }
  if (frameBuffer.colorBuffer != nullptr && frameBuffer.depthBuffer != nullptr) {
    auto outVertices = vertexPuller(nofVertices);

  }
}

// Load the vertices data into in vertices that are transformed into out vertices.
// Returns a vector of out vertices.
std::vector<OutVertex> GPU::vertexPuller(uint32_t nofVertices) {
  auto vao = vertexPullers.at(activeVP).get();
  bool vaoIndexing = isBuffer(vao->indexing.bufferID);

  std::vector<OutVertex> outVertices;
  InVertex inVertex;
  inVertex.gl_VertexID = 0;   // current inVertex ID

  // inVertexID = counter for vertexPuller invocation count
  for (uint32_t i = 0; i < nofVertices; i++) {
    if (vaoIndexing) {
        getBufferData(vao->indexing.bufferID, i * (uint32_t) vao->indexing.indexType,
          (uint64_t) vao->indexing.indexType, &inVertex.gl_VertexID);
    } else {
      inVertex.gl_VertexID = i;
    }

    for (size_t j = 0; j < maxAttributes; j++) {
      auto head = vao->heads[j];
      if (head.enabled) {  // if head is enabled
        switch (head.type)
        {
        case AttributeType::FLOAT:
          getBufferData(head.bufferID, head.offset + head.stride * inVertex.gl_VertexID,
            (uint64_t) head.type * 4, &inVertex.attributes[j].v1);
          break;
        case AttributeType::VEC2:
          getBufferData(head.bufferID, head.offset + head.stride * inVertex.gl_VertexID,
            (uint64_t) head.type * 4, &inVertex.attributes[j].v2);
          break;
        case AttributeType::VEC3:
          getBufferData(head.bufferID, head.offset + head.stride * inVertex.gl_VertexID,
            (uint64_t) head.type * 4, &inVertex.attributes[j].v3);
          break;
        case AttributeType::VEC4:
          getBufferData(head.bufferID, head.offset + head.stride * inVertex.gl_VertexID,
            (uint64_t) head.type * 4, &inVertex.attributes[j].v4);
          break;
        default: break;
        }
      }
    }
    outVertices.emplace_back();
    vertexProcessor(outVertices.back(), inVertex);
    if (outVertices.size() == 3) {
      primitiveAssembly(outVertices);
      outVertices.clear();
    }
  }
  outVertices.clear();
  return outVertices;
}

// Load the in vertex into vertex shader which transforms it into out vertex.
void GPU::vertexProcessor(OutVertex &outVertex, InVertex &inVertex) {
  auto program = programs.at(activeShader).get();
  program->vs(outVertex, inVertex, program->uniforms);
}

// Take 3 consecutive out vertices and build them into a triangle.
void GPU::primitiveAssembly(std::vector<OutVertex> outVertices) {
  for (size_t i = 0; i < outVertices.size(); i+=3) {
    Triangle triangle;
    for (size_t j = 0; j < triangle.size(); j++) {
      triangle.at(j) = outVertices.at(i+j);
    }
    triangles.push_back(triangle);
  }
  clipping(triangles.at(0));
  rasterization();
  perFragmentOperation();
  triangles.clear();
}

// Removes triangle not in view.
void GPU::clipping(Triangle triangle) {
  auto A = triangle.at(0);
  auto B = triangle.at(1);
  auto C = triangle.at(2);
  int ineqCount = 0;
  std::vector<OutVertex> verticesToChange;
  for (auto vertex : triangle) {
    bool isInNearPlace = -vertex.gl_Position.w <= vertex.gl_Position.z;
    if (!isInNearPlace) {
      verticesToChange.push_back(vertex);
      ineqCount++;
    }
  }
  bool isAInNearPlace = -A.gl_Position.w <= A.gl_Position.z;
  bool isBInNearPlace = -B.gl_Position.w <= B.gl_Position.z;
  bool isCInNearPlace = -C.gl_Position.w <= C.gl_Position.z;

  OutVertex Ax, Bx, Cx, Xt1, Xt2;
  Triangle t1, t2, t;
  if (ineqCount == 1) {
    // one extra point is created, additional triangle needed
    if (!isAInNearPlace) {
      Xt1 = findClippedVertex(B, A);
      Xt2 = findClippedVertex(C, A);
      t1 = {B, Xt1, C};
      t2 = {C, Xt1, Xt2};
    } else if (!isBInNearPlace) {
      Xt1 = findClippedVertex(A, B);
      Xt2 = findClippedVertex(C, B);
      t1 = {A, Xt1, C};
      t2 = {C, Xt1, Xt2};
    } else if (!isCInNearPlace) {
      Xt1 = findClippedVertex(A, C);
      Xt2 = findClippedVertex(B, C);
      t1 = {A, Xt1, B};
      t2 = {B, Xt1, Xt2};
    }
    triangles.clear();
    triangles.push_back(t1);
    triangles.push_back(t2);
  } else if (ineqCount == 2) {
    // two points are changed, it's still one triangle
    if (!isAInNearPlace && !isBInNearPlace) {
      Ax = findClippedVertex(C, A);
      Bx = findClippedVertex(C, B);
      t = {C, Ax, Bx};
    } else if (!isBInNearPlace && !isCInNearPlace) {
      Bx = findClippedVertex(A, B);
      Cx = findClippedVertex(A, C);
      t = {A, Bx, Cx};
    } else if (!isAInNearPlace && !isCInNearPlace) {
      Cx = findClippedVertex(B, C);
      Ax = findClippedVertex(B, A);
      t = {B, Cx, Ax};
    }
    triangles.clear();
    triangles.push_back(t);
  } else if (ineqCount == 3) {
    // the whole triangle is outside the view, clip it away
    triangles.clear();
  }
}

OutVertex GPU::findClippedVertex(OutVertex A, OutVertex B) {
  OutVertex Ax;
  float t = (-A.gl_Position.w - A.gl_Position.z) /
            (B.gl_Position.w - A.gl_Position.w + B.gl_Position.z - A.gl_Position.z);
  Ax.gl_Position = A.gl_Position + t * (B.gl_Position - A.gl_Position);
  auto activePrg = programs.at(activeShader).get();
  for (size_t i = 0; i < activePrg->vs2fs.size(); i++) {
    switch (activePrg->vs2fs[i])
    {
    case AttributeType::FLOAT:
      Ax.attributes[i].v1 = A.attributes[i].v1 + t * (B.attributes[i].v1 - A.attributes[i].v1);
      break;
    case AttributeType::VEC2:
      Ax.attributes[i].v2 = A.attributes[i].v2 + t * (B.attributes[i].v2 - A.attributes[i].v2);
      break;
    case AttributeType::VEC3:
      Ax.attributes[i].v3 = A.attributes[i].v3 + t * (B.attributes[i].v3 - A.attributes[i].v3);
      break;
    case AttributeType::VEC4:
      Ax.attributes[i].v4 = A.attributes[i].v4 + t * (B.attributes[i].v4 - A.attributes[i].v4);
      break;
    default: break;
    }
  }
  return Ax;
}

// Transform homogenous coordinates to Cartesian coordinates
void GPU::perspectiveDivision(OutVertex &vertex) {
  auto &vector = vertex.gl_Position;
  if (vector.w == 0) { return; }
  vector.x /= vector.w;
  vector.y /= vector.w;
  vector.z /= vector.w;
}

// Transform NDC values to screen space
void GPU::viewportTransformation(OutVertex &vertex) {
  auto &vector = vertex.gl_Position;
  vector.x *= frameBuffer.width  / 2;
  vector.y *= frameBuffer.height / 2;
  vector.x += frameBuffer.width  / 2;
  vector.y += frameBuffer.height / 2;
}

// Rasterize triangle in screen space
void GPU::rasterization() {
  for (auto &triangle : triangles) {
    int width = frameBuffer.width;
    int height = frameBuffer.height;
    float xmin = width;
    float xmax = 0;
    float ymin = height;
    float ymax = 0;
    for (auto &vertex : triangle) {
      perspectiveDivision(vertex);
      viewportTransformation(vertex);

      auto vector = vertex.gl_Position;
      if (vector.x < xmin) {
        xmin = clamp(vector.x, 0, width);
      }
      if (vector.x > xmax) {
        xmax = clamp(vector.x, 0, width);
      }
      if (vector.y < ymin) {
        ymin = clamp(vector.y, 0, height);
      }
      if (vector.y > ymax) {
        ymax = clamp(vector.y, 0, height);
      }
    }

    for (size_t y = ymin; y < ymax; y++) {
      for (size_t x = xmin; x < xmax; x++) {
        glm::vec2 pixel = {x + 0.5f, y + 0.5f};
        pinedaTriangle(triangle, pixel);
      }
    }
  }
}

float GPU::clamp(float x, float min, float max) {
  return std::min(std::max(x, min), max);
}

/* Helper functions for DrawTriangles functions */

void GPU::pinedaTriangle(Triangle triangle, glm::vec2 p) {
  OutVertex vertex0 = triangle.at(0);
  OutVertex vertex1 = triangle.at(1);
  OutVertex vertex2 = triangle.at(2);
  auto V0 = vertex0.gl_Position;
  auto V1 = vertex1.gl_Position;
  auto V2 = vertex2.gl_Position;

  float area = edgeFunction(V0, V1, V2); // area of the triangle multiplied by 2
  float w0 = edgeFunction(V1, V2, p); // signed area of the triangle v1v2p multiplied by 2
  float w1 = edgeFunction(V2, V0, p); // signed area of the triangle v2v0p multiplied by 2
  float w2 = edgeFunction(V0, V1, p); // signed area of the triangle v0v1p multiplied by 2

  bool inside = ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 < 0 && w1 < 0 && w2 < 0));

  // if point p is inside triangles defined by vertices V0, V1, V2
  if (inside) {
    // barycentric coordinates are the areas of the sub-triangles divided by the area of the main triangle
    w0 /= area;
    w1 /= area;
    w2 /= area;

    InFragment inFragment;
    inFragment.gl_FragCoord.x = p.x;
    inFragment.gl_FragCoord.y = p.y;
    inFragment.gl_FragCoord.z = 0;
    interpolate(inFragment, vertex0, vertex1, vertex2, w0, w1, w2);

    // Fragment Procesor
    OutFragment outFragment;
    fragmentProcessor(outFragment, inFragment);

    if (frameBuffer.depthBuffer != nullptr && frameBuffer.colorBuffer != nullptr) {
      if (inFragment.gl_FragCoord.z < getDepth((int) p.x, (int) p.y)) {
        // get color
        uint8_t r = (uint8_t) (clamp(outFragment.gl_FragColor.r, 0.f, 1.f) * 255);
        uint8_t g = (uint8_t) (clamp(outFragment.gl_FragColor.g, 0.f, 1.f) * 255);
        uint8_t b = (uint8_t) (clamp(outFragment.gl_FragColor.b, 0.f, 1.f) * 255);
        uint8_t a = (uint8_t) (clamp(outFragment.gl_FragColor.a, 0.f, 1.f) * 255);
        putPixel({r, g, b, a}, (int) p.x, (int) p.y);
        if (inFragment.gl_FragCoord.z == -1.f) {
        }
        putDepth(inFragment.gl_FragCoord.z, (int) p.x, (int) p.y);
      }
    }
  }
}

void GPU::interpolate(InFragment &inFragment, OutVertex V0, OutVertex V1, OutVertex V2, float w0, float w1, float w2) {
  auto program = programs.at(activeShader).get();

  float h0 = V0.gl_Position.w;
  float h1 = V1.gl_Position.w;
  float h2 = V2.gl_Position.w;
  // interpolate attributes
  for (size_t i = 0; i < program->vs2fs.size(); i++) {
    auto attributeType = program->vs2fs[i];
    // TODO: check for active head
    // TODO: move formula into seperate function
    switch (attributeType)
    {
    case AttributeType::FLOAT:
      inFragment.attributes[i].v1 =
        (((V0.attributes[i].v1 * w0) / h0) + ((V1.attributes[i].v1 * w1) / h1) +
         ((V2.attributes[i].v1 * w2) / h2)) / ((w0 / h0) + (w1 / h1) + (w2 / h2));
      break;
    case AttributeType::VEC2:
      inFragment.attributes[i].v2 =
        (((V0.attributes[i].v2 * w0) / h0) + ((V1.attributes[i].v2 * w1) / h1) +
         ((V2.attributes[i].v2 * w2) / h2)) / ((w0 / h0) + (w1 / h1) + (w2 / h2));
      break;
    case AttributeType::VEC3:
      inFragment.attributes[i].v3 =
        (((V0.attributes[i].v3 * w0) / h0) + ((V1.attributes[i].v3 * w1) / h1) +
         ((V2.attributes[i].v3 * w2) / h2)) / ((w0 / h0) + (w1 / h1) + (w2 / h2));
      break;
    case AttributeType::VEC4:
      inFragment.attributes[i].v4 =
        (((V0.attributes[i].v4 * w0) / h0) + ((V1.attributes[i].v4 * w1) / h1) +
         ((V2.attributes[i].v4 * w2) / h2)) / ((w0 / h0) + (w1 / h1) + (w2 / h2));
      break;
    default: break;
    }
  }
  // interpolate Z
  inFragment.gl_FragCoord.z = (((V0.gl_Position.z * w0) / h0) + ((V1.gl_Position.z * w1) / h1) +
     ((V2.gl_Position.z * w2) / h2)) / ((w0 / h0) + (w1 / h1) + (w2 / h2));
}

void GPU::fragmentProcessor(OutFragment &outFragment, InFragment &inFragment) {
  auto program = programs.at(activeShader).get();
  program->fs(outFragment, inFragment, program->uniforms);
}

void GPU::perFragmentOperation() {

}
// Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
float GPU::edgeFunction(glm::vec4 a, glm::vec4 b, glm::vec2 p)
{
  return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
  // return (a.x - b.x) * (p.y - a.y) - (a.y - b.y) * (p.x - a.x);
}


void GPU::putPixel(RGBA rgba, int x, int y) {
	if (x >= 0 && y >= 0 && x < frameBuffer.width && y < frameBuffer.height) {
		frameBuffer.colorBuffer[(y * frameBuffer.width + x) * 4 + 0] = rgba.at(0);
		frameBuffer.colorBuffer[(y * frameBuffer.width + x) * 4 + 1] = rgba.at(1);
		frameBuffer.colorBuffer[(y * frameBuffer.width + x) * 4 + 2] = rgba.at(2);
		frameBuffer.colorBuffer[(y * frameBuffer.width + x) * 4 + 3] = rgba.at(3);
  }
}

GPU::RGBA GPU::getPixel(int x, int y) {
  if (x >= 0 && y >- 0 && x < frameBuffer.width && y < frameBuffer.height) {
    auto r = frameBuffer.colorBuffer[(y * frameBuffer.width + x) * 4 + 0];
    auto g = frameBuffer.colorBuffer[(y * frameBuffer.width + x) * 4 + 1];
    auto b = frameBuffer.colorBuffer[(y * frameBuffer.width + x) * 4 + 2];
    auto a = frameBuffer.colorBuffer[(y * frameBuffer.width + x) * 4 + 3];
    return {r, g, b, a};
  }
	else {
		return {0, 0, 0, 255};
  }
}

void GPU::putDepth(float depth, int x, int y) {
  if (x >= 0 && y >= 0 && x < frameBuffer.width && y < frameBuffer.height) {
    frameBuffer.depthBuffer[y * frameBuffer.width + x] = depth;
  }
}

float GPU::getDepth(int x, int y) {
  if (x >= 0 && y >= 0 && x < frameBuffer.width && y < frameBuffer.height) {
    return frameBuffer.depthBuffer[y * frameBuffer.width + x];
  } else {
    return 0;
  }
}

/// @}
