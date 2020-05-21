/*!
 * @file
 * @brief This file contains implementation of phong rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *         Marek Filip, xfilip46@stud.fit.vutbr.cz
 * @date   2020-May-19
 */

#include <student/phongMethod.hpp>
#include <student/bunny.hpp>

/** \addtogroup shader_side 06. Implementace vertex/fragment shaderu phongovy metody
 * Vašim úkolem ve vertex a fragment shaderu je transformovat trojúhelníky pomocí view a projekční matice a spočítat phongův osvětlovací model.
 * Vašim úkolem také je správně vypočítat procedurální barvu.
 * Následující obrázek zobrazuje shadery v různém stupni implementace.
 * Horní řáděk zobrazuje procedurální texturu.
 * Prostřední řádek zobrazuje model králička s nanesenou texturou, ale bez použití transformačních matic.
 * Dolní řádek zobrazuje model po použítí transformačních matic.
 *
 * \image html images/texture.svg "Vypočet procedurální textury." width=1000
 *
 *
 *
 *
 *
 * @{
 */


/**
 * @brief This function represents vertex shader of phong method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void phong_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
  /// clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
  /// world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
  /// ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
  /// osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
  /// view a projekční matici.
  /// View matici čtěte z nulté uniformní proměnné a projekční matici
  /// čtěte z první uniformní proměnné.
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
  /// struktuře.
  /// \image html images/vertex_shader_tasks.svg "Vizualizace vstupů a výstupů vertex shaderu" width=1000
  const auto inPosition = inVertex.attributes[0].v4;
  const auto inNormal = inVertex.attributes[1].v4;
  const auto view = uniforms.uniform[0].m4;
  const auto projection = uniforms.uniform[1].m4;
  auto &outGl_Position = outVertex.gl_Position;
  auto &outPosition = outVertex.attributes[0].v4;
  auto &outNormal = outVertex.attributes[1].v4;

  outGl_Position = projection * view * inPosition;
  outPosition = inPosition;
  outNormal = inNormal;
}

PhongMethod::RGB getSineTextureColors(float x, float y) {
  PhongMethod::RGB rgb;
  float offset = sin(y * 10) / 10;
  // add the offset to x and  multiply it by 5 to make it 10-stripe texture
  x = (x + offset) * 5;

  if (glm::fract(x) > 0.5) {
    rgb = {1, 1, 0};
  } else {
    rgb = {0, 0.5, 0};
  }
  return rgb;
}

float getInterpolarityParam(float y) {
  // normal is facing upright or horizontally from the triangle, not down
  float t = 0;
  if (y > 0 || y <= 1) {
    t = y * y;
  }
  return t;
}

/**
 * @brief This function represents fragment shader of phong method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void phong_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací
  /// model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat
  /// interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br>
  /// Barvu zapište do proměnné gl_FragColor ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní 3 a pozici
  /// světla přečtěte z uniformní 2.
  /// <br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
  /// rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné gl_FragColor ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// <br>
  /// <br>
  /// Difuzní barva materiálu (textura) by měla být procedurálně generována.
  /// Textura je složena zde dvou částí: sinusové pruhy a bílý sněhový poprašek.
  /// Textura je zkombinována z těchto dvou částí podle sklonu normály.
  /// V případě, že normála směřuje kolmo vzhůru je textura čistě bílá.
  /// V případě, že normála směřuje vodorovně nebo dolů je textura složena ze sinusových pruhů.
  /// Bílá textura a textura sinusových pruhů je lineráně míchana pomocí interpolačního parameteru t.
  /// Interpolační parameter t spočtěte z y komponenty normály pomocí t = y*y (samozřejmě s ohledem na negativní čísla).
  ///
  /// \image html images/texture_combination.svg "Vizualizace kombinace textury sinusových pruhů a sněhového poprašku." width=600
  /// <br>
  /// <br>
  /// Sinusové pruhy jsou naneseny v rovině xy pomocí planárního nanášení textur, viz následující obrázek:
  /// \image html images/texture_plane.svg "Vizualizace nanášení textury sinusových pruhů." width=600
  ///
  /// Sinusová textura obsahuje 10 pruhů 5 zelených (0,0.5,0) a pět žlutých (1,1,0).
  /// Pruhy mají stejnou tloušku.
  /// Textura má velikost 1x1 a opakuje se (platí i pro záporné souřadnice).
  /// Pruhy jsou zvlněny pomocí funkce sinus.
  /// Amplituda funkce sinus je 0.1.
  /// Perioda je nastavena na \f$ \frac{2 \cdot \pi}{10} \f$.
  /// Výchylku pruhu lze tedy spočítat jako \f$ \frac{sin(y \cdot 10)}{10} \f$
  /// Tvorba textury je znázorněna na následujícím obrázku:
  ///
  /// \image html images/stripes_texture.svg "Vizualizace tvorby sinusové textury" width=600
  ///
  ///
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  ///
  /// \image html images/fragment_shader_tasks.svg "Vizualizace výpočtu ve fragment shaderu" width=1000
  const auto inPosition = inFragment.attributes[0].v3;
  const auto inNormal = inFragment.attributes[1].v3;
  const auto light = uniforms.uniform[2].v3;
  const auto camera = uniforms.uniform[3].v3;
  float &r = outFragment.gl_FragColor.r;
  float &g = outFragment.gl_FragColor.g;
  float &b = outFragment.gl_FragColor.b;

  auto textureRGB = getSineTextureColors(inPosition.x, inPosition.y);
  auto t = getInterpolarityParam(inNormal.y);

  r = t + (textureRGB.at(0) * (1.f - t));
  g = t + (textureRGB.at(1) * (1.f - t));
  b = t + (textureRGB.at(2) * (1.f - t));

  // phong reflection
  const float shininess = 40.f;
  auto N = glm::normalize(inNormal);
  auto L = glm::normalize(light - inPosition);
  auto V = glm::normalize(camera - inPosition);

  float diffuse = std::max(0.f, glm::dot(L, N));
  auto R = glm::normalize(2 * diffuse * N - L);
  float specular = std::max(0.f, std::pow(std::max(0.f, glm::dot(R, V)), shininess));

  outFragment.gl_FragColor *= diffuse;

  // to prevent light shining from below the bunny, only add specular to nonblack
  if (r != 0 || g != 0 || b != 0) {
    outFragment.gl_FragColor += specular;
  }

  // clamp the values
  r = std::min(1.f, r);
  g = std::min(1.f, g);
  b = std::min(1.f, b);
  // r = std::min(std::max(r, 0.f), 1.f);
  // g = std::min(std::max(g, 0.f), 1.f);
  // b = std::min(std::max(b, 0.f), 1.f);
}




/// @}

/** \addtogroup cpu_side 07. Implementace vykreslení králička s phongovým osvětlovacím modelem.
 * Vaším úkolem je využít naimplementovanou grafickou kartu a vykreslit králička s phongovým osvětlovacím modelem a stínováním a procedurální texturou.
 * @{
 */


