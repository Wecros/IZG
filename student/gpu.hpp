/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *         Marek Filip, xfilip46@stud.fit.vutbr.cz
 * @date   2020-May-11
 */
#pragma once

#include <student/fwd.hpp>
#include <iostream>           // input, output
#include <unordered_map>      // std::unordered_map
#include <string>             // std::string
#include <vector>             // std::vector
#include <SDL.h>              // SDL functions
#include <cstring>            // std::memmove
#include <memory>             // std::unique_ptr
#include <array>              // std::array

/**
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    /// @}

    typedef std::array<OutVertex, 3> Triangle;

    std::vector<OutVertex> vertexPuller(uint32_t nofVertices);
    void vertexProcessor(OutVertex &outVertex, InVertex &inVertex);
    void primitiveAssembly(std::vector<OutVertex> outVertices);
    void clipping(Triangle triangle);
    void perspectiveDivision();
    void viewportTransformation();
    void rasterization();
    void fragmentProcessor();
    void perFragmentOperation();

    OutVertex findClippedVertex(OutVertex A, OutVertex B);

    typedef std::array<uint8_t, 4> RGBA;

    void pinedaTriangle(Triangle triangle, glm::vec2 p);
    inline bool edgeFunction(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c);


    void putPixel(RGBA rgba, int x, int y);
    RGBA getPixel(int x, int y);

  private:
    struct Indexing {
      BufferID  bufferID;
      IndexType indexType;
    };

    struct Head {
      BufferID      bufferID;
      uint64_t      offset;
      uint64_t      stride;
      AttributeType type;
      bool          enabled;
    };

    struct VAO {
      Indexing indexing;
      std::array<Head, maxAttributes> heads;
    };

    struct ProgramSettings {
      VertexShader    vs;
      FragmentShader  fs;
      Uniforms        uniforms;
      std::array<AttributeType, maxAttributes> vs2fs;
    };

    struct FrameBuffer {
      uint32_t width;
      uint32_t height;
      uint8_t *colorBuffer;
      float   *depthBuffer;
    };


    typedef std::unordered_map<BufferID, void*> BufferMap;
    typedef std::unordered_map<VertexPullerID, std::unique_ptr<VAO>> VPMap;
    typedef std::unordered_map<VertexPullerID, std::unique_ptr<ProgramSettings>> ProgramMap;

    BufferMap buffers;

    VPMap vertexPullers;
    VertexPullerID activeVP;

    ProgramMap programs;
    ProgramID activeShader;

    FrameBuffer frameBuffer;

    // #define triangleVertices 3
    std::vector<Triangle> triangles;


    inline float clamp(float x, float min, float max) {
      return std::min(std::max(x, min), max);
    }
};