/**
 * @brief Constructoro f phong method
 */
PhongMethod::PhongMethod(){
/// \todo Doprogramujte inicializační funkci.
/// Zde byste měli vytvořit buffery na GPU, nahrát data do bufferů, vytvořit
/// vertex puller a správně jej nakonfigurovat, vytvořit program, připojit k
/// němu shadery a nastavit atributy, které se posílají mezi vs a fs.
/// Do bufferů nahrajte vrcholy králička (pozice, normály) a indexy na vrcholy
/// ze souboru \ref bunny.hpp.
/// Shader program by měl odkazovat na funkce/shadery phong_VS a phong_FS.
/// V konfiguraci vertex pulleru nastavte dvě čtecí hlavy.
/// Jednu pro pozice vrcholů a druhou pro normály vrcholů.
/// Nultý vertex/fragment atribut by měl obsahovat pozici vertexu.
/// První vertex/fragment atribut by měl obsahovat normálu vertexu.
/// Nastavte, které atributy (jaký typ) se posílají z vertex shaderu do fragment shaderu.
/// <b>Seznam funkcí, které jistě využijete:</b>
///  - gpu.createBuffer()
///  - gpu.setBufferData()
///  - gpu.createVertexPuller()
///  - gpu.setVertexPullerIndexing()
///  - gpu.setVertexPullerHead()
///  - gpu.enableVertexPullerHead()
///  - gpu.createProgram()
///  - gpu.attachShaders()
///  - gpu.setVS2FSType()
  // buffers init
  const auto verticesSize = sizeof(bunnyVertices);
  vbo = gpu.createBuffer(verticesSize);
  gpu.setBufferData(vbo, 0, verticesSize, bunnyVertices);
  const auto indicesSize = sizeof(bunnyIndices);
  ebo = gpu.createBuffer(indicesSize);
  gpu.setBufferData(ebo, 0, indicesSize, bunnyIndices);

  // vertex puller init
  vao = gpu.createVertexPuller();
  gpu.setVertexPullerIndexing(vao, IndexType::UINT32, ebo);
  const unsigned verticesPosHead = 0;
  const unsigned verticesNormalHead = 1;
  gpu.setVertexPullerHead(vao, verticesPosHead,
                          AttributeType::VEC3, sizeof(struct BunnyVertex), 0, vbo);
  gpu.setVertexPullerHead(vao, verticesNormalHead,
                          AttributeType::VEC3, sizeof(struct BunnyVertex), 12, vbo);
  gpu.enableVertexPullerHead(vao, verticesPosHead);
  gpu.enableVertexPullerHead(vao, verticesNormalHead);

  // shader init
  prg = gpu.createProgram();
  gpu.attachShaders(prg, phong_VS, phong_FS);
  gpu.setVS2FSType(prg, 0, AttributeType::VEC3);
  gpu.setVS2FSType(prg, 1, AttributeType::VEC3);
}


/**
 * @brief This function draws phong method.
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void PhongMethod::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){

/// \todo Doprogramujte kreslící funkci.
/// Zde byste měli aktivovat shader program, aktivovat vertex puller, nahrát
/// data do uniformních proměnných a
/// vykreslit trojúhelníky pomocí funkce \ref GPU::drawTriangles.
/// Data pro uniformní proměnné naleznete v parametrech této funkce.
/// <b>Seznam funkcí, které jistě využijete:</b>
///  - gpu.bindVertexPuller()
///  - gpu.useProgram()
///  - gpu.programUniformMatrix4f()
///  - gpu.programUniform3f      ()
///  - gpu.drawTriangles()
///  - gpu.unbindVertexPuller()
  gpu.clear(.5f,.5f,.5f,1.f);

  gpu.bindVertexPuller(vao);
  gpu.useProgram(prg);

  gpu.programUniformMatrix4f(prg, 0, view);
  gpu.programUniformMatrix4f(prg, 1, proj);
  gpu.programUniform3f(prg, 2, light);
  gpu.programUniform3f(prg, 3, camera);

  const unsigned verticesRows = sizeof(bunnyIndices) / sizeof(bunnyIndices[0]);
  const unsigned verticesCols = sizeof(bunnyIndices[0]) / sizeof(bunnyIndices[0][0]);
  const unsigned bunnyVertices = verticesRows * verticesCols;
  gpu.drawTriangles(bunnyVertices);
  gpu.unbindVertexPuller();
}

/**
 * @brief Destructor of phong method.
 */
PhongMethod::~PhongMethod(){
  ///\todo Zde uvolněte alokované zdroje
  /// <b>Seznam funkcí</b>
  ///  - gpu.deleteProgram()
  ///  - gpu.deleteVertexPuller()
  ///  - gpu.deleteBuffer()
  gpu.deleteVertexPuller(vao);
  gpu.deleteProgram(prg);
  gpu.deleteBuffer(vbo);
  gpu.deleteBuffer(ebo);
}

/// @}
